/**
 * @file   AIOUSB_Core.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  General header files for the AIOUSB library
 *
 */
#include "AIOUSB_Version.h"
#include "ADCConfigBlock.h"
#include "AIOUSB_Core.h"
#include "AIODeviceTable.h"
#include "AIOUSB_ADC.h"
#include "AIOTypes.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libusb.h>

#ifdef BACKTRACE
#include <execinfo.h>

#define BACKTRACE_DEBUG(n) do { \
          int j, nptrs; \
          void *buffer[100]; \
          char **strings; \
          nptrs = backtrace(buffer, 100); \
          nptrs = (nptrs > n ? n : nptrs); \
          strings = backtrace_symbols(buffer, nptrs); \
          for(j = 0; j < nptrs; j++) \
              printf("%s\n", strings[j]); \
  } while(0);
#else
#define BACKTRACE_DEBUG(n) {}
#endif


#ifdef __cplusplus
namespace AIOUSB {
#endif

int aio_errno;

ProductIDName productIDNameTable[] = {
    { USB_DA12_8A_REV_A , "USB-DA12-8A-A"  },
    { USB_DA12_8A       , "USB-DA12-8A"    },
    { USB_DA12_8E       , "USB-DA12-8E"    },
    { USB_DIO_32        , "USB-DIO-32"     },
    { USB_DIO_32I       , "USB-DIO-32I"     },
    { USB_DIO_48        , "USB-DIO-48"     },
    { USB_DIO_96        , "USB-DIO-96"     },
    { USB_DI16A_REV_A1  , "USB-DI16A-A1"   },
    { USB_DO16A_REV_A1  , "USB-DO16A-A1"   },
    { USB_DI16A_REV_A2  , "USB-DI16A-A2"   },
    { USB_DIO_16H       , "USB-DIO-16H"    },
    { USB_DI16A         , "USB-DI16A"      },
    { USB_DO16A         , "USB-DO16A"      },
    { USB_DIO_16A       , "USB-DIO-16A"    },
    { USB_IIRO_16       , "USB-IIRO-16"    },
    { USB_II_16         , "USB-II-16"      },
    { USB_RO_16         , "USB-RO-16"      },
    { USB_IIRO_8        , "USB-IIRO-8"     },
    { USB_II_8          , "USB-II-8"       },
    { USB_IIRO_4        , "USB-IIRO-4"     },
    { USB_IDIO_16       , "USB-IDIO-16"    },
    { USB_II_16_OLD     , "USB-II-16-OLD"  },
    { USB_IDO_16        , "USB-IDO-16"     },
    { USB_IDIO_8        , "USB-IDIO-8"     },
    { USB_II_8_OLD      , "USB-II-8-OLD"   },
    { USB_IDIO_4        , "USB-IDIO-4"     },
    { USB_CTR_15        , "USB-CTR-15"     },
    { USB_IIRO4_2SM     , "USB-IIRO4-2SM"  },
    { USB_IIRO4_COM     , "USB-IIRO4-COM"  },
    { USB_DIO16RO8      , "USB-DIO16RO8"   },
    { PICO_DIO16RO8     , "PICO-DIO16RO8"  },
    { USBP_II8IDO4A     , "USBP-II8IDO4A"  },
    { USB_AI16_16F      , "USB-AI16-16F"   },
    { USB_AI16_16A      , "USB-AI16-16A"   },
    { USB_AI16_16E      , "USB-AI16-16E"   },
    { USB_AI12_16A      , "USB-AI12-16A"   },
    { USB_AI12_16       , "USB-AI12-16"    },
    { USB_AI12_16E      , "USB-AI12-16E"   },
    { USB_AI16_64MA     , "USB-AI16-64MA"  },
    { USB_AI16_64ME     , "USB-AI16-64ME"  },
    { USB_AI12_64MA     , "USB-AI12-64MA"  },
    { USB_AI12_64M      , "USB-AI12-64M"   },
    { USB_AI12_64ME     , "USB-AI12-64ME"  },
    { USB_AI16_32A      , "USB-AI16-32A"   },
    { USB_AI16_32E      , "USB-AI16-32E"   },
    { USB_AI12_32A      , "USB-AI12-32A"   },
    { USB_AI12_32       , "USB-AI12-32"    },
    { USB_AI12_32E      , "USB-AI12-32E"   },
    { USB_AI16_64A      , "USB-AI16-64A"   },
    { USB_AI16_64E      , "USB-AI16-64E"   },
    { USB_AI12_64A      , "USB-AI12-64A"   },
    { USB_AI12_64       , "USB-AI12-64"    },
    { USB_AI12_64E      , "USB-AI12-64E"   },
    { USB_AI16_96A      , "USB-AI16-96A"   },
    { USB_AI16_96E      , "USB-AI16-96E"   },
    { USB_AI12_96A      , "USB-AI12-96A"   },
    { USB_AI12_96       , "USB-AI12-96"    },
    { USB_AI12_96E      , "USB-AI12-96E"   },
    { USB_AI16_128A     , "USB-AI16-128A"  },
    { USB_AI16_128E     , "USB-AI16-128E"  },
    { USB_AI12_128A     , "USB-AI12-128A"  },
    { USB_AI12_128      , "USB-AI12-128"   },
    { USB_AI12_128E     , "USB-AI12-128E"  },
    { USB_AO16_16A      , "USB-AO16-16A"   },
    { USB_AO16_16       , "USB-AO16-16"    },
    { USB_AO16_12A      , "USB-AO16-12A"   },
    { USB_AO16_12       , "USB-AO16-12"    },
    { USB_AO16_8A       , "USB-AO16-8A"    },
    { USB_AO16_8        , "USB-AO16-8"     },
    { USB_AO16_4A       , "USB-AO16-4A"    },
    { USB_AO16_4        , "USB-AO16-4"     },
    { USB_AO12_16A      , "USB-AO12-16A"   },
    { USB_AO12_16       , "USB-AO12-16"    },
    { USB_AO12_12A      , "USB-AO12-12A"   },
    { USB_AO12_12       , "USB-AO12-12"    },
    { USB_AO12_8A       , "USB-AO12-8A"    },
    { USB_AO12_8        , "USB-AO12-8"     },
    { USB_AO12_4A       , "USB-AO12-4A"    },
    { USB_AO12_4        , "USB-AO12-4"     },
    { USB_AIO16_16F     , "USB-AIO16-16F"  },
    { USB_AIO16_16A     , "USB-AIO16-16A"  },
    { USB_AIO16_16E     , "USB-AIO16-16E"  },
    { USB_AIO12_16A     , "USB-AIO12-16A"  },
    { USB_AIO12_16      , "USB-AIO12-16"   },
    { USB_AIO12_16E     , "USB-AIO12-16E"  },
    { USB_AIO16_64MA    , "USB-AIO16-64MA" },
    { USB_AIO16_64ME    , "USB-AIO16-64ME" },
    { USB_AIO12_64MA    , "USB-AIO12-64MA" },
    { USB_AIO12_64M     , "USB-AIO12-64M"  },
    { USB_AIO12_64ME    , "USB-AIO12-64ME" },
    { USB_AIO16_32A     , "USB-AIO16-32A"  },
    { USB_AIO16_32E     , "USB-AIO16-32E"  },
    { USB_AIO12_32A     , "USB-AIO12-32A"  },
    { USB_AIO12_32      , "USB-AIO12-32"   },
    { USB_AIO12_32E     , "USB-AIO12-32E"  },
    { USB_AIO16_64A     , "USB-AIO16-64A"  },
    { USB_AIO16_64E     , "USB-AIO16-64E"  },
    { USB_AIO12_64A     , "USB-AIO12-64A"  },
    { USB_AIO12_64      , "USB-AIO12-64"   },
    { USB_AIO12_64E     , "USB-AIO12-64E"  },
    { USB_AIO16_96A     , "USB-AIO16-96A"  },
    { USB_AIO16_96E     , "USB-AIO16-96E"  },
    { USB_AIO12_96A     , "USB-AIO12-96A"  },
    { USB_AIO12_96      , "USB-AIO12-96"   },
    { USB_AIO12_96E     , "USB-AIO12-96E"  },
    { USB_AIO16_128A    , "USB-AIO16-128A" },
    { USB_AIO16_128E    , "USB-AIO16-128E" },
    { USB_AIO12_128A    , "USB-AIO12-128A" },
    { USB_AIO12_128     , "USB-AIO12-128"  },
    { USB_AIO12_128E    , "USB-AIO12-128E" }
};

#define AIOUSB_ENABLE_MUTEX


#if defined(AIOUSB_ENABLE_MUTEX)
static pthread_mutex_t aiousbMutex;
#endif



/**
 * @brief
 * Notes on mutual exclusion / threading:
 * - Our mutual exclusion scheme is _not_ intended to be bulletproof. It's primarily intended
 *   to ensure mutually exclusive access to deviceTable[] and other global variables. It does
 *   NOT ensure mutually exclusive access to the USB bus. In fact, we want to permit threads
 *   to communicate with multiple devices simultaneously, to the extent possible with USB.
 *
 * - Nor does this scheme prevent multiple threads from altering the configuration of the same
 *   device or communicating with the same device. In other words, it's entirely possible for
 *   one thread to configure and communicate with a device, only to have another thread come
 *   along and to the same. It's up to the users of this library to ensure that such a scenario
 *   doesn't occur.
 *
 * - This library does seek to permit one thread to control one device, and another thread to
 *   control another device. Each thread may then safely communicate with its own device and
 *   alter the portion of deviceTable[] that pertains to its device.
 *
 * - Our mutual exclusion scheme also permits two threads to cooperate in the operation of a
 *   single device, such as in cases where a background thread does the actual work and the
 *   foreground thread monitors the progress. In such a case, the background thread might update
 *   a status variable which the foreground thread monitors. This form of resource sharing is
 *   supported by our mutual exclusion scheme.
 */

AIOUSB_BOOL AIOUSB_Lock() {
    assert(AIOUSB_IsInit());
#if defined(AIOUSB_ENABLE_MUTEX)
    return(pthread_mutex_lock(&aiousbMutex) == 0);
#else
    return AIOUSB_TRUE;
#endif
}

AIOUSB_BOOL AIOUSB_UnLock() {
    assert(AIOUSB_IsInit());
#if defined(AIOUSB_ENABLE_MUTEX)
    return(pthread_mutex_unlock(&aiousbMutex) == 0);
#else
    return AIOUSB_TRUE;
#endif
}

PUBLIC_EXTERN AIORET_TYPE AIOUSB_ResetChip( unsigned long DeviceIndex )
{
    unsigned char data[2] = {0x01};
    int usbval;
    AIORET_TYPE retval;
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, (AIORESULT*)&retval );
    AIO_ERROR_VALID_AIORET_TYPE( retval, retval == AIOUSB_SUCCESS);

