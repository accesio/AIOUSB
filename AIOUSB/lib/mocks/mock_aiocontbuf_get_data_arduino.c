#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <libusb.h>
#include <unistd.h>
#include <sys/stat.h>
#include "aiousb.h"
#include "AIOTypes.h"
#include "USBDevice.h"
#include "AIOUSB_Log.h"
#include "AIOUSB_Core.h"

#include <dlfcn.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif

AIORET_TYPE aiocontbuf_get_bulk_data( AIOContinuousBuf *buf, 
                                 USBDevice *usb, 
                                 unsigned char endpoint, 
                                 unsigned char *data,
                                 int datasize,
                                 int *bytes,
                                 unsigned timeout 
                                 )
{
    AIORET_TYPE usbresult;
    uint16_t *counts = (uint16_t*)data;
    int number_scans = buf->num_scans;
    *bytes = 0;
    int pos;
    static int scan_count = 0;
    static int channel_count = 0;
    static int os = 0;
    static char arduino_counter = 0;
    double scale_down = 1;
    int noisy_ground = 30;
    int noisy_signal = 10;
    int usb_delay = 0;
    if ( getenv("AIO_USB_DELAY") ) {
        usb_delay = atoi(getenv("AIO_USB_DELAY"));
    }
    if ( getenv("MOCK_SCALE_DOWN") ) { 
        scale_down = strtod(getenv("MOCK_SCALE_DOWN"),NULL);
    } 
    if ( getenv("MOCK_NOISY_GROUND") )  {
        noisy_ground = atoi(getenv("MOCK_NOISY_GROUND"));
    }
    if ( getenv("MOCK_NOISY_SIGNAL") )  {
        noisy_signal = atoi(getenv("MOCK_NOISY_SIGNAL"));
    }

    int initial = (scan_count *(AIOContinuousBufNumberChannels(buf)*(AIOContinuousBufGetOversample(buf)+1))) + 
        channel_count * ( AIOContinuousBufGetOversample(buf)+1 ) + os;

    for ( ; scan_count < number_scans && *bytes < datasize ; scan_count ++ ) { 
        for ( ; channel_count < AIOContinuousBufNumberChannels(buf) && *bytes < datasize; channel_count ++ ) {
            for ( ; os <= AIOContinuousBufGetOversample(buf) && *bytes < datasize ; os ++ ) { 
                pos = (scan_count *(AIOContinuousBufNumberChannels(buf)*(AIOContinuousBufGetOversample(buf)+1))) + 
                    channel_count * ( AIOContinuousBufGetOversample(buf)+1 ) + os - initial;

                int tmpval;
                if ( channel_count < 3 ) { 
                    if ( channel_count == 0 ) { 
                        /* 10110101  >> 4 */
                        tmpval = ((arduino_counter) >> 4 ) & 1;
                    } else if ( channel_count == 1 ) { 
                        tmpval = ((arduino_counter) >> 5 ) & 1;
                    } else {
                        tmpval = ((arduino_counter) >> 6 ) & 1;
                    }
                    if ( tmpval == 1 ) { 
                        tmpval = (int)((65535 - ( rand() % noisy_signal )) / scale_down);
                    } else {
                        tmpval = (rand() % noisy_ground );
                    }
                } else {
                    tmpval = rand() % noisy_ground;
                }
                counts[pos] = tmpval;

                if ( pos > 65536 /2 ) {
                    fprintf(stderr,"ERROR: pos=%d\n", pos );
                }
                *bytes += 2;

            }
            os = 0;
        }
        arduino_counter ++;
        channel_count = 0;
    }

    usleep(usb_delay);

    usbresult = *bytes;
    return usbresult;
}

AIORET_TYPE adc_get_bulk_data( ADCConfigBlock *config,
                               USBDevice *usb, 
                               unsigned char endpoint, 
                               unsigned char *data,
                               int datasize,
                               int *bytes,
                               unsigned timeout 
                               )
{
    AIORET_TYPE usbresult = AIOUSB_SUCCESS;
    uint16_t *counts = (uint16_t*)data;
    *bytes = 0;
    int pos;

    double scale_down = 1;
    int noisy_ground = 30;
    int noisy_signal = 10;
    /* int usb_delay = 0; */
    static char arduino_counter = 0;
    /* if ( getenv("AIO_USB_DELAY") ) { */
    /*     usb_delay = atoi(getenv("AIO_USB_DELAY")); */
    /* } */
    if ( getenv("MOCK_SCALE_DOWN") ) { 
        scale_down = strtod(getenv("MOCK_SCALE_DOWN"),NULL);
    } 
    if ( getenv("MOCK_NOISY_GROUND") )  {
        noisy_ground = atoi(getenv("MOCK_NOISY_GROUND"));
    }
    if ( getenv("MOCK_NOISY_SIGNAL") )  {
        noisy_signal = atoi(getenv("MOCK_NOISY_SIGNAL"));
    }

    for ( int channel = ADCConfigBlockGetStartChannel( config ); 
          channel <= ADCConfigBlockGetEndChannel(config); 
          channel ++ 
          ) {
        for ( int os = 0; os <= ADCConfigBlockGetOversample(config) ; os ++ ) { 
            pos = os + channel*(1+ADCConfigBlockGetOversample(config));
            
            int tmpval;
            if ( channel < 3 ) { 
                if ( channel == 0 ) { 
                    /* 10110101  >> 4 */
                    tmpval = ((arduino_counter) >> 4 ) & 1;
                } else if ( channel == 1 ) { 
                    tmpval = ((arduino_counter) >> 5 ) & 1;
                } else {
                    tmpval = ((arduino_counter) >> 6 ) & 1;
                }
                if ( tmpval == 1 ) { 
                    tmpval = (int)((65535 - ( rand() % noisy_signal )) / scale_down);
                } else {
                    tmpval = (rand() % noisy_ground );
                }
            } else {
                tmpval = rand() % noisy_ground;
            }
            counts[pos] = tmpval;
            
            if ( pos > 65536 /2 ) {
                fprintf(stderr,"ERROR: pos=%d\n", pos );
            }
            *bytes += 2;
        }
    }
    arduino_counter ++;

    return usbresult;
}

void CloseAllDevices(void) {
    fprintf(stderr,"Closing devices\n");
}

#ifdef __cplusplus
}
#endif
