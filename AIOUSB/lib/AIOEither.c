/**
 * @file   AIOEither.h
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  General structure for AIOUSB Fifo
 *
 */


#include "AIOTypes.h"
#include "AIOEither.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
namespace AIOUSB 
{
#endif

#define LOOKUP(T) aioret_value_ ## T 

AIORET_TYPE AIOEitherClear( AIOEither *retval )
{
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_AIOEITHER , retval );
    switch(retval->type) { 
    case aioeither_value_string:
        free(retval->right.object);
        break;
    case aioeither_value_obj:
        free(retval->right.object);
        break;
    default:
        ;
    }
    if ( retval->errmsg ) {
        free(retval->errmsg );
        retval->errmsg = 0;
    }
    return AIOUSB_SUCCESS;
}
    
AIORET_TYPE AIOEitherSetRight(AIOEither *retval, AIO_EITHER_TYPE val , void *tmp, ... )
{
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_AIOEITHER , retval );
    va_list ap;
    switch(val) { 
    case aioeither_value_int:
        {
            int t = *(int *)tmp;
            *(int *)(&retval->right.number) = t;
            retval->type = aioeither_value_int;
        }
        break;
    case aioeither_value_unsigned:
        {
            unsigned t = *(unsigned*)tmp;
            *(unsigned *)(&retval->right.number) = t;
            retval->type = aioeither_value_unsigned;
        }
        break;
    case aioeither_value_uint16_t:
        {
            uint16_t t = *(uint16_t*)tmp;
            *(uint16_t *)&retval->right.number = t;
            retval->type = aioeither_value_uint16_t;
        }
        break;
    case aioeither_value_double:
        {
            double t = *(double *)tmp;
            *(double *)&retval->right.number = t;
            retval->type = aioeither_value_double;
        }
        break;
    case aioeither_value_longdouble_t:
         {
            long double t = *(long double *)tmp;
            *(long double *)&retval->right.number = t;
            retval->type = aioeither_value_longdouble_t;
        }
         break;
    case aioeither_value_string:
        { 
            char *t = *(char **)tmp;
            retval->right.object = strdup(t);
            retval->size     = strlen(t)+1;
            retval->type     = aioeither_value_string;
        }
        break;
    case aioeither_value_obj:
        {    
            va_start(ap, tmp);
            int d = va_arg(ap, int);
            va_end(ap);
            retval->right.object = malloc(d);
            retval->type = aioeither_value_obj;
            retval->size = d;
            memcpy(retval->right.object, tmp, d );
        }
        break;
    default:
        break;
    }
    return AIOUSB_SUCCESS;
}

AIORET_TYPE AIOEitherGetRight(AIOEither *retval, void *tmp, ... )
{
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_AIOEITHER , retval );
    va_list ap;
    switch(retval->type) { 
    case aioeither_value_int:
        {
            int *t = (int *)tmp;
            *t = *(int*)(&retval->right.number);
        }
        break;
    case aioeither_value_unsigned:
        {
            unsigned *t = (unsigned *)tmp;
            *t = *(unsigned*)(&retval->right.number);
        }
        break;
    case aioeither_value_uint16_t:
        {
            uint16_t *t = (uint16_t*)tmp;
            *t = *(uint16_t*)(&retval->right.number);
        }
        break;
    case aioeither_value_double_t:
        {
            double *t = (double *)tmp;
            *t = *(double*)(&retval->right.number);
        }
        break;
    case aioeither_value_longdouble_t:
        {
            long double *t = (long double *)tmp;
            *t = *(long double*)(&retval->right.number);
        }
        break;
    case aioeither_value_string:
        { 
            memcpy(tmp, retval->right.object, strlen((char *)retval->right.object)+1);
        }
        break;
    case aioeither_value_obj:
        {
            va_start(ap, tmp);
            int d = va_arg(ap, int);
            va_end(ap);
            memcpy(tmp, retval->right.object, d );
        }
        break;
    default:
        break;
    }
    return AIOUSB_SUCCESS;
}

AIORET_TYPE AIOEitherSetLeft(AIOEither *retval, int val)
{
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_AIOEITHER , retval );
    retval->left = val;
    return AIOUSB_SUCCESS;
}

