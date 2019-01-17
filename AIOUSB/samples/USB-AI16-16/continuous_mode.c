/**
 * @file   continuous_mode.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * 
 * @page sample_usb_ai16_16_continuous_mode continuous_mode.c
 *
 * @par Continuous Mode
 *
 * continuous_mode.cpp is simple program that demonstrates using
 * the AIOUSB C library's Continuous mode acquisition API.
 *
 * The key steps for running a continuous mode acquisition are:
 * 
 * 1. Allocate an AIOContinuousBuf using NewAIOContinuousBuf()
 * 2. Set the channel ranges ( using AIOContinuousBufSetStartAndEndChannel ), the number of 
 *    oversamples ( using AIOContinuousBufSetOversample ) and then the gain mode for each channel and 
 *    whether you will use differential or single ended mode ( using AIOContinuousBufSetAllGainCodeAndDiffMode ). 
 * 3. Save the settings to the AIO board using AIOContinuousBufSaveConfig().
 * 4. Set the clock rate for the acquisition using AIOContinuousBufSetClock().
 * 5. Start the acquisition with AIOContinuousBufInitiateCallbackAcquisition().
 * 6. Process ( ie read ) the data in the AIOContinuousBuf using AIOContinuousBufReadIntegerScanCounts()
 *
 * @todo Reference building tag
 */
#ifndef DOXYGEN_IGNORED
#include <stdio.h>
#include <aiousb.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

#include "AIOCountsConverter.h"
#include "AIOUSB_Log.h"
#include <getopt.h>
#endif

#include <aiousb.h>
#include "aiocommon.h"
AIOUSB_BOOL fnd( AIOUSBDevice *dev ) { 
    if ( dev->ProductID >= USB_AI16_16A && dev->ProductID <= USB_AI12_128E ) { 
        return AIOUSB_TRUE;
    } else if ( dev->ProductID >=  USB_AIO16_16A && dev->ProductID <= USB_AIO12_128E ) {
        return AIOUSB_TRUE;
    } else {
        return AIOUSB_FALSE;
    }
}

