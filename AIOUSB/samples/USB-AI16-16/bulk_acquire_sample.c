/**
 * @file   AIOUSB_Core.c
 * 
 * @page bulk_acquire_test bulk_aquire_test.c
 *
 * @deprecated This is a Deprecated sample. Please look at burst_test.c, continuous_mode_callback.c
 * or continuous_mode_callback.c
 *
 */

#include <aiousb.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

struct opts {
    int num_oversamples;
    int num_channels;
    int num_scans;
    int clock_speed;
    int cal_channel;
    int max_channels;
    int clock_scale;
    int calibration_enabled;
};

void process_cmd_line( struct opts *, int argc, char *argv[] );

int main( int argc, char **argv ) {
    unsigned long deviceMask;
    int MAX_NAME_SIZE = 20;
    char name[ MAX_NAME_SIZE + 2 ];
    unsigned long productID, nameSize, numDIOBytes, numCounters;
    unsigned long deviceIndex = 0;
    AIOUSB_BOOL deviceFound = AIOUSB_FALSE;
    ADConfigBlock configBlock;

    struct opts options = {0, 16, 100000, 500000 , 5 , 128 , 1 , 0 };

    process_cmd_line( &options, argc, argv );
    int CAL_CHANNEL      = options.cal_channel;
    int MAX_CHANNELS     = options.max_channels;
    int NUM_CHANNELS     = options.num_channels;
    int NUM_OVERSAMPLES  = options.num_oversamples;
    int NUM_SCANS        = options.num_scans;
    
   

    unsigned short counts[ MAX_CHANNELS ];
    double volts[ MAX_CHANNELS ];
    unsigned char gainCodes[ NUM_CHANNELS ];

    int BULK_BYTES = NUM_SCANS * NUM_CHANNELS * sizeof( unsigned short ) * (NUM_OVERSAMPLES+1);



    printf("USB-AI16-16A sample program version:'%s',date:'%s'\n"
           "This program demonstrates controlling a USB-AI16-16A device on\n"
           "the USB bus. For simplicity, it uses the first such device found\n"
           "on the bus.\n", 
           AIOUSB_GetVersion(), 
           AIOUSB_GetVersionDate()
           );

    /*
     * MUST call AIOUSB_Init() before any meaningful AIOUSB functions;
     * AIOUSB_GetVersion() above is an exception
     */
    unsigned long result = AIOUSB_Init();
    if ( result != AIOUSB_SUCCESS )
        goto exit_sample;

    /*
     * call GetDevices() to obtain "list" of devices found on the bus
     */
    deviceMask = GetDevices();
    if( !deviceMask != 0 ) {
        printf( "No ACCES devices found on USB bus\n" );
        goto out_sample;
    }

    /*
     * at least one ACCES device detected, but we want one of a specific type
     */
    AIOUSB_ListDevices();				// print list of all devices found on the bus

    while( deviceMask != 0 ) {
        if( ( deviceMask & 1 ) != 0 ) {
            // found a device, but is it the correct type?
            nameSize = MAX_NAME_SIZE;
            result = QueryDeviceInfo( deviceIndex, &productID, &nameSize, name, &numDIOBytes, &numCounters );
            if( result == AIOUSB_SUCCESS ) {
                if(
                   productID >= USB_AI16_16A
                   && productID <= USB_AIO12_128E
                   ) {
                    // found a USB-AI16-16A family device
                    options.calibration_enabled = ADC_CanCalibrate( productID );
                    deviceFound = AIOUSB_TRUE;
                    break;				// from while()
                }
            } else
                printf( "Error '%s' querying device at index %lu\n", 
                        AIOUSB_GetResultCodeAsString( result ), deviceIndex );
        }
        deviceIndex++;
        deviceMask >>= 1;
    }

    if( !deviceFound ) {
        printf( "Failed to find USB-AI16-16A device\n" );
        goto out_sample;
    }
    /* AIOUSB_Reset( deviceIndex ); */

    double CLOCK_SPEED = MIN(ADC_GetMaxClockRate( deviceTable[deviceIndex].ProductID,NUM_CHANNELS,NUM_OVERSAMPLES), options.clock_speed );
    if ( CLOCK_SPEED == 0 ) {
        fprintf(stderr, "Got incorrect minimum clock speed of 0 for device. It Looks like this ADC device (id=%#X) is not setup for ADC\n", deviceTable[deviceIndex].ProductID );
        exit(1);
    }

    AIOUSB_SetCommTimeout( deviceIndex, 1000 );
    AIOUSB_SetDiscardFirstSample( deviceIndex, AIOUSB_TRUE );

    uint64_t serialNumber;
    result = GetDeviceSerialNumber( deviceIndex, &serialNumber );
    if( result == AIOUSB_SUCCESS )
        printf( "Serial number of device at index %lu: %llx\n", deviceIndex, ( long long ) serialNumber );
    else
        printf( "Error '%s' getting serial number of device at index %lu\n", 
                AIOUSB_GetResultCodeAsString( result ), deviceIndex );

    /*
     * demonstrate A/D configuration; there are two ways to configure the A/D;
     * one way is to create an ADConfigBlock instance and configure it, and then
     * send the whole thing to the device using ADC_SetConfig(); the other way
     * is to use the discrete API functions such as ADC_SetScanLimits(), which
     * send the new settings to the device immediately; here we demonstrate the
     * ADConfigBlock technique; below we demonstrate use of the discrete functions
     */

    AIOUSB_InitConfigBlock( &configBlock, deviceIndex, AIOUSB_FALSE );
    AIOUSB_SetAllGainCodeAndDiffMode( &configBlock, AD_GAIN_CODE_10V, AIOUSB_FALSE );
    ADCConfigBlockSetCalMode( &configBlock, AD_CAL_MODE_NORMAL );
    ADCConfigBlockSetTriggerMode( &configBlock, 0 );
    AIOUSB_SetScanRange( &configBlock, 2, 13 );
    ADCConfigBlockSetOversample( &configBlock, 0 );
    result = ADC_SetConfig( deviceIndex, configBlock.registers, &configBlock.size );
    if( result < AIOUSB_SUCCESS ) {
        printf( "Error '%s' setting A/D configuration\n" , AIOUSB_GetResultCodeAsString( result ) );
        goto out_sample;
    }

    printf( "A/D settings successfully configured\n" );

    /*
     * demonstrate automatic A/D calibration
     */

    if ( options.calibration_enabled ) { 
        result = ADC_SetCal( deviceIndex, ":AUTO:" );
        if( result == AIOUSB_SUCCESS )
            printf( "Automatic calibration completed successfully\n" );
        else
            printf( "Error '%s' performing automatic A/D calibration\n", AIOUSB_GetResultCodeAsString( result ) );
        
        /*
         * verify that A/D ground calibration is correct
         */

        ADC_SetOversample( deviceIndex, 0 );
        ADC_SetScanLimits( deviceIndex, CAL_CHANNEL, CAL_CHANNEL );
        ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_GROUND );
        result = ADC_GetScan( deviceIndex, counts );
        if( result < AIOUSB_SUCCESS )
            printf( "Error '%s' attempting to read ground counts\n", 
                    AIOUSB_GetResultCodeAsString( result ) );
        else
            printf( "Ground counts = %u (should be approx. 0)\n", counts[ CAL_CHANNEL ] );


        /*
         * verify that A/D reference calibration is correct
         */
        ADC_ADMode( deviceIndex, 0 , AD_CAL_MODE_REFERENCE ); /* TriggerMode */
        result = ADC_GetScan( deviceIndex, counts );
        if( result < AIOUSB_SUCCESS )
            printf( "Error '%s' attempting to read reference counts\n", AIOUSB_GetResultCodeAsString( result ) );
        else
            printf( "Reference counts = %u (should be approx. 65130)\n", counts[ CAL_CHANNEL ] );
        
    }

    /*
     * demonstrate scanning channels and measuring voltages
     */
    for( int channel = 0; channel < NUM_CHANNELS; channel++ )
        gainCodes[ channel ] = AD_GAIN_CODE_0_10V;

    ADC_RangeAll( deviceIndex, gainCodes, AIOUSB_TRUE );
    ADC_SetOversample( deviceIndex, NUM_OVERSAMPLES );
    ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );
    ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_NORMAL );
    result = ADC_GetScanV( deviceIndex, volts );

    if( result >= AIOUSB_SUCCESS ) {
        printf( "Volts read:\n" );
        for( int channel = 0; channel < NUM_CHANNELS; channel++ )
            printf( "  Channel %2d = %f\n", channel, volts[ channel ] );
    } else
        printf( "Error '%s' performing A/D channel scan\n", AIOUSB_GetResultCodeAsString( result ) );

    /*
     * demonstrate reading a single channel in volts
     */
    result = ADC_GetChannelV( deviceIndex, CAL_CHANNEL, &volts[ CAL_CHANNEL ] );
    if( result >= AIOUSB_SUCCESS )
        printf( "Volts read from A/D channel %d = %f\n", CAL_CHANNEL, volts[ CAL_CHANNEL ] );
    else
        printf( "Error '%s' reading A/D channel %d\n", 
                AIOUSB_GetResultCodeAsString( result ), 
                CAL_CHANNEL );

    /*
     * demonstrate bulk acquire
     */

    AIOUSB_Reset( deviceIndex );
    ADC_SetOversample( deviceIndex, NUM_OVERSAMPLES );
    ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );

    AIOUSB_SetStreamingBlockSize( deviceIndex, 64*1024 );

    printf("Allocating %d Bytes\n", BULK_BYTES );
    unsigned short * dataBuf = ( unsigned short * ) malloc( BULK_BYTES );
    if ( !dataBuf ) {
        printf( "Error: unable to allocate memory\n");
        goto out_sample;
    }

    /** make sure counter is stopped */
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
    printf("Using Clock speed %d to acquire data\n", (int)CLOCK_SPEED);
    AIOUSB_SetMiscClock( deviceIndex, CLOCK_SPEED );
    /* exit(0); */
    result = ADC_BulkAcquire( deviceIndex, BULK_BYTES, dataBuf );


    if( result == AIOUSB_SUCCESS )
        printf( "Started bulk acquire of %d bytes\n", BULK_BYTES );
    else
        printf( "Error '%s' attempting to start bulk acquire of %d bytes\n", 
                AIOUSB_GetResultCodeAsString( result ), 
                BULK_BYTES );
    /*
     * use bulk poll to monitor progress
     */
    if( result == AIOUSB_SUCCESS ) {
        unsigned long bytesRemaining = BULK_BYTES;
        for( int seconds = 0; seconds < 100; seconds++ ) {
            sleep( 1 );
            result = ADC_BulkPoll( deviceIndex, &bytesRemaining );
            if( result == AIOUSB_SUCCESS ) {
                printf( "  %lu bytes remaining\n", bytesRemaining );
                if( bytesRemaining == 0 )
                    break;
            } else {
                printf( "Error '%s' polling bulk acquire progress\n", 
                        AIOUSB_GetResultCodeAsString( result ) );
                sleep(1);
                break;
            }
        }

        /*
         * turn off timer-triggered mode
         */
        ADC_ADMode( deviceIndex, 0, AD_CAL_MODE_NORMAL );

        /*
         * if all the data was apparently received, scan it for zeros; it's
         * unlikely that any of the data would be zero, so any zeros, particularly
         * a large block of zeros would suggest that the data is not valid
         */
        if( result == AIOUSB_SUCCESS && bytesRemaining == 0 ) {
            AIOUSB_BOOL anyZeroData = AIOUSB_FALSE;
            int zeroIndex = -1;
            for( int index = 0; index < BULK_BYTES / ( int ) sizeof( unsigned short ); index++ ) {
                if( dataBuf[ index ] == 0 ) {
                    anyZeroData = AIOUSB_TRUE;
                    if( zeroIndex < 0 )
                        zeroIndex = index;
                } else {
                    /* if( zeroIndex >= 0 ) { */
                    /*     printf( "  Zero data from index %d to %d\n", zeroIndex, index - 1 ); */
                    /*     zeroIndex = -1; */
                    /* } */
                }
            }
            if( anyZeroData == AIOUSB_FALSE )
                printf( "Successfully bulk acquired %d bytes\n", BULK_BYTES );
        } else {
            printf( "Failed to bulk acquire %d bytes: %d\n",  BULK_BYTES , (int)result );
            result = -3;
        }
    }

    free( dataBuf );

    /*
     * MUST call AIOUSB_Exit() before program exits,
     * but only if AIOUSB_Init() succeeded
     */
 out_sample:
    AIOUSB_Exit();

 exit_sample:
    return ( int ) result;
}

