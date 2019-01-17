#include <iostream>
#include "AIOTypes.h"
#include "USBDevice.h"
#include "libusb.h"
#include "AIODeviceTable.h"
#include "AIOEither.h"


#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace AIOUSB;

using ::testing::Return;
using ::testing::_;


struct USBMock : public USBDevice {
    MOCK_METHOD8(usb_control_transfer,  int(USBDevice *, uint8_t, uint8_t, uint16_t, uint16_t, unsigned char *, uint16_t, unsigned int ));
};

TEST(USBDevice,TestMock)
{
    ASSERT_EQ(1,1);
    USBMock foo;
    EXPECT_CALL( foo, usb_control_transfer( &foo, 0,0,0,0,0,0,0 ))
        .WillOnce( Return(2) )
        .WillOnce( Return(0) );
    int val = foo.usb_control_transfer( &foo, 0,0,0,0,0,0,0 );
    EXPECT_EQ( 2, val );
    val = foo.usb_control_transfer( &foo, 0,0,0,0,0,0,0 );
    EXPECT_EQ( 0, val );
    USBMock *bar = new USBMock();
    EXPECT_CALL( *bar, usb_control_transfer( bar, 0,0,0,0,0,0,0 ))
        .WillOnce( Return(1) )
        .WillOnce( Return(2) );
    val = bar->usb_control_transfer( bar, 0,0,0,0,0,0,0 );
    EXPECT_EQ( 1, val );
    val = bar->usb_control_transfer( bar, 0,0,0,0,0,0,0 );
    EXPECT_EQ( 2, val );

    delete bar;
}

int
main(int argc, char *argv[] )
{

  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
  return RUN_ALL_TESTS();  
}