    usbval = usb->usb_control_transfer(usb, 0x40, 0xA0, 0xE600, 0 , data, 1, usb->timeout );
    data[0] = 0;

    usbval = usb->usb_control_transfer(usb, 0x40, 0xA0, 0xE600, 0 , data, 1, usb->timeout );
    retval = (AIORET_TYPE )usbval;

    return retval;


}


unsigned long AIOUSB_Validate_Lock(unsigned long *DeviceIndex)
{
    unsigned long result = (unsigned long)AIOUSB_SUCCESS;
    assert(DeviceIndex != 0);

    if(*DeviceIndex == diFirst) {
        /*
         * find first device on bus
         */
        result = AIOUSB_ERROR_FILE_NOT_FOUND;
        int index;
        for(index = 0; index < MAX_USB_DEVICES; index++) {
            if(deviceTable[ index ].usb_device != NULL) {
                *DeviceIndex = index;
                result = AIOUSB_SUCCESS;
                break;                                              // from for()
            }
        }
    } else if (*DeviceIndex == diOnly) {
        /*
         * find first device on bus, ensuring that it's the only device
         */
        result = AIOUSB_ERROR_FILE_NOT_FOUND;
        int index;
        for(index = 0; index < MAX_USB_DEVICES; index++) {
            if(deviceTable[ index ].usb_device != NULL) {
                if(result != AIOUSB_SUCCESS) {
                    /*
                     * this is the first device found; save this index, but
                     * keep checking to see that this is the only device
                     */
                    *DeviceIndex = index;
                    result = AIOUSB_SUCCESS;
                } else {
                    /*
                     * there are multiple devices on the bus
                     */
                    result = AIOUSB_ERROR_DUP_NAME;
                    break;
                }
            }
        }
    } else {
        if(
           *DeviceIndex < MAX_USB_DEVICES &&
           deviceTable[ *DeviceIndex ].usb_device != NULL
           )
            result = AIOUSB_SUCCESS;
        else
            result = AIOUSB_ERROR_INVALID_INDEX;
    }

    return result;
}

