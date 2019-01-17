/**
 * @file   AIODeviceQuery.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief A simple structure for querying a USB card .  This provides a simpler interface for more 
 * complicated queries going forward.
 *
 */

#include "AIODeviceQuery.h"
#include "AIODeviceTable.h"


#ifdef __cplusplus
namespace AIOUSB {
#endif

/*------------------------------------------------------------------------*/
/**
 * @brief Constructor of a AIODeviceQuery, and using the DeviceIndex , queries
 * the device at that index.
 * @param DeviceIndex 
 * @return AIODeviceQuery * object
 */
AIODeviceQuery *NewAIODeviceQuery( unsigned long DeviceIndex )
{
    AIORET_TYPE retval;
    AIODeviceQuery *nq = (AIODeviceQuery *)calloc(1,sizeof(AIODeviceQuery));
    AIO_ASSERT_RET( NULL, nq );
    int MAX_NAME_SIZE = 20;
    nq->nameSize = MAX_NAME_SIZE;

    char name[ MAX_NAME_SIZE + 2 ];

    retval = QueryDeviceInfo( DeviceIndex, &nq->productID, &nq->nameSize, name, &nq->numDIOBytes, &nq->numCounters );
    if ( retval != AIOUSB_SUCCESS ) {
        free(nq);
        return NULL;
    }
    nq->name = strdup( name );
    nq->index = DeviceIndex;
    return nq;
}

/*------------------------------------------------------------------------*/
/**
 * @brief Destructor for AIODeviceQuery *
 * @param devq AIODeviceQuery *
 * @return >= 0, success, otherwise error
 */
AIORET_TYPE DeleteAIODeviceQuery( AIODeviceQuery *devq )
{
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_AIODEVICE_QUERY, devq );
    if ( devq->name )
        free(devq->name);
    
    free(devq);
    /* printf("Deleting AIODeviceQuery\n"); for testing destrouctor*/
    return AIOUSB_SUCCESS;
}

/*------------------------------------------------------------------------*/
/**
 * @brief Converts the AIODeviceQuery into a string representation
 * @param devq AIODeviceQuery *
 * @return String representing the Device query, NULL if not defined
 */
char *AIODeviceQueryToStr( AIODeviceQuery *devq )
{
    static char tmp[96];
    AIO_ASSERT_RET( NULL, devq );

    snprintf( tmp,sizeof(tmp),"ProductID: %#x\nProductName: %s\nProductNumCounters: %d\nProductNumDIOBytes: %d\n",
              (int)AIODeviceQueryGetProductID( devq ),
              AIODeviceQueryGetName( devq ),
              (int)AIODeviceQueryGetNumCounters( devq ),
              (int)AIODeviceQueryGetNumDIOBytes( devq )
              );

    return tmp;
}

/*------------------------------------------------------------------------*/
/**
 * @brief Repr version of this product
 * @param devq AIODeviceQuery *
 * @return String representing the Device query, NULL if not defined
 */
char *AIODeviceQueryToRepr( AIODeviceQuery *devq )
{
    static char tmp[34];
    AIO_ASSERT_RET( NULL, devq );

    snprintf( tmp,sizeof(tmp),"[ Index: %d, ProductID: %#x ]",
              (int)AIODeviceQueryGetIndex( devq ),
              (int)AIODeviceQueryGetProductID( devq )
              );

    return tmp;
}

/*------------------------------------------------------------------------*/
/**
 * @brief Returns the Product ID of the device in question
 * @param devq AIODeviceQuery *
 * @return >= 0, the product ID in question, otherwise error
 */
AIORET_TYPE AIODeviceQueryGetProductID( AIODeviceQuery *devq )
{
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_AIODEVICE_QUERY, devq );
    return devq->productID;
}

/*------------------------------------------------------------------------*/
/**
 * @brief Returns the Index associated with the AIODeviceQuery
 * @param devq AIODeviceQuery *
 * @return >= 0 index , otherwise error
 */ 
AIORET_TYPE AIODeviceQueryGetIndex( AIODeviceQuery *devq )
{
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_AIODEVICE_QUERY, devq );
    return devq->index;
}

/*------------------------------------------------------------------------*/
/**
 * @brief Returns the strlenght of the Device name of the device in question
 * @param devq AIODeviceQuery *
 * @return >= 0, the name length in question, otherwise error
 */
AIORET_TYPE AIODeviceQueryNameSize( AIODeviceQuery *devq )
{
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_AIODEVICE_QUERY, devq );
    return devq->nameSize;
}


/*------------------------------------------------------------------------*/
/**
 * @brief Returns the name of the Device at the index in question
 * @param devq AIODeviceQuery *
 * @return != 0 the name of the card, otherwise an error
 */
char * AIODeviceQueryGetName( AIODeviceQuery *devq )
{
    AIO_ASSERT_RET( NULL, devq );
    return devq->name;
}

/*------------------------------------------------------------------------*/
/**
 * @brief Returns number of Digital bytes for the device in question
 * @param devq AIODeviceQuery *
 * @return >= 0 the number of dio bytes of the card, otherwise an error
 */
AIORET_TYPE AIODeviceQueryGetNumDIOBytes( AIODeviceQuery *devq )
{
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_AIODEVICE_QUERY, devq );
    return devq->numDIOBytes;
}
/**
 * @brief Returns number of Counters for the device in question
 * @param devq AIODeviceQuery *
 * @return != 0 the name of the card, otherwise an error
 */
AIORET_TYPE AIODeviceQueryGetNumCounters( AIODeviceQuery *devq )
{
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_AIODEVICE_QUERY, devq );
    return devq->numCounters;
}




#ifdef __cplusplus
}
#endif

/*****************************************************************************
 * Self-test
 * @note This section is for stress testing the Continuous buffer in place
 * without using the USB features
 *
 ****************************************************************************/

#ifdef SELF_TEST
#include "gtest/gtest.h"

#include "AIODeviceTable.h"
#include <stdlib.h>

using namespace AIOUSB;

TEST(AIODeviceQuery, VerifyQuery) {
    int numDevices = 0;
    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_AIO16_16A );
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_DIO_48 );

    ASSERT_EQ( numDevices, 2 );
    AIODeviceQuery *ndev = NewAIODeviceQuery( 0 );
    ASSERT_TRUE( ndev );

    ASSERT_EQ( ndev->productID, USB_AIO16_16A );

    ASSERT_EQ( AIODeviceQueryGetNumDIOBytes(ndev), 2 );
    ASSERT_EQ( AIODeviceQueryGetNumCounters(ndev), 1 );

    ASSERT_STREQ( AIODeviceQueryGetName(ndev), "USB-AIO16-16A");

    DeleteAIODeviceQuery( ndev );

    ndev = NewAIODeviceQuery( 1 );
    ASSERT_STREQ( AIODeviceQueryGetName(ndev), "USB-DIO-48");

    DeleteAIODeviceQuery( ndev );
    ClearAIODeviceTable( numDevices );
    
}

TEST(AIODeviceQuery,StringRepresentation)
{
    int numDevices = 0;
    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_AIO16_16A );

    AIODeviceQuery *ndev = NewAIODeviceQuery( 0 );

    char *tmpstr = AIODeviceQueryToStr( ndev );
    ASSERT_TRUE( tmpstr );
    
    /* Cleanup */

    DeleteAIODeviceQuery( ndev );
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
