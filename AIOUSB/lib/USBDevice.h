#ifndef _USB_DEVICE_H
#define _USB_DEVICE_H

#include <stdint.h>
#include <libusb.h>
#include <stdlib.h>
#include "ADCConfigBlock.h"
#include "AIOEither.h"

#ifdef __aiousb_cplusplus
namespace AIOUSB {
#endif

#if defined( __cplusplus) && defined(mocktesting)
#define INTERNAL_METHOD( NAME, RETVAL, ... )\
    RETVAL NAME( __VA_ARGS__ )
#else
#define INTERNAL_METHOD( NAME, RETVAL, ... )\
    RETVAL (*NAME)( __VA_ARGS__ )
#endif
typedef struct USBDevice USBDevice;

struct USBDevice { 
    INTERNAL_METHOD( usb_control_transfer , int, USBDevice *usbdev, uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout  );
    int (*usb_bulk_transfer)( USBDevice *dev_handle,
                      unsigned char endpoint, unsigned char *data, int length,
                      int *actual_length, unsigned int timeout );

    int (*usb_request)( USBDevice *usbdev, uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout );
    int (*usb_reset_device)(USBDevice *usbdev );
    int (*usb_put_config)( USBDevice *usb, ADCConfigBlock *configBlock );
    int (*usb_get_config)( USBDevice *usb, ADCConfigBlock *configBlock );



    uint8_t timeout;
    libusb_device *device;
    libusb_device_handle *deviceHandle;
    struct libusb_device_descriptor deviceDesc;
    AIOUSB_BOOL debug;
    int usblp_attached;
    int iface;
    int verbose;
    int conf;
    int origconf;
    int altset;
};

typedef struct aiousb_libusb_args {
    struct libusb_device *dev;
    struct libusb_device_handle *handle;
    struct libusb_device_descriptor *deviceDesc;
} LIBUSBArgs;

/* BEGIN AIOUSB_API */
PUBLIC_EXTERN USBDevice * NewUSBDevice(libusb_device *dev, libusb_device_handle *handle );
PUBLIC_EXTERN void DeleteUSBDevice( USBDevice *dev );
PUBLIC_EXTERN USBDevice *CopyUSBDevice( USBDevice *usb );
PUBLIC_EXTERN AIOEither InitializeUSBDevice( USBDevice *usb, LIBUSBArgs *args );
PUBLIC_EXTERN AIORET_TYPE AddAllACCESUSBDevices( libusb_device ***deviceList , USBDevice **devs , int *size );
PUBLIC_EXTERN void DeleteUSBDevices( USBDevice *devs);
PUBLIC_EXTERN int USBDeviceClose( USBDevice *dev );

PUBLIC_EXTERN int USBDeviceGetIdProduct( USBDevice *device );
PUBLIC_EXTERN int USBDeviceFetchADCConfigBlock( USBDevice *device, ADCConfigBlock *config );
PUBLIC_EXTERN int USBDevicePutADCConfigBlock( USBDevice *usb, ADCConfigBlock *configBlock );

PUBLIC_EXTERN int usb_control_transfer(USBDevice *dev_handle,
                         uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                 unsigned char *data, uint16_t wLength, unsigned int timeout);
PUBLIC_EXTERN int usb_bulk_transfer(USBDevice *dev_handle,
                              unsigned char endpoint, unsigned char *data, int length,
                              int *actual_length, unsigned int timeout);
PUBLIC_EXTERN int usb_request(USBDevice *dev_handle,
                        uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                        unsigned char *data, uint16_t wLength, unsigned int timeout);
PUBLIC_EXTERN int usb_reset_device( USBDevice *usb );

 
PUBLIC_EXTERN libusb_device_handle *get_usb_device( USBDevice *dev );
PUBLIC_EXTERN libusb_device_handle *USBDeviceGetUSBDeviceHandle( USBDevice *usb );
/* END AIOUSB_API */

#ifdef __aiousb_cplusplus
}
#endif

#endif
