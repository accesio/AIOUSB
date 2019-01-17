#include "AIOTypes.h"
#include "AIOCommandLine.h"
#include "gtest/gtest.h"
#include "tap.h"

using namespace AIOUSB;



TEST(AIOBuf, CanIterate ) 
{
    AIORET_TYPE retval =  AIOUSB_SUCCESS;
    AIOBuf *buf = NewAIOBuf( AIO_COUNTS_BUF, 100 );
    uint16_t testvalues[100];
    AIOBuf *tmpbuf = NewAIOBuf( AIO_COUNTS_BUF, 0 );
    AIOBufIterator iter;
    unsigned short *counts;
    int i;

    for ( counts = (uint16_t *)buf->_buf, i = 0; counts < (uint16_t*)buf->_buf + 100; counts += 1 , i ++ )
        *counts = (uint16_t)i;

    iter = AIOBufGetIterator( buf );

    ASSERT_TRUE( *(int*)&iter );
    ASSERT_TRUE( iter.buf );

    ASSERT_TRUE( iter.loc );
    ASSERT_TRUE( iter.next );
    ASSERT_TRUE( AIOBufIteratorIsValid( &iter ) );

    iter = AIOBufGetIterator( tmpbuf );
    ASSERT_FALSE( AIOBufIteratorIsValid( &iter ));

    iter = AIOBufGetIterator( buf );
    iter.loc = &((uint16_t *)buf->_buf)[100];
    ASSERT_FALSE( AIOBufIteratorIsValid( &iter )) << "Very end should indicate we've gone too far\n";

    iter.loc = &((uint16_t *)buf->_buf)[99];
    ASSERT_TRUE( AIOBufIteratorIsValid( &iter )) << "Still should have an element left\n";

    iter.next(&iter);
    ASSERT_EQ( iter.loc, &((uint16_t *)buf->_buf)[100] );

    iter = AIOBufGetIterator( buf );
    iter.next(&iter);
    ASSERT_EQ( iter.loc, &((uint16_t *)buf->_buf)[1] );

    int j = 0;

    for ( iter = AIOBufGetIterator( buf ), j = 0; AIOBufIteratorIsValid(&iter); iter.next(&iter) , j ++ ) {
        AIOEither either;
        testvalues[j] = AIOEitherToShort( &( either = AIOBufIteratorGetValue(&iter)), &retval );
        ASSERT_EQ( AIOUSB_SUCCESS, retval );
    }

    ASSERT_EQ( 100, j );

    ASSERT_EQ( 0, memcmp( testvalues, buf->_buf , sizeof(testvalues)) );

    retval = DeleteAIOBuf( buf );
    ASSERT_EQ( AIOUSB_SUCCESS, retval );
    
    retval = DeleteAIOBuf( tmpbuf );
    ASSERT_EQ( AIOUSB_SUCCESS, retval );

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