unsigned long AIOUSB_Validate(unsigned long *DeviceIndex)
{
    unsigned long result = AIOUSB_ERROR_INVALID_INDEX;

    if(*DeviceIndex == diFirst) {
      /*
       * find first device on bus
       */
        result = AIOUSB_ERROR_FILE_NOT_FOUND;
        int index;
        for(index = 0; index < MAX_USB_DEVICES; index++) {
            if(deviceTable[ index ].usb_device != NULL) {
                *DeviceIndex = index;
                result = AIOUSB_SUCCESS;
                break;                                                      // from for()
            }
        }
    } else if(*DeviceIndex == diOnly) {
      /*
       * find first device on bus, ensuring that it's the only device
       */
          result = AIOUSB_ERROR_FILE_NOT_FOUND;
          for(int index = 0; index < MAX_USB_DEVICES; index++) {
                if(deviceTable[ index ].usb_device != NULL) {
                      if(result != AIOUSB_SUCCESS) { /* found a device */
                        /*
                         * this is the first device found; save this index, but
                         * keep checking to see that this is the only device
                         */
                        *DeviceIndex = index;
                        result = AIOUSB_SUCCESS;
                      } else {
                        /*
                         * there are multiple devices on the bus
                         */
                            result = AIOUSB_ERROR_DUP_NAME;
                            break;
                        }
                }
          }
    } else {
      /*
       * simply verify that the supplied index is valid
       */
      if(
         *DeviceIndex < MAX_USB_DEVICES &&
         deviceTable[ *DeviceIndex ].usb_device != NULL
         )
        result = AIOUSB_SUCCESS;
      else
        result = AIOUSB_ERROR_INVALID_INDEX;
    }

    return result;
}

