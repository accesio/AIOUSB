#include "aiousb.h"

#include "gtest/gtest.h"

using namespace AIOUSB;
TEST(DIO, APIChecks )
{
 
    unsigned long DeviceIndex = 0;
    unsigned char bTristate = 2;
    AIOChannelMask *mask = NULL;
    DIOBuf *buf = NULL;
    AIORESULT result = AIOUSB_SUCCESS;
    int numDevices = 0;

    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_DIO_32 );
    
    result = AIOUSB::DIO_ConfigureWithDIOBuf(DeviceIndex,bTristate,mask,buf ); 
    ASSERT_EQ( -AIOUSB_ERROR_INVALID_PARAMETER, result );
    bTristate = AIOUSB_TRUE;
    result = AIOUSB::DIO_ConfigureWithDIOBuf(DeviceIndex,bTristate,mask,buf ); 
    ASSERT_EQ( -AIOUSB_ERROR_INVALID_AIOCHANNELMASK, result );
    
    result = AIOUSB::DIO_ConfigureWithDIOBuf(DeviceIndex,bTristate,(AIOChannelMask*)42,buf ); 
    ASSERT_EQ( -AIOUSB_ERROR_INVALID_DIOBUF, result );

    result = AIOUSB::DIO_ConfigureWithDIOBuf(DeviceIndex,bTristate,(AIOChannelMask*)42,(DIOBuf*)42 ); 
    ASSERT_EQ( -AIOUSB_ERROR_INVALID_USBDEVICE, result ) << "No USBdevice has been setup, expect -AIOUSB_ERROR_INVALID_USBDEVICE";

    deviceTable[0].LastDIOData = 0;
    deviceTable[0].usb_device = (USBDevice *)42;
    result = AIOUSB::DIO_ConfigureWithDIOBuf(DeviceIndex,bTristate,(AIOChannelMask*)42,(DIOBuf*)42 ); 

    ASSERT_EQ( -AIOUSB_ERROR_NOT_ENOUGH_MEMORY, result );
}

int main(int argc, char *argv[] )
{
    testing::InitGoogleTest(&argc, argv);
    testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
#ifdef GTEST_TAP_PRINT_TO_STDOUT
    delete listeners.Release(listeners.default_result_printer());
#endif

    return RUN_ALL_TESTS();  
}
