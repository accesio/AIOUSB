/*
 * sample program to write out the calibration table and then 
 * reload it again, verify that the data is in fact reversed
 * 
 *
 * 
 */

#include <aiousb.h>
#include <stdio.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "TestCaseSetup.h"
using namespace AIOUSB;

void goDoIt( TestCaseSetup &t);

int main( int argc, char **argv ) {
  printf("Sample test for Checking the Calibration on the board: %s, %s", AIOUSB_GetVersion(), AIOUSB_GetVersionDate());

  unsigned long result = AIOUSB_Init();

  if( result == AIOUSB_SUCCESS ) {
          
    unsigned long deviceMask = GetDevices();
    if( deviceMask != 0 ) {
      /*
       * at least one ACCES device detected, but we want one of a specific type
       */
      AIOUSB_ListDevices();                // print list of all devices found on the bus
      //const int MAX_NAME_SIZE = 20;
      //char name[ MAX_NAME_SIZE + 2 ];

      TestCaseSetup tcs;
      try { 
        // tcs.doSomething();
        tcs.findDevice();
        tcs.doPreSetup();
        tcs.doBulkConfigBlock();

#ifdef CALIBRATE_IT
        tcs.doSetAutoCalibration();
#endif

#ifdef VERIFY_GROUND_CALIBRATION
        tcs.doVerifyGroundCalibration();
#endif

#ifdef VERIFY_REFERENCE_CALIBRATION
        tcs.doVerifyReferenceCalibration();
#ifdef JUST_VERIFY_REFERENCE_CALIBRATION
        exit(0);
#endif
#endif

#ifdef DEMONSTRATE_SCAN_CHANNELS
        printf("HERE\n");
        tcs.doDemonstrateReadVoltages();
#ifdef JUST_VERIFY_SCAN_CHANNELS
    exit(0);
#endif
#endif

    goDoIt( tcs );

        // tcs.doVerifyReferenceCalibration();
    printf("Reached\n");

#if 0
    int deviceIndex=  0;
    if( 1 ) {


#ifndef CALIBRATE
            const int CAL_CHANNEL = 5;
            const int MAX_CHANNELS = 128;
            const int NUM_CHANNELS = 16;
            unsigned short counts[ MAX_CHANNELS ];
            double volts[ MAX_CHANNELS ];
            unsigned char gainCodes[ NUM_CHANNELS ];
            printf( "A/D settings successfully configured\n" );
            /*
             * demonstrate automatic A/D calibration
             */
            result = ADC_SetCal( deviceIndex, ":AUTO:" );

            if( result == AIOUSB_SUCCESS )
              printf( "Automatic calibration completed successfully\n" );
            else
              printf( "Error '%s' performing automatic A/D calibration\n", AIOUSB_GetResultCodeAsString( result ) );
#endif


            /*
             * verify that A/D ground calibration is correct
             */
            ADC_SetOversample( deviceIndex, 0 );
            ADC_SetScanLimits( deviceIndex, CAL_CHANNEL, CAL_CHANNEL );
            ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_GROUND );
            result = ADC_GetScan( deviceIndex, counts );

            if( result == AIOUSB_SUCCESS )
              printf( "Ground counts = %u (should be approx. 0)\n", counts[ CAL_CHANNEL ] );
            else
              printf( "Error '%s' attempting to read ground counts\n" , AIOUSB_GetResultCodeAsString( result ) );



            /*
             * VERIFY A/D REFERENCE CALIBRATION IS CORRECT
             */
            ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_REFERENCE );
            result = ADC_GetScan( deviceIndex, counts );

            if( result == AIOUSB_SUCCESS )
              printf( "Reference counts = %u (should be approx. 65130)\n", counts[ CAL_CHANNEL ] );
            else
              printf( "Error '%s' attempting to read reference counts\n"
                      /*API*/                        , AIOUSB_GetResultCodeAsString( result ) );





            /*
             * demonstrate scanning channels and measuring voltages
             */
            for( int channel = 0; channel < NUM_CHANNELS; channel++ )
              gainCodes[ channel ] = AD_GAIN_CODE_0_10V;
            /*API*/                ADC_RangeAll( deviceIndex, gainCodes, AIOUSB_TRUE );
            /*API*/                ADC_SetOversample( deviceIndex, 10 );
            /*API*/                ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );
            /*API*/                ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_NORMAL );
            /*API*/                result = ADC_GetScanV( deviceIndex, volts );
            if( result == AIOUSB_SUCCESS ) {
              printf( "Volts read:\n" );
              for( int channel = 0; channel < NUM_CHANNELS; channel++ )
                printf( "  Channel %2d = %f\n", channel, volts[ channel ] );
            } else
              printf( "Error '%s' performing A/D channel scan\n"
                      /*API*/                        , AIOUSB_GetResultCodeAsString( result ) );

            /*
             * demonstrate reading a single channel in volts
             */
            result = ADC_GetChannelV( deviceIndex, CAL_CHANNEL, &volts[ CAL_CHANNEL ] );

            if( result == AIOUSB_SUCCESS )
              printf( "Volts read from A/D channel %d = %f\n", CAL_CHANNEL, volts[ CAL_CHANNEL ] );
            else
              printf( "Error '%s' reading A/D channel %d\n" , AIOUSB_GetResultCodeAsString( result ) , CAL_CHANNEL );

            /*
             * demonstrate bulk acquire
             */
            AIOUSB_Reset( deviceIndex );
            ADC_SetOversample( deviceIndex, 10 );
            ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );
            AIOUSB_SetStreamingBlockSize( deviceIndex, 100000 );


            const int BULK_BYTES = 100000 /* scans */
              * NUM_CHANNELS
              * sizeof( unsigned short ) /* bytes / sample */
              * 11 /* 1 sample + 10 oversamples */;
            const double CLOCK_SPEED = 100000;    // Hz
            unsigned short *const dataBuf = ( unsigned short * ) malloc( BULK_BYTES );
            if( dataBuf != 0 ) {
              /*
               * make sure counter is stopped
               */
              double clockHz = 0;
              CTR_StartOutputFreq( deviceIndex, 0, &clockHz );

              /*
               * configure A/D for timer-triggered acquisition
               */
              ADC_ADMode( deviceIndex, AD_TRIGGER_SCAN | AD_TRIGGER_TIMER, AD_CAL_MODE_NORMAL );

              /*
               * start bulk acquire; ADC_BulkAcquire() will take care of starting
               * and stopping the counter; but we do have to tell it what clock
               * speed to use, which is why we call AIOUSB_SetMiscClock()
               */
              AIOUSB_SetMiscClock( deviceIndex, CLOCK_SPEED );
              result = ADC_BulkAcquire( deviceIndex, BULK_BYTES, dataBuf );

              if( result == AIOUSB_SUCCESS )
                printf( "Started bulk acquire of %d bytes\n", BULK_BYTES );
              else
                printf( "Error '%s' attempting to start bulk acquire of %d bytes\n" , AIOUSB_GetResultCodeAsString( result ) , BULK_BYTES );

              /*
               * use bulk poll to monitor progress
               * Have some issues with this section, it 
               * is causing a crash ... something to do with the 
               * oversampling that is going on 
               *
               */

              if( result == AIOUSB_SUCCESS ) {
                unsigned long bytesRemaining = BULK_BYTES;
                for( int seconds = 0; seconds < 100; seconds++ ) {
                  sleep( 1 );
                  result = ADC_BulkPoll( deviceIndex, &bytesRemaining );

                  if( result == AIOUSB_SUCCESS ) {
                    printf( "  %lu bytes remaining\n", bytesRemaining );
                    if( bytesRemaining == 0 )
                      break;    // from for()
                  } else {
                    printf( "Error '%s' polling bulk acquire progress\n" , AIOUSB_GetResultCodeAsString( result ) );
                    break;
                  }
                }

                /*
                 * turn off timer-triggered mode
                 */
                /*API*/                        ADC_ADMode( deviceIndex, 0, AD_CAL_MODE_NORMAL );

                /*
                 * if all the data was apparently received, scan it for zeros; it's
                 * unlikely that any of the data would be zero, so any zeros, particularly
                 * a large block of zeros would suggest that the data is not valid
                 */
                if( result == AIOUSB_SUCCESS && bytesRemaining == 0 ) { 
                  bool anyZeroData = false;
                  int zeroIndex = -1;
                  for( int index = 0; index < BULK_BYTES / ( int ) sizeof( unsigned short ); index++ ) {
                    if( dataBuf[ index ] == 0 ) {
                      anyZeroData = true;
                      if( zeroIndex < 0 )
                        zeroIndex = index;
                    } else {
                      if( zeroIndex >= 0 ) {
                        printf( "  Zero data from index %d to %d\n", zeroIndex, index - 1 );
                        zeroIndex = -1;
                      }    
                    }
                  }
                  if( anyZeroData == false )
                    printf( "Successfully bulk acquired %d bytes\n", BULK_BYTES );
                } else
                  printf( "Failed to bulk acquire %d bytes\n", BULK_BYTES );
              }

              free( dataBuf );
            }    // if( dataBuf ...

        }        