unsigned long ResolveDeviceIndex(unsigned long DeviceIndex) {
    return (AIOUSB_Validate(&DeviceIndex) == AIOUSB_SUCCESS)
           ? DeviceIndex
           : diNone;
}

/*------------------------------------------------------------------------*/
DeviceDescriptor *DeviceTableAtIndex( unsigned long DeviceIndex ) {
    AIOUSB_Validate( &DeviceIndex  );

    DeviceDescriptor * deviceDesc = &deviceTable[ DeviceIndex ];

    return deviceDesc;
}

/*------------------------------------------------------------------------*/
/**
 * @todo Replace AIOUSB_Lock() with thread safe lock on a per device index basis
 * @todo Insert correct error messages into global error string in case of failure
 */
DeviceDescriptor *DeviceTableAtIndex_Lock( unsigned long DeviceIndex )
{
    unsigned long result = AIOUSB_Validate( &DeviceIndex  );
    if ( result != AIOUSB_SUCCESS ) {
        return NULL;
    }
    DeviceDescriptor * deviceDesc = &deviceTable[ DeviceIndex ];

    return deviceDesc;
}



/**
 *
 */
DeviceDescriptor *AIOUSB_GetDevice( unsigned long DeviceIndex )
{
    AIORESULT result = AIOUSB_Validate(&DeviceIndex);

    if(result != AIOUSB_SUCCESS) {
        return NULL;
    }

    DeviceDescriptor *deviceDesc = &deviceTable[ DeviceIndex ];

    return deviceDesc;
}

/**
 *
 */
ADConfigBlock *AIOUSB_GetConfigBlock( DeviceDescriptor *dev)
{
    if ( !dev ) {
        return NULL;
    } else {
        return &dev->cachedConfigBlock;
    }
}

/*------------------------------------------------------------------------*/
/**
 * @brief This function is deprecated.
 * @param DeviceIndex
 * @return 0 or greater if the blocksize is correct, negative number on
 *              error
 */
