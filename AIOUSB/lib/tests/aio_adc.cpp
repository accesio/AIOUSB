#include "AIOCommandLine.h"
#include "gtest/gtest.h"
#include "tap.h"

using namespace AIOUSB;



TEST(ADCFunctions, CanDetermineCalibration ) 
{
    ProductIDS ids[] = { USB_AI16_16A   ,
                         USB_AI16_16E   ,
                         USB_AI12_16A   ,
                         USB_AI12_16    ,
                         USB_AI12_16E   ,
                         USB_AI16_64MA  ,
                         USB_AI16_64ME  ,
                         USB_AI12_64MA  ,
                         USB_DIO_48
    };
    int expected[] = { 1, 0,1,0,0,1,0,1,0} ;
    
    for ( int i = 0; i < sizeof(expected) / sizeof(int) ; i ++ ) { 
        if ( expected[i] ) { 
            EXPECT_TRUE( ADC_CanCalibrate( ids[i] ) ) << "i=" << i << " num=" << ids[i];
        } else {
            EXPECT_FALSE( ADC_CanCalibrate( ids[i] ) ) << "i=" << i << " num=" << ids[i];
        }
    }

}


#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[] )
{
  
  AIORET_TYPE retval;

  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
#ifdef GTEST_TAP_PRINT_TO_STDOUT
  delete listeners.Release(listeners.default_result_printer());
#endif

  return RUN_ALL_TESTS();  

}


