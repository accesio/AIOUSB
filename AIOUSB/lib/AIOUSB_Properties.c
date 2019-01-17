/**
 * @file   AIOUSB_Properties.c
 * @author $Author$
 * @date   $Date$
 * @copy
 * @brief ACCES I/O USB Property utilities for Linux. These functions assist with identifying
 *       cards and verifying the devices attached are the correct type of card.
 *
 * @todo Implement a friendly FindDevices() function as well as FindDeviceByCriteria() 
 *       function to replace all of the standard looping while ( deviceMask != 0 )...
 */

#include "AIOUSB_Properties.h"
#include "AIODeviceTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "AIOList.h"

#ifdef __cplusplus
namespace AIOUSB {
#endif



/*----------------------------------------------------------------------------*/
#define RESULT_TEXT_SIZE 40


/**
 * @brief AIOUSB result codes
 */
static struct ResultCodeName {
    unsigned int result;
    char text[ RESULT_TEXT_SIZE + 2 ];
} resultCodeTable[] = {
    { AIOUSB_SUCCESS                                             , "AIOUSB_SUCCESS"                     },
    { AIOUSB_ERROR_DEVICE_NOT_CONNECTED                          , "AIOUSB_ERROR_DEVICE_NOT_CONNECTED"  },
    { AIOUSB_ERROR_DUP_NAME                                      , "AIOUSB_ERROR_DUP_NAME"              },
    { AIOUSB_ERROR_FILE_NOT_FOUND                                , "AIOUSB_ERROR_FILE_NOT_FOUND"        },
    { AIOUSB_ERROR_INVALID_DATA                                  , "AIOUSB_ERROR_INVALID_DATA"          },
    { AIOUSB_ERROR_INVALID_INDEX                                 , "AIOUSB_ERROR_INVALID_INDEX"         },
    { AIOUSB_ERROR_INVALID_MUTEX                                 , "AIOUSB_ERROR_INVALID_MUTEX"         },
    { AIOUSB_ERROR_INVALID_PARAMETER                             , "AIOUSB_ERROR_INVALID_PARAMETER"     },
    { AIOUSB_ERROR_INVALID_THREAD                                , "AIOUSB_ERROR_INVALID_THREAD"        },
    { AIOUSB_ERROR_NOT_ENOUGH_MEMORY                             , "AIOUSB_ERROR_NOT_ENOUGH_MEMORY"     },
    { AIOUSB_ERROR_NOT_SUPPORTED                                 , "AIOUSB_ERROR_NOT_SUPPORTED"         },
    { AIOUSB_ERROR_OPEN_FAILED                                   , "AIOUSB_ERROR_OPEN_FAILED"           },

                                /* SPECIAL CASE that should not occur, but supposedly can */
    { AIOUSB_ERROR_LIBUSB                                        , "AIOUSB_ERROR_LIBUSB"                },
                                /* LIBUSB result codes */
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_IO)            , "LIBUSB_ERROR_IO"                    },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_INVALID_PARAM) , "LIBUSB_ERROR_INVALID_PARAM"         },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_ACCESS)        , "LIBUSB_ERROR_ACCESS"                },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_NO_DEVICE)     , "LIBUSB_ERROR_NO_DEVICE"             },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_NOT_FOUND)     , "LIBUSB_ERROR_NOT_FOUND"             },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_BUSY)          , "LIBUSB_ERROR_BUSY"                  },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_TIMEOUT)       , "LIBUSB_ERROR_TIMEOUT"               },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_OVERFLOW)      , "LIBUSB_ERROR_OVERFLOW"              },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_PIPE)          , "LIBUSB_ERROR_PIPE"                  },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_INTERRUPTED)   , "LIBUSB_ERROR_INTERRUPTED"           },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_NO_MEM)        , "LIBUSB_ERROR_NO_MEM"                },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_NOT_SUPPORTED) , "LIBUSB_ERROR_NOT_SUPPORTED"         },
    { LIBUSB_RESULT_TO_AIOUSB_RESULT(LIBUSB_ERROR_OTHER)         , "LIBUSB_ERROR_OTHER"                 }
};

