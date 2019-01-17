/*
 * sample program to write out the calibration table and then 
 * reload it again, verify that the data is in fact reversed
 */

#include <aiousb.h>
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>
#include <string.h>



AIOUSB_BOOL find_ai_board( AIOUSBDevice *dev ) { 
    if ( dev->ProductID >= USB_AI16_16A && dev->ProductID <= USB_AI12_128E ) { 
        return AIOUSB_TRUE;
    } else if ( dev->ProductID >=  USB_AIO16_16A && dev->ProductID <= USB_AIO12_128E ) {
        return AIOUSB_TRUE;
    } else {
            return AIOUSB_FALSE;
    }
}


int main( int argc, char **argv ) 
{
    AIOCommandLineOptions *options = NewDefaultAIOCommandLineOptions();
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    intlist *indices = Newintlist();
    ADCConfigBlock *config;
    AIOUSBDevice *dev;
    USBDevice *usb;
    double *volts;
    struct timespec starttime , curtime, prevtime ;

    /**
     * @brief these functions are not needed IF you use
     * the function AIOProcessCmdLine(). It will make the 
     * following call sequence
     * 
     * @verbatim
    * retval = AIOUSB_Init();
    * if ( retval != AIOUSB_SUCCESS ) { 
    *     fprintf(stderr,"Error calling AIOUSB_Init(): %d\n", (int)retval );
    *     exit(retval );
    * }
    * AIOUSB_ListDevices();
    * @endverbatim
    */

    retval = AIOProcessCommandLine( options, &argc, argv );
    /**
     * @brief this is the preferred form over 
     * @verbatim
     *        retval = AIOProcessCmdline( options, argc, argv );
     * @verbatim
     *        in the case where you want your program to process 
     *        extra options on the command line as the options not 
     *        parsed by AIOProcessCommandLine will be retained
     *        in argv.
     * @endverbatim
     */

    if ( retval < 0 ) 
        exit(retval);

    /**
     * Alternatives are 
     * @verbatim
     * AIOUSB_FindDevices( &indices, &num_devices, find_ai_board );
     * AIOUSB_FindDevicesByGroup( &indices, &num_devices, AIO_ANALOG_INPUT() );
     * @endverbatim
     */
    AIOUSB_FindDeviceIndicesByGroup( indices, AIO_ANALOG_INPUT() );

    if ( ( retval = AIOCommandLineOptionsListDevices( options, indices )) < AIOUSB_SUCCESS )
        exit(retval);

    if ( (config = NewADCConfigBlockFromJSON( AIOCommandLineOptionsGetDefaultADCJSONConfig(options) )) == NULL )
        exit(AIOUSB_ERROR_INVALID_ADCCONFIG);


    if ( (retval = AIOCommandLineOptionsOverrideADCConfigBlock( config, options )) != AIOUSB_SUCCESS )
        exit(retval);


    /* Save the config for the device index  in question */
    dev = AIODeviceTableGetDeviceAtIndex( AIOCommandLineOptionsGetDeviceIndex(options) , (AIORESULT*)&retval );
    if ( retval != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error getting device at index %d\n", (int)AIOCommandLineOptionsGetDeviceIndex(options) );
        exit(retval);
    }
    
    usb = AIOUSBDeviceGetUSBHandle( dev );


    retval = ADC_SetCal(AIOCommandLineOptionsGetDeviceIndex(options), ":AUTO:");
    // or 
    /* retval = ADC_SetCal(AIOCommandLineOptionsGetDeviceIndex(options), ":1TO1:"); */
    if ( retval < AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error setting calibration %d\n", (int)retval);
        exit(retval);
    }


    /**
     * Copy the modified config settings back to the 
     * device ave config to the device 
     */
    retval = ADCConfigBlockCopy( AIOUSBDeviceGetADCConfigBlock( dev ), config );
    retval = USBDevicePutADCConfigBlock( usb, config );
    /* or do this     
     * ADC_SetConfig( AIOCommandLineOptionsGetDeviceIndex(options), config->registers, &config->size ); */



    volts = (double*)malloc((ADCConfigBlockGetEndChannel( config )-ADCConfigBlockGetStartChannel( config )+1)*sizeof(double));
    
#ifdef UNIX
    if ( AIOCommandLineOptionsGetIncludeTiming( options ) ) {
        clock_gettime( CLOCK_MONOTONIC_RAW, &starttime );
        memcpy(&prevtime,&starttime,sizeof(starttime));
    }
#endif

    for ( int i = 0, channel = 0; i < AIOCommandLineOptionsGetScans(options); i ++ , channel = 0) {
        if ( AIOCommandLineOptionsGetCounts( options ) ) { /* --counts will write out the raw values */
#ifdef UNIX
            if ( AIOCommandLineOptionsGetIncludeTiming( options ) )
                clock_gettime( CLOCK_MONOTONIC_RAW, &prevtime );
#endif
            retval = ADC_GetScan( AIOCommandLineOptionsGetDeviceIndex(options), (unsigned short*)volts );
	    if ( retval != AIOUSB_SUCCESS ) fprintf(stderr,"Error: Got code %d\n", (int)retval );
#ifdef UNIX
            if ( AIOCommandLineOptionsGetIncludeTiming( options ) ) 
                clock_gettime( CLOCK_MONOTONIC_RAW, &curtime );
#endif
            unsigned short *counts = (unsigned short *)volts;
            if( AIOCommandLineOptionsGetIncludeTiming( options ) )
                fprintf(stdout,"%ld,%ld,%ld,", curtime.tv_sec, (( prevtime.tv_sec - starttime.tv_sec )*1000000000 + (prevtime.tv_nsec - starttime.tv_nsec )), (curtime.tv_sec-prevtime.tv_sec)*1000000000 + ( curtime.tv_nsec - prevtime.tv_nsec) );



            for ( int j = ADCConfigBlockGetStartChannel( config ); j < ADCConfigBlockGetEndChannel( config ) ; j ++ , channel ++) {
                printf("%u,", counts[channel] );
            }
            printf("%u\n", counts[channel] );


        } else {
#ifdef UNIX
            if ( AIOCommandLineOptionsGetIncludeTiming( options ) )
                clock_gettime( CLOCK_MONOTONIC_RAW, &prevtime );
#endif
            retval = ADC_GetScanV( AIOCommandLineOptionsGetDeviceIndex(options) , volts );
	    if ( retval != AIOUSB_SUCCESS ) fprintf(stderr,"Error: Got code %d\n", (int)retval );
#ifdef UNIX
            if ( AIOCommandLineOptionsGetIncludeTiming( options ) ) 
                clock_gettime( CLOCK_MONOTONIC_RAW, &curtime );
#endif
            if( AIOCommandLineOptionsGetIncludeTiming( options ) )
                fprintf(stdout,"%ld,%ld,%ld,", curtime.tv_sec, (( prevtime.tv_sec - starttime.tv_sec )*1000000000 + (prevtime.tv_nsec - starttime.tv_nsec )), (curtime.tv_sec-prevtime.tv_sec)*1000000000+ ( curtime.tv_nsec - prevtime.tv_nsec) );

            for ( int j = ADCConfigBlockGetStartChannel( config ); j < ADCConfigBlockGetEndChannel( config ) ; j ++ , channel ++) {
                printf("%.3f,", volts[channel] );
            }
            printf("%f\n", volts[channel] );


        }

    }

    DeleteAIOCommandLineOptions( options );
    return 0;
}



