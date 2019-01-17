#include "AIOChannelMask.h"
#include "AIOTypes.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif

/*----------------------------------------------------------------------------*/
/**
 * @brief Constructor AIOChannelMask bit mask object
 * @param number_channels The number of bits in our Bit Mask
 */
AIOChannelMask * NewAIOChannelMask( unsigned number_channels ) {
    AIOChannelMask *tmp = (AIOChannelMask *)malloc(sizeof(AIOChannelMask ));
    unsigned i;
    if( !tmp ) {
        goto out_NewAIOChannelMask;
    }
    tmp->size = ((number_channels+BITS_PER_BYTE-1)/BITS_PER_BYTE); /* Ceil function */

    tmp->signals = (aio_channel_obj *)calloc(sizeof(aio_channel_obj), ( tmp->size + 1) );
    if( !tmp->signals ) 
        goto out_cleansignals;
    tmp->strrep          = 0;
    tmp->strrepsmall     = 0;
    tmp->number_signals  = number_channels;
    tmp->active_signals  = 0;

    tmp->signal_indices = (int*)malloc(sizeof(int)*(number_channels+1) );
    tmp->signal_index = 0;
    for ( i = 0; i < number_channels + 1 ; i ++ ) {
        tmp->signal_indices[i] = -1;
    }
 out_NewAIOChannelMask:
    return tmp;
 out_cleansignals:
    free(tmp);
    tmp = NULL;
    return tmp;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Destructor for the AIOChannelMask object
 * @param mask AIOChannelMask to delete
 */
void DeleteAIOChannelMask( AIOChannelMask *mask ) {
    if ( mask->strrep ) 
        free(mask->strrep);
    if( mask->strrepsmall )
        free( mask->strrepsmall );
    free(mask->signal_indices );
    free(mask->signals);
    free(mask);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Returns an interator to the indices that are valid high ( 1).
 */
AIORET_TYPE AIOChannelMaskIndices( AIOChannelMask *mask , int *pos ) {
    AIORET_TYPE retval;
    if ( !mask || !pos )
        return -AIOUSB_ERROR_INVALID_DATA;
    *pos = 0;
    retval = mask->signal_indices[*pos];
    *pos +=1;
    return retval;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Part of the iterator pair of functions for finding the indices where
 * the mask has a 1. 
 */
AIORET_TYPE AIOChannelMaskNextIndex( AIOChannelMask *mask , int *pos ) {
    int retval;
    if ( *pos >= (int)mask->number_signals ) {
        retval = *pos = -1;
    } else {
        retval = mask->signal_indices[*pos];
        *pos +=1 ;
    }
    return retval;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Sets the AIOChannelMask using the regular notion of or'ing of shifted bytes, 
 * 
 */
AIORET_TYPE AIOChannelMaskSetMaskFromInt( AIOChannelMask *obj, unsigned field ) {
    int i;
    AIORET_TYPE ret = AIOUSB_SUCCESS;  
    if ( obj->size <  (int)( sizeof(field) / sizeof(aio_channel_obj )) ) {
        return -AIOUSB_ERROR_INVALID_PARAMETER;
    }
    obj->signal_index = 0;
    for ( i = 0; i < obj->size ; i ++ ) {
      char curfield = *(((char *)&field)+i);
      obj->signals[(obj->size-1-i)] = curfield;
      for( int j = 0; j < BITS_PER_BYTE; j ++ ) {
         if( ( 1 << j ) & curfield ? 1 : 0 ) { 
           obj->active_signals ++;
           obj->signal_indices[obj->signal_index++] = (i*BITS_PER_BYTE) + j;
         }
      }
    }
    return ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Sets the Bit Mask at specified index to the values contained in 
 * field
 */
AIORET_TYPE AIOChannelMaskSetMaskAtIndex( AIOChannelMask *obj, char field, unsigned index  )
{
    if ( index >= (unsigned)obj->size )
        return -AIOUSB_ERROR_INVALID_INDEX;
    

    obj->signals[obj->size-index-1] = field;
    return AIOUSB_SUCCESS;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Retrieves the mask at offset index, and saves it to tmp
 * @param obj The AIOChannelMask bit mask object
 * @param *tmp The object we save the BitMask to
 * @param index into the AIOChannelMask that we wish to retrieve the bitmask for
 */
AIORET_TYPE AIOChannelMaskGetMaskAtIndex( AIOChannelMask *obj , char *tmp , unsigned index ) {
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    if( index >= (unsigned)obj->size )
        return -AIOUSB_ERROR_INVALID_INDEX;
    *tmp = obj->signals[obj->size-1-index];
    return retval;
}
/*----------------------------------------------------------------------------*/
/**
 *  @brief Gives the size of the given BitMask
 */
AIORET_TYPE AIOChannelMaskGetSize( AIOChannelMask *obj ) {
     return (AIORET_TYPE)obj->size;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Returns channels that are set to High ( not low ) * 
 * @param obj 
 * @return 
 */
AIORET_TYPE AIOChannelMaskNumberChannels( AIOChannelMask *obj ) {
    return (AIORET_TYPE)obj->active_signals;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOChannelMaskNumberSignals( AIOChannelMask *obj ) {
    return (AIORET_TYPE)obj->number_signals;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Rely on the base type to determine the sizes
 * @param obj 
 * @param bitfields a character string that contains 0s and 1s. 
 *
 */

AIORET_TYPE AIOChannelMaskSetMaskFromStr( AIOChannelMask *obj, const char *bitfields ) {
    AIORET_TYPE ret = AIOUSB_SUCCESS;
    unsigned j;
    aio_channel_obj tmpval = 0;
    int number_channels = strlen(bitfields);
    obj->signal_index = 0;
    obj->number_signals  = number_channels;
    obj->signals = (char*)realloc( obj->signals, strlen(bitfields)*sizeof(char));
    obj->signal_indices = (int*)realloc( obj->signal_indices, sizeof(int)*(strlen(bitfields)) );
    obj->size = (strlen(bitfields) + BITS_PER_BYTE-1 ) / BITS_PER_BYTE ;
    memset(obj->signals,0,obj->size);
    int index = obj->size-1;
    for( int i = strlen(bitfields) , shiftval = 0; i > 0 ; i -- , shiftval = (shiftval + 1)%BITS_PER_BYTE ) {
        j = strlen(bitfields)-i;
        if( bitfields[i-1] == '1' ) {
            tmpval |= 1 << shiftval;
            obj->active_signals ++;
            obj->signal_indices[obj->signal_index++] = j;
        }
        if ( j > 0 && (( (j+1) % BITS_PER_BYTE) == 0)  ) {
            obj->signals[index--] = tmpval;
            tmpval = 0;
        }
    }
    obj->signals[index] = tmpval;

    return ret;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Creates a new AIOChannelMask object from a character string of 1's and 0's
 * @param bitfields
 * @return a new AIOChannelMask object 
 * @todo Add smarter error checking
 */
AIOChannelMask *NewAIOChannelMaskFromStr( const char *bitfields ) {
    AIOChannelMask *tmp = NewAIOChannelMask( strlen(bitfields) );
    AIOChannelMaskSetMaskFromStr( tmp, bitfields );
    return tmp;
}

/*----------------------------------------------------------------------------*/
AIOChannelMask *NewAIOChannelMaskFromChr( const char bits )
{
    AIOChannelMask *tmp = NewAIOChannelMask( sizeof(bits) );
    char tmpbuf[9];
    int i;
    for ( i = 0; i <= 7 ; i ++ )
        tmpbuf[i]= ( (bits & (1 << ( 7-i) )) ? '1' : '0' );
    tmpbuf[8] = '\0';
    AIOChannelMaskSetMaskFromStr( tmp, tmpbuf );
    return tmp;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Returns a string representation for the AIOChannel Bit mask in question
 * @param obj AIOChannelMask to convert to string form
 */
char *AIOChannelMaskToString( AIOChannelMask *obj ) {
     if( obj->strrep ) {
         free(obj->strrep);
     }
     obj->strrep = (char *)malloc( obj->number_signals+1*sizeof(char) );
     memset(obj->strrep,0,obj->number_signals+1);
     char *retval = obj->strrep;
     int i, j, pos, startpos;
     pos = 0;
     for ( i = 0;  i < obj->size; i ++ ) {
         /** @todo Check for the case where we have say 17 signals( non-integer multiple of 
          * BITS_PER_BYTE 
          */
         if ( i == 0 && (obj->number_signals % BITS_PER_BYTE != 0) ) {
             startpos = (( obj->number_signals % BITS_PER_BYTE ) - 1);
         } else {
             startpos = BITS_PER_BYTE-1;
         }
         for ( j = startpos ; j >= 0 && pos < (int)obj->number_signals ; j -- ) { 
             retval[pos] = ((( 1 << j ) & ( obj->signals[i] )) ? '1' : '0');
             pos ++;
         }
     }

     return retval;
 }
/*----------------------------------------------------------------------------*/
/**
 * @brief Returns a mask for the index in question
 *
 * @param obj AIOChannelMask bit mask object
 * @param index into byte array that we wish to return a byte worth of bits from
 */
char *AIOChannelMaskToStringAtIndex( AIOChannelMask *obj, unsigned index ) {
    if ( index >= (unsigned)obj->size ) {
        return NULL;
    }
    obj->strrepsmall = (char *)realloc( obj->strrepsmall, obj->number_signals+1 );
    memset(obj->strrepsmall,0,obj->number_signals+1);
    char *retval = obj->strrepsmall;
    int i, j, pos, startpos;
    pos = 0;

    i = ( obj->size - index - 1 );
        /**
         * @note Check for the case where we have say 17 signals( non-integer multiple of 
         * BITS_PER_BYTE 
         */
    if ( i == 0 && (obj->number_signals % BITS_PER_BYTE != 0) ) {
        startpos = (( obj->number_signals % BITS_PER_BYTE ) - 1);
    } else {
        startpos = BITS_PER_BYTE-1;
    }
    for ( j = startpos ; j >= 0 ; j -- ) { 
        retval[pos] = ((( 1 << j ) & ( obj->signals[i] )) ? '1' : '0');
        pos ++;
    }
    return retval;
}

/*----------------------------------------------------------------------------*/
char *AIOChannelMaskGetMask( AIOChannelMask *obj ) {
    char *tmp = (char *)malloc(obj->size+1);
    if ( tmp ) {
        memset(tmp,0,obj->size+1);
        /* memcpy(tmp,obj->signals,obj->size); */
        for ( int i = 0; i < obj->size ; i ++ ) 
            tmp[i] = obj->signals[i];
    }
    return tmp;
}


#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST
/**
 * @brief Self test for verifying basic functionality of the AIOChannelMask interface
 */ 

#include "gtest/gtest.h"

using namespace AIOUSB;


TEST(AIOChannelMask, Channel_Mask_from_int ) {
    int expected[] = {0,1,3,7,30};
    int received[4] = {0};
    int i,j,pos;
    AIOChannelMask *mask = NewAIOChannelMask( 32 );
    AIOChannelMaskSetMaskFromInt( mask , 1 | 2 | 1 << 3 | 1 << 7 | 1 << 30 );
    EXPECT_STREQ( "01000000000000000000000010001011", AIOChannelMaskToString( mask ) );
    EXPECT_STREQ( "10001011" , AIOChannelMaskToStringAtIndex( mask, 0 ));
    EXPECT_EQ( 5, AIOChannelMaskNumberChannels( mask ));
    j = 0;
    pos = 0;
    for ( i = AIOChannelMaskIndices(mask, &j ) ; i >= 0 ; i = AIOChannelMaskNextIndex(mask, &j ) ) { 
        received[pos] = i;
        EXPECT_EQ( expected[pos], received[pos] );
        pos ++;
    }
    DeleteAIOChannelMask( mask );
}

TEST(AIOChannelMask, Channel_Mask_From_String ) {
    int expected[] = {0,1,3,7,30};
    int expected_long[] = {0,1,3,7,30,32,33,35,39,62};
    int received[4] = {0};
    int i,j,pos;
    char tmpmask;
    AIOChannelMask *mask = NewAIOChannelMaskFromStr( "0100000000000000000000001000101101000000000000000000000010001011" );
    /* twice as long, verify that index works and that couning still works */
    EXPECT_STREQ( "01000000", AIOChannelMaskToStringAtIndex( mask, 7 ));
    EXPECT_EQ( 10, AIOChannelMaskNumberChannels( mask ) );
    EXPECT_STREQ( "00000000", AIOChannelMaskToStringAtIndex( mask, 1 ));
    EXPECT_STREQ( "01000000", AIOChannelMaskToStringAtIndex( mask, 3 ));
    AIOChannelMaskSetMaskAtIndex( mask, 0xff, 2 );
    EXPECT_STREQ( "11111111", AIOChannelMaskToStringAtIndex( mask, 2 ));
    AIOChannelMaskGetMaskAtIndex( mask, &tmpmask, 2 );
    EXPECT_EQ( 0xff, (unsigned char)tmpmask );
    AIOChannelMaskSetMaskAtIndex( mask, 0xf0, 2 );
    EXPECT_STREQ( "11110000" , AIOChannelMaskToStringAtIndex(mask, 2 ));

    pos = 0;
    j = 0;
    for ( i = AIOChannelMaskIndices( mask, &j ); i >= 0 ; i = AIOChannelMaskNextIndex( mask, &j )) { 
        received[pos] = i;
        EXPECT_EQ( expected_long[pos], received[pos] );
        pos ++;
    }
    DeleteAIOChannelMask( mask );

}

TEST(AIOChannelMask, Mask_Indices ) {
    char signals[32] = {'\0'};
    int expected[] = {0,1,3,7,30};
    int expected_long[] = {0,1,3,7,30,32,33,35,39,62};
    int received[4] = {0};
    int i,j,pos;
    char tmpmask;
    AIOChannelMask *mask;
    strcpy(signals,"00000000000001000011000101011111" );
    mask = NewAIOChannelMask( strlen(signals) );
    AIOChannelMaskSetMaskFromStr( mask, signals );
    EXPECT_STREQ( "01011111", AIOChannelMaskToStringAtIndex( mask, 0 ));
    EXPECT_STREQ( "00000100", AIOChannelMaskToStringAtIndex( mask, 2 ));
    EXPECT_EQ( 10, AIOChannelMaskNumberChannels( mask ));
    DeleteAIOChannelMask( mask );
}

TEST(AIOChannelMask, Testing_indices ) {
    char signals[32] = {'\0'};
    int expected[] = {0,1,3,7,30};
    int expected_long[] = {0,1,3,7,30,32,33,35,39,62};
    int received[4] = {0};
    int i,j,pos;
    char tmpmask;
    AIOChannelMask *mask;
    mask = NewAIOChannelMaskFromStr( "010101010101010100" );
    EXPECT_STREQ( "01010100", AIOChannelMaskToStringAtIndex ( mask, 0 ));
    EXPECT_STREQ( "01010101", AIOChannelMaskToStringAtIndex( mask, 1 ));
    EXPECT_STREQ( "01", AIOChannelMaskToStringAtIndex( mask, 2 ));
    EXPECT_STREQ( "010101010101010100", AIOChannelMaskToString( mask ));
    EXPECT_EQ( 8, AIOChannelMaskNumberChannels( mask ));
    DeleteAIOChannelMask( mask );
}

TEST(AIOChannelMask, One_more_thing ) {
    char signals[32] = {'\0'};
    int received[4] = {0};
    int i,j,pos;
    char tmpmask;
    AIOChannelMask *mask;
    mask = NewAIOChannelMaskFromStr("01010100011001010111001101110100");
    EXPECT_STREQ( "Test", AIOChannelMaskGetMask( mask ) );
    DeleteAIOChannelMask( mask );
}

TEST(AIOChannelMask, Setup_After_null_initialization ) {
    AIOChannelMask *mask = NewAIOChannelMaskFromStr("1111");
    char tmpmask;
    int retval;
    EXPECT_STREQ( "1111", AIOChannelMaskToString(mask ));
    AIOChannelMaskGetMaskAtIndex( mask, &tmpmask, 0 );
    EXPECT_EQ( 15, tmpmask );
    DeleteAIOChannelMask( mask );
    mask = NewAIOChannelMask(0);
    retval = (int)AIOChannelMaskSetMaskFromStr(mask, "1010");
    EXPECT_EQ(0, retval );
    EXPECT_STREQ( "1010", AIOChannelMaskToString( mask ));
    EXPECT_EQ(4, AIOChannelMaskNumberSignals( mask ));
    EXPECT_EQ(2, AIOChannelMaskNumberChannels( mask ));
}
TEST(AIOChannelMask, NewChannelMaskFromChar ) { 
    AIOChannelMask *mask = NewAIOChannelMaskFromChr( (const char)0xAA ) ;
    EXPECT_STREQ( "10101010", AIOChannelMaskToString(mask) );
    
    DeleteAIOChannelMask( mask );
    
    mask = NewAIOChannelMaskFromChr( -1 );
    EXPECT_STREQ( "11111111", AIOChannelMaskToString(mask) );
    DeleteAIOChannelMask( mask );
}


int main(int argc, char *argv[] )
{
  AIORET_TYPE retval;
  int i,j,pos;
  char signals[32] = {'\0'};
  char tmpmask;
  int expected[] = {0,1,3,7,30};
  int expected_long[] = {0,1,3,7,30,32,33,35,39,62};
  int received[4] = {0};
  AIOChannelMask *mask = NewAIOChannelMask( 32 );

  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
#ifdef GTEST_TAP_PRINT_TO_STDOUT
  delete listeners.Release(listeners.default_result_printer());
#endif

  return RUN_ALL_TESTS();  

}

#endif
