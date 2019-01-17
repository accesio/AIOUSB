#include "AIOTuple.h"
#include <stdarg.h>
#include <string.h>


#ifdef __cplusplus
namespace AIOUSB {
#endif



#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST

#include "gtest/gtest.h"
#include <iostream>

using namespace AIOUSB;


TEST(Tuple,Basic2)
{
    AIOTuple2_AIORET_TYPE__CStringArray_p *bar = NewAIOTuple2_AIORET_TYPE__CStringArray_p( AIOUSB_ERROR_INVALID_DATA, NewCStringArrayWithStrings(2 , (char *)"Hello",(char *)"There"));

    char *tmp = AIOTUPLE2_TO_STR( AIOTuple2_AIORET_TYPE__CStringArray_p , bar );

    ASSERT_STREQ( tmp, "(6,Hello There)");
    free(tmp);
    DeleteAIOTuple2_AIORET_TYPE__CStringArray_p( bar );
}


TEST(Tuple,CStringArray)
{
    
    AIOTuple2_AIORET_TYPE__CStringArray *tmp;
    CStringArray t1(3 , (char *)"Hello",(char *)"There",(char*)"Blah" );
    AIOTuple2_AIORET_TYPE__CStringArray bar( AIOUSB_ERROR_INVALID_DATA, t1 );

    ASSERT_EQ(1,1);
}



int main(int argc, char *argv[] )
{
    testing::InitGoogleTest(&argc, argv);
    testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();

    return RUN_ALL_TESTS();  

}
#endif
