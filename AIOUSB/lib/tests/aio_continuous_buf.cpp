/*****************************************************************************
 * Self-test 
 * @note This section is for stress testing the Continuous buffer in place
 * without using the USB features
 *
 ****************************************************************************/ 

#include "aiousb.h"
#include "gtest/gtest.h"
#include "tap.h"
#include <iostream>

using namespace AIOUSB;


int bufsize = 1000;

class AIOContinuousBufSetup : public ::testing::Test 
{
 protected:
    virtual void SetUp() {
        numAccesDevices = 0;
        AIOUSB_Init();
        result = AIOUSB_SUCCESS;
        AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( &numAccesDevices, USB_AI16_16E, NULL );
        device = AIODeviceTableGetDeviceAtIndex( numAccesDevices ,  &result );
    }
  
    virtual void TearDown() { 

    }
    int numAccesDevices;
    AIORESULT result;
    AIOUSBDevice *device;
    unsigned short *data;
};

TEST(AIOContinuousBuf,BasicReadAndWritingTest) 
{
    int num_channels = 1;
    int num_scans = 5000, size = num_scans;

    AIOContinuousBuf *buf =  NewAIOContinuousBufForCounts( 0, num_scans , num_channels );
    AIOBufferType *tmp = (AIOBufferType*)calloc(1,size*sizeof(AIOBufferType));
    unsigned short *countbuf = (unsigned short *)calloc(1,size*sizeof(unsigned short));
    AIORET_TYPE retval;

    retval = AIOContinuousBufWriteCounts( buf, countbuf, size, 1000, AIOCONTINUOUS_BUF_NORMAL );
    EXPECT_EQ( 1000, retval );
    EXPECT_EQ( 1000, AIOContinuousBufGetWritePosition(buf) );    
    DeleteAIOContinuousBuf(buf);

}

class ParamTest : public ::testing::TestWithParam<std::tuple<int,int,int,int>> {};

/**
 * @cond TEST_DOCUMENTATION
 * @brief 
 * @todo Rely on Test fixture to set up the buffer
 * @todo Create tear down that deallocates all tmpbuffers for these
 *       copy tests
 *
 * @param num_channels_per_scan := 16
 * @param num_scans             := 2048
 * @param num_scans_to_read     := 10*num_scans
 * @param lambda_write          := 4
 * @param lambda_read           := 3
 * @endcond
 */
TEST_P(ParamTest, WriteMultipleTimesSizeOfBuffer )
{
//     /* int num_channels_per_scan = 16; */
//     int num_channels_per_scan = std::get<0>(GetParam());
//     int num_scans = std::get<1>(GetParam());
//     int num_scans_to_read = 10*(num_scans+1);
//     int tobuf_size = num_channels_per_scan * (num_scans+1);
//     int use_data_size = num_channels_per_scan * (num_scans+1);
//     int read_scans = 0;
//     int lambda_write = std::get<2>(GetParam()), lambda_read = std::get<3>(GetParam());
//     unsigned short *use_data  = (unsigned short *)calloc(1, use_data_size * sizeof(unsigned short) );
//     unsigned short *tobuf     = (unsigned short *)calloc(1, tobuf_size * sizeof(unsigned short ) );
//     AIORET_TYPE retval;
//     AIOContinuousBuf *buf = NewAIOContinuousBufForCounts( 0, (num_scans+1), num_channels_per_scan );
//     /**
//      * @brief Keep reading and writing
//      */
//     // set_write_pos(buf, 0 );
//     // set_read_pos(buf,  0 );
//     int numrepeat = 0;
//     while ( read_scans < num_scans_to_read ) {
//         for ( int i = 0 ; i < lambda_write ; i ++ ) {
//             retval = AIOContinuousBufWriteCounts( buf, tobuf, tobuf_size, num_channels_per_scan, AIOCONTINUOUS_BUF_ALLORNONE );
//             if ( retval < 0 ) { 
//                 std::cout << "";
//             }
//             // ASSERT_EQ( num_channels_per_scan, retval ) << "bufsize " << buffer_size(buf) << " read_pos " << AIOContinuousBufGetReadPosition(buf) << "   write_pos " << AIOContinuousBufGetWritePosition(buf) << std::endl;
//         }
//         for ( int i = 0 ; i < lambda_read ; i ++ ) {
//             ASSERT_GE( AIOContinuousBufCountScansAvailable(buf), 0 );
//             retval = AIOContinuousBufReadIntegerNumberOfScans( buf, tobuf, tobuf_size, 1 );
//             EXPECT_EQ( retval, 1 );
//             ASSERT_GE( retval, 0 );
//             read_scans += retval;
//         }
//         numrepeat ++;
//         std::cout << "";
//     }
}

std::vector<int> nscans { 128 , 100} ;
std::vector<int> nchannels { 16 , 4 };
std::vector<int> lambda_w { 4 };
std::vector<int> lambda_r { 4 };


INSTANTIATE_TEST_CASE_P(AllCombinations, ParamTest,  ::testing::Combine(::testing::ValuesIn(nscans),::testing::ValuesIn(nchannels),
                                                                        ::testing::ValuesIn(lambda_w),::testing::ValuesIn(lambda_r))
                        );

/**
 * @todo When we create a new AIOContinuousbuf with testing enabled, the device
 *       created should actually be enabled for testing
 */
TEST_F( AIOContinuousBufSetup, SetsTesting ) {
    int i, count = 0, buf_unit = 10;
    int actual_bufsize = 10;

    AIORESULT result = AIOUSB_SUCCESS;
    int numDevices = 0;
    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_AIO16_16A );
    EXPECT_EQ( numDevices, 1 );

    AIOContinuousBuf * buf = NewAIOContinuousBufForCounts( 0, actual_bufsize , buf_unit );
    AIOContinuousBufSetTesting( buf, AIOUSB_TRUE );

    AIOUSBDevice *dev = AIODeviceTableGetDeviceAtIndex( AIOContinuousBufGetDeviceIndex( buf ) , &result );
    EXPECT_EQ( result, AIOUSB_SUCCESS );    

    EXPECT_EQ( AIOUSB_TRUE , AIOUSBDeviceGetTesting( dev )  );

    EXPECT_EQ( AIOUSB_TRUE, AIOUSBDeviceGetADCConfigBlock( dev )->testing );
}


/**
 * @todo Creating a new Continuous Buffer and setting it up should 
 */
TEST(AIOContinuousBuf, CanAssignDeviceToConfig) {
    AIORESULT result = AIOUSB_SUCCESS;
    int numDevices = 0;
    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_AIO16_16A );
    EXPECT_EQ( numDevices, 1 );

    AIOContinuousBuf *buf = NewAIOContinuousBufForCounts( 0, 10, 16 );
    AIOUSBDevice *dev = AIODeviceTableGetDeviceAtIndex( AIOContinuousBufGetDeviceIndex( buf ) , &result );
    ASSERT_EQ( result , AIOUSB_SUCCESS );
    ADCConfigBlock *ad = AIOUSBDeviceGetADCConfigBlock ( dev );
    
    EXPECT_TRUE( ad );

}

#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[] )
{
  
  AIORET_TYPE retval;

  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();

  return RUN_ALL_TESTS();  

}