#ifdef __cplusplus
const int NUM_RESULT_CODES = sizeof(resultCodeTable) / sizeof(resultCodeTable[ 0 ]);
#else
#define NUM_RESULT_CODES (sizeof(resultCodeTable) / sizeof(resultCodeTable[ 0 ]))
#endif

/*----------------------------------------------------------------------------*/
int non_usb_supported_device(int minProductID, int maxProductID, int maxDevices, int *deviceList)
{
    return minProductID < 0 ||
           minProductID > 0xffff ||
           maxProductID < minProductID ||
           maxProductID > 0xffff ||
           maxDevices < 1 ||
           maxDevices > 127 ||     // sanity check; USB can support only 127 devices on a single bus
           deviceList == NULL;
}

/*----------------------------------------------------------------------------*/
/**
 * @param minProductID 
 * @param maxProductID 
 * @param maxDevices 
 * @param deviceList  [ 1 + maxDevices * 2 ] 
 * @return 
 */
unsigned long AIOUSB_GetDeviceByProductID(int minProductID,
                                          int maxProductID,
                                          int maxDevices,
                                          int *deviceList
                                          ) 
{
    if(non_usb_supported_device(minProductID, maxProductID, maxDevices, deviceList))
        return AIOUSB_ERROR_INVALID_PARAMETER;

    int index, numDevices = 0;

    for(index = 0; index < MAX_USB_DEVICES && numDevices < maxDevices; index++) {
        if( deviceTable[ index ].usb_device != NULL &&
            deviceTable[ index ].ProductID >= ( unsigned )minProductID &&
            deviceTable[ index ].ProductID <= ( unsigned )maxProductID
            ) {
            /**< deviceList[] contains device index-product ID pairs, one pair per device found */
            deviceList[ 1 + numDevices * 2 ] = index;
            deviceList[ 1 + numDevices * 2 + 1 ] = ( int )deviceTable[ index ].ProductID;
            numDevices++;
        }
    }
    deviceList[ 0 ] = numDevices;

    return AIOUSB_SUCCESS;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_GetDeviceSerialNumber( unsigned long  DeviceIndex ) 
{
    uint64_t val = 0;
    unsigned long retval;
    retval = GetDeviceSerialNumber( DeviceIndex, &val );
    if( retval != AIOUSB_SUCCESS ) {
      return -1*(AIORET_TYPE)retval;
    } else {
      return (AIORET_TYPE)val;
    }
}

/**
 * @brief 
 * @param DeviceIndex 
 * @param pSerialNumber 
 * @return 0 if successful, otherwise
 */
AIORESULT GetDeviceSerialNumber(unsigned long DeviceIndex, uint64_t *pSerialNumber ) 
{
    if( !pSerialNumber )
        return AIOUSB_ERROR_INVALID_PARAMETER;

    unsigned long bytes_read = sizeof(uint64_t);
    uint64_t buffer_data = -1;
    AIORESULT result = AIOUSB_SUCCESS;

    AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    result = GenericVendorRead( DeviceIndex, AUR_EEPROM_READ , EEPROM_SERIAL_NUMBER_ADDRESS, 0 , &buffer_data, &bytes_read );

    if( result != AIOUSB_SUCCESS )
        goto out_GetDeviceSerialNumber;

    *pSerialNumber = buffer_data;

out_GetDeviceSerialNumber:

    return result;
}

/*----------------------------------------------------------------------------*/
unsigned long GetDeviceBySerialNumber(uint64_t serialNumber) 
{
    unsigned long deviceIndex = diNone;

    if (serialNumber == 0 )
        return deviceIndex;

    int index;
    for(index = 0; index < MAX_USB_DEVICES; index++) {
          if(deviceTable[ index ].usb_device != NULL) {

                uint64_t deviceSerialNumber;
                unsigned long result = GetDeviceSerialNumber(index, &deviceSerialNumber);

                if( result == AIOUSB_SUCCESS && deviceSerialNumber == serialNumber ) {
                      deviceIndex = index;
                      break;
                  }
                /**
                 * else, even if we get an error requesting the serial number from
                 * this device, keep searching
                 */
          }
      }

    return deviceIndex;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Friendly function that can be called first. It 
 * @param where 
 * @param length 
 * @param is_ok_device 
 * 
 * @return 
 */
AIORET_TYPE AIOUSB_FindDevices( int **where, int *length , AIOUSB_BOOL (*is_ok_device)( AIOUSBDevice *dev )  )
{
    AIO_ASSERT( where );
    AIO_ASSERT( length );
    if ( !AIOUSB_IsInit() )
        AIOUSB_Init();

    unsigned long deviceMask = AIOUSB_GetAllDevices();
    static int indices[MAX_USB_DEVICES];
    int index = 0;
    AIORESULT retval = AIOUSB_ERROR_DEVICE_NOT_FOUND;
    *length = 0;

    while ( deviceMask  ) {
        if ( deviceMask & 1 ) {
            if ( is_ok_device( &deviceTable[index] ) == AIOUSB_TRUE )  { 
                retval = AIOUSB_SUCCESS;
                indices[(*length)++] = index;
            }
        }
        index++;
        deviceMask >>= 1;
    }
    if ( retval == AIOUSB_SUCCESS )
        *where = indices;
    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_FindDeviceIndicesByGroup( intlist *indices, AIOProductGroup *pg )
{
    AIO_ASSERT( indices );
    AIO_ASSERT( pg ); 
    if ( !AIOUSB_IsInit() )
        AIOUSB_Init();

    int index = 0;
    unsigned long deviceMask = AIOUSB_GetAllDevices();

    AIORET_TYPE retval = -AIOUSB_ERROR_DEVICE_NOT_FOUND;
    while ( deviceMask ) {
        if ( deviceMask & 1 ) {
            AIOUSBDevice *dev = &deviceTable[index];
            if ( AIOProductGroupContains( pg, dev->ProductID ) >= AIOUSB_SUCCESS ) {
                retval = AIOUSB_SUCCESS;
                TailQListintInsert( indices , NewTailQListEntryint( index ));
            }
        }
        index++;
        deviceMask >>= 1;
    }

    DeleteAIOProductGroup( pg );

    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_FindDevicesByGroup( int **where, int *length, AIOProductGroup *pg )
{
    AIO_ASSERT( where );
    AIO_ASSERT( length );
    AIO_ASSERT( pg );
    if ( !AIOUSB_IsInit() )
        AIOUSB_Init();
    AIORET_TYPE retval = -AIOUSB_ERROR_DEVICE_NOT_FOUND;
    unsigned long deviceMask = AIOUSB_GetAllDevices();
    static int indices[MAX_USB_DEVICES];

    int index = 0;

    *length = 0;

    while ( deviceMask  ) {
        if ( deviceMask & 1 ) {
            AIOUSBDevice *dev = &deviceTable[index];
            if ( AIOProductGroupContains( pg, dev->ProductID ) >= AIOUSB_SUCCESS ) {
                retval = AIOUSB_SUCCESS;
                indices[(*length)++] = index;
            }
        }
        index++;
        deviceMask >>= 1;
    }
    if ( retval == AIOUSB_SUCCESS )
        *where = indices;

    DeleteAIOProductGroup( pg );
    return retval;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief AIOUSB_GetDeviceProperties() returns a richer amount of information 
 * than QueryDeviceInfo()
 */
unsigned long AIOUSB_GetDeviceProperties(unsigned long DeviceIndex, DeviceProperties *properties ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    if(properties == 0)
        return AIOUSB_ERROR_INVALID_PARAMETER;
    
    properties->Name = deviceDesc->cachedName; /* if NULL, name will be requested from device */
    properties->SerialNumber = deviceDesc->cachedSerialNumber; /* if 0, serial number will be requested from device */
    properties->ProductID = deviceDesc->ProductID;
    properties->DIOPorts = deviceDesc->DIOBytes;
    properties->Counters = deviceDesc->Counters;
    properties->Tristates = deviceDesc->Tristates;
    properties->RootClock = deviceDesc->RootClock;
    properties->DACChannels = deviceDesc->ImmDACs;
    properties->ADCChannels = deviceDesc->ADCChannels;
    properties->ADCMUXChannels = deviceDesc->ADCMUXChannels;
    properties->ADCChannelsPerGroup = deviceDesc->ADCChannelsPerGroup;

    if(properties->Name == NULL)
        properties->Name = GetSafeDeviceName(DeviceIndex);
    if(properties->SerialNumber == 0)
        result = GetDeviceSerialNumber(DeviceIndex, &properties->SerialNumber);

    return result;
}
/*----------------------------------------------------------------------------*/
static int CompareResultCodes(const void *p1, const void *p2)
{
    assert(p1 != 0 &&
           (*( struct ResultCodeName** )p1) != 0 &&
           p2 != 0 &&
           (*( struct ResultCodeName** )p2) != 0);
    const unsigned int result1 = (*( struct ResultCodeName** )p1)->result,
                       result2 = (*( struct ResultCodeName** )p2)->result;
    if(result1 < result2)
        return -1;
    else if(result1 > result2)
        return 1;
    else
        return 0;
}

/*----------------------------------------------------------------------------*/
const char *AIOUSB_GetResultCodeAsString(unsigned long result_value)
{
    const char *resultText = "UNKNOWN";
    static struct ResultCodeName *resultCodeIndex[ NUM_RESULT_CODES ];  /* index of result codes in resultCodeTable[] */
    unsigned long INIT_PATTERN = 0x100c48b9ul; /* random pattern */
    static unsigned long resultCodeIndexCreated = 0; /* == INIT_PATTERN if index has been created */
    if(resultCodeIndexCreated != INIT_PATTERN) {
        /**
         * build index of result codes
         */
        int index;
        for(index = 0; index < NUM_RESULT_CODES; index++)
            resultCodeIndex[ index ] = &resultCodeTable[ index ];
        qsort(resultCodeIndex, NUM_RESULT_CODES, sizeof(struct ResultCodeName *), CompareResultCodes);
        resultCodeIndexCreated = INIT_PATTERN;
    }

    struct ResultCodeName key;                                  // key.name not used
    key.result = result_value;
    struct ResultCodeName *pKey = &key;
    struct ResultCodeName **resultCode = ( struct ResultCodeName** )bsearch(&pKey, 
                                                                            resultCodeIndex, 
                                                                            NUM_RESULT_CODES, 
                                                                            sizeof(struct ResultCodeName *), 
                                                                            CompareResultCodes
                                                                            );
    if(resultCode != 0)
        resultText = (*resultCode)->text;

    return resultText;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_ShowDevices( AIODisplayType display_type )
{
    int found = 0;

    switch ( display_type ) {
    case BASIC:
        fprintf(stderr,"ACCES devices\n");
        break;
    case TERSE:
        fprintf(stderr,"AIODevices:\n");
        break;
    case JSON:
        printf("{\"devices\":[");
        break;
    case YAML:
        printf("---\n");
        printf("devices:\n");
        break;
    default:
        break;
    }
    int previous=0;
    for(int index = 0; index < MAX_USB_DEVICES; index++) {
        if ( deviceTable[ index ].usb_device ) {
            int MAX_NAME_SIZE = 100;
            char name[ MAX_NAME_SIZE + 1 ];
            unsigned long productID;
            unsigned long nameSize = MAX_NAME_SIZE;
            unsigned long numDIOBytes;
            unsigned long numCounters;

            unsigned long result = QueryDeviceInfo(index, &productID, &nameSize, name, &numDIOBytes, &numCounters);
            /* Get the serial number */
            uint64_t serNum = 0; 
            GetDeviceSerialNumber( index, &serNum );
            if (result == AIOUSB_SUCCESS) {
                name[ nameSize ] = '\0';
                if (!found) {
                    /* Print heading before the first device found */
                    switch (display_type ) { 
                    case BASIC:
                        fprintf(stderr," found:\n");
                        break;
                    default:
                        break;
                    } 
                    found = AIOUSB_TRUE;
                }
                switch ( display_type ) {
                case BASIC:
                    fprintf(stderr,"  Device at index %d:\n  Product ID: %#.4lx\n  Product name: %s\n  Number of digital I/O bytes: %lu\n  Number of counters: %lu\n  SerialNumber: %#lx\n",
                            index,
                            productID,
                            name,
                            (unsigned long)numDIOBytes,
                            (unsigned long)numCounters,
                            (unsigned long)serNum
                           );
                    break;
                case TERSE:
                    fprintf(stderr,"index=%d,product_id=%#lx,product_name=%s,numIO=%lu,numCounters=%lu,serailNumber=%lu\n",index,productID,name,(unsigned long)numDIOBytes,(unsigned long)numCounters, (unsigned long)serNum);
                    break;
                case JSON:
                    if ( previous ) 
                        printf(",");
                    printf("{\"%s\":\"%d\",\"%s\":\"%#lx\",\"%s\":\"%s\",\"%s\":%lu,\"%s\":%lu,\"%s\":%lu}",
                           "index", index, "product_id",productID,"product_name",name,"numIO",(unsigned long )numDIOBytes,"numCounters",(unsigned long)numCounters,"serial_number",(unsigned long)serNum);
                    previous = 1;
                    break;
                case YAML:
                    printf("  - index: %d\n    numCounters: %lu\n    numIO: %lu\n    product_id: %#lx\n    product_name: %s\n    serial_number: %lu\n",
                           index,
                           (unsigned long)numCounters,
                           (unsigned long)numDIOBytes,
                           (unsigned long)productID,
                           name,
                           (unsigned long)serNum
                           );
                           
                    break;
                default:
                    break;
                }
            
            }
        }
    }
    switch ( display_type ) {
    case JSON:
        printf("]}");
        break;
    case YAML:
        printf("\n");
        break;
    default:
        break;
    }

    return found;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE  AIOUSB_ListDevices() 
{
    if ( !AIOUSB_IsInit() )
        AIOUSB_Init();
    AIORET_TYPE found = AIOUSB_SUCCESS;
    found = AIOUSB_ShowDevices( BASIC );
    if ( found < AIOUSB_SUCCESS )
        printf("No ACCES devices found\n");
    return found;
}

#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST
#include "gtest/gtest.h"

#include <stdlib.h>

using namespace AIOUSB;


TEST(AIODeviceTable, SetsUpDefaults )
{
    int numDevices = 0;
    AIOUSB_BOOL tmp;
    AIORESULT retval;
    AIORET_TYPE ret;
    int *indices = 0;
    int length = 0;

    AIODeviceTableInit();    
    retval = AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_AI16_16E, NULL );
    EXPECT_EQ( retval, AIOUSB_SUCCESS );
    AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_DIO_32, NULL );
    EXPECT_EQ( retval, AIOUSB_SUCCESS );

    ret = AIOUSB_FindDevicesByGroup( &indices, &length, AIO_ANALOG_INPUT() );

    EXPECT_GE( ret, AIOUSB_SUCCESS );
    EXPECT_EQ( length, 1 );

    ret = AIOUSB_FindDevicesByGroup( &indices, &length, NewAIOProductGroup(1,AIO_RANGE(USB_DIO_32,USB_AI16_16E)));    

    EXPECT_GE( ret, AIOUSB_SUCCESS );
    EXPECT_EQ( length, 2 );

    ClearAIODeviceTable( numDevices );
}

TEST(AIODeviceTable, FindIndices )
{
    int numDevices = 0;
    AIOUSB_BOOL tmp;
    AIORESULT retval;
    AIORET_TYPE ret;

    int length = 0;
    intlist *indices = Newintlist();

    AIODeviceTableInit();    
    retval = AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_AI16_16E, NULL );
    EXPECT_EQ( retval, AIOUSB_SUCCESS );
    AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_DIO_32, NULL );
    EXPECT_EQ( retval, AIOUSB_SUCCESS );
    retval = AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numDevices, USB_AIO16_16A, NULL );
    EXPECT_EQ( retval, AIOUSB_SUCCESS );

    ret =  AIOUSB_FindDeviceIndicesByGroup( indices, AIO_ANALOG_INPUT() );

    char *tmpstr;
    ASSERT_STREQ( (tmpstr=intlistToString( indices )), "0,2");
    free(tmpstr);

    ret = Deleteintlist( indices );

    ASSERT_EQ( AIOUSB_SUCCESS, ret );

    ClearAIODeviceTable( numDevices );
}


int 
main(int argc, char *argv[] )
{
    testing::InitGoogleTest(&argc, argv);
    testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
    return RUN_ALL_TESTS();  
}


#endif
