#include "AIOCommandLine.h"
#include "gtest/gtest.h"
#include "tap.h"

using namespace AIOUSB;

TEST(Properties,FindingDevices )
{
    int numDevices = 0;
    int *indices;
    int num_devices;
    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_AIO16_16A );
    EXPECT_EQ( numDevices, 1 );

    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_DIO_32 );
    EXPECT_EQ( numDevices, 2 );

#if FANCY_LAMBDA

    AIOUSB_FindDevices( &indices, &num_devices,  LAMBDA( AIOUSB_BOOL, (AIOUSBDevice *dev), {
                std::cout << "Using fancy lambda\n";
                if ( dev->ProductID < USB_AIO16_16A || 
                     dev->ProductID > USB_AIO16_16A ) 
                    return AIOUSB_FALSE;
                else
                    return AIOUSB_TRUE;
            }
            )
            );

#else
    AIOUSB_FindDevices( &indices, &num_devices,  [](AIOUSBDevice *dev)->AIOUSB_BOOL {
            if ( dev->ProductID < USB_AIO16_16A || 
                 dev->ProductID > USB_AIO16_16A ) 
                return AIOUSB_FALSE;
            else
                return AIOUSB_TRUE;
        }
        );
#endif
    
    EXPECT_EQ( 1, num_devices ) << "Found only the one device we wanted\n";

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
