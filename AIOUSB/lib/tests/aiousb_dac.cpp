#include "aiousb.h"

#include "gtest/gtest.h"

using namespace AIOUSB;
TEST(DAC, RangeChecking )
{
    int di = 0;
    unsigned short data[32] = {0};
    int datacount = 0;
    AIORESULT retval = 0;
    int numDevices = 0;
    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_AO16_16A );
    
    retval = DACMultiDirect(di, data, datacount );
    ASSERT_EQ( retval, AIOUSB_SUCCESS );

    datacount = 16;
    deviceTable[di].bDACOpen = AIOUSB_TRUE;
    deviceTable[di].bDACSlowWaveStream = AIOUSB_TRUE;

    retval = DACMultiDirect(di, data, datacount );

    ASSERT_EQ( AIOUSB_ERROR_OPEN_FAILED, retval );
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
