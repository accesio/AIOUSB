/**
 * @file   burst_test.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @page sample_usb_ai16_16_burst_test burst_test.c
 * @tableofcontents
 * @section sample_usb_ai16_16_burst_test_overview Overview
 *
 * burst_test.c is simple program that performs a high speed
 * continuous acquisition using the AIOUSB C library's Continuous mode
 * acquisition API.  It allows one to setup a simple AIOContinuousBuf,
 * specify the clock rate for the acquisition ( ) , specify the number
 * of channels that the user would like to acquire , start the
 * acquisition and then write to the file called "output.txt".
 * 
 * The output file, _output.txt_, is just a Command Separated Value
 * (csv) file that can be analyzed using R, Matlab or Excel to
 * examine the waveforms generated.
 * 
 * @section sample_usb_ai16_16_burst_test_desc Parts of the sample
 * 
 * @subsection sample_usb_ai16_16_burst_test_cmdline Command line parsing
 *
 * This is just the introductory code that handles command line
 * parsing for most of the Linux and Mac based AIOUSB samples.  There
 * is a standard set of parameters that you can examine if you run 
 * @verbatim
shell> ./burst_test --help 
./burst_test - Options
        -D | --debug  ARG
             --dump
             --dumpadcconfig
        -S | --buffer_size  ARG
        -N | --num_scans  ARG
        -n | --num_channels  ARG
        -O | --num_oversamples  ARG
        -g | --gaincode  ARG
        -c | --clockrate  ARG
        -C | --calibration  ARG
        -h | --help
        -i | --index  ARG
        -R | --range  ARG
             --repeat  ARG
        -r | --reset
        -f | --outfile  ARG
        -V | --verbose
        -B | --block_size  ARG
        -T | --timing
        -q | --query
        -L | --ratelimit  ARG
        -p | --physical
             --counts
        -Y | --yaml
        -J | --json
             --jsonconfig  ARG
@endverbatim
 *
 * @todo Document the Command line Parsing helper library
 *
 *
 */

#include <stdio.h>
#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "AIOUSB_Log.h"
#include "aiocommon.h"
#include <getopt.h>
#include <ctype.h>
#include <time.h>

#define  _FILE_OFFSET_BITS 64  