#endif


      } catch ( Error &e ) {
        printf("Problems locating device : '%s'\n", e.what());
      }
     
      AIOUSB_Exit();
    }
    return ( int ) result;
  }    // main()
}

void goDoIt( TestCaseSetup &tcs  )
{
  int deviceIndex= 0;
  int result;

#ifdef CALIBRATE_IT
  const int CAL_CHANNEL = tcs.CAL_CHANNEL;
  const int MAX_CHANNELS = tcs.MAX_CHANNELS;
  const int NUM_CHANNELS = tcs.NUM_CHANNELS;
  
  double *volts            = tcs.getVolts() ;
  unsigned char *gainCodes = tcs.getGainCodes();
  unsigned short *counts   = tcs.getCounts();
 
#endif


#ifndef CALIBRATE_IT
    //CALIBRATE_IT
    const int CAL_CHANNEL = 5;
    const int MAX_CHANNELS = 128;
    const int NUM_CHANNELS = 16;
    unsigned short counts[ MAX_CHANNELS ];
    double volts[ MAX_CHANNELS ];
    unsigned char gainCodes[ NUM_CHANNELS ];
    printf( "A/D settings successfully configured\n" );

    /*
     * demonstrate automatic A/D calibration
     */
    result = ADC_SetCal( deviceIndex, ":AUTO:" );
    if( result == AIOUSB_SUCCESS )
      printf( "Automatic calibration completed successfully\n" );
    else
      printf( "Error '%s' performing automatic A/D calibration\n"
              /*API*/						, AIOUSB_GetResultCodeAsString( result ) );

#endif

#ifndef VERIFY_GROUND_CALIBRATION
    /*
     * verify that A/D ground calibration is correct
     */
    ADC_SetOversample( deviceIndex, 0 );
    ADC_SetScanLimits( deviceIndex, CAL_CHANNEL, CAL_CHANNEL );
    ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_GROUND );
    result = ADC_GetScan( deviceIndex, counts );

    if( result == AIOUSB_SUCCESS )
      printf( "Ground counts = %u (should be approx. 0)\n", counts[ CAL_CHANNEL ] );
    else
      printf( "Error '%s' attempting to read ground counts\n"
              /*API*/						, AIOUSB_GetResultCodeAsString( result ) );

