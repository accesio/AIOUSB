
#include "TestCaseSetup.h"
#include <stdlib.h>
#include <aiousb.h>
#include <AIOUSB_Core.h>
#include "AIOTypes.h"

using namespace AIOUSB;

int CURRENT_DEBUG_LEVEL = 1;

TestCaseSetup::TestCaseSetup() : DeviceIndex(0) , 
                                 deviceFound(false) , 
                                 CAL_CHANNEL(DEF_CAL_CHANNEL), 
                                 MAX_CHANNELS(DEF_MAX_CHANNELS) , 
                                 NUM_CHANNELS(DEF_NUM_CHANNELS) ,
                                 number_oversamples(10)
{
  counts                  = (unsigned short*)malloc( sizeof(unsigned short)*MAX_CHANNELS);
  volts                   = (double *)malloc(sizeof(double)* MAX_CHANNELS);
  gainCodes               = (unsigned char *)malloc(sizeof(unsigned char)*NUM_CHANNELS);
  dataBuf                 = NULL;
  maxcounts               = -1;
  // Turn on the Debug level 
}


TestCaseSetup::TestCaseSetup(int deviceIndex, int numChannels ) : DeviceIndex(deviceIndex) , 
                                                                  deviceFound(false) , 
                                                                  CAL_CHANNEL(DEF_CAL_CHANNEL), 
                                                                  MAX_CHANNELS(DEF_MAX_CHANNELS) , 
                                                                  NUM_CHANNELS(numChannels) ,
                                                                  number_oversamples(0)
{
  counts                  = (unsigned short*)malloc( sizeof(unsigned short)*MAX_CHANNELS);
  volts                   = (double *)malloc(sizeof(double)* MAX_CHANNELS);
  gainCodes               = (unsigned char *)malloc(sizeof(unsigned char)*numChannels);
  dataBuf                 = NULL;
  maxcounts               = -1;
  // Turn on the Debug level 
}




TestCaseSetup::~TestCaseSetup() 
{
  AIOUSB_Exit();
  free(counts);
  free(volts);
  free(gainCodes);
}


void TestCaseSetup::setCurrentDeviceIndex( int DI )
{
  DeviceIndex = DI;
}

void TestCaseSetup::resetCPU(void) {
  LOG("Need to do something");
}


void TestCaseSetup::findDevice(void) {
    unsigned long deviceMask = GetDevices();
    unsigned long productID, nameSize, numDIOBytes, numCounters;
    unsigned long result;
    char name[ MAX_NAME_SIZE + 2 ];

    while( deviceMask != 0 ) {
        if( ( deviceMask & 1 ) != 0 ) {
            // found a device, but is it the correct type?
            nameSize = MAX_NAME_SIZE;
            result = QueryDeviceInfo( this->DeviceIndex, &productID, &nameSize, name, &numDIOBytes, &numCounters );
            if( result == AIOUSB_SUCCESS ) {
                if( VALID_PRODUCT( productID ) ) {
                    deviceFound = true;
                    break;
                }
            } else {
                throw std::string("Unable to find device");
            }
        }
        this->DeviceIndex++;
        deviceMask >>= 1;
    }
}

void TestCaseSetup::findDevice( AIOUSB_BOOL (*is_ok_device)( AIOUSBDevice *dev ) ) {
    int *indices;
    int num_devices;
  
    AIOUSB_FindDevices( &indices, &num_devices,  is_ok_device  );
    if ( num_devices == 0 ) {
        throw std::string("Unable to find a device\n");
    }
    DeviceIndex = indices[0];

    calibration_enabled = ADC_CanCalibrate( deviceTable[DeviceIndex].ProductID );

}