AIORET_TYPE AIOEitherGetLeft(AIOEither *retval)
{
    return retval->left;
}

AIOUSB_BOOL AIOEitherHasError( AIOEither *retval )
{
    return (retval->left == 0 ? AIOUSB_FALSE : AIOUSB_TRUE );
}

char *AIOEitherToString( AIOEither *retval, AIORET_TYPE *result )
{
    AIO_ASSERT_RET( NULL, result );


    return (char *)&(retval->right.object);

}
 
#define AIO_EITHER_CHECK_VALUE( RETVAL, TYPE )          \
    if ( RETVAL->left ) {                               \
        *result = RETVAL->left;                         \
    } else {                                            \
        *result = AIOUSB_SUCCESS;                       \
        return *(TYPE *)&(RETVAL->right.number);        \
    }                                                   \
    return (TYPE)AIO_ERROR_VALUE;

/* #define AIO_EITHER_GET_VALUE( RETVAL, TYPE ) ({ if(RETVAL.left) { errno=RETVAL.left };  RETVAL.left ? (AIO_ERROR_VALUE) : *(TYPE *)&(RETVAL.right.number) })  */
#define AIO_EITHER_GET_VALUE( RETVAL, TYPE ) ({                 \
            int tmp;                                            \
            if(RETVAL.left) {                                   \
                errno=RETVAL.left;                              \
                tmp=(TYPE)AIO_ERROR_VALUE;                      \
            } else {                                            \
                tmp=*(TYPE *)&(RETVAL.right.number);            \
            };                                                  \
            tmp;})


int AIOEitherToInt( AIOEither retval )
{
    return AIO_EITHER_GET_VALUE( retval, int );
}

short AIOEitherToShort( AIOEither *retval, AIORET_TYPE *result )
{
    AIO_ASSERT_RET(0xffff, result  );
    AIO_EITHER_CHECK_VALUE( retval, short );
}

unsigned AIOEitherToUnsigned( AIOEither *retval, AIORET_TYPE *result )
{
    AIO_ASSERT_RET(0xffffffff, result );
    AIO_EITHER_CHECK_VALUE( retval, unsigned );
}

double AIOEitherToDouble( AIOEither *retval, AIORET_TYPE *result )
{
    AIO_ASSERT_RET(0xffffffff, result );
    AIO_EITHER_CHECK_VALUE( retval, double );
}

AIO_NUMBER AIOEitherToAIONumber( AIOEither *retval, AIORET_TYPE *result )
{
    AIO_ASSERT_RET(0xffffffffffffffff, result );
    AIO_EITHER_CHECK_VALUE( retval, AIO_NUMBER );
}

AIORET_TYPE AIOEitherToAIORetType( AIOEither either )
{
    return either.left;
}


#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST

#include "AIOUSBDevice.h"
#include "AIOEither.h"
#include "gtest/gtest.h"

#include <iostream>
using namespace AIOUSB;

struct testobj {
    int a;
    int b;
    int c;
};