#ifdef JUST_VERIFY_GROUND_CALIBRATION
    exit(0);
#endif

#endif

#ifndef VERIFY_REFERENCE_CALIBRATION

    /*
     * verify that A/D reference calibration is correct
     */
    ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_REFERENCE );
    result = ADC_GetScan( deviceIndex, counts );
    if( result == AIOUSB_SUCCESS )
      printf( "Reference counts = %u (should be approx. 65130)\n", counts[ CAL_CHANNEL ] );
    else
      printf( "Error '%s' attempting to read reference counts\n"
              /*API*/						, AIOUSB_GetResultCodeAsString( result ) );

#ifdef JUST_VERIFY_REFERENCE_CALIBRATION
    exit(0);
#endif

#endif

#ifndef DEMONSTRATE_SCAN_CHANNELS

    /*
     * DEMONSTRATE SCANNING CHANNELS AND MEASURING VOLTAGES
     */
    for( int channel = 0; channel < NUM_CHANNELS; channel++ )
      gainCodes[ channel ] = AD_GAIN_CODE_0_10V;

    ADC_RangeAll( deviceIndex, gainCodes, AIOUSB_TRUE );
    ADC_SetOversample( deviceIndex, 10 );
    ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );
    ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_NORMAL );
    result = ADC_GetScanV( deviceIndex, volts );

    if( result == AIOUSB_SUCCESS ) {
      printf( "Volts read:\n" );
      for( int channel = 0; channel < NUM_CHANNELS; channel++ )
        printf( "  Channel %2d = %f\n", channel, volts[ channel ] );
    } else
      printf( "Error '%s' performing A/D channel scan\n"
              /*API*/						, AIOUSB_GetResultCodeAsString( result ) );

