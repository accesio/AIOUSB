/**
 * @file   mock_capture_usb.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  This file will allow capturing of all USB traffic, in and out
 * 
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "AIOTypes.h"
#include "USBDevice.h"
#include "AIOUSB_Core.h"
#include "AIOUSB_Log.h"
#include "USBDevice.h"
#include "libusb.h"
#include <assert.h>

#include <dlfcn.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif


typedef enum {
    IN,
    OUT
} IO_DIRECTION;

IO_DIRECTION direction;
ADCConfigBlock *KEEP = NULL;


int mock_usb_control_transfer(USBDevice *dev_handle,
                              uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                              unsigned char *data, uint16_t wLength, unsigned int timeout);

int mock_usb_bulk_transfer( USBDevice *usb,
                            unsigned char endpoint, 
                            unsigned char *data, 
                            int length,
                            int *actual_length, 
                            unsigned int timeout
                            );

int mock_usb_reset_device( USBDevice *usb );
int mock_USBDevicePutADCConfigBlock( USBDevice *usb, ADCConfigBlock *configBlock );
int mock_USBDeviceFetchADCConfigBlock( USBDevice *usb, ADCConfigBlock *configBlock );

typedef AIORET_TYPE (*add_devices_fn)( libusb_device **deviceList , USBDevice **devs , int *size );

AIORET_TYPE AddAllACCESUSBDevices( libusb_device **deviceList , USBDevice **devs , int *size ) 
{

    AIORET_TYPE result = 0;
    char *tmp;
    char delim[] = ",";
    char *pos;

    static AIORET_TYPE (*orig_AddAllACCESUSBDevices)( libusb_device **deviceList , USBDevice **devs , int *size ) = NULL;

    if (!orig_AddAllACCESUSBDevices)
        orig_AddAllACCESUSBDevices = (add_devices_fn)dlsym(RTLD_NEXT,"AddAllACCESUSBDevices");

    
    tmp = getenv((const char *)"AIO_TEST_PRODUCTS");
    if ( tmp ) {
        printf("Found envvar AIO_TEST_PRODUCTS\n");

        for ( char *token = strtok_r( tmp, delim, &pos );  token ; token = strtok_r(NULL, delim , &pos) ) {
                    *size += 1;
                    *devs = (USBDevice*)realloc( *devs, (*size )*(sizeof(USBDevice)));
                    USBDevice *usb = &(*devs)[*size-1];
                    usb->debug = AIOUSB_FALSE;
                    usb->usb_control_transfer  = mock_usb_control_transfer;
                    usb->usb_bulk_transfer     = mock_usb_bulk_transfer;
                    usb->usb_put_config        = mock_USBDevicePutADCConfigBlock;
                    usb->usb_get_config        = mock_USBDeviceFetchADCConfigBlock;  
                    usb->usb_reset_device      = mock_usb_reset_device;
                    struct libusb_device_descriptor tmp = {
                        bLength: 18,
                        bDescriptorType: 1,
                        bcdUSB: 512,
                        bDeviceClass: 0,
                        bDeviceSubClass: 0,
                        bDeviceProtocol: 0,
                        bMaxPacketSize0: 64,
                        idVendor: 5637,
                        idProduct: (uint16_t)atoi(token),
                        bcdDevice: 0,
                        iManufacturer: 1,
                        iProduct: 2,
                        iSerialNumber: 0,
                        bNumConfigurations: 1
                    };
                    memcpy( &usb->deviceDesc , &tmp, sizeof(struct libusb_device_descriptor)); 

                    result += 1;
        }
    } else {
        printf("Using original AddAllACCESUSBDevices\n");
        result = orig_AddAllACCESUSBDevices( deviceList, devs, size );
    }

    return result;
}


int mock_usb_control_transfer(USBDevice *dev_handle,
                              uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                              unsigned char *data, uint16_t wLength, unsigned int timeout)
{

    /* libusb_device_handle *handle = get_usb_device( dev_handle ); */
    printf("Mock control ");

    if ( request_type ==  USB_WRITE_TO_DEVICE ) {
        direction = OUT;
        printf(" OUT ");
    } else if ( request_type == USB_READ_FROM_DEVICE ) {
        printf(" IN ");
        direction = IN;
    }
    printf("\n");
    switch ( direction ) {
    case IN:
        switch ( bRequest ) {
        case CYPRESS_GET_DESC:
            printf("Got a cypress request\n");
            break;
        default:
            break;
        }
        break;
    case OUT:
        break;
    default:
        printf("UNknown direction!!!\n");
        break;
    }

    return wLength;
}

int mock_usb_bulk_transfer( USBDevice *usb,
                            unsigned char endpoint, 
                            unsigned char *data, 
                            int length,
                            int *actual_length, 
                            unsigned int timeout
                            )
{
    /* int libusbResult = LIBUSB_SUCCESS; */
    /* int total = 0; */
    printf("Fake mock_usb_bulk_transfer\n");
    AIO_ASSERT_USB( usb );
    AIO_ASSERT( data );
    AIO_ASSERT( actual_length );
    usleep(100);
    *actual_length = length;
    return 0;
}

int mock_USBDevicePutADCConfigBlock( USBDevice *usb, ADCConfigBlock *configBlock )
{
    if ( !KEEP ) {
        KEEP = (ADCConfigBlock*)calloc(1,sizeof(ADCConfigBlock));
        memcpy(KEEP,configBlock, sizeof( ADCConfigBlock ));
    }
    return 0;
}

int mock_USBDeviceFetchADCConfigBlock( USBDevice *usb, ADCConfigBlock *configBlock )
{
    if ( !KEEP ) {
        return -AIOUSB_ERROR_INVALID_ADCCONFIG;
    }
    memcpy( configBlock->registers, KEEP->registers, KEEP->size );

    return 0;
}
int mock_usb_reset_device( USBDevice *usb )
{    
    printf("Fake mock_usb_reset_device\n");
    return 0;
}


#ifdef __cplusplus
}
#endif