long AIOUSB_GetStreamingBlockSize(unsigned long DeviceIndex) {
    long BlockSize;

    unsigned long result = AIOUSB_Validate(&DeviceIndex);
    if (result != AIOUSB_SUCCESS)
          return -result;


    DeviceDescriptor *deviceDesc = &deviceTable[ DeviceIndex ];
    if(deviceDesc->bADCStream || deviceDesc->bDIOStream)
        BlockSize = deviceDesc->StreamingBlockSize;
    else
        BlockSize = -AIOUSB_ERROR_NOT_SUPPORTED;

    return BlockSize;
}

unsigned long AIOUSB_SetStreamingBlockSize(
                             unsigned long DeviceIndex,
                             unsigned long BlockSize
                             ) {

     unsigned long result = AIOUSB_Validate(&DeviceIndex);
     if(result != AIOUSB_SUCCESS)
          return result;

     DeviceDescriptor * deviceDesc = &deviceTable[ DeviceIndex ];

     BlockSize = ( BlockSize < 1024*64 ? 1024 * 64 : BlockSize );

     if (deviceDesc->bADCStream) {
          if ((BlockSize & 0x1FF) != 0)
               BlockSize = (BlockSize & 0xFFFFFE00ul) + 0x200;
          deviceDesc->StreamingBlockSize = BlockSize;
     } else if(deviceDesc->bDIOStream) {
          if((BlockSize & 0xFF) != 0)
               BlockSize = (BlockSize & 0xFFFFFF00ul) + 0x100;
          deviceDesc->StreamingBlockSize = BlockSize;
     } else
          result = AIOUSB_ERROR_NOT_SUPPORTED;

     return result;
}

unsigned long AIOUSB_ClearFIFO(
                               unsigned long DeviceIndex,
                               FIFO_Method Method
                               )
{
    AIORESULT result = AIOUSB_SUCCESS;

    if(!VALID_ENUM(FIFO_Method, Method))
        return AIOUSB_ERROR_INVALID_PARAMETER;

    DeviceDescriptor *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    /* translate method into vendor request message */
    int request;
    switch(Method) {

    case CLEAR_FIFO_METHOD_AUTO:
        request = AUR_GEN_CLEAR_FIFO_NEXT;
        break;

    case CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT:
        request = AUR_GEN_ABORT_AND_CLEAR;
        break;

    case CLEAR_FIFO_METHOD_WAIT:
        request = AUR_GEN_CLEAR_FIFO_WAIT;
        break;

    default:
        request = AUR_GEN_CLEAR_FIFO;
        break;

    }

    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     request,
                                                     0,
                                                     0,
                                                     0,
                                                     0,
                                                     deviceDesc->commTimeout
                                                     );
    if(bytesTransferred != 0)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    return result;
}


const char *AIOUSB_GetVersion() {
    return VERSION_TAG;
}

const char *AIOUSB_GetVersionDate() {
    return VERSION_DATE;
}

AIORESULT AIOUSB_GetMiscClock(
                              unsigned long DeviceIndex
                              )
{
    double clockHz = 0;                                                         // return reasonable value on error

    if( AIOUSB_Validate(&DeviceIndex) == AIOUSB_SUCCESS )
        clockHz = deviceTable[ DeviceIndex ].miscClockHz;
    return clockHz;
}

AIORESULT AIOUSB_SetMiscClock(
                              unsigned long DeviceIndex,
                              double clockHz
                              )
{
    if(clockHz <= 0)
        return AIOUSB_ERROR_INVALID_PARAMETER;

    unsigned long result = AIOUSB_Validate(&DeviceIndex);
    if(result == AIOUSB_SUCCESS)
        deviceTable[ DeviceIndex ].miscClockHz = clockHz;

    return result;
}

unsigned AIOUSB_GetCommTimeout(
                               unsigned long DeviceIndex
                               ) {
    unsigned timeout = 1000;

    if (AIOUSB_Validate(&DeviceIndex) == AIOUSB_SUCCESS)
        timeout = deviceTable[ DeviceIndex ].commTimeout;

    return timeout;
}

unsigned long AIOUSB_SetCommTimeout(
                                                  unsigned long DeviceIndex,
                                                  unsigned timeout
                                    )
{

    unsigned long result = AIOUSB_Validate(&DeviceIndex);
    if(result == AIOUSB_SUCCESS)
        deviceTable[ DeviceIndex ].commTimeout = timeout;

    return result;
}

