#include "AIODeviceTable.h"
#include "AIOPlugNPlay.h"
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif

AIOUSBDevice deviceTable[ MAX_USB_DEVICES ];


static ProductIDName productIDNameTable[] = {
    { USB_DA12_8A_REV_A , "USB-DA12-8A-A"  },
    { USB_DA12_8A       , "USB-DA12-8A"    },
    { USB_DA12_8E       , "USB-DA12-8E"    },
    { USB_DIO_32        , "USB-DIO-32"     },
    { USB_DIO_32I       , "USB-DIO-32I"    },
    { USB_DIO_48        , "USB-DIO-48"     },
    { USB_DIO_96        , "USB-DIO-96"     },
    { USB_DIO_24        , "USB-DIO-24"     },
    { USB_DIO24_CTR6    , "USB-DIO24-CTR6" },
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
    { USB_AI16_16F      , "USB-AI16-16F"   },
    { USB_AO_ARB1       , "USB_AO_ARB1"    },
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
    { USB_AIO12_128E    , "USB-AIO12-128E" },
    { USB_AIO16_16F     , "USB-AIO16-16F"  }
};// keep sorted by productID
#ifdef __cplusplus
const int NUM_PROD_NAMES = sizeof(productIDNameTable) / sizeof(productIDNameTable[ 0 ]);
#else
#define NUM_PROD_NAMES (sizeof(productIDNameTable) / sizeof(productIDNameTable[ 0 ]))
#endif


unsigned long AIOUSB_INIT_PATTERN = 0x9b6773adul;  /* random pattern */
unsigned long aiousbInit = 0;                    /* == AIOUSB_INIT_PATTERN if AIOUSB module is initialized */


/*----------------------------------------------------------------------------*/
AIOUSBDevice *_get_device( unsigned long index , AIORESULT *result )
{
    AIOUSBDevice *dev = NULL;
    if ( index > MAX_USB_DEVICES ) {
        *result = AIOUSB_ERROR_INVALID_INDEX;
        return NULL;
    }
    dev = (AIOUSBDevice*)&deviceTable[index];
    if ( !dev ) {
        if ( result )
            *result = AIOUSB_ERROR_INVALID_DATA;
        return NULL;
    } else if ( result )
        *result = AIOUSB_SUCCESS;
    return dev;
}

AIOUSBDevice *_verified_device( AIOUSBDevice *dev, AIORESULT *result )
{
    if ( dev && dev->valid == AIOUSB_TRUE ) {
        return dev;
    } else {
        *result = AIOUSB_ERROR_INVALID_DEVICE_SETTING;
        return NULL;
    }
}

AIOUSBDevice *_get_device_no_error( unsigned long index )
{
    return (AIOUSBDevice *)&deviceTable[ index ];
}


AIOUSB_BOOL AIOUSB_SetInit()
{
    aiousbInit = AIOUSB_INIT_PATTERN;
    return AIOUSB_TRUE;
}

/*----------------------------------------------------------------------------*/
void AIODeviceTableInit(void)
{
    int index;
    AIORESULT result;
    for(index = 0; index < MAX_USB_DEVICES; index++) {
        AIOUSBDevice *device = _get_device( index , &result );
        /* libusb handles */
        if ( index == 0 ) {
            if ( device->usb_device ) {
                DeleteUSBDevice( device->usb_device );
                device->usb_device = NULL;
            }
        } else {
            device->usb_device = NULL;
        }

        /* run-time settings */
        device->discardFirstSample = AIOUSB_FALSE;
        device->commTimeout = 5000;
        device->miscClockHz = 1;

        /* device-specific properties */
        device->ProductID = 0;
        device->DIOBytes
            = device->Counters
            = device->Tristates
            = device->ConfigBytes
            = device->ImmDACs
            = device->DACsUsed
            = device->ADCChannels
            = device->ADCMUXChannels
            = device->ADCChannelsPerGroup
            = device->WDGBytes
            = device->ImmADCs
            = device->FlashSectors
            = 0;
        device->RootClock
            = device->StreamingBlockSize
            = 0;
        device->bGateSelectable
            = device->bGetName
            = device->bDACStream
            = device->bADCStream
            = device->bDIOStream
            = device->bDIOSPI
            = device->bClearFIFO
            = device->bDACBoardRange
            = device->bDACChannelCal
            = AIOUSB_FALSE;

        /* device state */
        device->bDACOpen
            = device->bDACClosing
            = device->bDACAborting
            = device->bDACStarted
            = device->bDIOOpen
            = device->bDIORead
            = AIOUSB_FALSE;
        device->DACData = NULL;
        device->PendingDACData = NULL;
        device->LastDIOData = NULL;
        device->cachedName = NULL;
        device->cachedSerialNumber = 0;
        device->cachedConfigBlock.size = 0;       // .size == 0 == uninitialized

        /* worker thread state */
        device->workerBusy = AIOUSB_FALSE;
        device->workerStatus = 0;
        device->workerResult = AIOUSB_SUCCESS;
        device->valid = AIOUSB_FALSE;
        device->testing = AIOUSB_FALSE;
    }
    AIOUSB_SetInit();
}

/*----------------------------------------------------------------------------*/
AIOUSB_BOOL AIOUSB_IsInit()
{
    return ( aiousbInit == AIOUSB_INIT_PATTERN );
}

/*----------------------------------------------------------------------------*/
unsigned long AIOUSB_InitTest(void) {
    AIODeviceTableInit();
    aiousbInit = AIOUSB_INIT_PATTERN;
    return AIOUSB_SUCCESS;
}

/*----------------------------------------------------------------------------*/
AIOUSB_BOOL AIOUSB_Cleanup()
{
    aiousbInit = ~ AIOUSB_INIT_PATTERN;
    memset( &deviceTable[0], 0, MAX_USB_DEVICES * AIOUSBDeviceSize() );
    return AIOUSB_TRUE;
}

/*----------------------------------------------------------------------------*/
static int CompareProductIDs(const void *p1, const void *p2)
{
    assert(p1 != 0 &&
           (*( ProductIDName** )p1) != 0 &&
           p2 != 0 &&
           (*( ProductIDName** )p2) != 0);
    const unsigned int productID1 = (*( ProductIDName** )p1)->id,
                       productID2 = (*( ProductIDName** )p2)->id;
    if(productID1 < productID2)
        return -1;
    else if(productID1 > productID2)
        return 1;
    else
        return 0;
}

