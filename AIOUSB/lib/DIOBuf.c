/**
 * @file   DIOBuf.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  A smart buffer for handling Bit values and performing Bit arithmatic. This 
 *         alleviates the need to perform bitwise operations on unsigned chars or other
 *         primitive data types in programming languages.
 *
 *
 *
 */

#include "DIOBuf.h"

#ifdef __cplusplus
namespace AIOUSB {
#endif


int _determine_strbuf_size( unsigned size )
{
    return (((size / BITS_PER_BYTE)+1)*BITS_PER_BYTE) + strlen("0x") + 1;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Constructor for creating a new DIOBuf object. The parameter represents
 *        the number of *bits* that you want to have in your in your buffer. 
 *        Typical values would be multiples of 8 
 * @param size Preallocates the buffer to size 
 * @return DIOBuf * or Null if failure
 */
DIOBuf *NewDIOBuf( unsigned size ) {
    DIOBuf *tmp = (DIOBuf *)malloc( sizeof(DIOBuf) );
    if( ! tmp ) 
        return tmp;
    tmp->buffer = (DIOBufferType *)calloc(size, sizeof(DIOBufferType));
    if ( !tmp->buffer ) {
        free( tmp );
        return NULL;
    }
    tmp->strbuf_size = _determine_strbuf_size( size );
    tmp->strbuf = (char *)malloc(sizeof(char) * tmp->strbuf_size );
    if ( !tmp->strbuf ) {
        free(tmp->buffer);
        free(tmp);
        return NULL;
    }
    tmp->size = size;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void _copy_to_buf( DIOBuf *tmp, const char *ary, int size_array ) {
    int tot_bit_size = tmp->size;
    int i; 
    for ( i = 0 ; i < tot_bit_size ; i ++ ) { 
        int curindex = i / 8;
        tmp->buffer[ i ] = (( ary[curindex] >> (( 8-1 ) - (i%8)) ) & 1 ? 1 : 0 );
    }
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Constructor for creating a new DIOBuf object but it accepts an array
 *        of bytes with size_array providing the length , or total number of 
 *        bytes in the input *Ary*.
 * @param ary 
 * @param size_array 
 * @return DIOBuf if successful or NULL if there was an error.  Will set errno
 *         to the reason in question but it will almost always be due to
 *         memory allocation problems.
 */
DIOBuf *NewDIOBufFromChar( const char *ary, int size_array ) {
    int tot_bit_size = size_array*8;
    DIOBuf *tmp = NewDIOBuf( tot_bit_size );
    if( ! tmp ) 
      return tmp;

    _copy_to_buf( tmp, ary , size_array );
    return tmp;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Constructor from a string argument like "101011011";
 * @param ary String that contains 1's and 0's. I.E: "11110000"
 * @return DIOBuf if successful or NULL if there was an error.
 */
DIOBuf *NewDIOBufFromBinStr( const char *ary ) {
    int tot_bit_size = strlen(ary);
    DIOBuf *tmp = NewDIOBuf( tot_bit_size );
    int i;
    if( ! tmp ) 
        return tmp;
    for ( i = tot_bit_size - 1; i >= 0 ; i -- ) { 
        DIOBufSetIndex( tmp, tot_bit_size - 1 - i  , (ary[i] == '0' ? 0 : 1 ) );
    }
    return tmp;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Replaces the content of the buffer buf with the new array , of size size * 
 * @param buf DIOBuf buffer one wishes to replace the content of
 * @param ary Array of raw bytes values that will replace the original
 * @param size_array The size, in bytes, of the *ary* that will be copied in
 * @return DIOBuf if successful or NULL if there was an error and _errno_ will be
 *         set to the error in question
 */
DIOBuf *DIOBufReplaceString( DIOBuf *buf, char *ary, int size_array ) 
{ 
    if ( buf  )
        if( DIOBufResize( buf, size_array*8 ) )
            _copy_to_buf( buf, ary, size_array );
    return buf;
}

/*----------------------------------------------------------------------------*/
DIOBuf *DIOBufReplaceBinString( DIOBuf *buf, char *bitstr ) 
{ 
    if ( buf  ) {
        if ( strlen(bitstr) != DIOBufSize( buf ) )
            DIOBufResize(buf, strlen(bitstr) );

        for ( int i = strlen(bitstr)-1 ; i >= 0 ; i -- ) {
            DIOBufSetIndex( buf, i, (bitstr[i] == '0' ? 0 : 1)  );
        }
    }
  return buf;
}
/*----------------------------------------------------------------------------*/
void DeleteDIOBuf( DIOBuf *buf ) 
{
    buf->size = 0;
    free( buf->buffer );
    free( buf->strbuf );
    free( buf );
}
/*----------------------------------------------------------------------------*/
DIOBuf *DIOBufResize( DIOBuf *buf , unsigned newsize ) 
{
    buf->buffer = (unsigned char *)realloc( buf->buffer, newsize*sizeof(unsigned char));
    if ( !buf->buffer ) {
        buf->size = 0;
        buf->strbuf_size = _determine_strbuf_size( newsize );
        buf->strbuf = (char *)realloc( buf->strbuf, buf->strbuf_size * sizeof(char) );
        buf->strbuf[0] = '\0';
        return NULL;
    }
    if( newsize > buf->size )
        memset( &buf->buffer[buf->size], 0, ( newsize - buf->size ));

    buf->strbuf_size = _determine_strbuf_size( newsize );
    buf->strbuf = (char *)realloc( buf->strbuf, buf->strbuf_size * sizeof(char));

    if ( !buf->strbuf )
        return NULL;
    buf->size = newsize;
    return buf;
}
/*----------------------------------------------------------------------------*/
unsigned  DIOBufSize( DIOBuf *buf ) {
  return buf->size;
}

/*----------------------------------------------------------------------------*/
unsigned DIOBufByteSize( DIOBuf *buf ) {
  return buf->size / BITS_PER_BYTE;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Converts the DIOBuf buf into a string of 1's and 0's representing the 
 *        buf's value in binary.
 * @param buf DIOBuf one wished to print in string format
 * @return A string containing 1's and 0's if successful, NULL if failure and 
 *         errno is set.
 */
char *DIOBufToString( DIOBuf *buf ) {
    
  unsigned i;
  memset(buf->strbuf,0, buf->strbuf_size );
  for( i = 0; i < buf->size ; i ++ )
      buf->strbuf[i] = ( buf->buffer[i] == 0 ? '0' : '1' );
  buf->strbuf[buf->size] = '\0';
  return buf->strbuf;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Creates a hex string representation of the DIOBuf buffer. This is useful
 *        for log message which require a more terse representation.
 * @param buf DIOBuf one wishes to convert to Hex
 * @return A Hex string , prefixed with "0x", that represents the hexidecimal 
 *         representation of the DIOBuf buffer's contents. NULL indicates 
 *         a failure and it sets the errno to the cause of the error.
 */
char *DIOBufToHex( DIOBuf *buf ) {
    AIO_ASSERT_RET( NULL, buf );

    int size = DIOBufSize(buf) / BITS_PER_BYTE + ( DIOBufSize(buf) % BITS_PER_BYTE > 0 );
    char *tmp = (char *)malloc( size );
   
    memset( buf->strbuf, 0, buf->strbuf_size );
    memcpy( tmp, DIOBufToBinary( buf ), size );


    strcpy(&buf->strbuf[0], "0x" );
    int j = strlen(buf->strbuf);

    for ( int i = 0 ; i <  size ; i ++ , j = strlen(buf->strbuf)) {
        sprintf(&buf->strbuf[j], "%.2x", (unsigned char)tmp[i] );
    }
    buf->strbuf[j] = 0;
    free(tmp);
    return buf->strbuf;
}

/*----------------------------------------------------------------------------*/
char *DIOBufToBinary( DIOBuf *buf ) {
    AIO_ASSERT_RET( NULL, buf );
    int i, j;
    memset(buf->strbuf, 0, buf->strbuf_size );
    for ( i = 0, j = 0 ; i < (int)DIOBufSize(buf) ; i ++ , j = ( (j+1) % 8 )) { 
        buf->strbuf[i / BITS_PER_BYTE] |= buf->buffer[i] << ( 7 - (j % BITS_PER_BYTE) );
    }
    buf->strbuf[ ((i/ BITS_PER_BYTE)+1)*BITS_PER_BYTE ] = '\0';
    return buf->strbuf;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Creates an inverted binary version of the original DIOBuf. This is
 *        in contrast to just inverting the string of 1s to become 0s and 
 *        vice versea. This is useful in 
 * @param buf DIOBuf to invert
 * @return A binary string that represents the inverted value. NULL indicates 
 *         a failure and it sets the errno
 */
char *DIOBufToInvertedBinary( DIOBuf *buf ) {
    int i;
    char *orig = DIOBufToBinary(buf);
    int size = DIOBufSize(buf);
    int size_to_alloc = ((size / BITS_PER_BYTE)+1);
    char *tmp  = (char *)malloc( size_to_alloc );
    memcpy(tmp, orig, size_to_alloc ) ;
    for ( i = 0; i < size_to_alloc - 1; i ++ ) { 
        tmp[i] = ~tmp[i];
    }
    return tmp;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Sets the value of the DIOBuf buffer at index  to the value specified. 
 *        The value is required to be either a '0' or a '1', otherwise an error
 *        will be generated for this result.
 * @param buf 
 * @param index 
 * @param value A boolean value of either 0 or 1
 * @return success if  >= AIOUSB_SUCCESS , < 0 otherwise
 */
AIORET_TYPE DIOBufSetIndex( DIOBuf *buf, int index, unsigned value )
{
    AIO_ASSERT_RET( -AIOUSB_ERROR_INVALID_INDEX, index < (int)buf->size && index >= 0 );
    AIO_ASSERT_RET( -AIOUSB_ERROR_INVALID_PARAMETER, value == 0 || value == 1 );

    buf->buffer[buf->size - 1 - index] = ( value == AIOUSB_TRUE ? 1 : AIOUSB_FALSE );
    return 0;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Returns the bit value at the index specified.
 * @param buf DIOBuf we wish to inspect
 * @param index Index of the bit we wish to examine
 * @return 0 or 1 if successful, < 0 indicated a failure
 */
AIORET_TYPE DIOBufGetIndex( DIOBuf *buf, int index ) {
    AIO_ASSERT_RET( -AIOUSB_ERROR_INVALID_INDEX, index < (int)buf->size && index >= 0 );
  
    return buf->buffer[buf->size - 1 - index ];
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE DIOBufGetByteAtIndex( DIOBuf *buf, unsigned index , char *value ) {
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    if ( index >= buf->size / BITS_PER_BYTE )   
        return -AIOUSB_ERROR_INVALID_INDEX;
    *value = 0;
    int actindex = index * BITS_PER_BYTE;
    for ( int i = actindex ; i < actindex + BITS_PER_BYTE ; i ++ ) {
        *value |= ( DIOBufGetIndex( buf, i ) == 1 ? 1 << ( i % BITS_PER_BYTE ) : 0 );
    }

    return retval;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE DIOBufSetByteAtIndex( DIOBuf *buf, unsigned index, char  value ) {
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    if ( index >= buf->size / BITS_PER_BYTE )   
        return -AIOUSB_ERROR_INVALID_INDEX;
    int actindex = index * BITS_PER_BYTE;
    for ( int i = actindex ; i < actindex + BITS_PER_BYTE ; i ++ ) {
        DIOBufSetIndex( buf, i,  (( (1 << i % BITS_PER_BYTE ) & value ) ? 1 : 0 ));
    }

    return retval;
}

#ifdef __cplusplus 
}
#endif


/**
 * @brief Self test for verifying basic functionality of the DIO interface
 *
 */ 
#ifdef SELF_TEST

#include <math.h>

#include "gtest/gtest.h"


using namespace AIOUSB;

TEST(DIOBuf , Toggle_Bits ) {

    DIOBuf *buf = NewDIOBuf(100);
    for ( int i = 2 ; i < 10 ; i ++ ) {
      float c = powf( 2, (float)i );
      int size = (int)c;
      DIOBufResize( buf, size );
      EXPECT_EQ( size, DIOBufSize(buf) );
      for( int j = 0 ; j < DIOBufSize(buf); j ++ ) {
        DIOBufSetIndex( buf, j, ( i % 2 == 0 ? 1 : 0 ));
      }
      char *tmp = (char *)malloc( (DIOBufSize(buf)+1)*sizeof(char));
      for( int k = 0; k < DIOBufSize( buf ); k ++ ) {
        tmp[k] = ( i % 2 == 0 ? '1': '0' );
      }
      tmp[DIOBufSize(buf)] = '\0';
      EXPECT_STREQ( DIOBufToString(buf), tmp );
      free(tmp);
    }
    DeleteDIOBuf(buf);
}

TEST(DIOBuf, CharStr_Constructor ) {
    DIOBuf *buf = NewDIOBufFromChar((char *)"Test",4 );
    EXPECT_STREQ( DIOBufToString(buf), "01010100011001010111001101110100" );
    DIOBufReplaceString( buf, (char *)"FooBar", 6);
    EXPECT_STREQ( DIOBufToString(buf), "010001100110111101101111010000100110000101110010" );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, CharStrIncomplete ) {
    DIOBuf *buf = NewDIOBufFromBinStr("0100011001101111011011110100001001100001011100");
    EXPECT_STREQ( DIOBufToBinary(buf), "FooBap" );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, BinStr_Constructor ) {
    DIOBuf *buf = NewDIOBufFromBinStr("10110101101010101111011110111011111" );
    EXPECT_STREQ( DIOBufToString(buf), "10110101101010101111011110111011111" );    
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Binary_Output ) {
    DIOBuf *buf = NewDIOBufFromChar("Test",4 );
    EXPECT_STREQ( DIOBufToBinary(buf), "Test" );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Binary_Output2 ) {
    DIOBuf *buf = NewDIOBufFromBinStr("0000000001100011");
    EXPECT_STREQ( DIOBufToBinary(buf), "\000c" );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Inverted_Binary ) { 
    DIOBuf *buf = NewDIOBufFromBinStr("0000000001100011");
    char *tmp = DIOBufToInvertedBinary(buf);
    EXPECT_STREQ( tmp, "\xFF\x9C" );
    free(tmp);
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, CorrectHex ) { 
    DIOBuf *buf = NewDIOBufFromBinStr("0011111000111111");
    char *tmp = DIOBufToHex(buf);
    EXPECT_STREQ( tmp, "0x3e3f" );
    DeleteDIOBuf( buf );
    buf = NewDIOBufFromBinStr("011111000111111");
    tmp = DIOBufToHex( buf );
    EXPECT_STREQ( tmp, "0x7c7e" );
    DeleteDIOBuf( buf );
}


TEST(DIOBuf, Resize_Test ) {
    DIOBuf *buf = NewDIOBuf(0);
    DIOBufResize(buf, 10 ); 
    EXPECT_STREQ( DIOBufToString(buf), "0000000000" );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Hex_Output ) {
    DIOBuf *buf = NewDIOBufFromChar("Test",4 );
    EXPECT_STREQ( "0x54657374", DIOBufToHex(buf) );
    DIOBufReplaceString( buf, (char *)"This is a very long string to convert", 37);
    EXPECT_STREQ(  "0x5468697320697320612076657279206c6f6e6720737472696e6720746f20636f6e76657274", DIOBufToHex(buf) );
    DeleteDIOBuf( buf );
    
    buf =  NewDIOBufFromBinStr("00000000000000000000000011111111" );
    char *tmp = DIOBufToHex( buf );
    EXPECT_STREQ( tmp, "0x000000ff" );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Indexing_is_correct ) {
    DIOBuf *buf = NewDIOBufFromBinStr("0010101101010101111011110111011011" );
    EXPECT_EQ( 1, DIOBufGetIndex(buf, 0 ) );
    EXPECT_EQ( 1, DIOBufGetIndex(buf, 1 ) );
    EXPECT_EQ( 0, DIOBufGetIndex(buf, 2 ) );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Correct_Null_Output ) {
    DIOBuf *buf = NewDIOBuf(16);
    EXPECT_STREQ( DIOBufToString(buf), "0000000000000000");
    EXPECT_STREQ( DIOBufToBinary(buf), "" );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Correct_Index_Reading ) {
    DIOBuf *buf = NewDIOBufFromBinStr("10101010001100111111000011111111" );
    char val;
    DIOBufGetByteAtIndex(buf, 0, &val );
    EXPECT_EQ( (unsigned char)val, 0xff );
    DIOBufGetByteAtIndex(buf, 1, &val );
    EXPECT_EQ( (unsigned char)val, 0xf0 );
    DIOBufGetByteAtIndex(buf, 2, &val );
    EXPECT_EQ( (unsigned char)val, 0x33 );
    DIOBufGetByteAtIndex(buf, 3, &val );
    EXPECT_EQ( (unsigned char)val, 0xaa );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Correct_Index_Writing ) {
    DIOBuf *buf = NewDIOBufFromBinStr("10101010001100111111000011111111" );
    char val = 0xff;
    DIOBufSetByteAtIndex( buf, 1, 0xff );
    EXPECT_STREQ( DIOBufToString(buf), "10101010001100111111111111111111" );
    DIOBufSetByteAtIndex( buf, 2, 0xff );
    EXPECT_STREQ( DIOBufToString(buf), "10101010111111111111111111111111" );
    DIOBufSetByteAtIndex( buf, 2, 0x0f );
    EXPECT_STREQ( DIOBufToString(buf), "10101010000011111111111111111111" );
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, ReplaceWithLargerString )
{
    DIOBuf *buf = NewDIOBuf(16);
    std::string tmp = "101010101010101011111";
    ASSERT_TRUE( buf );

    DIOBufReplaceBinString( buf, (char *)tmp.c_str() );
    ASSERT_TRUE(buf);

    ASSERT_EQ( strlen(tmp.c_str()), DIOBufSize(buf));
    DeleteDIOBuf( buf );
}

TEST(DIOBuf, Toggle_interview ) {
    DIOBuf *buf = NewDIOBuf(100);
    char *tmp = (char*)malloc(DIOBufSize(buf)+1);
    for ( int i = 1 ; i < DIOBufSize(buf); i ++ ) {
        for ( int j = i ; j < DIOBufSize(buf); j += i ) {
            DIOBufSetIndex( buf, j , DIOBufGetIndex( buf , j ) == 0 ? 1 : 0 );
        }
    }
    for( int k = 0; k <DIOBufSize(buf); k ++ ) {
        tmp[k] = '0';
    }
    for( int k = 1; k*k < DIOBufSize(buf); k ++ ) {
        tmp[DIOBufSize(buf) - 1 - k*k] = '1';
    }
    tmp[100] = '\0';
    EXPECT_STREQ( DIOBufToString(buf), tmp );
    DeleteDIOBuf(buf);
    free(tmp);
}


int main( int argc , char *argv[] ) 
{
    testing::InitGoogleTest(&argc, argv);
    testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();

    return RUN_ALL_TESTS();  

}


#endif