unsigned long AIOUSB_Validate_Device(unsigned long DeviceIndex) {
    unsigned long result;
    DeviceDescriptor *const deviceDesc = &deviceTable[ DeviceIndex ];

    result = AIOUSB_Validate(&DeviceIndex);
    if (result != AIOUSB_SUCCESS)
        goto RETURN_AIOUSB_Validate_Device;

    if (deviceDesc->bADCStream == AIOUSB_FALSE) {
        result = AIOUSB_ERROR_NOT_SUPPORTED;
        goto RETURN_AIOUSB_Validate_Device;
    }

    result = ADC_QueryCal(DeviceIndex);

RETURN_AIOUSB_Validate_Device:
    return result;
}


/*------------------------------------------------------------------------*/
/**
 * @param config
 * @param DeviceIndex
 * @param defaults
 */
AIORESULT AIOUSB_InitConfigBlock(ADConfigBlock *config, unsigned long DeviceIndex, AIOUSB_BOOL defaults)
{
    assert(config);
    if ( !config )
        return -AIOUSB_ERROR_INVALID_PARAMETER;
    /*
     * mark as uninitialized unless this function succeeds
     */
    config->device = 0;
    config->size = 0;

    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    ADCConfigBlockInitializeFromAIOUSBDevice( config , deviceDesc );

    return result;
}

/*------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param fileName
 * @return
 */
unsigned long AIOUSB_ADC_LoadCalTable(
                                      unsigned long DeviceIndex,
                                      const char *fileName
                                      )
{
    if(fileName == 0)
        return AIOUSB_ERROR_INVALID_PARAMETER;

    unsigned long result = AIOUSB_Validate(&DeviceIndex);

    DeviceDescriptor *const deviceDesc = &deviceTable[ DeviceIndex ];
    if (deviceDesc->bADCStream == AIOUSB_FALSE)
        return AIOUSB_ERROR_NOT_SUPPORTED;

    if((result = ADC_QueryCal(DeviceIndex)) != AIOUSB_SUCCESS)
          return result;

    unsigned short *const calTable = ( unsigned short* )malloc(CAL_TABLE_WORDS * sizeof(unsigned short));
    assert(calTable != 0);
    if(calTable != 0) {
          struct stat fileInfo;
          if(stat(fileName, &fileInfo) == 0) {
                if(fileInfo.st_size == CAL_TABLE_WORDS * sizeof(unsigned short)) {
                      FILE *const calFile = fopen(fileName, "r");
                      if(calFile != NULL) {
                            const size_t wordsRead = fread(calTable, sizeof(unsigned short), CAL_TABLE_WORDS, calFile);
                            fclose(calFile);
                            if(wordsRead == ( size_t )CAL_TABLE_WORDS)
                                result = AIOUSB_ADC_SetCalTable(DeviceIndex, calTable);
                            else
                                result = AIOUSB_ERROR_FILE_NOT_FOUND;
                        }else
                          result = AIOUSB_ERROR_FILE_NOT_FOUND;
                  }else
                    result = AIOUSB_ERROR_INVALID_DATA;              // file size incorrect
            }else
              result = AIOUSB_ERROR_FILE_NOT_FOUND;
          free(calTable);
      }else
        result = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;

    return result;
}
/*------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param calTable
 * @return
 */
