#include "AIODeviceInfo.h"
#include "AIODeviceTable.h"
#include "AIOUSBDevice.h"
#include "AIOUSB_Core.h"

#ifdef __cplusplus
namespace AIOUSB {
#endif

AIODeviceInfo *NewAIODeviceInfo() {
    AIODeviceInfo *tmp = (AIODeviceInfo *)malloc(sizeof(AIODeviceInfo));
    return tmp;
}

/*----------------------------------------------------------------------------*/
void DeleteAIODeviceInfo( AIODeviceInfo *di ) 
{ 
    if ( di ) {
        if (di->Name )
            free(di->Name);
        free(di);
    }
}

/*----------------------------------------------------------------------------*/
const char *AIODeviceInfoGetName( AIODeviceInfo *di )
{
    if ( di ) 
        return di->Name;
    return NULL;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIODeviceInfoGetCounters( AIODeviceInfo *di ) 
{
    if ( di )
        return di->Counters;
    return -AIOUSB_ERROR_INVALID_PARAMETER;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIODeviceInfoGetDIOBytes( AIODeviceInfo *di ) 
{
    if ( di ) 
        return di->DIOBytes;
    return -AIOUSB_ERROR_INVALID_PARAMETER;
}

/*----------------------------------------------------------------------------*/
AIODeviceInfo *AIODeviceInfoGet( unsigned long DeviceIndex )
{
    AIORESULT result = AIOUSB_SUCCESS;

    AIODeviceInfo *tmp = NewAIODeviceInfo();
    if ( !tmp ) {
        aio_errno = -AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
        return NULL;
    }
    memset(tmp,0,sizeof(AIODeviceInfo));

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS ) {
        free(tmp);
        return NULL;
    }

    tmp->PID       = deviceDesc->ProductID;
    tmp->DIOBytes  = deviceDesc->DIOBytes;
    tmp->Counters  = deviceDesc->Counters;

    const char *deviceName = ProductIDToName( tmp->PID );
    if ( deviceName ) {
        tmp->Name = strdup( deviceName );
    }

    return tmp;

}

#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST

#include <math.h>
#include "gtest/gtest.h"

#include "AIODeviceTable.h"
#include "AIODeviceInfo.h"
#include "AIOUSB_Core.h"


#ifdef __cplusplus
using namespace AIOUSB;
#endif

TEST(AIODeviceInfo,Get_Defaults) {
    /* Mock object that looks like a DeviceTable */
    unsigned long products[] = {USB_AIO16_16A, USB_DIO_32};
    AIODeviceTablePopulateTableTest( products, 2 );
    AIODeviceInfo *tmp = AIODeviceInfoGet( 0 );

    EXPECT_TRUE(tmp);


    EXPECT_STREQ(AIODeviceInfoGetName(tmp), "USB-AIO16-16A" );
    EXPECT_EQ(AIODeviceInfoGetCounters(tmp), 1 );
    EXPECT_EQ(AIODeviceInfoGetDIOBytes(tmp), 2 );

    tmp =  AIODeviceInfoGet( 1 );
    EXPECT_TRUE( tmp );
    EXPECT_STREQ(AIODeviceInfoGetName(tmp), "USB-DIO-32" );
    EXPECT_EQ(3, AIODeviceInfoGetCounters(tmp) );
    EXPECT_EQ(4, AIODeviceInfoGetDIOBytes(tmp) );

    DeleteAIODeviceInfo( tmp );
}

/**
 * @brief Verifies that the two device index constants work as 
 * they should.
 */
TEST( AIODeviceInfo, CheckDeviceConstants ) 
{
    unsigned long products[] = {USB_AIO16_16A, USB_DIO_32};
    AIODeviceTablePopulateTableTest( products, 2 );
    AIODeviceInfo *tmp = AIODeviceInfoGet( diFirst  );
    EXPECT_TRUE( tmp );

    tmp = AIODeviceInfoGet( diOnly );
    EXPECT_FALSE( tmp );
    
    AIODeviceTableClearDevices();

    tmp = AIODeviceInfoGet( diFirst  );
    EXPECT_FALSE( tmp );

    tmp = AIODeviceInfoGet( diFirst  );
    EXPECT_FALSE( tmp );
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
