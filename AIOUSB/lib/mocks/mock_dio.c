#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <libusb.h>
#include <unistd.h>
#include <sys/stat.h>
#include "AIOTypes.h"
#include "USBDevice.h"
#include "AIOUSB_Core.h"


#include <dlfcn.h>

typedef enum {
    IN,
    OUT
} IO_DIRECTION;


static int libusb_file_number = 0;

/*----------------------------------------------------------------------------*/
void save_results( char *prefix,
                   uint8_t request_type, 
                   uint8_t bRequest, 
                   uint16_t wValue, 
                   uint16_t wIndex,
                   unsigned char *data, 
                   uint16_t wLength, 
                   unsigned int timeout )
{
    FILE *fp = NULL;
    char *tmpstr = NULL;
    char *buf = (char *)calloc(sizeof(uint8_t)*2+sizeof(uint16_t)*2+wLength+sizeof(uint16_t),1);
    int size = 0;
    int logresults = 0;
    char *tmpprefix;
    if( !prefix ) {
        tmpprefix = "before";
    } else {
        tmpprefix = prefix;
    }

    if( !buf ) {
        fprintf(stderr,"Unable to allocate temporary buffer for");
    } else {
        size = 0;
        memcpy(&buf[size], &request_type, sizeof(uint8_t));
        size += sizeof(uint8_t);
        memcpy(&buf[size], &bRequest, sizeof(uint8_t));
        size += sizeof(uint8_t);
        memcpy(&buf[size], &wValue, sizeof(uint16_t));
        size += sizeof(uint16_t);
        memcpy(&buf[size], &wIndex, sizeof(uint16_t));
        size += sizeof(uint16_t);
        memcpy(&buf[size], data, wLength);
        size += wLength;
        memcpy(&buf[size], &wLength, sizeof(uint16_t));
        size += sizeof(uint16_t);
    }
    struct stat stats;
    /**
     * Improve this to allow the utility to make the directory in question
     * and write the results there
     */

    if( getenv("LIBUSB_DIR" ) && ( lstat(getenv("LIBUSB_DIR"), &stats ) >= 0 ) && S_ISDIR(stats.st_mode ) ) {
        asprintf(&tmpstr,"%s/%s_libusb_data_%.3d.dat",  getenv("LIBUSB_DIR"),tmpprefix,libusb_file_number );
    } else {
        asprintf(&tmpstr,"%s_libusb_data_%.3d.dat", tmpprefix, libusb_file_number );
    }
    if ( buf ) 
        fp = fopen( tmpstr, "w+" );
    if (! fp ) { 
        fprintf(stderr,"Unable to open '%s' for logging usb data\n", tmpstr );
    }
    if( fp && buf )
        fwrite(buf, size,1, fp);

    if (fp)
        fclose(fp);

}

/*----------------------------------------------------------------------------*/
int mock_usb_control_transfer(USBDevice *dev_handle,
                         uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                         unsigned char *data, uint16_t wLength, unsigned int timeout)
{
    if ( request_type == USB_READ_FROM_DEVICE ) { 
        printf("Reading\n");
    } else {
        printf("Writing\n");
    }
    printf("MockUSB Control\n");
    return wLength;
}

/*----------------------------------------------------------------------------*/
int mock_usb_bulk_transfer(USBDevice *dev_handle,
                      unsigned char endpoint, unsigned char *data, int length,
                      int *actual_length, unsigned int timeout
                      )
{
    IO_DIRECTION direction;

    if ( endpoint & LIBUSB_ENDPOINT_OUT ) {
        direction = OUT;
    } else if ( endpoint & LIBUSB_ENDPOINT_IN ) {
        direction = IN;
    }

    printf("MockUSB Bulk");
    return length;
}

/*----------------------------------------------------------------------------*/
USBDevice *_check_dio_get_device_handle( unsigned long DeviceIndex,
                                         AIOUSBDevice **device,
                                         AIORESULT *result )
{
    static  USBDevice *(*dio_get_dh_real)(unsigned long DeviceIndex,AIOUSBDevice **device,AIORESULT *result)=NULL;
    unsigned char *c;
    int port,ok=1;
    if (!dio_get_dh_real) dio_get_dh_real = dlsym(RTLD_NEXT,"_check_dio_get_device_handle");
    USBDevice *real = dio_get_dh_real( DeviceIndex, device, result );
    if ( *result == AIOUSB_SUCCESS ) {
        real->usb_control_transfer = mock_usb_control_transfer;
        real->usb_bulk_transfer    = mock_usb_bulk_transfer;
    }
    return real;
}