void TestCaseSetup::doPreSetup()
{

  AIOUSB_Reset( DeviceIndex );
  AIOUSB_SetCommTimeout( DeviceIndex, 1000 );
  AIOUSB_SetDiscardFirstSample( DeviceIndex, AIOUSB_TRUE );
  
  uint64_t serialNumber;
  int result = GetDeviceSerialNumber( DeviceIndex, &serialNumber );
  if( result != AIOUSB_SUCCESS  ) {
    std::stringstream er;
    er << "Error '" << AIOUSB_GetResultCodeAsString( result );
    er << "' getting serial number of device at index " << DeviceIndex ;
    throw Error( er.str().c_str() );
  }
}
void TestCaseSetup::doDACDirectSetup()
{
  unsigned long result;
  const int MAX_DIO_BYTES = 4;
  const int MASK_BYTES = ( MAX_DIO_BYTES + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE;
  const int MAX_NAME_SIZE = 20;
  unsigned char outputMask[ MASK_BYTES ];
  unsigned char writeBuffer[ MAX_DIO_BYTES ];
  char name[ MAX_NAME_SIZE + 2 ];
  unsigned long productID;
  unsigned long nameSize;
  unsigned long numDIOBytes;
  unsigned long numCounters;

  AIOUSB_SetCommTimeout(  DeviceIndex, 1000 );

  result = QueryDeviceInfo( DeviceIndex , &productID , &nameSize, name, &numDIOBytes, &numCounters );  

  for( int port = 0; port < (int)numDIOBytes; port++ ) { 
    writeBuffer[ port ] = 0x11 * ( port + 1 );
  }

  result = DIO_Configure( DeviceIndex, AIOUSB_FALSE , outputMask, writeBuffer );

  printf( "Writing patterns to devices:" );
  fflush( stdout );				// must do for "real-time" feedback

  for( int pattern = 0x00; pattern <= 0xf0; pattern += 0x10 ) {
    for( int port = 0; port < (int)numDIOBytes; port++ )
      writeBuffer[ port ] = pattern + DeviceIndex * 0x04 + port;
          
    result = DIO_WriteAll( DeviceIndex , writeBuffer );
          
    if( result != AIOUSB_SUCCESS )
      throw("Error performing DIO_WriteAll");
  }
}


void TestCaseSetup::doDACDirect(int channel, unsigned short value )
{
  //DACDirect( DeviceIndex, channel, value  );
  int result =   DIO_WriteAll(DeviceIndex, &value );
  std::ostringstream err; 
  if( result != AIOUSB_SUCCESS ) {
    err << "Error on DIO_WriteAll result='" << result << "'";
    throw(err.str());
  }

}



/** 
 * @brief Exception handler
 * 
 * @param result 
 * @param linnum 
 */
void 
TestCaseSetup::ThrowError( unsigned long result , int linnum )
{
  std::stringstream er;
  er << "Error at line:" << linnum << " " << AIOUSB_GetResultCodeAsString( result ) << "' setting A/D configuration";
  throw Error(er.str().c_str());
}


void 
TestCaseSetup::doFastITScanSetup()
{

  DeviceDescriptor *const deviceDesc = &deviceTable[ DeviceIndex ];
  unsigned long result;

  result = ADC_SetCal( DeviceIndex, ":AUTO:");
  CHECK_RESULT( result );

  result = ADC_GetConfig( DeviceIndex, ADC_GetADConfigBlock_Registers( &deviceDesc->cachedConfigBlock ), &deviceDesc->ConfigBytes );
  CHECK_RESULT( result );

  for( int i = 0 ; i <= 15 ; i ++ ) 
    AIOUSB_SetRegister( &deviceDesc->cachedConfigBlock, i , 0x00 );

  AIOUSB_SetRegister( &deviceDesc->cachedConfigBlock, 0x13 , 0x07 );
  

  result = ADC_SetConfig( DeviceIndex, &deviceDesc->cachedConfigBlock.registers[0] , &deviceDesc->ConfigBytes );
  CHECK_RESULT( result );
}

void
TestCaseSetup::doFastITScan( int numgets )
{
  unsigned long result;
  double *data;

  result = ADC_InitFastITScanV( DeviceIndex  );
  CHECK_RESULT( result );
  data = (double *)malloc( sizeof(double)*16 );

  
  for( int i = 0 ; i < numgets ; i ++ )  {
    result = ADC_GetFastITScanV( DeviceIndex , data);
    for( int j = 0; j <= 15; j ++ ) { 
      std::cout << data[j] << ",";
    }
    std::cout << std::endl;
    // Display the results
    
    CHECK_RESULT( result );
  }

  free(data);

  // Now perform a reset at the end
  ADC_ResetFastITScanV( DeviceIndex );
 
}


/** 
 * @brief Uploads a bulk configuration block
 * 
 */
void TestCaseSetup::doBulkConfigBlock()
{
    AIOUSB_InitConfigBlock( &configBlock, DeviceIndex, AIOUSB_FALSE );
    AIOUSB_SetAllGainCodeAndDiffMode( &configBlock, AD_GAIN_CODE_10V, AIOUSB_FALSE );
    ADCConfigBlockSetCalMode( &configBlock, AD_CAL_MODE_NORMAL );
    ADCConfigBlockSetTriggerMode( &configBlock, 0 );
    AIOUSB_SetScanRange( &configBlock, 2, 13 );
    ADCConfigBlockSetOversample( &configBlock, 0 );
    int result = ADC_SetConfig( DeviceIndex, configBlock.registers, &configBlock.size );
    if( result != AIOUSB_SUCCESS  ) {
        std::stringstream er;
        er << "Error '" << AIOUSB_GetResultCodeAsString( result ) << "' setting A/D configuration";
        throw Error(er.str().c_str());
    }                  
}

/** 
 * @brief Demonstrate bulk acquire
 * 
 */
void TestCaseSetup::doBulkAcquire(void)
{
  unsigned int BLOCK_SIZE   = 100000;
  int OVER_SAMPLE  = 2;
  const double CLOCK_SPEED = 100000;	// Hz
  doBulkAcquire( BLOCK_SIZE, OVER_SAMPLE, CLOCK_SPEED );
}



/** 
 * @brief writes the bytes to a file in question. Will be binary
 *       unless the user specifies CSV as an argument
 **/
void TestCaseSetup::writeBuffer( char *filename ) {
  FILE *fp =  fopen(filename,"w");
  if ( !fp  ) {
    perror("Unable to open file ");
    return;
  }
  //number_oversamples
  
  for ( unsigned int i = 0; i < block_size ; i ++ ) {
      
      for( int channel = 0; channel < NUM_CHANNELS ; channel ++ ) { 
          double tot = 0.0;
          for ( unsigned int j = 0; j < number_oversamples ; j ++ ) {
              tot += dataBuf[(i*16)*(11)+(11*channel)+j];
          }
          double v = tot / number_oversamples / (float)(0xffff+1);
          // int rc = ADC_GetRangeCode( 0 );
          int rc = 0;
        
          if ( (rc & 1 ) != 0 )
              v = v*2 - 1;
        
          if ( (rc & 2 ) == 0 ) 
              v = v * 2;
          if ( (rc & 4 ) == 0 )
              v = v * 5;

          fprintf(fp,"%e", v );
          if ( channel != NUM_CHANNELS ) 
              fprintf(fp,",");
      }
      fprintf(fp,"\n");
  }
  fclose(fp);
}

/** 
 * @brief Demonstrate bulk acquire
 * @param blck_size 
 * @param ovr_sampl 
 * @param clk_speed 
 */
void TestCaseSetup::doBulkAcquire(unsigned int blck_size, unsigned int ovr_sampl, unsigned int clk_speed )
{

  block_size          = ( blck_size <= 0 ? 100000  : blck_size );
  number_oversamples  = ( ovr_sampl <= 0 || ovr_sampl > 255 ? 10 : ovr_sampl );
  double clock_speed  = ADC_GetMaxClockRate( deviceTable[DeviceIndex].ProductID,NUM_CHANNELS,number_oversamples );
  

  //                                     scans       *    bytes / sample        *    1 sample + number_oversampless */
  const int BULK_BYTES = block_size  * NUM_CHANNELS  * sizeof( unsigned short ) *    11;


  double clockHz = 0;
  int result;
  int SLEEP_TIME = 1;

  INFO("Setting up over samples and scan limits\n");

  AIOUSB_Reset( DeviceIndex );
  ADC_SetOversample( DeviceIndex, number_oversamples );
  ADC_SetScanLimits( DeviceIndex, 0, NUM_CHANNELS - 1 );
  AIOUSB_SetStreamingBlockSize( DeviceIndex, block_size );

  // AIOUSB_Reset( DeviceIndex );
  // ADC_SetOversample( DeviceIndex, number_oversamples );
  // ADC_SetScanLimits( DeviceIndex, 0, NUM_CHANNELS - 1 );
  // AIOUSB_SetStreamingBlockSize( DeviceIndex, block_size );
    
  // unsigned short *const dataBuf = ( unsigned short * ) malloc( BULK_BYTES );
  dataBuf = ( unsigned short * ) malloc( BULK_BYTES );

  if( dataBuf != 0 ) {
    /*
     * make sure counter is stopped
     */
    CTR_StartOutputFreq( DeviceIndex, 0, &clockHz );

    // configure A/D for timer-triggered acquisition
       
    ADC_ADMode( DeviceIndex, AD_TRIGGER_SCAN | AD_TRIGGER_TIMER, AD_CAL_MODE_NORMAL );

    
    // start bulk acquire; ADC_BulkAcquire() will take care of starting
    // and stopping the counter; but we do have to tell it what clock
    // speed to use, which is why we call AIOUSB_SetMiscClock()
    
    AIOUSB_SetMiscClock( DeviceIndex, clock_speed );
    result = ADC_BulkAcquire( DeviceIndex, BULK_BYTES, dataBuf );

    THROW_IF_ERROR( result, "attempting to start bulk acquire" );
    INFO("Started bulk acquire of %d bytes\n", BULK_BYTES );
    // Use bulk poll to monitor progress
    if( result == AIOUSB_SUCCESS ) {
      unsigned long bytesRemaining = BULK_BYTES;
      for( int seconds = 0; seconds < 100; seconds++ ) {
        TRACE("Sleeping %d seconds\n", SLEEP_TIME );
        sleep(SLEEP_TIME );
        result = TEST_ADC_BulkPoll( DeviceIndex, &bytesRemaining );
        if( result == AIOUSB_SUCCESS ) {
          DEBUG( "  %lu bytes remaining\n", bytesRemaining );

          if( bytesRemaining == 0 )
            break;	// from for()

        } else {

          ERROR( "Error '%s' polling bulk acquire progress\n" , AIOUSB_GetResultCodeAsString( result ) );

          THROW_IF_ERROR( result, " polling bulk acquire progress" );
        }
      }
      
      // Turn off timer-triggered mode
      ADC_ADMode( DeviceIndex, 0, AD_CAL_MODE_NORMAL );
    }
  } 
}

unsigned short *
TestCaseSetup::doGetBuffer()
{
  return this->dataBuf;
}


void TestCaseSetup::doCleanupAfterBulk()
{
  TRACE("doCleanupAfterBulk:\n");
  INFO("Deallocating buffer\n");
  free( dataBuf );
  TRACE("doCleanupAfterBulk: Leaving\n");
}




// 
// Test for the bulk poll that might determine the problems
// 
// 
unsigned long TestCaseSetup::TEST_ADC_BulkPoll( unsigned long DeviceIndex,
                                                unsigned long *BytesLeft
                                                ) 
{
  if( BytesLeft == NULL )
    return AIOUSB::AIOUSB_ERROR_INVALID_PARAMETER;

  if( ! AIOUSB::AIOUSB_Lock() )
    return AIOUSB::AIOUSB_ERROR_INVALID_MUTEX;

  unsigned long result;
  result = AIOUSB::AIOUSB_Validate( &DeviceIndex );
  if( result != AIOUSB::AIOUSB_SUCCESS ) {
    ERROR("Time out occured here");
    AIOUSB::AIOUSB_UnLock();
    return result;
  }

  DeviceDescriptor *const deviceDesc = &deviceTable[ DeviceIndex ];
  if( deviceDesc->bADCStream == AIOUSB::AIOUSB_FALSE ) {
    AIOUSB::AIOUSB_UnLock();
    return AIOUSB::AIOUSB_ERROR_NOT_SUPPORTED;
  }

  *BytesLeft = deviceDesc->workerStatus;
  result     = deviceDesc->workerResult;

  if( result != AIOUSB::AIOUSB_SUCCESS )  {
    ERROR("Timeout from workerResult\n");
  }
  AIOUSB::AIOUSB_UnLock();

  return result;
}



//
// @brief verify that A/D ground calibration is correct 
// 
void TestCaseSetup::doVerifyGroundCalibration(void)
{
  TRACE("doVerifyGroundCalibration:\tVerifying the ground counts\n");
  if ( calibration_enabled ) { 
      ADC_SetOversample( DeviceIndex, 0 );
      ADC_SetScanLimits( DeviceIndex, CAL_CHANNEL, CAL_CHANNEL );
      ADC_ADMode( DeviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_GROUND );
      
      int result = ADC_GetScan( DeviceIndex, this->counts );
      THROW_IF_ERROR( result, "attempting to read ground counts" );
      INFO("Ground counts = %u (should be approx. 0)\n", this->counts[ CAL_CHANNEL ] );
  } else  {
      INFO("Unable to perform VerifyGroundCalibration on card\n");
  }
     
  TRACE("leaving doVerifyGroundCalibration\n");
}

void TestCaseSetup::THROW_IF_ERROR(int result , const char *format, ... ) {
  char buffer[BUFSIZ];
  va_list args;
  va_start(args, format );
  vsprintf(buffer, format, args );
  va_end(args);
}


int TestCaseSetup::envGetInteger(const char *env)
{
  char *tmp = getenv(env);
  if( !tmp ) {
    return 0;
  } else {
    return atoi(tmp);
  }
}

double TestCaseSetup::envGetDouble(const char *env)
{
  char *tmp = getenv(env);
  if( !tmp ) {
    return (double)0;
  } else {
    return  atol(env);
  }
}



double *TestCaseSetup::getVolts() { return volts ; }

unsigned short *TestCaseSetup::getCounts() { return counts ;}
unsigned char *TestCaseSetup::getGainCodes() { return gainCodes ;}


void TestCaseSetup::doGenericVendorWrite(unsigned char Request, unsigned short Value, unsigned short Index, unsigned long *DataSize, void *pData)
{
  int result = 0;
  INFO( "doGenericVendorWrite:\tDoing Generic Vendor Write\n" );
  /*result = GenericVendorWrite( DeviceIndex, Request, Value, Index, DataSize, pData );*/
  THROW_IF_ERROR( result, "performing GenericVendorWrite" );
  INFO( "doGenericVendorWrite:\tCompleted Generic Vendor Write\n" );
}


/** 
 * @brief Sets up the :auto: calibration mode
 * 
 */
void TestCaseSetup::doSetAutoCalibration(void)
{
  int result;
  TRACE("doSetAutoCalibtration:");
  /*
   * demonstrate automatic A/D calibration
   */
  if ( calibration_enabled ) { 
      INFO("Setting Auto Calibration\n" );
      result = ADC_SetCal( DeviceIndex, ":AUTO:" );
      THROW_IF_ERROR( result, "performing automatic A/D calibration" );
      INFO("A/D settings successfully configured\n");
  } else {
      INFO("Unable to perform SetAutoCalibration on card\n");
  }
  TRACE("Done doSetAutoCalibtration\n");
}

void TestCaseSetup::doTestSetAutoCalibration(void)
{
  int result = 3;
  INFO("doTestSetAutoCalibration:\tTesting the auto calibration");
    
  THROW_IF_ERROR( result, "Performing check of Auto calibration" );
  INFO("doTestSetAutoCalibration:\tAuto calibration testing completed successfully");
}


/** 
 * @brief Verify that A/D reference calibration is correct
 * 
 */
void TestCaseSetup::doVerifyReferenceCalibration(void)
{
  TRACE("doVerifyReferenceCalibration:\t");

  if ( calibration_enabled ) { 
      INFO( "Checking Reference Calibration\n");
      ADC_ADMode( DeviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_REFERENCE );
      int result = ADC_GetScan( DeviceIndex, counts );
      THROW_IF_ERROR( result, "attempting to read reference counts" );
      INFO( "Reference counts = %u (should be approx. 65130)\n", counts[ CAL_CHANNEL ] );
  } else {
      INFO("Unable to perform VerifyReferenceCalibration on card\n");
  }
  TRACE("doVerifyReferenceCalibration:\tCompleted");
}

/** 
 *  @brief DEMONSTRATE SCANNING CHANNELS AND MEASURING VOLTAGES 
 * 
 */
void TestCaseSetup::doDemonstrateReadVoltages() 
{
  LOG("Running test inside of the doDemonstrateReadVoltage\n");
  int result;
  for( int channel = 0; channel < NUM_CHANNELS; channel++ )
    gainCodes[ channel ] = AD_GAIN_CODE_0_10V;
  
  ADC_RangeAll( DeviceIndex, gainCodes, AIOUSB_TRUE );
  ADC_SetOversample( DeviceIndex, 10 );
  ADC_SetScanLimits( DeviceIndex, 0, NUM_CHANNELS - 1 );
  ADC_ADMode( DeviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_NORMAL );
  result = ADC_GetScanV( DeviceIndex, volts );
  
  THROW_IF_ERROR( result, " performing A/D channel scan" );

  LOG("Volts Read:\n");
  for( int channel = 0; channel < NUM_CHANNELS ; channel ++ ) { 
    LOG("\tChannel %d = %f\n", channel, volts[channel] );
  }
}

/** 
 * @brief Simple version that just outputs data to csv file
 * 
 */
void TestCaseSetup::doCSVReadVoltages()
{
  DEBUG("Running test inside of the doDemonstrateReadVoltage\n");
  struct timeval reftime;
  struct timeval delta;
  int result;
  int counter = 0;
  int maxvalue = ( maxcounts < 0 ? 10000: maxcounts );
  for( int channel = 0; channel < NUM_CHANNELS; channel++ )
      gainCodes[ channel ] = AD_GAIN_CODE_0_10V;
  gettimeofday( &reftime, 0 );

  while( counter < maxvalue ) {
      result = ADC_GetScanV( DeviceIndex, volts );
      if ( result != AIOUSB_SUCCESS ) 
          throw("Error performing ADC_GetScanV");
      gettimeofday( &delta, 0 );
      LOG("%lld,", (( (long long int)delta.tv_sec - (long long int)reftime.tv_sec)*1000000 ) + ( (long long int )delta.tv_usec - (long long int)reftime.tv_usec ));
      THROW_IF_ERROR( result, " performing A/D channel scan" );
      for( int channel = 0; channel < NUM_CHANNELS - 1 ; channel ++ ) { 
          LOG("%.3f,", volts[channel] );
      }
      LOG("%f\n", volts[NUM_CHANNELS-1] );

      if( maxcounts >= 0 ) {
          counter ++;
      }
  } 
}

void TestCaseSetup::doCSVWithGetChannelV()
{
    DEBUG("Running test inside of the doDemonstrateReadVoltage\n");
    struct timeval reftime;
    struct timeval delta;

    int counter = 0;
    int maxvalue = ( maxcounts < 0 ? 10000: maxcounts );
    double reading;
    int channel;
    for( channel = 0; channel < NUM_CHANNELS; channel++ )
        gainCodes[ channel ] = AD_GAIN_CODE_0_10V;

    gettimeofday( &reftime, 0 );

    while( counter < maxvalue ) {

        gettimeofday( &delta, 0 );
        LOG("%lld,", (( (long long int)delta.tv_sec - (long long int)reftime.tv_sec)*1000000 ) + ( (long long int )delta.tv_usec - (long long int)reftime.tv_usec ));
        for ( channel = 0; channel < NUM_CHANNELS-1 ; channel ++ )  {
            ADC_GetChannelV( DeviceIndex, channel, &reading );
            LOG("%.3f,", reading );
        }
        ADC_GetChannelV( DeviceIndex, NUM_CHANNELS-1, &reading );
        LOG("%f\n", reading );

        if( maxcounts >= 0 ) {
            counter ++;
        }
    }
}


void TestCaseSetup::setMaxCount( int val )
{
  maxcounts = val;
}


/** 
 * @brief Performs an immediate read of voltages
 * 
 */
void TestCaseSetup::doPreReadImmediateVoltages()
{
  DEBUG("Running test inside of the doPreReadImmediateVoltages\n");

  for( int channel = 0; channel < NUM_CHANNELS; channel++ )
    gainCodes[ channel ] = AD_GAIN_CODE_0_10V;
  
  ADC_RangeAll( DeviceIndex, gainCodes, AIOUSB_TRUE );
  ADC_SetOversample( DeviceIndex, 10 );
  ADC_SetScanLimits( DeviceIndex, 0, NUM_CHANNELS - 1 );
  ADC_ADMode( DeviceIndex, 0 , AD_CAL_MODE_NORMAL ); /*  0 corresponds to TriggerMode */

  DEBUG("Completed doPreReadImmediateVoltages\n");

}

/** 
 * @brief demonstrate reading a single channel in volts
 * 
 */
void TestCaseSetup::doScanSingleChannel()
{

  int result = ADC_GetChannelV( DeviceIndex , CAL_CHANNEL, &volts[ CAL_CHANNEL ] );
  THROW_IF_ERROR( result, " reading A/D channel " );
  LOG("Volts read from A/D channel %d = %f\n", CAL_CHANNEL, volts[ CAL_CHANNEL ] );
}

/** 
 * @brief sets up the voltage parameters for runs
 * 
 */
void TestCaseSetup::setupVoltageParameters(void)
{
  volts     = new double[MAX_CHANNELS];
  gainCodes = new unsigned char[NUM_CHANNELS];
}



