/**
 * @file   USBDevice.c
 * @author  $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 */

#include "AIOTypes.h"
#include "USBDevice.h"
#include "libusb.h"
#include "AIODeviceTable.h"
#include "AIOEither.h"

#ifdef __cplusplus
#include <iostream>
namespace AIOUSB {
#endif

/*----------------------------------------------------------------------------*/
AIOEither InitializeUSBDevice( USBDevice *usb, LIBUSBArgs *args )
{
    AIOEither retval = {0};

    AIO_ASSERT_AIOEITHER(-AIOUSB_ERROR_INVALID_USBDEVICE,"Invalid usb object", usb );
    AIO_ASSERT_AIOEITHER(-AIOUSB_ERROR_INVALID_PARAMETER,"Invalid args", args ); 

    struct libusb_device_descriptor devdesc;
    struct libusb_config_descriptor *confptr = NULL;

    int errcode;
    int verbose = usb->verbose; 
    int current, number1, number2;
    int retcode;

    usb->device                = args->dev;
    usb->deviceHandle          = args->handle;
    usb->deviceDesc            = *args->deviceDesc;

    errcode = libusb_open(  usb->device, &usb->deviceHandle );
    if ( errcode < 0 ) {
        retcode = asprintf(&retval.errmsg,"ERROR: Failed on libusb_open, code: %d\n", errcode);
        goto error;
    }

    if ((errcode = libusb_get_device_descriptor( usb->device, &devdesc)) < 0) {
        retcode = asprintf(&retval.errmsg,"ERROR: Failed to get device descriptor, code: %d\n", errcode);
        goto error;
    }

    errcode = libusb_kernel_driver_active(usb->deviceHandle, usb->iface);
    if (errcode == 0) {
        usb->usblp_attached = 0;
    } else if (errcode == 1) {
        usb->usblp_attached = 1;
        if ((errcode = libusb_detach_kernel_driver(usb->deviceHandle, usb->iface)) < 0) {
            retcode = asprintf(&retval.errmsg,"ERROR: Failed to detach \"usblp\" module from %04x:%04x\n", devdesc.idVendor, devdesc.idProduct);
            goto error;
        }
    } else {
        usb->usblp_attached = 0;
        
        if (errcode != LIBUSB_ERROR_NOT_SUPPORTED) {
            retcode = asprintf(&retval.errmsg,"ERROR: Failed to check whether %04x:%04x has the \"usblp\" ""kernel module attached\n", devdesc.idVendor, devdesc.idProduct);
          goto error;
        }
    }

    current = 0;
    if (libusb_control_transfer(usb->deviceHandle,
                                LIBUSB_REQUEST_TYPE_STANDARD | LIBUSB_ENDPOINT_IN |
                                LIBUSB_RECIPIENT_DEVICE,
                                8, /* GET_CONFIGURATION */
                                0, 0, (unsigned char *)&current, 1, 5000) < 0) {
        current = 0;			/* Assume not configured */

    }
    usb->origconf = current;

    if ((errcode = libusb_get_config_descriptor(usb->device, usb->conf, &confptr)) < 0) {
        retcode = asprintf(&retval.errmsg,"ERROR: Failed to get config descriptor for %04x:%04x\n", devdesc.idVendor, devdesc.idProduct);
        goto error;
    }
    number1 = confptr->bConfigurationValue;
    
    if (number1 != current) {
         if (verbose)
             fprintf(stderr, "DEBUG: Switching USB device configuration: %d -> %d\n",current, number1);
        if ((errcode = libusb_set_configuration(usb->deviceHandle, number1)) < 0) {
            if (errcode != LIBUSB_ERROR_BUSY) {
                retcode = asprintf(&retval.errmsg,"ERROR: Failed to set configuration %d for %04x:%04x\n",number1, devdesc.idVendor, devdesc.idProduct);
                goto error;
            }
        }
    }

    number1 = confptr->interface[usb->iface].altsetting[usb->altset].bInterfaceNumber;

    while ((errcode = libusb_claim_interface(usb->deviceHandle, number1)) < 0) {
            if (errcode != LIBUSB_ERROR_BUSY) {
                retcode = asprintf(&retval.errmsg,"ERROR: Failed to claim interface %d for %04x:%04x: %s\n",number1, devdesc.idVendor, devdesc.idProduct, strerror(errno));
                goto error;
            }
    }

    if (confptr->interface[usb->iface].num_altsetting > 1) {
        number1 = confptr->interface[usb->iface].
            altsetting[usb->altset].bInterfaceNumber;
        number2 = confptr->interface[usb->iface].
            altsetting[usb->altset].bAlternateSetting;
      
        while ((errcode = libusb_set_interface_alt_setting(usb->deviceHandle, number1, number2)) < 0) {
            if (errcode != LIBUSB_ERROR_BUSY) {
                retcode = asprintf(&retval.errmsg,"ERROR: Failed to set alternate interface %d for %04x:%04x: " "%s\n",
                         number2, devdesc.idVendor, devdesc.idProduct, strerror(errno));
                goto error;
            }
        }
    }

    libusb_free_config_descriptor(confptr);

    if (verbose)
        fputs("STATE: -connecting-to-device\n", stderr);
    
    usb->debug = AIOUSB_FALSE;
#if !defined(__cplusplus) || !defined(mocktesting)
    usb->usb_control_transfer  = usb_control_transfer;
#endif
    usb->usb_bulk_transfer     = usb_bulk_transfer;
    usb->usb_request           = usb_request;
    usb->usb_reset_device      = usb_reset_device;
    usb->usb_put_config        = USBDevicePutADCConfigBlock;
    usb->usb_get_config        = USBDeviceFetchADCConfigBlock;

    return retval;
 error:
    
    libusb_close(usb->deviceHandle);
    usb->device = NULL;
    if ( retcode < 0 ) { 
        retval.left = -AIOUSB_ERROR_INVALID_AIOEITHER_ALLOCATION;
    } else {
        retval.left = -errcode;
    }
    return retval;
}

/*----------------------------------------------------------------------------*/
USBDevice * NewUSBDevice( libusb_device *dev, libusb_device_handle *handle)
{
    USBDevice *obj = (USBDevice *)calloc(sizeof(USBDevice), 1 );
    if ( obj ) {
        LIBUSBArgs args = { dev, handle, NULL };
        InitializeUSBDevice( obj, &args ) ;
    }
    return obj;
}

/*----------------------------------------------------------------------------*/
USBDevice *CopyUSBDevice( USBDevice *usb )
{
    USBDevice *newusb = (USBDevice *)calloc(sizeof(USBDevice), 1 );
    memcpy(newusb, usb, sizeof(USBDevice));
    return newusb;
}


/*----------------------------------------------------------------------------*/
int USBDeviceClose( USBDevice *usb )
{
    AIO_ASSERT_USB(usb);
    
    libusb_close(usb->deviceHandle);
    usb->deviceHandle = NULL;

    libusb_unref_device( usb->device );

    return AIOUSB_SUCCESS;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AddAllACCESUSBDevices( libusb_device ***deviceList , USBDevice **devs , int *size )
{
    AIORET_TYPE result = AIOUSB_ERROR_DEVICE_NOT_FOUND;
    int numAccesDevices = 0;
    int numDevices = libusb_get_device_list(NULL, deviceList);
    if (numDevices > 0) {
        for ( int index = 0; index < numDevices && numAccesDevices < MAX_USB_DEVICES; index++, numAccesDevices ++) {
            struct libusb_device_descriptor libusbDeviceDesc;
            libusb_device *usb_device = (*deviceList)[ index ];

            int libusbResult = libusb_get_device_descriptor(usb_device, &libusbDeviceDesc);

            if (libusbResult == LIBUSB_SUCCESS) {
                if (libusbDeviceDesc.idVendor == ACCES_VENDOR_ID  && VALID_ENUM(ProductIDS, libusbDeviceDesc.idProduct ) 
                    ) {
                    *size += 1;
                    if (*devs)
                        *devs = (USBDevice*)realloc( *devs, (*size )*(sizeof(USBDevice)));
                    else
                        *devs = (USBDevice*)malloc((*size )*(sizeof(USBDevice)));
                    memset(&(*devs)[*size-1],0,sizeof(USBDevice));
                    LIBUSBArgs args = { libusb_ref_device(usb_device), NULL, &libusbDeviceDesc };
                    AIOEither usbretval = InitializeUSBDevice( &( *devs)[*size-1] , &args );
                    if ( AIOEitherHasError( &usbretval ) )
                        return -AIOUSB_ERROR_USB_INIT;
                    result = AIOUSB_SUCCESS;
                }
            }
        }
    }
    return (AIORET_TYPE)result;
}


/*----------------------------------------------------------------------------*/
 AIORET_TYPE AddDevice( int *size , int index, libusb_device **deviceList, USBDevice **devs , struct libusb_device_descriptor *libusbDeviceDesc )
{
    AIORET_TYPE retval  = AIOUSB_SUCCESS;
    libusb_device *usb_device = deviceList[ index ];
    *size += 1;
    *devs = (USBDevice*)realloc( *devs, (*size )*(sizeof(USBDevice)));
    LIBUSBArgs args = { libusb_ref_device(usb_device), NULL, libusbDeviceDesc };
    AIOEither usbretval = InitializeUSBDevice( &( *devs)[*size-1] , &args );
    if ( AIOEitherHasError( &usbretval ) )
        return -AIOUSB_ERROR_USB_INIT;
    retval = (AIORET_TYPE)1;
    return retval;
}

/*----------------------------------------------------------------------------*/
int USBDeviceGetIdProduct( USBDevice *device )
{
    AIO_ASSERT_USB(device);

    return (int)device->deviceDesc.idProduct;
}

/*----------------------------------------------------------------------------*/
void DeleteUSBDevices( USBDevice *devices )
{
    AIO_ASSERT_NO_RETURN( devices );

    free(devices);
    libusb_exit(NULL);
}

/*----------------------------------------------------------------------------*/
void DeleteUSBDevice( USBDevice *dev )
{
    AIO_ASSERT_NO_RETURN(dev);

    free(dev);
}

/*----------------------------------------------------------------------------*/
int USBDeviceSetDebug( USBDevice *usb, AIOUSB_BOOL debug )
{
    AIO_ASSERT_USB(usb);

    usb->debug = debug;
    return AIOUSB_SUCCESS;
}

/*----------------------------------------------------------------------------*/
libusb_device_handle *USBDeviceGetUSBDeviceHandle( USBDevice *usb )
{
    if( !usb )
        return NULL;
    return usb->deviceHandle;
}

/*----------------------------------------------------------------------------*/
libusb_device_handle *get_usb_device( USBDevice *dev )
{
    
    if ( !dev ) 
        return NULL;
    return dev->deviceHandle;
}

/*----------------------------------------------------------------------------*/
int USBDeviceFetchADCConfigBlock( USBDevice *usb, ADCConfigBlock *configBlock )
{
    int result = AIOUSB_SUCCESS;

    ADCConfigBlock config;

    AIO_ASSERT_CONFIG(configBlock);
    AIO_ASSERT_USB(usb);

    memcpy( &config, configBlock, sizeof( ADCConfigBlock ));

    if( configBlock->testing != AIOUSB_TRUE ) {
        int bytesTransferred = usb->usb_control_transfer( usb, 
                                                          USB_READ_FROM_DEVICE,
                                                          AUR_ADC_GET_CONFIG,
                                                          0,
                                                          0,
                                                          config.registers,
                                                          config.size,
                                                          config.timeout
                                                          );
        
        if ( bytesTransferred != ( int ) config.size)
            result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
        else
            result = ADCConfigBlockCopy( configBlock, &config );
    } else {
        result = configBlock->size;
    }

    return result;
}

/*----------------------------------------------------------------------------*/
int USBDevicePutADCConfigBlock( USBDevice *usb, ADCConfigBlock *configBlock )
{
    int retval;

    AIO_ASSERT_USB(usb);
    AIO_ASSERT_CONFIG( configBlock );

    if( configBlock->testing == AIOUSB_TRUE ) {
        retval = (int)configBlock->size;
    } else {
        int bytesTransferred = usb->usb_control_transfer( usb, 
                                                          USB_WRITE_TO_DEVICE,
                                                          AUR_ADC_SET_CONFIG,
                                                          0,
                                                          0,
                                                          configBlock->registers,
                                                          configBlock->size,
                                                          configBlock->timeout
                                                          );
        if ( bytesTransferred != (int)configBlock->size ) {
            retval = -LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
        } else {
            retval = bytesTransferred;
        }
    }
    return retval;
}

#if defined(__cplusplus) && defined(mocktesting)
int 
USBDevice::usb_control_transfer(USBDevice *dev_handle,
                                uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                unsigned char *data, uint16_t wLength, unsigned int timeout)
#else
int usb_control_transfer(USBDevice *dev_handle,
                         uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                         unsigned char *data, uint16_t wLength, unsigned int timeout)
#endif
{

    libusb_device_handle *handle = get_usb_device( dev_handle );
    AIO_ERROR_VALID_DATA(-AIOUSB_ERROR_INVALID_LIBUSB_DEVICE_HANDLE, handle );

    return libusb_control_transfer( handle,
                                    request_type,
                                    bRequest,
                                    wValue,
                                    wIndex,
                                    data,
                                    wLength, 
                                    timeout
                                    );

}

/*----------------------------------------------------------------------------*/
/**
 * @details This function is intended to improve upon
 * libusb_bulk_transfer() by receiving or transmitting packets until
 * the entire transfer request has been satisfied; it intentionally
 * restarts the timeout each time a packet is received, so the timeout
 * parameter specifies the longest permitted delay between packets,
 * not the total time to complete the transfer request
 */
int usb_bulk_transfer( USBDevice *usb,
                       unsigned char endpoint, 
                       unsigned char *data, 
                       int length,
                       int *actual_length, 
                       unsigned int timeout
                      )
{
    int libusbResult = LIBUSB_SUCCESS;
    int total = 0;

    AIO_ASSERT_USB( usb );
    AIO_ASSERT( data );
    AIO_ASSERT( actual_length );

    libusb_device_handle *handle = get_usb_device( usb );
    AIO_ERROR_VALID_DATA(-AIOUSB_ERROR_INVALID_LIBUSB_DEVICE_HANDLE, handle );

    while (length > 0) {
          int bytes;
          libusbResult = libusb_bulk_transfer( handle , 
                                               endpoint, 
                                               data, 
                                               length, 
                                               &bytes, 
                                               timeout
                                               );
          if (libusbResult == LIBUSB_SUCCESS) {
              if(bytes > 0) {
                  total += bytes;
                  data += bytes;
                  length -= bytes;
              }
          } else if(libusbResult == LIBUSB_ERROR_TIMEOUT) {
            /**
             * @note even if we get a timeout, some data may have been
             * transferred; if so, then this timeout is not an error;
             * if we get a timeout and no data was transferred, then
             * treat it as an error condition
             */
              if (bytes > 0) {
                  total += bytes;
                  data += bytes;
                  length -= bytes;
              } else
                  break;
          } else
              break;
    }
    *actual_length = total;
    return libusbResult;
}

/*----------------------------------------------------------------------------*/
int usb_request(USBDevice *dev_handle,
                        uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                        unsigned char *data, uint16_t wLength, unsigned int timeout)
{
    return 1;
}

/*----------------------------------------------------------------------------*/
int usb_reset_device( USBDevice *usb )
{
    AIO_ASSERT_USB( usb );

    int libusbResult = libusb_reset_device( usb->deviceHandle  );
    return libusbResult;
}



#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST
/**
 * @brief Self test for verifying basic functionality of the AIOChannelMask interface
 */ 

#include "gtest/gtest.h"

using namespace AIOUSB;

TEST(USBDevice,FailsCorrectly)
{
    unsigned char endpoint;
    unsigned char *data = 0;
    int length;
    int *actual_length = 0;
    unsigned int timeout;
    USBDevice *usb = NULL; 
    ASSERT_DEATH( { USBDeviceClose(usb); }, "Assertion `usb' failed.");
    ASSERT_DEATH( { usb_bulk_transfer(usb,endpoint,data,length,actual_length,timeout); }, "Assertion `usb' failed.");
    usb = (USBDevice *)42;
    ASSERT_DEATH( { usb_bulk_transfer(usb,endpoint,data,length,actual_length,timeout); }, "Assertion `data' failed");
    data = (unsigned char *)42;
    ASSERT_DEATH( { usb_bulk_transfer(usb,endpoint,data,length,actual_length,timeout); }, "Assertion `actual_length' failed");

    usb = NULL;
    LIBUSBArgs *args = NULL;
    ASSERT_DEATH( { InitializeUSBDevice(usb, args); } , "Assertion `usb' failed" );
    usb = (USBDevice*)42;
    ASSERT_DEATH( { InitializeUSBDevice(usb, args); } , "Assertion `args' failed" );
}

int main(int argc, char *argv[] )
{
  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
  return RUN_ALL_TESTS();  

}

#endif

