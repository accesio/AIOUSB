/**
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief
 * @see Compilation
 * @see CmakeCompilation
 */


#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "AIOTypes.h"



AIOUSB_BOOL find_ctr15( AIOUSBDevice *dev )
{
    if ( dev->ProductID == USB_CTR_15 )
        return AIOUSB_TRUE;
    else
        return AIOUSB_FALSE;
}

void print_usage(int argc, char **argv )
{
    printf("%s:  [--rapiddisplay | -D ]\n          -c CHANNEL_NUMBER=CLOCK_HZ [ -c CHANNEL_NUMBER=CLOCK_HZ ...]\n", argv[0] );
}

/**
 * @brief Computes the counter value that gives you the 
 *       best approximation at the frequency stated
 * 
 *
 *
 */ 
int frequency_to_counter( double frequency ) 
{
    return (int)sqrt( (double)10000000 / frequency );
}


int main(int argc, char *argv[] )
{
    int num_devices, *indices;
    int c, option_index;
    int display_counters = 0;
    double channel_frequencies[5] = {0.0};
    unsigned short counts[15] = {0};

    static struct option long_options[] = {
        {"channel"      , required_argument, 0,  'C'   },
        {"rapiddisplay" , no_argument      , 0,  'D'   },
        {"help"         , no_argument      , 0,  'H'   },
    };

    if ( argc <= 1 ) { 
        print_usage(argc, argv);
        exit(1);
    }



    /**
     * @brief Alternative ways to find your card
     * optional
     * retval = ClearDevices();
     * retval = GetDevices();
     * retval = GetDeviceSerialNumber(DeviceIndex, ref SerNum);
     * retval = QueryDeviceInfo(DeviceIndex, ref PID, ref NameSize, ref strName, out DIOBytes, out Counters);
     */
 
    /**
     * @brief 
     * Quick command line processing -C 1=
     */
    while (1) {
        c = getopt_long(argc, argv, "C:HD", long_options, &option_index);
        if ( c == -1 )
            break;

        switch (c) {
        case 'C':
            if ( !index(optarg,'=' ) ) {
                fprintf(stderr,"Must be -C CHANNEL_NUM=HZ\n");
                print_usage(argc, argv);
                exit(1);
            } 
            {
                int channel = atoi(optarg);
                char *end = strlen(optarg) + optarg;
                double hz = ( strtod(index(optarg,'=')+1, &end));
                if ( hz > 2500000.0 ) { 
                    fprintf(stderr,"Frequency for channel %d is too high, Using the maximum of  2,500,000 Hz instead\n", channel );
                    hz = 2500000.0;
                } 
                channel_frequencies[channel] = hz;
            }

            break;
        case 'D':
            display_counters = 1;
            break;
        case 'H': 
            print_usage( argc , argv );
            exit(1);
        default:
            fprintf(stderr,"Undefined option %s\n", optarg );
            break;
        }
    }

    AIOUSB_ListDevices();
    AIOUSB_FindDevices( &indices, &num_devices, find_ctr15 );
    num_devices = 1;
    if ( num_devices <= 0 ) {
        fprintf(stderr,"Unable to find a USB-CTR-16 device..exiting\n");
        exit(1);
    } else {
        printf("Using device at index %d\n", indices[0] );
    }



    /* Debug */
    for ( int i = 0; i < 5; i ++ ) {
        printf("Channel %i is set to %.2f Hz\n", i, channel_frequencies[i] );
    }


    for ( int i = 0; i < 5; i ++ ) {
        int counter_value;
        if ( channel_frequencies[i] == 0 ) { 
            counter_value = 62071;
        } else {
            counter_value = frequency_to_counter( channel_frequencies[i] );
        }
        printf("Setting channel %d counter to %d\n", i , counter_value );
            CTR_8254ModeLoad( indices[0], i, 1, 2, counter_value);
            CTR_8254ModeLoad( indices[0], i, 2, 3, counter_value);
    }

        
    if ( display_counters ) {
        int counter = 0;
        while ( counter < 100000) {
            CTR_8254ReadAll( indices[0] , &counts[0] );
            printf("%5.5hu,%5.5hu,%5.5hu   %5.5hu,%5.5hu,%5.5hu   %5.5hu,%5.5hu,%5.5hu   %5.5hu,%5.5hu,%5.5hu    %5.5hu,%5.5hu,%5.5hu",
                   counts[0], counts[1], counts[2], counts[3], counts[4], counts[5], counts[6], counts[7], counts[8], counts[9], counts[10], counts[11], counts[12], counts[13], counts[14] );               
            printf("\r");
            counter ++;
        }
        usleep(100);
        printf("\n");
    }

}