/*----------------------------------------------------------------------------*/
static int CompareProductNames(const void *p1, const void *p2)
{
    assert(p1 != 0 &&
           (*( ProductIDName** )p1) != 0 &&
           p2 != 0 &&
           (*( ProductIDName** )p2) != 0);
    return strcmp((*( ProductIDName** )p1)->name, (*( ProductIDName** )p2)->name);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Identifying devices on the USB bus
 * @param DeviceIndex
 * @param pPID
 * @param pNameSize
 * @param pName
 * @param pDIOBytes
 * @param pCounters
 * @return
 */
unsigned long QueryDeviceInfo( unsigned long DeviceIndex,
                               unsigned long *pPID,
                               unsigned long *pNameSize,
                               char *pName,
                               unsigned long *pDIOBytes,
                               unsigned long *pCounters
                               )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    if(pPID != NULL)
        *pPID = deviceDesc->ProductID;

    if(pDIOBytes != NULL)
        *pDIOBytes = deviceDesc->DIOBytes;

    if(pCounters != NULL)
        *pCounters = deviceDesc->Counters;

    if( pNameSize != NULL && pName != NULL ) {
        char *deviceName = GetSafeDeviceName(DeviceIndex);
          if(deviceName != 0) {
            /*
             * got a device name, so return it; pName[] is a character array, not a
             * null-terminated string, so don't append null terminator
             */
                int length = strlen(deviceName) + 1;
                if(length > ( int )*pNameSize)
                    length = ( int )*pNameSize;
                else
                    *pNameSize = length;
                memcpy(pName, deviceName, length);          // not null-terminated
            }
      }

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief this function returns the name of a product ID; generally,
 * it's best to use this only as a last resort, since most
 * devices return their name when asked in QueryDeviceInfo()
 */
PRIVATE char *ProductIDToName(unsigned int productID)
{
    char tmpstr[] = "UNKNOWN";
    char *name = tmpstr;


    /**
     * productIDIndex[] represents an index into
     * productIDNameTable[], sorted by product ID;
     * specifically, it contains pointers into
     * productIDNameTable[]; to get the actual product ID,
     * the pointer in productIDIndex[] must be
     * dereferenced; using a separate index table instead
     * of sorting productIDNameTable[] directly permits us
     * to create multiple indexes, in particular, a second
     * index sorted by product name
     */

    /* index of product IDs in productIDNameTable[] */
    static ProductIDName  *productIDIndex[ NUM_PROD_NAMES ];
    /* random pattern */
    unsigned long INIT_PATTERN = 0xe697f8acul;

    /* == INIT_PATTERN if index has been created */
    static unsigned long productIDIndexCreated = 0;
    if(productIDIndexCreated != INIT_PATTERN) {
        /* build index of product IDs */
        for(int index = 0; index < NUM_PROD_NAMES; index++)
            productIDIndex[ index ] = &productIDNameTable[ index ];

        qsort(productIDIndex, NUM_PROD_NAMES, sizeof(ProductIDName *), CompareProductIDs);
        productIDIndexCreated = INIT_PATTERN;
    }

    ProductIDName key;
    key.id = productID;
    ProductIDName * pKey = &key;
    ProductIDName **product = (  ProductIDName** )bsearch(&pKey,
                                                          productIDIndex,
                                                          NUM_PROD_NAMES,
                                                          sizeof(ProductIDName *),
                                                          CompareProductIDs
                                                          );
    if(product != 0)
        name = (*product)->name;

    return name;
}

/*----------------------------------------------------------------------------*/
/**
 * @details This function is the complement of ProductIDToName() and
 * returns the product ID for a given name; this function should be
 * used with care; it will work reliably if passed a name obtained
 * from ProductIDToName(); however, if passed a name obtained from the
 * device itself it may not work; the reason is that devices contain
 * their own name strings, which are most likely identical to the
 * names defined in this module, but not guaranteed to be so; that's
 * not as big a problem as it sounds, however, because if one has the
 * means to obtain the name from the device, then they also have
 * access to the device's product ID, so calling this function is
 * unnecessary; this function is mainly for performing simple
 * conversions between product names and IDs, primarily to support
 * user interfaces
 *
 * @param name
 *
 * @return
 */
PRIVATE AIORET_TYPE ProductNameToID(const char *name)
{
    AIO_ASSERT( name );
    AIORET_TYPE retval = AIOUSB_SUCCESS;

    /**
     * productNameIndex[] represents an index into
     * productIDNameTable[], sorted by product name (see notes for
     * ProductIDToName())
     */

    static ProductIDName *productNameIndex[ NUM_PROD_NAMES ]; /**<index of product names in productIDNameTable[] */

    unsigned long INIT_PATTERN = 0x7e6b2017ul;
    /** random pattern */

    static unsigned long productNameIndexCreated = 0;
    /** == INIT_PATTERN if index has been created */

    if (productNameIndexCreated != INIT_PATTERN) {
        /* build index of product names */
        int index;
        for(index = 0; index < NUM_PROD_NAMES; index++)
            productNameIndex[ index ] = &productIDNameTable[ index ];
        qsort(productNameIndex, NUM_PROD_NAMES, sizeof(ProductIDName *), CompareProductNames);
        productNameIndexCreated = INIT_PATTERN;
    }

    ProductIDName key;                                   // key.id not used
    strncpy(key.name, name, PROD_NAME_SIZE);
    key.name[ PROD_NAME_SIZE ] = 0;                     // in case strncpy() doesn't copy null
    ProductIDName *pKey = &key;
    ProductIDName **product = ( ProductIDName** )bsearch(&pKey, productNameIndex, NUM_PROD_NAMES, sizeof(ProductIDName *), CompareProductNames);
    if (product != 0)
        retval = (*product)->id;

    return retval;
}

/*----------------------------------------------------------------------------*/
/**
 * @note Will call AIOUSB_Init() in case the AIOUSB API has not been initialized
 * with the AIOUSB_Init() function. This is a convenience function.
 * @return if < 0 Error else SUCCESS
 */
AIORET_TYPE GetDevices(void)
{
    unsigned long deviceMask = 0;
    int index;
    if ( !AIOUSB_IsInit() ) {
        AIOUSB_Init();
    }

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_INIT, AIOUSB_IsInit() );
    /**
     * @note
     * we clear the device table to erase references to devices
     * which may have been unplugged; any device indexes to devices
     * that have not been unplugged, which the user may be using,
     * _should_ still be valid
     */
    for(index = 0; index < MAX_USB_DEVICES; index++) {
        if ( deviceTable[index].usb_device != NULL && deviceTable[index].valid == AIOUSB_TRUE )
            deviceMask =  (deviceMask << 1) | 1;
    }

    return (AIORET_TYPE)deviceMask;
}

/**
 * @brief Checks whether the device has firmware 2.0 enabled or not
 * @param DeviceIndex
 * @return < 0 if failure
 */