AIOUSB_BOOL find_ai_board( AIOUSBDevice *dev ) { 
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
    struct timespec starttime , curtime, prevtime;

    AIORET_TYPE retval = AIOUSB_SUCCESS;
    int *indices;
    int num_devices;
    process_aio_cmd_line( &options, argc, argv );

    int tobufsize = (options.default_num_scans+1)*options.default_num_channels*20;
    uint16_t *tobuf = (uint16_t *)malloc(sizeof(uint16_t)*tobufsize);

    AIOUSB_Init();
    AIOUSB_ListDevices();
    AIOUSB_FindDevices( &indices, &num_devices, find_ai_board );
    aio_list_devices( &options, indices, num_devices ); /* will exit if no devices found */

    if ( ( retval = aio_supply_default_command_line_settings(&options)) != AIOUSB_SUCCESS )
        exit(retval);

    /**
     * @page sample_usb_ai16_16_burst_test burst_test.c
     * @tableofcontents
     * @subsection create_new_buf Create a new AIOContinuousBuf
     * @brief This blurb allocates a new AIOContinuousBuf buffer for
     * reading counts, or unsigned shorts, from the Analog board.
     *
     * @snippet burst_test.c newbuf_for_counts
     */ 
    /*[newbuf_for_counts] */
    buf = (AIOContinuousBuf *)NewAIOContinuousBufForCounts(options.index, options.num_scans, options.num_channels );
    if( !buf ) {
      fprintf(stderr,"Can't create AIOContinuousBuf \n");
      exit(1);
    }
    /*[newbuf_for_counts] */

    /**
     * @page sample_usb_ai16_16_burst_test burst_test.c
     * @tableofcontents
     * @subsection sample_usb_ai16_16_set_index Set the device index for the AIOContinuousBuf 
     * 
     * @brief The following command associates the AIOContinuousBuf
     * with a particular index. Typically you either pass in the
     * device index to the constructor ( see NewAIOContinuousBuf() ) or,
     * you can set it after the fact with this routine.
     * 
     * @snippet burst_test.c Assign the first matching device for this sample
     */
    /*[Assign the first matching device for this sample] */
    AIOContinuousBufSetDeviceIndex( buf, options.index );
    /*[Assign the first matching device for this sample] */

    if( options.reset ) {
        fprintf(stderr,"Resetting device at index %d\n",buf->DeviceIndex );
        /* AIOContinuousBufResetDevice( buf ); */
        AIOUSB_ResetChip( buf->DeviceIndex );
        exit(0);
    }
    FILE *fp = fopen(options.outfile,"w");
    if( !fp ) {
      fprintf(stderr,"Unable to open '%s' for writing\n", options.outfile );
      exit(1);
    }

    retval = ADC_SetCal(options.index, ":AUTO:");
    if ( retval < AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error setting calibration %d\n", (int)retval);
        exit(retval);
    }


    /**
     * @page sample_usb_ai16_16_burst_test burst_test.c
     * @tableofcontents
     * @subsection sample_usb_ai16_16_burst_test_init Initialize the AIOContinuousBuf  
     * @brief
     * Setup the AIOContinuousBuf 's ADCConfig object for
     * Acquisition. This is a simplified interface for an easy
     * configuration. Alternatively, you may use functionality
     * provided through ADC_SetConfig()  to set the configuration
     * registers and ADC_GetConfig() to read the configuration registers.
     *
     * @snippet burst_test.c initaiobuf
     */
    /* [initaiobuf] */
    AIOContinuousBufInitConfiguration( buf );
    /* [initaiobuf] */

    if ( options.slow_acquire ) {
        unsigned char bufData[64];
        unsigned long bytesWritten = 0;
        GenericVendorWrite( 0, 0xDF, 0x0000, 0x001E, bufData, &bytesWritten  );
    }

    AIOContinuousBufSetOversample( buf, 0 );
    AIOContinuousBufSetStartAndEndChannel( buf, options.start_channel, options.end_channel );
    if( !options.number_ranges ) { 
        AIOContinuousBufSetAllGainCodeAndDiffMode( buf , options.gain_code , AIOUSB_FALSE );
    } else {
        for ( int i = 0; i < options.number_ranges ; i ++ ) {
            AIOContinuousBufSetChannelRange( buf, 
                                             options.ranges[i]->start_channel, 
                                             options.ranges[i]->end_channel,
                                             options.ranges[i]->gaincode
                                             );
        }
    }
    AIOContinuousBufSaveConfig(buf);
    
    if ( retval < AIOUSB_SUCCESS ) {
        printf("Error setting up configuration\n");
        exit(1);
    }
  
    options.block_size = ( options.block_size < 0 ? 1024*64 : options.block_size );
    if ( options.clock_rate < 1000 ) { 
        AIOContinuousBufSetStreamingBlockSize( buf, 512 );
    } else  {
        AIOContinuousBufSetStreamingBlockSize( buf, options.block_size );
    }

    /**
     * @page sample_usb_ai16_16_burst_test burst_test.c
     * @tableofcontents
     * @subsection sample_usb_ai16_16_burst_test_setclocks Set the Clock rate / Data Acquisition speed
     * @brief
     * 
     * Setup the sampling clock rate, in this case 10000000 / 1000
     * @snippet burst_test.c setclock
     */ 
    /* [setclock] */
    AIOContinuousBufSetClock( buf, options.clock_rate );
    /* [setclock] */

    /**
     * @page sample_usb_ai16_16_burst_test burst_test.c
     * @tableofcontents
     * @subsection sample_usb_ai16_16_burst_test_startcallback Start the continuous mode callback 
     * @brief Start the Callback that fills up the
     * AIOContinuousBuf. This fires up an thread that performs the
     * acquistion, while you go about doing other things.
     * @snippet burst_test.c startcallback
     */ 
    /* [startcallback] */
    AIOContinuousBufInitiateCallbackAcquisition(buf);
    /* [startcallback] */

    /**
     * in this example we read bytes in blocks of our core num_channels parameter. 
     * the channel order
     */
#ifdef UNIX
    if ( options.with_timing ) 
        clock_gettime( CLOCK_MONOTONIC_RAW, &starttime );
#endif

    int scans_remaining;
    int read_count = 0;
    int scans_read = 0;
    
    /**
     * @page sample_usb_ai16_16_burst_test burst_test.c
     * @tableofcontents
     * @subsection sample_usb_ai16_16_burst_test_acquiredata Acquire data until completed
     * @brief This part shows how to acquire data continuously until
     * there is nore more data remaining. It makes use of the function
     * AIOContinuousBufPending() that indicates that data is still
     * available for acquisition.
     *
     * @snippet burst_test.c looping_and_waiting
     * <i>Do something with the data</i>
     * @snippet burst_test.c ended_waiting
     */
    /* [looping_and_waiting] */
    while ( AIOContinuousBufPending(buf) ) {

        if ( (scans_remaining = AIOContinuousBufCountScansAvailable(buf) ) > 0 ) { 
    /* [looping_and_waiting] */
            if ( scans_remaining ) { 

#ifdef UNIX
                if ( options.with_timing )
                    clock_gettime( CLOCK_MONOTONIC_RAW, &prevtime );
#endif
                scans_read = AIOContinuousBufReadIntegerScanCounts( buf, tobuf, tobufsize, AIOContinuousBufNumberChannels(buf)*AIOContinuousBufCountScansAvailable(buf) );

#ifdef UNIX
                if ( options.with_timing )
                    clock_gettime( CLOCK_MONOTONIC_RAW, &curtime );
#endif
                read_count += scans_read;

                if ( options.verbose )
                    fprintf(stdout,"Waiting : total=%d, readpos=%d, writepos=%d, scans_read=%d\n", (int)AIOContinuousBufGetScansRead(buf), 
                            (int)AIOContinuousBufGetReadPosition(buf), (int)AIOContinuousBufGetWritePosition(buf), scans_read);

                for( int scan_count = 0; scan_count < scans_read ; scan_count ++ ) { 
                    if( options.with_timing )
                        fprintf(fp,"%ld,%ld,%ld,", curtime.tv_sec, (( prevtime.tv_sec - starttime.tv_sec )*1000000000 + (prevtime.tv_nsec - starttime.tv_nsec )), (curtime.tv_sec-prevtime.tv_sec)*1000000000 + ( curtime.tv_nsec - prevtime.tv_nsec) );


                    for( int ch = 0 ; ch < AIOContinuousBufNumberChannels(buf); ch ++ ) {
                        fprintf(fp,"%u,",tobuf[scan_count*AIOContinuousBufNumberChannels(buf)+ch] );
                        if( (ch+1) % AIOContinuousBufNumberChannels(buf) == 0 ) {
                            fprintf(fp,"\n");
                        }
                    }
                }
            }
    /* [ended_waiting] */
        } else {
            usleep(100);
        }
    }
    /* [ended_waiting] */

    fclose(fp);
    fprintf(stdout,"Test completed...exiting\n");
    AIOUSB_Exit();
    return 0;
}

