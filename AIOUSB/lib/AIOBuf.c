/**
 * @file   AIOBuf.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  
 */

#include "AIOBuf.h"
#include <stdio.h>
#include <stdlib.h>


#ifdef __cplusplus
namespace AIOUSB {
#endif 

/*----------------------------------------------------------------------------*/
AIOBuf * NewAIOBuf( AIOBufType type , size_t size )
{
    AIOBuf *ret = (AIOBuf *)calloc(1,sizeof(AIOBuf));
    if (!ret )
        return ret;
    ret->_buf = malloc( ((int)type)*size );

    if ( !ret->_buf ) {
        free(ret);
        ret = NULL;
    }
    ret->size = size;
    ret->type = type;
    ret->defined = AIOUSB_TRUE;
    return ret;
}

/*----------------------------------------------------------------------------*/

AIORET_TYPE DeleteAIOBuf( AIOBuf *buf )
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIO_ASSERT( buf );
    if ( buf->_buf ) {
        free( buf->_buf );

    } else {
        retval = -AIOUSB_ERROR_INVALID_MEMORY;
    }
    free( buf );
    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOBufGetSize( AIOBuf *buf )
{
    AIO_ASSERT( buf );

    AIORET_TYPE retval = buf->size;

    return retval;
}

AIORET_TYPE AIOBufGetTotalSize( AIOBuf *buf )
{
    AIO_ASSERT( buf );
    AIO_ERROR_VALID_DATA( -AIOUSB_ERROR_INVALID_AIOBUFTYPE, (int)buf->type > 0 );
    return buf->size * (int)buf->type;
}



/*----------------------------------------------------------------------------*/
AIOBufType AIOBufGetType( AIOBuf *buf )
{
    AIO_ASSERT_RET( AIO_ERROR_BUF, buf );

    AIOBufType retval = buf->type;
    
    return retval;
}

/*----------------------------------------------------------------------------*/
void *AIOBufGetRaw( AIOBuf *buf )
{
    AIO_ASSERT_RET(NULL, buf );
    return buf->_buf;
}


/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOBufGetTypeSize( AIOBuf *buf )
{
    AIO_ASSERT( buf );
    
    AIOBufType retval = buf->type;

    return (AIORET_TYPE)(int)retval;
}


/*----------------------------------------------------------------------------*/

AIORET_TYPE AIOBufRead( AIOBuf *buf, void *tobuf, size_t size_tobuf )
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIO_ASSERT( buf );
    AIO_ASSERT( tobuf );
    
    int ibufsize;
    
    AIO_ASSERT_AIORET_TYPE(-AIOUSB_ERROR_INVALID_AIOBUFTYPE, (ibufsize = (int)AIOBufGetTotalSize(buf) ) >= 0  );