AIORET_TYPE AIOUSB_CheckFirmware20( unsigned long DeviceIndex )
{
    AIORESULT res = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc;
    unsigned char memflags[3];
    USBDevice *usb;
    int bytesTransferred;

    deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &res );
    AIO_ERROR_VALID_DATA( -(AIORET_TYPE)res, res == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( -AIOUSB_ERROR_DEVICE_NOT_FOUND, deviceDesc );

    if ( deviceDesc->bFirmware20 ) return AIOUSB_SUCCESS;

    usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_USBDEVICE_NOT_FOUND, usb );


    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_READ_FROM_DEVICE,
                                                 CUR_RAM_READ,
                                                 0x8000,
                                                 1,
                                                 memflags,
                                                 sizeof(memflags),
                                                 deviceDesc->commTimeout
                                                 );

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED,  bytesTransferred == sizeof(memflags) );

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, bytesTransferred >= (int)sizeof(memflags) );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, memflags[0] != 0xFF );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, memflags[0] >= 3 );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, (memflags[2] & 0x02) != 0);

    deviceDesc->bFirmware20 = AIOUSB_TRUE;

    return retval;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex Device index we are probing
 * @param[out] res Error code if unable to find USB device
 * @return USBDevice * A Usb handle that can be used for USB transactions
 */
USBDevice *AIODeviceTableGetUSBDeviceAtIndex( unsigned long DeviceIndex, AIORESULT *res )
{
    AIOUSBDevice *dev = AIODeviceTableGetDeviceAtIndex( DeviceIndex , res );
    AIO_ERROR_VALID_DATA_W_CODE( NULL, *res = -AIOUSB_ERROR_DEVICE_NOT_FOUND, dev );
    AIO_ERROR_VALID_DATA( NULL, *res == AIOUSB_SUCCESS );

    USBDevice *usb = AIOUSBDeviceGetUSBHandle( dev );
    AIO_ERROR_VALID_DATA_W_CODE( NULL, *res = -AIOUSB_ERROR_INVALID_USBDEVICE, usb );

    return usb;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param name
 * @brief descriptor strings returned by the device are Unicode, not
 *        ASCII, and occupy two bytes per character, so we need to handle our
 *        maximum lengths accordingly
 * @note 1
 * @verbatim
 * extract device name from descriptor and copy to cached name buffer
 * descData[ 0 ] = 1 (descriptor length) + 1 (descriptor type) + 2 (Unicode) * string_length
 * descData[ 1 ] = \x03 (descriptor type: string)
 * descData[ 2 ] = low byte of first character of Unicode string
 * descData[ 3 ] = \0 (high byte)
 * descData[ 4 ] = low byte of second character of string
 * descData[ 5 ] = \0 (high byte)
 * ...
 * descData[ string_length * 2 ] = low byte of last character of string
 * descData[ string_length * 2 + 1 ] = \0 (high byte)
 * @endverbatim
 * @return
 *
 */
static unsigned long GetDeviceName(unsigned long DeviceIndex, char **name)
{

    AIORESULT result = AIOUSB_SUCCESS;
    AIO_ASSERT( name );

    int srcLength,  bytesTransferred;
    unsigned char descData[CYPRESS_MAX_DESC_SIZE];
    USBDevice *usb;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_DEVICE_NOT_FOUND, result == AIOUSB_SUCCESS );

    if ( deviceDesc->cachedName ) {
        *name = deviceDesc->cachedName;
        goto out_GetDeviceName;
    }

    usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_USBDEVICE_NOT_FOUND, usb );


    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_READ_FROM_DEVICE,
                                                 CYPRESS_GET_DESC,
                                                 CYPRESS_DESC_PARAMS,
                                                 0,
                                                 descData,
                                                 CYPRESS_MAX_DESC_SIZE,
                                                 deviceDesc->commTimeout
                                                 );

    AIO_ERROR_VALID_DATA( LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred), bytesTransferred == CYPRESS_MAX_DESC_SIZE );

    /* SEE Note 1 */
    srcLength = ( int )((descData[ 0 ] - 2) / 2);

    deviceDesc->cachedName = ( char* )malloc(CYPRESS_MAX_DESC_SIZE + 2);
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_ENOUGH_MEMORY, deviceDesc->cachedName  );


    for(int srcIndex = 2 , dstIndex = 0; dstIndex < srcLength && dstIndex < AIOUSB_MAX_NAME_SIZE; srcIndex += 2 , dstIndex++ ) {
        deviceDesc->cachedName[ dstIndex ] = descData[ srcIndex ];
    }

    *name = deviceDesc->cachedName;


 out_GetDeviceName:

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief GetSafeDeviceName() returns a null-terminated device name;
 * if GetSafeDeviceName() is unable to obtain a legitimate device name
 * it returns something like "UNKNOWN" or 0
 */
char *GetSafeDeviceName(unsigned long DeviceIndex)
{
    char *deviceName = 0;

    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return deviceName;


    if (deviceDesc->bGetName) {
        /*
         * device supports getting its product name, so use it instead of the
         * name from the local product name table
         */

        GetDeviceName(DeviceIndex, &deviceName); /* if we fail, use name from devicetable */
        deviceName = ProductIDToName(deviceDesc->ProductID);

    }

    return deviceName;
}