unsigned long AIOUSB_ADC_SetCalTable(
                                     unsigned long DeviceIndex,
                                     const unsigned short calTable[]
                                     )
{
    if(calTable == 0)
        return AIOUSB_ERROR_INVALID_PARAMETER;
    unsigned long result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    int bytesTransferred = 0;

    /*
     * send calibration table to SRAM one block at a time; according to
     * the documentation, the proper procedure is to bulk transfer a block
     * of calibration data to "endpoint 2" and then send a control message
     * to load it into the SRAM
     */

    int SRAM_BLOCK_WORDS = 1024;
    int sramAddress = 0;
    int wordsRemaining = 65536;

    while(wordsRemaining > 0) {
        int num_to_write = (wordsRemaining < SRAM_BLOCK_WORDS) ? wordsRemaining : 1024 ;
        int libusbResult = usb->usb_bulk_transfer(usb,
                                                  0x2,
                                                  (unsigned char *)(calTable + sramAddress),
                                                  num_to_write*sizeof(unsigned short),
                                                  &bytesTransferred,
                                                  deviceDesc->commTimeout
                                                  );
        if(libusbResult != LIBUSB_SUCCESS) {
            result = LIBUSB_RESULT_TO_AIOUSB_RESULT(libusbResult);
            break;
        } else if(bytesTransferred != ( int )(num_to_write * sizeof(unsigned short))) {
            result = AIOUSB_ERROR_INVALID_DATA;
            break;
        } else {
            bytesTransferred = usb->usb_control_transfer( usb,
                                                          0x40,
                                                          0xBB,
                                                          sramAddress,
                                                          0x400,
                                                          NULL,
                                                          0,
                                                          deviceDesc->commTimeout );

            if (bytesTransferred != 0) {
                result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
                break;
            }
        }
        wordsRemaining -= num_to_write;
        sramAddress += num_to_write;
    }

    return result;
}




/**
 * @brief Performs a generic vendor USB write
 */
unsigned long GenericVendorWrite(
                                 unsigned long deviceIndex,
                                 unsigned char Request,
                                 unsigned short Value,
                                 unsigned short Index,
                                 void *bufData,
                                 unsigned long *bytes_written
                                 ) {
    unsigned long result;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( deviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;
    int bytesTransferred;

    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( deviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE,
                                                 Request,
                                                 Value,
                                                 Index,
                                                 (unsigned char*)bufData,
                                                 *bytes_written,
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred != (int)*bytes_written) {
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    }

    return result;
}

/**
 * @brief Performs basic low level USB vendor request
 * @return
 */
unsigned long GenericVendorRead(
                                unsigned long deviceIndex,
                                unsigned char Request,
                                unsigned short Value,
                                unsigned short Index,
                                void *bufData,
                                unsigned long *bytes_read
                                )
{
    unsigned long result;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( deviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( deviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    result = AIOUSB_Validate(&deviceIndex);
    if (result != AIOUSB_SUCCESS)
        return result;

    unsigned timeout = deviceDesc->commTimeout;

    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_READ_FROM_DEVICE,
                                                     Request,
                                                     Value,
                                                     Index,
                                                     (unsigned char*)bufData,
                                                     *bytes_read,
                                                     timeout
                                                     );
    if(bytesTransferred != (int)*bytes_read)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    return result;
}

Ushort_Array *new_ushortarray(size_t nelements )
{
    Ushort_Array *tmp = (Ushort_Array*)malloc(sizeof(Ushort_Array));
    if (!nelements)
        return NULL;
    if (!tmp)
        return tmp;
    tmp->ary = (unsigned short *)malloc(sizeof(unsigned short)*nelements );
    if ( !tmp->ary ) {
        free(tmp);
        return NULL;
    }
    tmp->size = (unsigned)nelements;
    return tmp;
}


#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST

#include <math.h>
#include "gtest/gtest.h"

#include "AIOUSB_Core.h"
#include "AIODeviceTable.h"

#ifdef __cplusplus
using namespace AIOUSB;
#endif

TEST(AIOUSB_Core,FakeInit ) {

    AIOUSB_InitTest();
    EXPECT_EQ(AIOUSB_TRUE, AIOUSB_IsInit() );

}

TEST(AIOUSB_Core,MockObjects) {
    int numDevices = 0;
    AIODeviceTableInit();
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_AIO16_16A );
    EXPECT_EQ( numDevices, 1 );
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_DIO_32 );
    EXPECT_EQ( numDevices, 2 );

    EXPECT_EQ( ((AIOUSBDevice *)&deviceTable[0])->ProductID, USB_AIO16_16A  );
    EXPECT_EQ( ((AIOUSBDevice *)&deviceTable[1])->ProductID, USB_DIO_32  );

}

int main( int argc , char *argv[] )
{
    testing::InitGoogleTest(&argc, argv);
    testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
#ifdef GTEST_TAP_PRINT_TO_STDOUT
    delete listeners.Release(listeners.default_result_printer());
#endif

    return RUN_ALL_TESTS();
}


#endif




