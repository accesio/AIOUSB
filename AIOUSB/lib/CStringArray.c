#include "CStringArray.h"
#include "AIOTypes.h"
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif


/*----------------------------------------------------------------------------*/
CStringArray *NewCStringArrayWithStrings(size_t numstrings, ... )
{
    CStringArray *tmpsa = NewCStringArray( numstrings );
    if ( !tmpsa )return tmpsa;

    va_list arguments;
    va_start( arguments, numstrings );
    for ( int i = 0; i < (int)numstrings; i ++ ) { 
        char *tmp = va_arg(arguments, char * );
        tmpsa->_strings[i] = strdup(tmp);
    }
    va_end(arguments);
    return tmpsa;
}

/*----------------------------------------------------------------------------*/
CStringArray *NewCStringArray(size_t numstrings)
{
    if ( !numstrings ) return NULL;
    CStringArray *tmp = (CStringArray *)malloc(sizeof(CStringArray));
    if (!tmp) return NULL;
    tmp->_size = numstrings;
    tmp->_strings = (char **)malloc(sizeof(char *)*numstrings);
    if (!tmp->_strings )
        goto err;

    return tmp;
        
 err:
    free(tmp );

    return NULL;
}

/*----------------------------------------------------------------------------*/
CStringArray *NewCStringArrayFromCArgs( int argc, char *argv[] )
{
    AIO_ASSERT_RET( NULL, argv );
    if ( argc == 0 ) 
        return NULL;
    CStringArray *tmp = NewCStringArray( argc );
    if (!tmp) return NULL;
    for ( int i = 0; i < argc; i ++ ) {
        tmp->_strings[i] = strdup(argv[i]);
    }
    return tmp;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE DeleteCStringArray( CStringArray *str)
{
    AIO_ASSERT( str );
    for ( int i = 0; i < str->_size; i ++ ) {
        free( str->_strings[i] );
    }
    free(str->_strings);
    free(str);
    return AIOUSB_SUCCESS;
}


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus


CStringArray::CStringArray(int size, ... ) : _size(size) 
{
     this->_strings = new char *[size];
     va_list arguments;
     va_start( arguments, size );
     for ( int i = 0; i < (int)size; i ++ ) { 
         char *tmp = va_arg(arguments, char * );
         this->_strings[i] = strdup(tmp);
     }
     va_end(arguments);
}

CStringArray::~CStringArray() { 
    for ( int i = 0; i < this->_size; i ++ ) {
        free(this->_strings[i] );
    }
    delete [] this->_strings;
}

CStringArray::CStringArray(const CStringArray &ref)
{
    _size = ref._size;
    _strings = new char *[_size] ;
    for ( int i = 0; i < _size ;i ++ ) { 
        _strings[i] = strdup( ref._strings[i] );
    }
}

#endif

/*----------------------------------------------------------------------------*/
CStringArray *CopyCStringArray( CStringArray *str )
{
    AIO_ASSERT_RET( NULL, str );
    CStringArray *tmp = NewCStringArray( str->_size );
    if ( !tmp ) return tmp;
    memcpy(tmp->_strings, str->_strings, sizeof(char *)*str->_size );
    return tmp;
}

/*----------------------------------------------------------------------------*/
char *CStringArrayToString( CStringArray *str )
{
    return CStringArrayToStringWithDelimeter( str, NULL);
}

/*----------------------------------------------------------------------------*/
char *CStringArrayToStringWithDelimeter( CStringArray *str, const char *delim)
{

    AIO_ASSERT_RET( NULL, str );
    char *tmpdelim  = ((char *)delim == NULL ? (char *)" " : (char *)delim );
    int i;
    int retcode = 0;
    char *retval = NULL;
    char *tmp;
    for ( i = 0; i < str->_size -1 ; i ++ ) { 
        if ( i == 0 && retval == NULL ) { 
            retcode = asprintf(&retval, "%s%s", str->_strings[i], tmpdelim );
        } else if ( i != 0 && retval == NULL ) {
            retcode = asprintf(&retval, "%s%s", str->_strings[i], tmpdelim );
        } else {
            tmp = strdup(retval );
            free(retval);
            retcode = asprintf(&retval, "%s%s%s", tmp, str->_strings[i], tmpdelim );
            free(tmp);
        }
    }
    if ( retcode < 0 )
        return NULL;

    tmp = strdup(retval);
    free(retval);
    retcode = asprintf(&retval, "%s%s", tmp, str->_strings[i]);
    if ( retcode < 0 ) 
        return NULL;
    free(tmp);
    return retval;
}



#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST

#include <gtest/gtest.h>
#include <iostream>
using namespace AIOUSB;

#include <unistd.h>
#include <stdio.h>


TEST(CStringArray,Basics ) 
{
    CStringArray *tmp = NewCStringArrayWithStrings(4,(char*)"First",(char*)"Second",(char*)"Third",(char *)"Fourth");
    ASSERT_TRUE( tmp );
    DeleteCStringArray( tmp );
}


TEST(CStringArray,Stringification)
{
    CStringArray tmp = CStringArray(4,(char*)"1",(char*)"2",(char*)"3",(char *)"4" );
    char *tmpstr = CStringArrayToString( &tmp );
    ASSERT_STREQ( "1 2 3 4", tmpstr );
    free(tmpstr);
    tmpstr = CStringArrayToStringWithDelimeter( &tmp, (const char *)"," );
    ASSERT_STREQ( "1,2,3,4", tmpstr );
    free(tmpstr);
}

TEST(CStringArray,CopyXtor)
{
    CStringArray tmp1(4,(char*)"1",(char*)"2",(char*)"3",(char *)"4" );
    CStringArray tmp2(tmp1);

    for ( int i = 0; i < tmp1._size ; i ++ ) 
        ASSERT_STREQ( tmp1._strings[i], tmp2._strings[i] );
    
    CStringArray *p1 = new CStringArray(tmp2);

    for ( int i = 0; i < p1->_size ; i ++ ) 
        ASSERT_STREQ( p1->_strings[i], tmp2._strings[i] );
    
    delete p1;
}

TEST(CStringArray,FromCArgs ) 
{
    char **tmp = new char *[4]{(char*)"this",(char*)"is",(char *)"a",(char *)"string"};
    int argc = 4;
    CStringArray *sa = NewCStringArrayFromCArgs( argc, tmp );
    char *tstr;
    ASSERT_STREQ( (tstr = CStringArrayToString(sa)),"this is a string" );
    free(tstr);
    DeleteCStringArray( sa );
    delete [] tmp;
}

int main(int argc, char *argv[] )
{
  AIORET_TYPE retval;

  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();

  return RUN_ALL_TESTS();  

}
#endif