/*----------------------------------------------------------------------------*/
AIORESULT _Initialize_Device_Desc(unsigned long DeviceIndex)
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice * device = _get_device( DeviceIndex , &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    device->DIOBytes = 0;
    device->DIOConfigBits = 0;
    device->Counters = 0;
    device->RootClock = 0;
    device->Tristates = 0;
    device->bGetName = AIOUSB_FALSE;
    device->ConfigBytes = 0;
    device->bGateSelectable = AIOUSB_FALSE;
    device->bDACBoardRange = AIOUSB_FALSE;
    device->bDACChannelCal = AIOUSB_FALSE;
    device->ImmDACs = 0;
    device->ImmADCs = 0;
    device->ADCChannels = 0;
    device->ADCMUXChannels = 0;
    device->bDACStream = AIOUSB_FALSE;
    device->bADCStream = AIOUSB_FALSE;
    device->RangeShift = 0;
    device->bDIOStream = AIOUSB_FALSE;
    device->StreamingBlockSize = 31 * 1024;
    device->bDIODebounce = AIOUSB_FALSE;
    device->bDIOSPI = AIOUSB_FALSE;
    device->bClearFIFO = AIOUSB_FALSE;
    device->FlashSectors = 0;
    device->WDGBytes = 0;
    device->bSetCustomClocks = AIOUSB_FALSE;

    CheckPNPData(DeviceIndex );

    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT  _Card_Specific_Settings(unsigned long DeviceIndex)
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = _get_device( DeviceIndex , &result );

    if ( result != AIOUSB_SUCCESS )
        return result;

    switch(device->ProductID) {
      case 0x8001:
          device->DIOBytes = 4;
          device->Counters = 3;
          device->RootClock = 3000000;
          device->bGetName = AIOUSB_TRUE;
          device->bSetCustomClocks = AIOUSB_TRUE;
          device->bDIODebounce = AIOUSB_TRUE;
          break;

      case 0x8004:
          device->DIOBytes = 4;
          device->DIOConfigBits = 32;
          device->bGetName = AIOUSB_TRUE;
          device->bSetCustomClocks = AIOUSB_TRUE;
          break;

      case 0x8002:
          device->DIOBytes = 6;
          device->bGetName = AIOUSB_TRUE;
          break;

      case 0x8003:
          device->DIOBytes = 12;
          device->bGetName = AIOUSB_TRUE;
          break;

      case 0x8068:
          device->bDACBoardRange = AIOUSB_TRUE;
      case 0x8008:
      case 0x8009:
      case 0x800A:
          device->DIOBytes = 1;
          device->bGetName = AIOUSB_TRUE;
          device->bDIOStream = AIOUSB_TRUE;
          device->bDIOSPI = AIOUSB_TRUE;
          device->bClearFIFO = AIOUSB_TRUE;
          break;

      case 0x800C:
      case 0x800D:
      case 0x800E:
      case 0x800F:
          device->DIOBytes = 4;
          device->Tristates = 2;
          device->bGetName = AIOUSB_TRUE;
          device->bDIOStream = AIOUSB_TRUE;
          device->bDIOSPI = AIOUSB_TRUE;
          device->bClearFIFO = AIOUSB_TRUE;
          break;

          //USB-IIRO-16 family
      case 0x8010:
      case 0x8011:
      case 0x8012:
      case 0x8014:
      case 0x8015:
      case 0x8016:
        //USB-IDIO-16 family
      case 0x8018:
      case 0x801a:
      case 0x801c:
      case 0x801e:
      case 0x8019:
      case 0x801d:
      case 0x801f:
          device->DIOBytes = 4;
          device->bGetName = AIOUSB_TRUE;
          device->WDGBytes = 2;
          break;

      case 0x4001:
      case 0x4002:
          device->bGetName = AIOUSB_FALSE;
          device->bDACStream = AIOUSB_TRUE;
          device->ImmDACs = 8;
          device->DACsUsed = 5;
          device->bGetName = AIOUSB_TRUE;
          break;

      case 0x4003:
          device->bGetName = AIOUSB_FALSE;
          device->ImmDACs = 8;
          device->bGetName = AIOUSB_TRUE;
          break;

      case 0x8020:
          device->Counters = 5;
          device->bGateSelectable = AIOUSB_TRUE;
          device->RootClock = 10000000;
          device->bGetName = AIOUSB_TRUE;
          break;

      case 0x8030:
      case 0x8031:
          device->DIOBytes = 2;
          device->bGetName = AIOUSB_TRUE;
          break;

      case 0x8032:
          device->DIOBytes = 3;
          device->bGetName = AIOUSB_TRUE;
          break;

      case 0x8033:
          device->DIOBytes = 3;
          device->bGetName = AIOUSB_TRUE;
          break;

      case 0x8036:
          device->DIOBytes = 2;
          device->bGetName = AIOUSB_TRUE;
          device->ImmADCs = 2;
          break;

      case 0x8037:
          device->DIOBytes = 2;
          device->bGetName = AIOUSB_TRUE;
          device->ImmADCs = 2;
          break;

      case 0x8040:
      case 0x8041:
      case 0x8042:
      case 0x8043:
      case 0x8044:
      case 0x8140:
      case 0x8141:
      case 0x8142:
      case 0x8143:
      case 0x8144:
          device->DIOBytes = 2;
          device->Counters = 1;
          device->RootClock = 10000000;
          device->bGetName = AIOUSB_TRUE;
          device->bADCStream = AIOUSB_TRUE;
          device->ADCChannels = 16;
          device->ADCMUXChannels = 16;
          device->ConfigBytes = 20;
          device->RangeShift = 0;
          device->bClearFIFO = AIOUSB_TRUE;
          if ((device->ProductID & 0x0100) != 0) {
                device->bDACBoardRange = AIOUSB_TRUE;
                device->ImmDACs = 2;
            }
          break;

      case 0x8045:
      case 0x8046:
      case 0x8047:
      case 0x8048:
      case 0x8049:
      case 0x8145:
      case 0x8146:
      case 0x8147:
      case 0x8148:
      case 0x8149:
          device->DIOBytes = 2;
          device->Counters = 1;
          device->RootClock = 10000000;
          device->bGetName = AIOUSB_TRUE;
          device->bADCStream = AIOUSB_TRUE;
          device->ADCChannels = 16;
          device->ADCMUXChannels = 64;
          device->ConfigBytes = 21;
          device->RangeShift = 2;
          device->bClearFIFO = AIOUSB_TRUE;
          if ((device->ProductID & 0x0100) != 0) {
                device->bDACBoardRange = AIOUSB_TRUE;
                device->ImmDACs = 2;
            }
          break;

      case 0x804a:
      case 0x804b:
      case 0x804c:
      case 0x804d:
      case 0x804e:
      case 0x804f:
      case 0x8050:
      case 0x8051:
      case 0x8052:
      case 0x8053:
      case 0x8054:
      case 0x8055:
      case 0x8056:
      case 0x8057:
      case 0x8058:
      case 0x8059:
      case 0x805a:
      case 0x805b:
      case 0x805c:
      case 0x805d:
      case 0x805e:
      case 0x805f:
      case 0x814a:
      case 0x814b:
      case 0x814c:
      case 0x814d:
      case 0x814e:
      case 0x814f:
      case 0x8150:
      case 0x8151:
      case 0x8152:
      case 0x8153:
      case 0x8154:
      case 0x8155:
      case 0x8156:
      case 0x8157:
      case 0x8158:
      case 0x8159:
      case 0x815a:
      case 0x815b:
      case 0x815c:
      case 0x815d:
      case 0x815e:
      case 0x815f:
          device->DIOBytes = 2;
          device->Counters = 1;
          device->RootClock = 10000000;
          device->bGetName = AIOUSB_TRUE;
          device->bADCStream = AIOUSB_TRUE;
          device->ADCChannels = 16;
          device->ADCMUXChannels = 32 * ((((device->ProductID - 0x804A) & (~0x0100)) / 5) + 1);
          device->ConfigBytes = 21;
          device->RangeShift = 3;
          device->bClearFIFO = AIOUSB_TRUE;
          if ((device->ProductID & 0x0100) != 0) {
                device->bDACBoardRange = AIOUSB_TRUE;
                device->ImmDACs = 2;
            }
          break;

      case 0x8060:
      case 0x8070:
      case 0x8071:
      case 0x8072:
      case 0x8073:
      case 0x8074:
      case 0x8075:
      case 0x8076:
      case 0x8077:
      case 0x8078:
      case 0x8079:
      case 0x807a:
      case 0x807b:
      case 0x807c:
      case 0x807d:
      case 0x807e:
      case 0x807f:
          device->DIOBytes = 2;
          device->bGetName = AIOUSB_TRUE;
          device->FlashSectors = 32;
          device->bDACBoardRange = AIOUSB_TRUE;
          device->bDACChannelCal = AIOUSB_TRUE;
          //device->bClearFIFO = AIOUSB_TRUE;
          //Add a new-style DAC streaming
          switch(device->ProductID & 0x06) {
            case 0x00:
                device->ImmDACs = 16;
                break;

            case 0x02:
                device->ImmDACs = 12;
                break;

            case 0x04:
                device->ImmDACs = 8;
                break;

            case 0x06:
                device->ImmDACs = 4;
                break;
            }
          if ((device->ProductID & 1) == 0)
              device->ImmADCs = 2;
          break;

      default:
          device->bADCStream = AIOUSB_TRUE;
          device->bDIOStream = AIOUSB_TRUE;
          device->bDIOSPI = AIOUSB_TRUE;
          result = AIOUSB_SUCCESS;
          break;
      }
    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @return
 */
 AIORET_TYPE AIOUSB_EnsureOpen(unsigned long DeviceIndex)
 {
     AIORET_TYPE result = AIOUSB_SUCCESS;
     AIOUSBDevice *device = AIODeviceTableGetDeviceAtIndex( DeviceIndex, (AIORESULT*)&result );

     AIO_ERROR_VALID_DATA_RETVAL( result, result == AIOUSB_SUCCESS );
     if ( !device->usb_device  ) {
         AIO_ERROR_VALID_DATA( AIOUSB_ERROR_DEVICE_NOT_CONNECTED, !device->bDeviceWasHere );
         AIO_ERROR_VALID_DATA( AIOUSB_ERROR_USBDEVICE_NOT_FOUND, device->bDeviceWasHere );
     }
     if ( device->bOpen )
         return AIOUSB_SUCCESS;

     _Initialize_Device_Desc(DeviceIndex);

     result |= _Card_Specific_Settings(DeviceIndex);
     if (result != AIOUSB_SUCCESS)
         goto RETURN_AIOUSB_EnsureOpen;
     if (device->DIOConfigBits == 0)
         device->DIOConfigBits = device->DIOBytes;

 RETURN_AIOUSB_EnsureOpen:
     return result;
 }

/*----------------------------------------------------------------------------*/
AIOUSBDevice *AIODeviceTableGetDeviceAtIndex( unsigned long DeviceIndex , AIORESULT *res )
{
    AIOUSBDevice *retval = NULL;
    AIO_ERROR_VALID_DATA_W_CODE( NULL, *res = AIOUSB_ERROR_NOT_INIT, AIOUSB_IsInit());

    if (DeviceIndex == diFirst) { /* find first device on bus */
        *res = AIOUSB_ERROR_FILE_NOT_FOUND;
        int index;
        for(index = 0; index < MAX_USB_DEVICES; index++) {
            if ( (retval = _verified_device(_get_device(index , res ), res )) && *res == AIOUSB_SUCCESS ) {
                DeviceIndex = index;
                break;
            }
        }
    } else if (DeviceIndex == diOnly) {
        /*
         * find first device on bus, ensuring that it's the only device
         */
        *res = AIOUSB_ERROR_FILE_NOT_FOUND;
        int index;
        for(index = 0; index < MAX_USB_DEVICES; index++) {
            if ( (retval = _verified_device(_get_device(index, res ), res )) ) {
                /* found a device */
                if ( *res != AIOUSB_SUCCESS) {
                    /*
                     * this is the first device found; save this index, but
                     * keep checking to see that this is the only device
                     */
                    DeviceIndex = index;
                    *res = AIOUSB_SUCCESS;
                } else {
                    /*
                     * there are multiple devices on the bus
                     */
                    *res = AIOUSB_ERROR_DUP_NAME;
                    retval = NULL;
                    break;
                }
            }
        }
    } else {
        /*
         * simply verify that the supplied index is valid
         */
        retval = _verified_device( _get_device( DeviceIndex , res ), res );
    }

    return retval;
}

/*----------------------------------------------------------------------------*/
AIOUSBDevice *AIODeviceTableGetAIOUSBDeviceAtIndex( unsigned long DeviceIndex )
{
    AIOUSBDevice *retval = NULL;
    AIORESULT value = AIOUSB_SUCCESS;
    AIORESULT *res = &value;
    errno = AIOUSB_SUCCESS;

    AIO_ERROR_VALID_DATA_WITH_CODE( NULL, AIOUSB_ERROR_NOT_INIT, AIOUSB_IsInit() == AIOUSB_TRUE );

    if (DeviceIndex == diFirst) { /* find first device on bus */
        errno = AIO_ERROR(AIOUSB_ERROR_DEVICE_NOT_FOUND);
        int index;
        for(index = 0; index < MAX_USB_DEVICES; index++) {
            if ( (retval = _verified_device(_get_device(index , res ), res )) && *res == AIOUSB_SUCCESS ) {
                errno = AIOUSB_SUCCESS;
                DeviceIndex = index;
                break;
            }
        }
    } else if (DeviceIndex == diOnly) {
        /*
         * find first device on bus, ensuring that it's the only device
         */
        errno = AIO_ERROR(AIOUSB_ERROR_DEVICE_NOT_FOUND);
        int index;
        for(index = 0; index < MAX_USB_DEVICES; index++) {
            if ( (retval = _verified_device(_get_device(index, res ), res )) ) {
                /* found a device */
                if ( *res != AIOUSB_SUCCESS) {
                    /*
                     * this is the first device found; save this index, but
                     * keep checking to see that this is the only device
                     */
                    DeviceIndex = index;
                    errno = AIOUSB_SUCCESS;
                } else {
                    /*
                     * there are multiple devices on the bus
                     */
                    errno = AIO_ERROR(AIOUSB_ERROR_DUP_NAME);
                    retval = NULL;
                    break;
                }
            }
        }
    } else {
        /*
         * simply verify that the supplied index is valid
         */
        retval = _verified_device( _get_device( DeviceIndex , res ), res );
        if ( retval )
            errno = AIOUSB_SUCCESS;
        else
            errno = AIO_ERROR(AIOUSB_ERROR_DEVICE_NOT_FOUND);
    }

    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSBGetError()
{
    return (AIORET_TYPE)errno;
}

/*----------------------------------------------------------------------------*/
void _setup_device_parameters( AIOUSBDevice *device , unsigned long productID )
{
    device->ProductID = productID;
    device->StreamingBlockSize = 31ul * 1024ul;
    device->bGetName = AIOUSB_TRUE;             // most boards support this feature


    if (productID == USB_DIO_32 || productID == USB_DIO_32I ) {
        device->DIOBytes = 4;
        device->Counters = 3;

        device->bGetName = AIOUSB_TRUE;
        device->bSetCustomClocks = AIOUSB_TRUE;
        device->Tristates = 1;
        if ( productID == USB_DIO_32 ) {
            device->RootClock = 3000000;
            device->bDIODebounce = AIOUSB_TRUE;
        } else {
            device->DIOConfigBits = 32;
        }

    } else if ( productID == USB_DIO24_CTR6 || productID == USB_DIO_24 ) {
      device->DIOBytes = 3;
      device->DIOConfigBits = 4;
      device->Tristates = 1;
      device->bGetName = AIOUSB_TRUE;
      device->bDIODebounce = AIOUSB_TRUE;
      if (productID == USB_DIO24_CTR6 ) {
          device->Counters = 2;
          device->bSetCustomClocks = AIOUSB_TRUE;
          device->RootClock = 10000000;
      }

    } else if (productID == USB_DIO_48) {
        device->DIOBytes = 6;
    } else if (productID == USB_DIO_96) {
        device->DIOBytes = 12;
    } else if ( productID >= USB_DI16A_REV_A1 && productID <= USB_DI16A_REV_A2 ) {
        device->DIOBytes = 1;
        device->bDIOStream = AIOUSB_TRUE;
        device->bDIOSPI = AIOUSB_TRUE;
        device->bClearFIFO = AIOUSB_TRUE;
    } else if (
              productID >= USB_DIO_16H &&
              productID <= USB_DIO_16A
              ) {
        device->DIOBytes = 4;
        device->Tristates = 2;
        device->bDIOStream = AIOUSB_TRUE;
        device->bDIOSPI = AIOUSB_TRUE;
        device->bClearFIFO = AIOUSB_TRUE;
    } else if (
              productID == USB_IIRO_16 ||
              productID == USB_II_16 ||
              productID == USB_RO_16 ||
              productID == USB_IIRO_8 ||
              productID == USB_II_8 ||
              productID == USB_IIRO_4
              ) {
        device->DIOBytes = 4;
        device->WDGBytes = 2;
    } else if (
              productID == USB_IDIO_16 ||
              productID == USB_II_16_OLD ||
              productID == USB_IDO_16 ||
              productID == USB_IDIO_8 ||
              productID == USB_II_8_OLD ||
              productID == USB_IDIO_4
              ) {
        device->DIOBytes = 4;
        device->WDGBytes = 2;
    } else if (
              productID >= USB_DA12_8A_REV_A &&
              productID <= USB_DA12_8A
              ) {
        device->bDACStream = AIOUSB_TRUE;
        device->ImmDACs = 8;
        device->DACsUsed = 5;
        device->bGetName = AIOUSB_FALSE;
        device->RootClock = 12000000;
    } else if (productID == USB_DA12_8E) {
        device->ImmDACs = 8;
        device->bGetName = AIOUSB_FALSE;
    } else if (productID == USB_CTR_15) {
        device->Counters = 5;
        device->bGateSelectable = AIOUSB_TRUE;
        device->RootClock = 10000000;
    } else if (
              productID == USB_IIRO4_2SM ||
              productID == USB_IIRO4_COM
              ) {
        device->DIOBytes = 2;
    } else if (productID == USB_DIO16RO8) {
        device->DIOBytes = 3;
    } else if (productID == PICO_DIO16RO8) {
        device->DIOBytes = 3;
    } else if (
              (productID >= USB_AI16_16A && productID <= USB_AI16_16F) ||
              (productID >= USB_AIO16_16A && productID <= USB_AIO16_16F)
              ) {
        device->DIOBytes = 2;
        device->Counters = 1;
        device->RootClock = 10000000;
        device->bADCStream = AIOUSB_TRUE;
        device->ImmADCs = 1;
        device->ADCChannels = device->ADCMUXChannels = 16;
        device->ADCChannelsPerGroup = 1;
        device->ConfigBytes = AD_CONFIG_REGISTERS;
        device->bClearFIFO = AIOUSB_TRUE;
        if (productID & 0x0100) {
            device->ImmDACs = 2;
            device->bDACBoardRange = AIOUSB_TRUE;
        }
    } else if (
              (productID >= USB_AI16_64MA && productID <= USB_AI12_64ME) ||
              (productID >= USB_AIO16_64MA && productID <= USB_AIO12_64ME)
              ) {
        device->DIOBytes = 2;
        device->Counters = 1;
        device->RootClock = 10000000;
        device->bADCStream = AIOUSB_TRUE;
        device->ImmADCs = 1;
        device->ADCChannels = 16;
        device->ADCMUXChannels = 64;
        device->ADCChannelsPerGroup = 4;
        device->ConfigBytes = AD_MUX_CONFIG_REGISTERS;
        device->bClearFIFO = AIOUSB_TRUE;
        if (productID & 0x0100) {
            device->ImmDACs = 2;
            device->bDACBoardRange = AIOUSB_TRUE;
        }
    } else if (
              (productID >= USB_AI16_32A && productID <= USB_AI12_128E) ||
              (productID >= USB_AIO16_32A && productID <= USB_AIO12_128E)
              ) {
        device->DIOBytes = 2;
        device->Counters = 1;
        device->RootClock = 10000000;
        device->bADCStream = AIOUSB_TRUE;
        device->ImmADCs = 1;
        device->ADCChannels = 16;

        /*
         * there are four groups of five
         * products each in this family;
         * each group of five products has
         * 32 more MUX channels than the
         * preceding group; so we calculate
         * the number of MUX channels by
         * doing some arithmetic on the
         * product ID
         */

        int I = (productID - USB_AI16_32A) & ~0x0100;
        I /= 5;               /* products per group */
        device->ADCMUXChannels = 32 * (I + 1);
        device->ADCChannelsPerGroup = 8;
        device->ConfigBytes = AD_MUX_CONFIG_REGISTERS;
        device->bClearFIFO = AIOUSB_TRUE;
        if (productID & 0x0100) {
            device->ImmDACs = 2;
            device->bDACBoardRange = AIOUSB_TRUE;
        }
    } else if ( productID == USB_AO_ARB1 ) {
      device->DIOBytes      = 4;
      device->Tristates     = 1;
      device->bGetName      = AIOUSB_TRUE;
      device->bDIOStream    = AIOUSB_TRUE;
      device->bClearFIFO    = AIOUSB_TRUE;
      device->bDACDIOClock  = AIOUSB_TRUE;
      device->bDACBoardRange = AIOUSB_TRUE;
    } else if (
              productID >= USB_AO16_16A &&
              productID <= USB_AO12_4
              ) {
        device->DIOBytes = 2;
        device->FlashSectors = 32;
        device->bDACBoardRange = AIOUSB_TRUE;
        device->bDACChannelCal = AIOUSB_TRUE;

        /*
         * we use a few bits within the
         * product ID to determine the
         * number of DACs and whether or not
         * the product has ADCs
         */
        switch(productID & 0x0006) {
        case 0x0000:
            device->ImmDACs = 16;
            break;

        case 0x0002:
            device->ImmDACs = 12;
            break;

        case 0x0004:
            device->ImmDACs = 8;
            break;

        case 0x0006:
            device->ImmDACs = 4;
            break;
        }
        if ((productID & 0x0001) == 0)
            device->ImmADCs = 2;
    }

    /* allocate I/O image buffers */
    if (device->DIOBytes > 0) {
        /* calloc() zeros memory */
        device->LastDIOData = ( unsigned char* )calloc(device->DIOBytes, sizeof(unsigned char));
        // assert(device->LastDIOData != 0);
    }

    /* Clear up the CachedConfigBlock */
    ADCConfigBlockInitializeFromAIOUSBDevice( &device->cachedConfigBlock , device );
    device->cachedConfigBlock.size                              = device->ConfigBytes;
    device->cachedConfigBlock.testing                           = device->testing;
    device->cachedConfigBlock.mux_settings.ADCMUXChannels       = device->ADCMUXChannels;
    device->cachedConfigBlock.mux_settings.ADCChannelsPerGroup  = device->ADCChannelsPerGroup;
    device->cachedConfigBlock.mux_settings.defined              = AIOUSB_TRUE;

    device->valid = AIOUSB_TRUE;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief A mock function that can set up the DeviceTable with any type of devices
 **/
AIORESULT AIODeviceTableAddDeviceToDeviceTable( int *numAccesDevices, unsigned long productID )
{
    return AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( numAccesDevices, productID, NULL );
}

/*----------------------------------------------------------------------------*/
AIORESULT AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( int *numAccesDevices, unsigned long productID , USBDevice *usb_dev )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device  = _get_device( *numAccesDevices , &result );

    device->usb_device    = usb_dev;
    device->ProductID     = productID;
    device->isInit        = AIOUSB_TRUE;
    device->valid         = AIOUSB_TRUE;
    _setup_device_parameters( device , productID );

    CheckPNPData( *numAccesDevices );

    ADCConfigBlockSetDevice( AIOUSBDeviceGetADCConfigBlock( device ), device );

    *numAccesDevices += 1;
    return result;
}

/*----------------------------------------------------------------------------*/

/**
 * @param numDevices
 * @return
 * @brief cleans up the AIODeviceTable and frees any memory associated with it.
 */
AIORET_TYPE ClearAIODeviceTable( int numDevices )
{
    AIORESULT result = AIOUSB_SUCCESS;
    for ( int i = 0; i < numDevices ; i ++ ) {
        AIOUSBDevice *device = &deviceTable[i];
        if ( device->LastDIOData )
            free(device->LastDIOData );
    }

    return result;
}


/*----------------------------------------------------------------------------*/
AIORESULT AIODeviceTableSetDeviceID( int index, AIOUSBDevice *dev )
{
    AIORESULT result = AIOUSB_SUCCESS;
    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT AIOUSB_GetAllDevices()
{
    AIORESULT deviceMask = 0;

    if (AIOUSB_IsInit()) {
        int index;
        for (index = 0; index < MAX_USB_DEVICES; index++) {
            AIORESULT res = AIOUSB_SUCCESS;
            AIODeviceTableGetDeviceAtIndex( index , &res );
            if ( res == AIOUSB_SUCCESS ) {
                int MAX_NAME_SIZE = 100;
                char name[ MAX_NAME_SIZE + 1 ];
                unsigned long productID;
                unsigned long nameSize = MAX_NAME_SIZE;
                unsigned long numDIOBytes;
                unsigned long numCounters;

                unsigned long result = QueryDeviceInfo(index, &productID, &nameSize, name, &numDIOBytes, &numCounters);
                if (result == AIOUSB_SUCCESS) {
                    name[ nameSize ] = '\0';
                    deviceMask = (deviceMask << 1) | 1;
                }
            }
        }
    }

    return deviceMask;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIODeviceTablePopulateTableTest(unsigned long *products, int length )
{
    int numAccesDevices = 0;
    AIORESULT result;
    AIOUSB_InitTest();
    for( int i = 0; i < length ; i ++  ) {
        result = AIODeviceTableAddDeviceToDeviceTable( &numAccesDevices, products[i] );
        if ( result != AIOUSB_SUCCESS ) {
            deviceTable[numAccesDevices-1].usb_device = (USBDevice *)0x42;
        }
    }
    return AIOUSB_SUCCESS;
}

/*----------------------------------------------------------------------------*/
void CloseAllDevices(void)
{
    if (!AIOUSB_IsInit())
        return;
    int index;
    AIORESULT result = AIOUSB_SUCCESS;
    for(index = 0; index < MAX_USB_DEVICES; index++) {
        result = AIOUSB_SUCCESS;
        AIOUSBDevice *device = AIODeviceTableGetDeviceAtIndex( index, &result );
        if ( result == AIOUSB_SUCCESS )  {
            USBDevice *usb = AIOUSBDeviceGetUSBHandle( device );
            if ( usb )
                USBDeviceClose( usb );

            if (device->LastDIOData != NULL) {
                free(device->LastDIOData);
                device->LastDIOData = NULL;
            }

            if (device->cachedName != NULL) {
                free(device->cachedName);
                device->cachedName = NULL;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
unsigned long AIODeviceTableClearDevices(void)
{
    CloseAllDevices();
    AIODeviceTableInit();
    return AIOUSB_SUCCESS;
}

unsigned long ClearDevices( void ) {
    return AIODeviceTableClearDevices();
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/**
 * @brief populate device table with ACCES devices found on USB bus
 * @todo Rely on Global Header files for the functionality of devices / cards
 * as opposed to hard coding
 * @note
 * populate device table so users can use diFirst and diOnly immediately; be
 * sure to call PopulateDeviceTable() after 'aiousbInit = AIOUSB_INIT_PATTERN;'
 */
AIORET_TYPE AIODeviceTablePopulateTable(void)
{

    int libusbResult = libusb_init( NULL );
    if (libusbResult != LIBUSB_SUCCESS)
        return -libusbResult;
    int numAccesDevices = 0;
    AIORET_TYPE result;
    USBDevice *usbdevices = NULL;
    int size = 0;
    libusb_device **deviceList = 0;

    result = AddAllACCESUSBDevices( &deviceList, &usbdevices , &size );

    if ( result < AIOUSB_SUCCESS )
        return result;

    for ( int i = 0; i < size ; i ++ ) {
        AIOUSBDevice *device = (AIOUSBDevice *)&deviceTable[ numAccesDevices++ ];

        unsigned productID = USBDeviceGetIdProduct( &usbdevices[i] );
        _setup_device_parameters( device, productID );
        device->usb_device = CopyUSBDevice( &usbdevices[i] );
        CheckPNPData( i );

    }
    if (usbdevices)
        free(usbdevices);
    libusb_free_device_list(deviceList, AIOUSB_TRUE);

    return AIOUSB_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief AIOUSB_Init() and AIOUSB_Exit() are not thread-safe and
 * should not be called while other threads might be accessing global
 * variables. Hence you should just run AIOUSB_Init() once at the beginning
 * and then the AIOUSB_Exit() once at the end after every thread acquiring
 * data has been stopped.
 */
AIORET_TYPE AIOUSB_Init(void)
{
    AIORET_TYPE result = AIOUSB_SUCCESS;

    if (!AIOUSB_IsInit()) {
          AIODeviceTableInit();
#if defined(AIOUSB_ENABLE_MUTEX)
          pthread_mutexattr_t mutexAttr;
          if (pthread_mutexattr_init(&mutexAttr) == 0) {
                if (pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE) == 0) {
                    if (pthread_mutex_init(&aiousbMutex, &mutexAttr) == 0) {
                        /**
                         * @note
                         * populate device table so users can use diFirst and diOnly immediately; be
                         * sure to call PopulateDeviceTable() after 'aiousbInit = AIOUSB_INIT_PATTERN;'
                         */
                          if ( AIODeviceTablePopulateTable() != AIOUSB_SUCCESS ) {
                              pthread_mutex_destroy(&aiousbMutex);
                              result = LIBUSB_RESULT_TO_AIOUSB_RESULT(libusbResult);
                          }

                        } else
                          result = AIOUSB_ERROR_INVALID_MUTEX;
                  } else
                    result = AIOUSB_ERROR_INVALID_MUTEX;
                pthread_mutexattr_destroy(&mutexAttr);
            } else
              result = AIOUSB_ERROR_INVALID_MUTEX;
#else
          result  = AIODeviceTablePopulateTable();

#endif

    }
    return result;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_Exit()
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_INIT, AIOUSB_IsInit() );

    CloseAllDevices();
    libusb_exit(NULL);
#if defined(AIOUSB_ENABLE_MUTEX)
    pthread_mutex_destroy(&aiousbMutex);
#endif
    aiousbInit = 0;
    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_Reset( unsigned long DeviceIndex )
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    int libusbResult;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, (AIORESULT*)&retval );
    if ( retval != AIOUSB_SUCCESS )
        return retval;

    USBDevice *usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_DEVICE_NOT_CONNECTED, usb );

    libusbResult = usb->usb_reset_device(usb);
    if (libusbResult != LIBUSB_SUCCESS )
        retval = LIBUSB_RESULT_TO_AIOUSB_RESULT(libusbResult);
    usleep(250000);

    return retval;
}


#ifdef __cplusplus
}
#endif


#ifdef SELF_TEST
#include "gtest/gtest.h"

#include <stdlib.h>

using namespace AIOUSB;

TEST(AIODeviceTable,Cleanup) {
    int numDevices = 0;
    AIODeviceTableInit();
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_AIO16_16A );
    ClearAIODeviceTable( numDevices );

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

    EXPECT_TRUE( AIOUSBDeviceGetADCConfigBlock( (AIOUSBDevice *)&deviceTable[0] ) );

    EXPECT_EQ( (AIOUSBDevice *)&deviceTable[0], ADCConfigBlockGetAIOUSBDevice( AIOUSBDeviceGetADCConfigBlock( (AIOUSBDevice *)&deviceTable[0] ), NULL ));
    ClearAIODeviceTable( numDevices );
}

TEST(AIODeviceTable, AddingDeviceSetsInit )
{
    int numDevices = 0;
    int stclock_rate = 1000;
    AIODeviceTableInit();

    AIODeviceTableAddDeviceToDeviceTable( &numDevices , USB_AIO16_16A );
    ASSERT_EQ( deviceTable[0].cachedConfigBlock.clock_rate, stclock_rate );
    ASSERT_EQ( AIOUSB_IsInit(), AIOUSB_TRUE );

    ClearAIODeviceTable( numDevices );

}


#define DEVICE_POPULATOR_INTERFACE(T)                                   \
    AIORET_TYPE (*get_device_ids)( T *self );                           \
    unsigned long *products;                                            \
    int size;
#ifndef SWIG
typedef struct device_populator {
    DEVICE_POPULATOR_INTERFACE(struct device_populator );
    libusb_device *usb_device;
    libusb_device **deviceList;
    int numDevices;
    int numAccesDevices;
} AIODevicePopulator;
#endif

typedef struct test_populator
{
    DEVICE_POPULATOR_INTERFACE(struct device_populator );
} TestPopulator;

AIORET_TYPE test_get_device_ids( AIODevicePopulator *self )
{
    char *tmp,*orig;
    if ( (tmp = getenv("AIODEVICETABLE_PRODUCT_IDS" )) ) {
        self->size = 0;
        tmp = strdup( tmp );
        orig = tmp;
        char delim[] = ",";
        char *pos = NULL;
        char *savepos;
        for ( char *token = strtok_r( tmp, delim, &pos );  token ; token = strtok_r(NULL, delim , &pos) ) {
            if (token == NULL)
                break;
            if ( strlen(token) > 3 && strncmp(token,"USB",3 ) == 0 ) {
                unsigned int tmpproduct = ProductNameToID( token );
                if ( tmpproduct ) {
                    self->size ++;
                    self->products = (unsigned long *)realloc( self->products, (self->size)*sizeof(unsigned long)) ;
                    self->products[self->size-1] = tmpproduct;
                }
            }
        }

        free(orig);

    } else {
        self->products = (unsigned long *)malloc( (self->size)*sizeof(unsigned long ) );
        self->size = 2;
        self->products[0] = USB_AIO16_16A;
        self->products[1] = USB_DIO_32;
    }
}

TEST(AIODeviceTable, InitAddsConfigToDevicePointer )
{
    int numDevices = 0;
    AIOUSB_BOOL tmp;
    AIORESULT result;
    AIODeviceTableInit();
    result = AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_AI16_16E, NULL );
    EXPECT_EQ( result, AIOUSB_SUCCESS );
    AIOUSBDevice *dev = AIODeviceTableGetDeviceAtIndex( numDevices - 1 , &result );
    EXPECT_EQ( result, AIOUSB_SUCCESS );
    EXPECT_TRUE( dev );

    ClearAIODeviceTable( numDevices );

}


TEST(AIODeviceTable, SetsUpDefaults )
{
    int numDevices = 0;
    AIOUSB_BOOL tmp;
    AIORESULT retval;

    retval = AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_AI16_16E, NULL );
    EXPECT_EQ( retval, AIOUSB_SUCCESS );
    AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_DIO_32, NULL );
    EXPECT_EQ( retval, AIOUSB_SUCCESS );

    EXPECT_EQ( ((AIOUSBDevice *)&deviceTable[1])->DIOBytes, 4  );
    ClearAIODeviceTable( numDevices );
}

TEST(AIODeviceTable,IncorrectIndices)
{
    int numDevices = 0;
    AIORET_TYPE result;
    AIOUSB_BOOL tmp;
    AIODeviceTableInit();
    result = AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_AI16_16E, NULL );
    AIOUSBDevice *blah = AIODeviceTableGetAIOUSBDeviceAtIndex( 0 );

    ASSERT_TRUE( blah );


    blah = AIODeviceTableGetAIOUSBDeviceAtIndex( 55 );

    ASSERT_FALSE( blah );

    ASSERT_LT( errno, AIOUSB_SUCCESS );

    ClearAIODeviceTable( numDevices );
}


int
main(int argc, char *argv[] )
{

  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
#ifdef GTEST_TAP_PRINT_TO_STDOUT
  delete listeners.Release(listeners.default_result_printer());
#endif

  return RUN_ALL_TESTS();

}


#endif