#ifdef JUST_VERIFY_SCAN_CHANNELS
    exit(0);
#endif

#endif



    /*
     * demonstrate reading a single channel in volts
     */
    result = ADC_GetChannelV( deviceIndex, CAL_CHANNEL, &volts[ CAL_CHANNEL ] );
    if( result == AIOUSB_SUCCESS )
      printf( "Volts read from A/D channel %d = %f\n", CAL_CHANNEL, volts[ CAL_CHANNEL ] );
    else
      printf( "Error '%s' reading A/D channel %d\n" , 
              AIOUSB_GetResultCodeAsString( result ), 
              CAL_CHANNEL );

    /*
     * demonstrate bulk acquire
     */
#ifndef BULK_ACQUIRE
    AIOUSB_Reset( deviceIndex );
    ADC_SetOversample( deviceIndex, 10 );
    ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );
    AIOUSB_SetStreamingBlockSize( deviceIndex, 100000 );

    /*                                  scans    *    bytes / sample        * 1 sample + 10 oversamples */
    const int BULK_BYTES = 100000 * NUM_CHANNELS * sizeof( unsigned short ) * 11;

    unsigned short *const dataBuf = ( unsigned short * ) malloc( BULK_BYTES );
    if( dataBuf != 0 ) {
      double clockHz = 0;
      CTR_StartOutputFreq( deviceIndex, 0, &clockHz );
      /*
       * configure A/D for timer-triggered acquisition
       */
      ADC_ADMode( deviceIndex, AD_TRIGGER_SCAN | AD_TRIGGER_TIMER, AD_CAL_MODE_NORMAL );
      /*
       * start bulk acquire; ADC_BulkAcquire() will take care of starting
       * and stopping the counter; but we do have to tell it what clock
       * speed to use, which is why we call AIOUSB_SetMiscClock()
       */
      result = ADC_BulkAcquire( deviceIndex, BULK_BYTES, dataBuf );
      if( result == AIOUSB_SUCCESS )
        printf( "Started bulk acquire of %d bytes\n", BULK_BYTES );
      else
        printf( "Error '%s' attempting to start bulk acquire of %d bytes\n", 
                AIOUSB_GetResultCodeAsString( result ) , 
                BULK_BYTES );

      /*
       * use bulk poll to monitor progress
       */
      if( result == AIOUSB_SUCCESS ) {
        unsigned long bytesRemaining = BULK_BYTES;
        for( int seconds = 0; seconds < 100; seconds++ ) {
          sleep( 1 );
          /*API*/							result = ADC_BulkPoll( deviceIndex, &bytesRemaining );
          if( result == AIOUSB_SUCCESS ) {
            printf( "  %lu bytes remaining\n", bytesRemaining );
            if( bytesRemaining == 0 )
              break;	// from for()
          } else {
            printf( "Error '%s' polling bulk acquire progress\n"
                    /*API*/									, AIOUSB_GetResultCodeAsString( result ) );
            break;		// from for()
          }	// if( result ...
        }	// for( int seconds ...

        /*
         * turn off timer-triggered mode
         */
        ADC_ADMode( deviceIndex, 0, AD_CAL_MODE_NORMAL );

        /*
         * if all the data was apparently received, scan it for zeros; it's
         * unlikely that any of the data would be zero, so any zeros, particularly
         * a large block of zeros would suggest that the data is not valid
         */
        if(
           result == AIOUSB_SUCCESS
           && bytesRemaining == 0
           ) {
          bool anyZeroData = false;
          int zeroIndex = -1;
          for( int index = 0; index < BULK_BYTES / ( int ) sizeof( unsigned short ); index++ ) {
            if( dataBuf[ index ] == 0 ) {
              anyZeroData = true;
              if( zeroIndex < 0 )
                zeroIndex = index;
            } else {
              if( zeroIndex >= 0 ) {
                printf( "  Zero data from index %d to %d\n", zeroIndex, index - 1 );
                zeroIndex = -1;
              }	// if( zeroIndex ...
            }	// if( dataBuf[ ...
          }	// for( int index ...
          if( anyZeroData == false )
            printf( "Successfully bulk acquired %d bytes\n", BULK_BYTES );
        } else
          printf( "Failed to bulk acquire %d bytes\n", BULK_BYTES );
      }	// if( result ...

      free( dataBuf );
    }	// if( dataBuf ...
#endif

#if 0
  }
#endif
}


/* end of file */