TEST(AIOEitherTest,BasicAssignments)
{
    AIOEither a = {0};
    int tv_int = 22;

    uint32_t tv_uint = 23;
    double tv_double = 3.14159;
    long double tv_ld = 2323244234234.3434;
    char *tv_str = (char *)"A String";
    char readvals[100];
    struct testobj tv_obj = {1,2,3};

    AIOEitherSetRight( &a, aioeither_value_int32_t  , &tv_int );
    AIOEitherGetRight( &a, readvals );
    EXPECT_EQ( tv_int,  *(int*)&readvals[0] );
    AIOEitherClear( &a );

    AIOEitherSetRight( &a, aioeither_value_uint32_t , &tv_uint );
    AIOEitherGetRight( &a, readvals );
    EXPECT_EQ( tv_uint, *(uint32_t*)&readvals[0] );
    AIOEitherClear( &a );

    AIOEitherSetRight( &a, aioeither_value_double_t , &tv_double );
    AIOEitherGetRight( &a, readvals );
    EXPECT_EQ( tv_double, *(double*)&readvals[0] );
    AIOEitherClear( &a );

    AIOEitherSetRight( &a, aioeither_value_longdouble_t , &tv_ld );
    AIOEitherGetRight( &a, readvals );
    EXPECT_EQ( tv_ld, *(long double*)&readvals[0] );
    AIOEitherClear( &a );

    AIOEitherSetRight( &a, aioeither_value_string, &tv_str );
    AIOEitherGetRight( &a, readvals );
    EXPECT_STREQ( tv_str, readvals );
    AIOEitherClear( &a );

    AIOEitherSetRight( &a, aioeither_value_string, &tv_str );
    AIOEitherGetRight( &a, readvals );
    EXPECT_STREQ( tv_str, readvals );
    AIOEitherClear( &a );


    AIOEitherSetRight( &a, aioeither_value_obj, &tv_obj , sizeof(struct testobj));
    AIOEitherGetRight( &a, readvals, sizeof(struct testobj));
    EXPECT_EQ( ((struct testobj*)&readvals)->a, tv_obj.a );
    EXPECT_EQ( ((struct testobj*)&readvals)->b, tv_obj.b );
    EXPECT_EQ( ((struct testobj*)&readvals)->c, tv_obj.c );
    AIOEitherClear( &a );

    AIOEitherSetRight( &a, aioeither_value_obj, &tv_obj , sizeof(struct testobj));
    AIOEitherGetRight( &a, readvals, sizeof(struct testobj));
    EXPECT_EQ( ((struct testobj*)&readvals)->a, tv_obj.a );
    EXPECT_EQ( ((struct testobj*)&readvals)->b, tv_obj.b );
    EXPECT_EQ( ((struct testobj*)&readvals)->c, tv_obj.c );
    AIOEitherClear( &a );

}

TEST(CanCreate,Shorts)
{
    AIOEither a = {0};
    uint16_t tv_ui = 33;
    char readvals[100];

    AIOEitherSetRight( &a, aioeither_value_uint16_t, &tv_ui );
    AIOEitherGetRight( &a, readvals, sizeof(struct testobj));

    EXPECT_EQ( a.type, aioeither_value_uint16_t );
    EXPECT_EQ( tv_ui, *(uint16_t*)&readvals[0] );

}

typedef struct simple {
    char *tmp;
    int a;
    double b;
} Foo;


AIOEither doIt( Foo *) 
{
    AIOEither retval = {0};
    asprintf(&retval.errmsg, "Error got issue with %d\n", 3 );
    retval.left = 3;
    return retval;
}

TEST(CanCreate,Simple)
{
    Foo tmp = {NULL, 3,34.33 };
    AIOEither retval = doIt( &tmp );
    EXPECT_EQ( 3,  AIOEitherGetLeft( &retval ) );
    AIOEitherClear( &retval );
}


TEST(AIOEither,CheckErrors )
{
    AIOEither either = {0};
    AIORET_TYPE result = AIOUSB_SUCCESS;
    AIO_NUMBER retval;
    either.left = -1;

    retval = AIOEitherToInt( either );

    ASSERT_EQ( (int)AIO_ERROR_VALUE, retval  );

    *(int *)&either.right  = 42;
    either.left = 0;

    retval = AIOEitherToInt( either );
    ASSERT_EQ( 42, retval );

}

TEST(AIOEither,CheckCompound)
{
    AIOEither either = {0};
    AIORET_TYPE tmpresult = AIOUSB_SUCCESS;
    either.left = -1;
    *(int *)(&either.right) = 42;
    
    int value;

    int tmp = AIOEitherToInt( either );
    ASSERT_EQ( AIO_ERROR_VALUE, tmp );
    
    either.left = 0;

    value = ({ 
            AIORET_TYPE tmpresult = AIOUSB_SUCCESS;
            int tmp = AIOEitherToInt( either );
            AIO_ASSERT_EXIT( tmpresult == AIOUSB_SUCCESS );
            2;});

    EXPECT_EQ( 2, value );
}

AIOEither test_fn( void )
{
    AIOEither tmp = {0};
    tmp.left = -AIOUSB_ERROR_INVALID_MEMORY;
    *(int *)&tmp.right = 32;
    return tmp;
}


TEST(AIOEither, BuiltInAssertType ) 
{
    AIOEither retval = test_fn();
    EXPECT_EQ( -AIOUSB_ERROR_INVALID_MEMORY, AIOEitherToAIORetType( retval ) );
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