void print_usage(int argc, char **argv,  struct option *options)
{
    fprintf(stderr,"%s - Options\n", argv[0] );
    for ( int i =0 ; options[i].name != NULL ; i ++ ) {
      fprintf(stderr,"\t-%c | --%s ", (char)options[i].val, options[i].name);
      if( options[i].has_arg == optional_argument ) {
        fprintf(stderr, " [ ARG ]\n");
      } else if( options[i].has_arg == required_argument ) {
        fprintf(stderr, " ARG\n");
      } else {
        fprintf(stderr,"\n");
      }
    }
}

void process_cmd_line( struct opts *options, int argc, char *argv[] )
{

    int c;
    /* int digit_optind = 0; */
    int error = 0;
    /* int this_option_optind = optind ? optind : 1; */
    int option_index = 0;

    static struct option long_options[] = {
      {"num_oversamples" ,  required_argument , 0,  'O' },
      {"num_channels"    ,  required_argument , 0,  'n' },
      {"num_scans"       ,  required_argument , 0,  'S' },
      {"clock_speed"     ,  required_argument , 0,  'T' },
      {"cal_channel"     ,  required_argument , 0,  'C' },
      {"help"            ,  no_argument       , 0,  'h' },
      {0,         0,                 0,  0 }
    };
    while (1) { 
        c = getopt_long(argc, argv, "O:n:S:T:C:h", long_options, &option_index);
        if( c == -1 )
          break;
        switch (c) {
        case 'h':
          print_usage(argc, argv, long_options );
          _exit(1);
          break;
        case 'O':
          options->num_oversamples = atoi(optarg);
          break;
        case 'n':
            options->num_channels = atoi(optarg);
            break;
        case 'S':
            options->num_scans = atoi(optarg);
            break;
        case 'T':
            options->clock_speed = atoi(optarg);
            break;
        case 'C':
            options->cal_channel = atoi(optarg);
            break;
        default:
          fprintf(stderr, "Incorrect argument '%s'\n", optarg );
          error = 1;
          break;
        }
        if( error ) {
            print_usage(argc, argv, long_options);
            _exit(1);
        }
    }
}