    int act_size = MIN( ibufsize, (int)size_tobuf );
    memcpy( tobuf, buf->_buf, act_size );
    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOBufWrite( AIOBuf *buf, void *frombuf, size_t size_frombuf )
{
    AIO_ASSERT( buf );
    AIO_ASSERT( frombuf );
    int obufsize;

    AIO_ASSERT_AIORET_TYPE( -AIOUSB_ERROR_INVALID_AIOBUFTYPE, (obufsize = (int)AIOBufGetTotalSize(buf) ) >= 0 );

    int act_size = MIN( obufsize, (int)size_frombuf );
    memcpy( buf->_buf, frombuf, act_size );
    return act_size;
}

/*----------------------------------------------------------------------------*/
AIOBufIterator AIOBufGetIterator( AIOBuf *buf )
{

    AIO_ASSERT_RET((AIOBufIterator){0},buf);

    AIOBufIterator tmp = {0};
    
    tmp.loc = buf->_buf;
    tmp.next = AIOBufIteratorNext;
    tmp.buf = buf;

    return tmp;
}

/*----------------------------------------------------------------------------*/
AIOUSB_BOOL AIOBufIteratorIsValid( AIOBufIterator *biter )
{
    AIOUSB_BOOL retval = AIOUSB_TRUE;
    void *tmp;

    tmp = (void *)( (char *)biter->buf->_buf + AIOBufGetTotalSize( biter->buf ) );
    if ( biter->loc >= tmp ) 
        retval = AIOUSB_FALSE;

    return retval;
}

/*----------------------------------------------------------------------------*/
void AIOBufIteratorNext( AIOBufIterator *biter )
{
    biter->loc = (void *)((char*)biter->loc + AIOBufGetTypeSize(biter->buf));
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Returns a value from the current interator. Casts up to the largest
 *        number we have and then a user can Cast down to the number they
 *        wish to actually use. 
 * @todo make this better instead of using memcpy, just cast directly
 * @param biter Iterator 
 * @return AIO_NUMBER large precision number.
 */
AIOEither AIOBufIteratorGetValue( AIOBufIterator *biter )
{
    AIOEither bad = {0};
    bad.left = AIOUSB_ERROR_INVALID_PARAMETER;
    AIO_ASSERT_RET( bad, biter );
    AIOEither retval = {0};

    switch ( AIOBufGetType( biter->buf ) ) { 
    case AIO_COUNTS_BUF:
        { 
            uint16_t tmp;
            memcpy(&tmp, biter->loc , sizeof(uint16_t ));
            AIOEitherSetRight( &retval, aioeither_value_uint16_t, &tmp );
        }
        break;
    case AIO_VOLTS_BUF:
        {
            double tmp;
            memcpy(&tmp, biter->loc, sizeof(double));
            AIOEitherSetRight( &retval, aioeither_value_double_t , &tmp );
        }
        break;
    case AIO_DEFAULT_BUF:
        {
            uint8_t tmp;
            memcpy(&tmp, biter->loc, sizeof(uint8_t));
            AIOEitherSetRight( &retval, aioeither_value_uint8_t, &tmp );
        }
        break;
    default:
        AIOEitherSetLeft( &retval, -AIOUSB_ERROR_INVALID_AIOBUFTYPE);
    }
    return retval;
}


#ifdef __cplusplus
}
#endif 

#ifdef SELF_TEST
#include "AIOUSBDevice.h"
#include "AIOTypes.h"
#include "gtest/gtest.h"

using namespace AIOUSB;


TEST(AIOBuf, CreationAndDestruction) 
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOBuf *buf = NewAIOBuf( AIO_DEFAULT_BUF, 100 );
    ASSERT_TRUE( buf );
    
    ASSERT_EQ( 100, AIOBufGetSize( buf ));

    ASSERT_EQ( AIO_DEFAULT_BUF, AIOBufGetType( buf ));

    ASSERT_EQ( 100, AIOBufGetTotalSize(buf) );

    retval = DeleteAIOBuf( buf );
    ASSERT_EQ( AIOUSB_SUCCESS, retval );

    buf = NewAIOBuf( AIO_COUNTS_BUF, 100 );
    ASSERT_EQ( 100 * (int)AIO_COUNTS_BUF, AIOBufGetTotalSize(buf) );
    retval = DeleteAIOBuf( buf );
    ASSERT_EQ( AIOUSB_SUCCESS, retval );

}

TEST(AIOBuf, WriteIntoBuffer )
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOBuf *buf = NewAIOBuf( AIO_DEFAULT_BUF, 100 );
    char tmp[110];
    char tmp2[100];
    ASSERT_TRUE(tmp);

    for( int i = 0; i < sizeof(tmp); i ++ ) tmp[i] = (char )i;

    retval = AIOBufWrite( buf, tmp, sizeof(tmp) );
    ASSERT_EQ( 100, retval );

    EXPECT_EQ( 0 ,  memcmp( tmp, buf->_buf, sizeof( retval )) );
    
    AIOBufRead( buf, tmp2, sizeof(tmp2));
    EXPECT_EQ( 0 ,  memcmp( tmp2, tmp, sizeof( tmp2 )) );



    retval = DeleteAIOBuf( buf );
    ASSERT_EQ( AIOUSB_SUCCESS, retval );

}

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


#endif
