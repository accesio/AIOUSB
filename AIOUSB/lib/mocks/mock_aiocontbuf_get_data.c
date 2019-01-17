#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <libusb.h>
#include <unistd.h>
#include <sys/stat.h>
#include "aiousb.h"

#include <dlfcn.h>



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
    /* int number_scans = datasize / ( AIOContinuousBufNumberChannels(buf)*(AIOContinuousBufGetOversample(buf)+1)*sizeof(uint16_t)); */
    int number_scans = buf->num_scans;
    *bytes = 0;
    int pos;
    static int scan_count = 0;
    static int channel_count = 0;
    static int os = 0;
    int initial = (scan_count *(AIOContinuousBufNumberChannels(buf)*(AIOContinuousBufGetOversample(buf)+1))) + 
        channel_count * ( AIOContinuousBufGetOversample(buf)+1 ) + os;

    for ( ; scan_count < number_scans; scan_count ++ ) { 
        for ( ; channel_count < AIOContinuousBufNumberChannels(buf); channel_count ++ ) {
            for ( ; os < AIOContinuousBufGetOversample(buf)+1 ; os ++ ) {
                pos = (scan_count *(AIOContinuousBufNumberChannels(buf)*(AIOContinuousBufGetOversample(buf)+1))) + 
                    channel_count * ( AIOContinuousBufGetOversample(buf)+1 ) + os - initial;
                counts[pos] =  (uint16_t)(65535 / (AIOContinuousBufNumberChannels(buf)-1)) * channel_count;
                if ( pos > 65536 /2 ) {
                    fprintf(stderr,"ERROR: pos=%d\n", pos );
                }
                *bytes += 2;

                if ( *bytes >= (datasize) ) { 
                    os ++;
                    goto end_aiocontbuf_get_bulk_data;
                }
            }
            os = 0;
        }
        channel_count = 0;
    }
 end_aiocontbuf_get_bulk_data:
    /* printf("Final value=%d\n",*bytes); */
    /* usbresult = number_scans*AIOContinuousBufNumberChannels(buf)*( AIOContinuousBufGetOversample(buf)+1 )*sizeof(uint16_t); */
    usbresult = *bytes;
    return usbresult;
}