int 
main(int argc, char *argv[] ) 
{
    struct opts options = AIO_OPTIONS;
    AIOContinuousBuf *buf = 0;
    unsigned read_count = 0;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    int *indices;
    int num_devices;

    process_aio_cmd_line( &options, argc, argv );

    AIOUSB_Init();
    AIOUSB_ListDevices();

#if !defined(__clang__)
    AIOUSB_FindDevices( &indices, &num_devices, LAMBDA( AIOUSB_BOOL, (AIOUSBDevice *dev), { 
                if ( dev->ProductID >= USB_AI16_16A && dev->ProductID <= USB_AI12_128E ) { 
                    return AIOUSB_TRUE;
                } else if ( dev->ProductID >=  USB_AIO16_16A && dev->ProductID <= USB_AIO12_128E ) {
                    return AIOUSB_TRUE;
                } else {
                    return AIOUSB_FALSE;
                }
            } ) 
        );
#else
    AIOUSB_FindDevices( &indices, &num_devices, fnd );
#endif
    aio_list_devices( &options, indices, num_devices );

    if ( ( retval = aio_supply_default_command_line_settings(&options)) != AIOUSB_SUCCESS )
        exit(retval);

    buf = NewAIOContinuousBufForVolts( options.index, options.num_scans , options.num_channels ,  options.num_oversamples );

    /**
     * @page sample_usb_ai16_16_continuous_mode
     * 1. Each buf should have a device index associated with it, you  must setit first
     */
    AIOContinuousBufSetDeviceIndex( buf, options.index );


    if( !buf ) {
        fprintf(stderr,"Can't allocate memory for temporary buffer \n");
        exit(1);
    }
    if( options.reset ) {
        fprintf(stderr,"Resetting device at index %d\n",buf->DeviceIndex );
        AIOContinuousBufResetDevice( buf );
        exit(0);
    }
    FILE *fp = fopen(options.outfile,"w");
    if( !fp ) {
        fprintf(stderr,"Unable to open '%s' for writing\n", options.outfile );
        exit(1);
    }
 

    /**
     * @page sample_usb_ai16_16_continuous_mode
     * 2. Setup the Config object for Acquisition, either the more complicated 
     *    part in comments (BELOW) or using a simple interface.
     * 
     * @brief Alternative setup for the AIOContinuousBuf oversamples, gain code
     *        and trigger modes
     * 
     * @code{.c}
     * ADConfigBlock configBlock;                                                                                
     * AIOUSB_InitConfigBlock( &configBlock, AIOContinuousBuf_GetDeviceIndex(buf), AIOUSB_FALSE );               
     * AIOUSB_SetAllGainCodeAndDiffMode( &configBlock, AD_GAIN_CODE_0_5V, AIOUSB_FALSE );                        
     * AIOUSB_SetTriggerMode( &configBlock, AD_TRIGGER_SCAN | AD_TRIGGER_TIMER ); // 0x05
     * AIOUSB_SetScanRange( &configBlock, 0, 15 );                                                               
     * ADC_QueryCal( AIOContinuousBuf_GetDeviceIndex(buf) );                                                     
     * result = ADC_SetConfig( AIOContinuousBuf_GetDeviceIndex(buf), configBlock.registers, &configBlock.size ); 
     * @endcode
     */

    /**< New simpler interface */
    AIOContinuousBufInitConfiguration( buf );

    AIOContinuousBufSetOversample( buf, ( options.num_oversamples < 256 ? options.num_oversamples : options.default_num_oversamples ) );
    AIOContinuousBufSetStartAndEndChannel( buf, options.start_channel, options.end_channel );

    if( !options.number_ranges ) { 
        AIOContinuousBufSetAllGainCodeAndDiffMode( buf , options.gain_code , AIOUSB_FALSE );
    } else {
        for ( int i = 0; i < options.number_ranges ; i ++ ) {
            retval = AIOContinuousBufSetChannelRange( buf, 
                                                      options.ranges[i]->start_channel, 
                                                      options.ranges[i]->end_channel,
                                                      options.ranges[i]->gaincode
                                                      );
            if ( retval != AIOUSB_SUCCESS ) {
                fprintf(stderr,"Error setting ChannelRange: %d\n", (int)retval );
                exit(1);
            }
        }
    }
    AIOContinuousBufSaveConfig(buf);
#ifdef TEST_GET_CONFIG
    ADConfigBlock tmp;
    tmp.size = 20;
    ADC_GetConfig( 0, tmp.registers, &tmp.size );
#endif

    if ( retval < AIOUSB_SUCCESS ) {
        printf("Error setting up configuration\n");
        exit(1);
    }
  
    /**
     * 3. Setup the sampling clock rate, in this case 
     *    10_000_000 / 1000
     */ 
    if( options.block_size < 0 ) { 
        options.block_size = 1024*64;
    }

    if ( options.clock_rate < 1000 ) { 
        AIOContinuousBufSetStreamingBlockSize( buf, 512 );
    } else  {
        AIOContinuousBufSetStreamingBlockSize( buf, options.block_size );
    }

    AIOContinuousBufSetClock( buf, options.clock_rate );
    /**
     * 4. Start the Callback that fills up the 
     *    AIOContinuousBuf. This fires up an thread that 
     *    performs the acquistion, while you go about 
     *    doing other things.
     */ 
    AIOContinuousBufInitiateCallbackAcquisition(buf);

    int scans_remaining;
    int scans_read = 0;
    int tobufsize =  options.default_num_channels*(options.default_num_oversamples+1)*options.num_scans*sizeof(double);

    double *tobuf = (double*)malloc( tobufsize );

    while ( AIOContinuousBufGetRunStatus(buf) == RUNNING || read_count < options.num_scans ) {

        if ( (scans_remaining = AIOContinuousBufCountScansAvailable(buf) ) > 0 ) { 

            if ( scans_remaining > 0 ) { 

                scans_remaining = MIN( scans_remaining, options.num_scans - read_count );

                if ( options.verbose )
                    fprintf(stdout,"Waiting : total=%u, readpos=%d, writepos=%d\n", read_count, 
                            (int)AIOContinuousBufGetReadPosition(buf), (int)AIOContinuousBufGetWritePosition(buf));

                scans_read = AIOContinuousBufReadIntegerScanCounts( buf, (uint16_t*)tobuf, tobufsize, AIOContinuousBufNumberChannels(buf)*scans_remaining );

                read_count += scans_read;

                for( int scan_count = 0; scan_count < scans_read ; scan_count ++ ) { 

                    for( int ch = 0 ; ch < AIOContinuousBufNumberChannels(buf); ch ++ ) {
                        fprintf(fp,"%lf,",tobuf[scan_count*AIOContinuousBufNumberChannels(buf)+ch] );
                        if( (ch+1) % AIOContinuousBufNumberChannels(buf) == 0 ) {
                            fprintf(fp,"\n");
                        }
                    }
                }
            }
        } else {
        }

    }

    AIOUSB_Exit();
    fclose(fp);
    fprintf(stderr,"Test completed...exiting\n");
    retval = ( retval >= 0 ? 0 : - retval );
    return(retval);
}

