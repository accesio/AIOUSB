/**
 * @file   AIOFifo.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  General structure for AIOUSB Fifo
 *
 */

#include "AIOTypes.h"
#include "AIOFifo.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <stdarg.h>


#ifdef __cplusplus
namespace AIOUSB {
#endif 

size_t delta( AIOFifo *fifo  ) 
{
    return ( fifo->write_pos < fifo->read_pos ? (fifo->read_pos - fifo->write_pos - 1 ) : ( (fifo->size - fifo->write_pos) + fifo->read_pos - 1 ));
}

AIORET_TYPE AIOFifoWriteSizeRemaining( void *tmpfifo )
{
    AIOFifo *fifo = (AIOFifo*)tmpfifo;
    return fifo->delta((AIOFifo*)fifo);
}

AIORET_TYPE AIOFifoWriteSizeRemainingNumElements( void *tmpfifo )
{
    return AIOFifoWriteSizeRemaining(tmpfifo) / ((AIOFifo*)tmpfifo)->refsize;
}

AIORET_TYPE AIOFifoGetSize( void *tmpfifo )
{
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_AIOFIFO, tmpfifo );
    AIOFifo *fifo = (AIOFifo*)tmpfifo;
    return fifo->size - fifo->refsize ;
}

AIORET_TYPE AIOFifoGetSizeNumElements( void *tmpfifo )
{
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_AIOFIFO, tmpfifo );
    AIOFifo *fifo = (AIOFifo*)tmpfifo;
    return fifo->size / fifo->refsize - 1;
}


size_t rdelta( AIOFifo *fifo  ) 
{
    return ( fifo->read_pos <= fifo->write_pos ? (fifo->write_pos - fifo->read_pos ) : ( (fifo->size - fifo->read_pos) + fifo->write_pos ));
}

AIORET_TYPE AIOFifoReadSize( void *tmpfifo )
{
    AIOFifo *fifo = (AIOFifo*)tmpfifo;
    return rdelta( (AIOFifo*)fifo  );
}

AIORET_TYPE AIOFifoReadSizeNumElements( void *tmpfifo )
{
    return AIOFifoReadSize( tmpfifo ) / ((AIOFifo*)tmpfifo)->refsize;
}

AIORET_TYPE _AIOFifoResize( AIOFifo *fifo, size_t newsize )
{
    fifo->data = realloc( fifo->data, newsize );
    if ( !fifo->data ) 
        return -AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
    else 
        fifo->size = newsize;
    return AIOUSB_SUCCESS;
}

AIORET_TYPE AIOFifoResize( AIOFifo *fifo, size_t newsize )
{
    fifo->data = realloc( fifo->data, (newsize+1)*fifo->refsize );
    if ( !fifo->data ) 
        return -AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
    else 
        fifo->size = (newsize+1)*fifo->refsize;
    return AIOUSB_SUCCESS;
}

size_t _calculate_size_write( AIOFifo *fifo, unsigned maxsize)
{
    int actsize = MIN(MIN( maxsize, fifo->size ), fifo->delta(fifo ));
    actsize = (actsize / fifo->refsize) * fifo->refsize;
    return actsize;
}

size_t _calculate_size_read( AIOFifo *fifo, unsigned maxsize)
{
    return MIN(MIN( maxsize, fifo->size), rdelta(fifo) );
}

size_t _calculate_size_aon_write( AIOFifo *fifo, unsigned maxsize)
{
    return ( fifo->delta(fifo) < maxsize ? 0 : MIN(fifo->delta(fifo), maxsize ));
}

size_t _calculate_size_aon_read( AIOFifo *fifo, unsigned maxsize )
{
    return ( rdelta(fifo) < maxsize ? 0 : maxsize );
}

void AIOFifoInitialize( AIOFifo *nfifo, unsigned int size, unsigned refsize )
{
    nfifo->size     = size;
    nfifo->refsize  = refsize;
    nfifo->data     = malloc(size);
    nfifo->Read     = AIOFifoRead;
    nfifo->Write    = AIOFifoWrite;
    nfifo->delta    = delta;
    nfifo->rdelta   = rdelta;
    nfifo->_calculate_size_write = _calculate_size_write;
    nfifo->_calculate_size_read  = _calculate_size_read;
#ifdef HAS_THREAD
    nfifo->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;   /* Threading mutex Setup */
#endif
}

AIOFifo *NewAIOFifo( unsigned int size , unsigned refsize )
{ 
    AIOFifo *nfifo  = (AIOFifo *)calloc(1,sizeof(AIOFifo));
    AIOFifoInitialize( nfifo, size, refsize );
    return nfifo;
}

void AIOFifoAllOrNoneInitialize( AIOFifo *nfifo , unsigned int size, unsigned refsize ) 
{
    nfifo->Read     = AIOFifoReadAllOrNone;
    nfifo->Write    = AIOFifoWriteAllOrNone;
    nfifo->_calculate_size_write = _calculate_size_aon_write;
    nfifo->_calculate_size_read  = _calculate_size_aon_read;
}

AIOFifo *NewAIOFifoAllOrNone( unsigned int size , unsigned refsize )
{
    AIOFifo *nfifo  = NewAIOFifo( size, refsize );
    AIOFifoAllOrNoneInitialize( nfifo, size, refsize );
    return nfifo;
}

void AIOFifoReset( void *tmpfifo )
{
    assert(tmpfifo);
    AIOFifo *fifo = (AIOFifo*)tmpfifo;
    fifo->read_pos = fifo->write_pos = 0;
}

AIORET_TYPE AIOFifoGetRefSize( void *tmpfifo )
{
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_AIOFIFO, tmpfifo );
    return ((AIOFifo *)tmpfifo)->refsize;
}


#define LOOKUP(T) aioeither_value_ ## T


AIORET_TYPE Push( AIOFifoTYPE *fifo, TYPE a )
{
    TYPE tmp = a;
    int val = fifo->Write( (AIOFifo*)fifo, &tmp, sizeof(TYPE) );
    return val;
}
AIORET_TYPE PushN( AIOFifoTYPE *fifo, INPUT_TYPE *a, unsigned N ) {
    return fifo->Write( (AIOFifo*)fifo, a, N*sizeof(TYPE));
}
AIOEither Pop( AIOFifoTYPE *fifo )
{
    TYPE tmp;
    AIOEither retval = {0};
    int tmpval = fifo->Read( (AIOFifo*)fifo, &tmp, sizeof(TYPE) );

    if( tmpval <= 0 ) {
        retval.left = tmpval;
    } else {
        AIOEitherSetRight( &retval, LOOKUP( uint32_t ), &tmp );
    }

    return retval;
}

AIORET_TYPE PopN( AIOFifoTYPE *fifo, INPUT_TYPE *in, unsigned N)
{
    AIORET_TYPE retval = {0};
    int tmpval = fifo->Read( (AIOFifo*)fifo, in, sizeof(TYPE)*N );

    retval = ( tmpval < 0 ? -AIOUSB_FIFO_COPY_ERROR : retval );
   
    return retval;
}

AIOFifoTYPE *NewAIOFifoTYPE( unsigned int size )
{
    AIOFifoTYPE *nfifo = (AIOFifoTYPE*)calloc(1,sizeof(AIOFifoTYPE));
    AIOFifoInitialize( (AIOFifo*)nfifo , (size+1)*sizeof(TYPE), sizeof(TYPE));
    nfifo->Push = Push;
    nfifo->PushN = PushN;
    nfifo->Pop = Pop;
    nfifo->PopN = PopN;
    return nfifo;
}
void DeleteAIOFifoTYPE( AIOFifoTYPE *fifo )
{
    DeleteAIOFifo( (AIOFifo*)fifo);
}


void DeleteAIOFifo( AIOFifo *fifo ) 
{
    free(fifo->data);
    free(fifo);
}

size_t increment(AIOFifo *fifo, size_t idx)
{
    return (idx + 1) % fifo->size; 
}

AIORET_TYPE AIOFifoRead( AIOFifo *fifo, void *tobuf , unsigned maxsize ) 
{
    GRAB_RESOURCE( fifo );
    int actsize = fifo->_calculate_size_read( fifo, maxsize );
    if ( actsize ) {
        int basic_copy = MIN( actsize + fifo->read_pos, fifo->size ) - fifo->read_pos, wrap_copy = actsize - basic_copy;
        memcpy( tobuf                       , &((char *)fifo->data)[fifo->read_pos], basic_copy );
        memcpy( &((char*)tobuf)[basic_copy] , &((char *)fifo->data)[0]             , wrap_copy );
        fifo->read_pos = (fifo->read_pos + actsize ) % fifo->size ;
    }
    RELEASE_RESOURCE( fifo );
    return actsize;
}

AIORET_TYPE AIOFifoWrite( AIOFifo *fifo, void *frombuf , unsigned maxsize ) {
    GRAB_RESOURCE( fifo );
    int actsize = fifo->_calculate_size_write( fifo, maxsize );
    if ( actsize ) {
        int basic_copy = MIN( actsize + fifo->write_pos, fifo->size ) - fifo->write_pos, wrap_copy = actsize - basic_copy;
        memcpy( &((char *)fifo->data)[fifo->write_pos], frombuf, basic_copy );
        memcpy( &((char *)fifo->data)[0], (void*)((char *)frombuf+basic_copy), wrap_copy );
        fifo->write_pos = (fifo->write_pos + actsize ) % fifo->size ;
    }
    RELEASE_RESOURCE( fifo );
    return actsize;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief for AllOrNoneTesting
 */
AIORET_TYPE AIOFifoWriteAllOrNone( AIOFifo *fifo, void *frombuf , unsigned maxsize ) {
    GRAB_RESOURCE( fifo );
    int actsize = fifo->_calculate_size_write( fifo, maxsize );
    if ( actsize ) {
        int basic_copy = MIN( actsize + fifo->write_pos, fifo->size ) - fifo->write_pos, wrap_copy = actsize - basic_copy;
        memcpy( &((char *)fifo->data)[fifo->write_pos], frombuf, basic_copy );
        memcpy( &((char *)fifo->data)[0], (void*)((char *)frombuf+basic_copy), wrap_copy );
        int tmp = (fifo->write_pos + actsize ) % fifo->size;
        fifo->write_pos = tmp;
    } 

    RELEASE_RESOURCE( fifo );
    return actsize;
}

AIORET_TYPE AIOFifoReadAllOrNone( AIOFifo *fifo, void *tobuf , unsigned maxsize ) 
{
    GRAB_RESOURCE( fifo );
    int actsize = fifo->_calculate_size_read( fifo, maxsize );
    if ( actsize ) { 
        int basic_copy = MIN( actsize + fifo->read_pos, fifo->size ) - fifo->read_pos, wrap_copy = actsize - basic_copy;
        memcpy( tobuf                       , &((char *)fifo->data)[fifo->read_pos], basic_copy );
        memcpy( &((char*)tobuf)[basic_copy] , &((char *)fifo->data)[0]             , wrap_copy );
        fifo->read_pos = (fifo->read_pos + actsize ) % fifo->size ;
    }
    RELEASE_RESOURCE( fifo );
    return actsize;
}

AIORET_TYPE AIOFifoReadPosition( void *nfifo )   { return ((AIOFifo *)nfifo)->read_pos ;  } ;
AIORET_TYPE AIOFifoWritePosition( void *nfifo )  { return ((AIOFifo *)nfifo)->write_pos;  } ;


TEMPLATE_AIOFIFO_API( Counts, uint16_t );
TEMPLATE_AIOFIFO_API( Volts, double );


#ifdef __cplusplus
}
#endif 

#ifdef SELF_TEST

#include "AIOUSBDevice.h"
#include "gtest/gtest.h"

#include <iostream>
#include <math.h>
using namespace AIOUSB;


TEST(Initialization,Callback )
{

    AIOFifo *bar = AIOUSB::NewAIOFifo(10000,sizeof(unsigned short));
    AIOFifo *fifo = bar;
    int retval;
    int maxsize = 4000;
    unsigned short *frombuf = (unsigned short *)malloc(20000);
    unsigned short *tobuf = (unsigned short *)malloc(20000);
    for( int i = 0; i < 10000; i ++ ) { frombuf[i] = i ; }
    
    fifo->Write( fifo, frombuf, 2000 );
    fifo->Write( fifo, frombuf, 2000 );
    fifo->Write( fifo, frombuf, 2000 );    
    fifo->Write( fifo, frombuf, 2000 );

    retval = fifo->Read( fifo, tobuf, 20000 );

    EXPECT_EQ( retval, 8000 );
    EXPECT_EQ( fifo->write_pos, 8000 );
    DeleteAIOFifo(fifo);
}

TEST(Initialization,NoOverWrite )
{
    int size = 1000;
    AIOFifo *bar = AIOUSB::NewAIOFifo((size+1)*sizeof(unsigned short),sizeof(unsigned short));
    AIOFifo *fifo = bar;
    int retval;
    int maxsize = 4000;
    unsigned short *frombuf = (unsigned short *)malloc(size*sizeof(unsigned short));
    unsigned short *tobuf = (unsigned short *)malloc(size*sizeof(unsigned short));
    for( int i = 0; i < size; i ++ ) { frombuf[i] = i ; }
    
    retval = fifo->Write( fifo, frombuf, size/2 * sizeof(unsigned short ) );
    EXPECT_EQ( size, retval );
    retval = fifo->Write( fifo, frombuf, size/2 * sizeof(unsigned short ) );
    EXPECT_EQ( size, retval );
    retval = fifo->Write( fifo, frombuf, size/2 * sizeof(unsigned short ) );

    EXPECT_EQ( 0, retval ) << "Should only be able to write an integer of the smallest size";

    retval = fifo->Read( fifo, tobuf, size );
    EXPECT_EQ( retval, size );

    EXPECT_EQ( fifo->write_pos, size*sizeof(unsigned short ) );
    DeleteAIOFifo(fifo);
}


TEST(Initialization,AllOrNoneTesting )
{
    int size = 1000;
    AIOFifo *bar = AIOUSB::NewAIOFifoAllOrNone((size+1)*sizeof(unsigned short),sizeof(unsigned short));
    AIOFifo *fifo = bar;
    int retval;
    int maxsize = 4000;
    unsigned short *frombuf = (unsigned short *)malloc(size*sizeof(unsigned short));
    unsigned short *tobuf = (unsigned short *)malloc(size*sizeof(unsigned short));
    for( int i = 0; i < size; i ++ ) { frombuf[i] = i ; }
    
    retval = fifo->Write( fifo, frombuf, (2*size* sizeof(unsigned short ))/3 );
    EXPECT_EQ( retval, (2*size * sizeof(unsigned short ))/3);
    retval = fifo->Write( fifo, frombuf, (2*size * sizeof(unsigned short ))/3 );
    EXPECT_EQ( 0, retval ) << "Should only be able to write an integer of the smallest size";

    retval = fifo->Read( fifo, tobuf, size );
    EXPECT_EQ( retval, size );

    retval = fifo->Read( fifo, tobuf, size );
    EXPECT_EQ( 0, retval ) << "Should limit the size of the Read ";

    DeleteAIOFifo(fifo);
}

TEST(NewType,PushAndPop )
{
    int size = 1000;
    int retval = 0;
    AIOEither keepvalue;
    AIOFifoTYPE *fifo = AIOUSB::NewAIOFifoTYPE( size );
    TYPE *tmp = (TYPE*)malloc(sizeof(TYPE)*size);
    for( int i = 0 ; i < size ; i ++ ) tmp[i] = (TYPE)i;

    for( int i = 0 ; i < size ; i ++ ) {
        retval = fifo->Push( fifo, tmp[i] );
        EXPECT_EQ( sizeof(TYPE), retval ) << "Expected 4==" << retval << " on index " << i ;
    }

    for (int i = 0 ; i < size ; i ++ ) { 
        TYPE tval;
        keepvalue = fifo->Pop( fifo );
        EXPECT_EQ( keepvalue.left, 0 );
        AIOEitherGetRight( &keepvalue,&tval);
        EXPECT_EQ( tval , i );
    }
    AIOUSB::DeleteAIOFifoTYPE( fifo );
}

TEST(NewType,PushAndPopArrays )
{
    int size = 1000;
    int retval = 0;
    AIOEither keepvalue;
    AIOFifoTYPE *fifo = AIOUSB::NewAIOFifoTYPE( size );
    TYPE *tmp = (TYPE*)malloc(sizeof(TYPE)*size);
    for( int i = 0 ; i < size ; i ++ ) tmp[i] = (TYPE)i;
    
    fifo->PushN( fifo, tmp, size ); /* Write the whole array at once */

    for (int i = 0 ; i < size ; i ++ ) { 
        TYPE tval;
        keepvalue = fifo->Pop( fifo );
        EXPECT_EQ( keepvalue.left, 0 );
        AIOEitherGetRight( &keepvalue,&tval);
        EXPECT_EQ( tval , i );
    }
    keepvalue = fifo->Pop( fifo );
    EXPECT_EQ( keepvalue.left, 0 );

    fifo->PushN( fifo, tmp, size );
    ASSERT_EQ( tmp[3], 3 );
    memset(tmp,0,sizeof(TYPE)*size );
    ASSERT_EQ( tmp[3], 0 );

    for ( int i  = 0; i < 10 ; i ++ ) {
        retval = fifo->PopN( fifo, tmp, size );
        EXPECT_GE( retval, 0 );
        retval = fifo->PushN( fifo, tmp, size );
        EXPECT_GE( retval, 0 );
    }


}

TEST(Counts,Testing )
{
    int size = 1000;
    AIOFifoCounts *cfifo = NewAIOFifoCounts(size);
    unsigned short tmp[size];
    AIORET_TYPE retval;
    for( int i = 0; i < size ; i ++ ) tmp[i] = i;
    
    cfifo->PushN( cfifo, tmp, size );
    
    for( int i = 0; i < size ; i ++ ) { 
        AIOEither tval = cfifo->Pop(cfifo);

        EXPECT_EQ( AIOEitherToShort( &tval , &retval ), tmp[i] );
    }
    DeleteAIOFifoCounts( cfifo );

    cfifo = NewAIOFifoCounts(size);
    int num_scans = 4000;
    int num_channels = 16;
    cfifo = NewAIOFifoCounts( num_scans*num_channels );
    int tmpsize = num_scans*num_channels*4;

    uint16_t *frombuf = (uint16_t*)malloc( sizeof(uint16_t)*tmpsize );

    for ( int i = 0 ; i < tmpsize; i ++ ) { 
        frombuf[i] = rand() % size;
    }

    retval = cfifo->PushN( cfifo, frombuf, tmpsize );

    EXPECT_EQ( 0, retval ) << "Should have not enough memory error\n";
    
    cfifo->write_pos = cfifo->read_pos = cfifo->size;
    retval = cfifo->PushN( cfifo, frombuf, size );
    EXPECT_EQ( retval, size*sizeof(uint16_t) );

    cfifo->write_pos = cfifo->read_pos = cfifo->size / 2;
    retval = cfifo->PushN( cfifo, frombuf, size );
    EXPECT_EQ( retval, size*sizeof(uint16_t) );


}

TEST(Volts,Testing )
{
    AIOFifoVolts *cfifo = NewAIOFifoVolts(1000);
    double tmp[1000];
    AIORET_TYPE retval;
    for( int i = 0; i < 1000 ; i ++ ) tmp[i] = 3.342*sqrt((double)i);
    
    cfifo->PushN( cfifo, tmp, 1000 );
    
    for( int i = 0; i < 1000 ; i ++ ) { 
        AIOEither tval = cfifo->Pop(cfifo);
        EXPECT_EQ( AIOEitherToDouble( &tval, &retval ), tmp[i] );
    }

}

TEST(AIOFifo, Resizing ) 
{
    AIOFifoVolts *vfifo = NewAIOFifoVolts( 1000 );
    EXPECT_EQ( AIOFifoGetSizeNumElements( vfifo ), 1000 ) << "Expected straight forward number of volts remaining";

    DeleteAIOFifoVolts( vfifo );

    AIOFifoCounts *counts = NewAIOFifoCounts( 1000 );
    EXPECT_EQ( AIOFifoGetSizeNumElements( counts ), 1000 ) << "Expected straight forward number of counts remaining";

    AIOFifoCountsResize( counts, 1001 );
    EXPECT_EQ( AIOFifoGetSizeNumElements( counts ), 1001 ) << "Expected straight forward number of counts remaining";

    AIOFifoResize( (AIOFifo*)counts, 123 );
    EXPECT_EQ( AIOFifoGetSizeNumElements( counts ), 123 ) << "Generic resize function";

    DeleteAIOFifoCounts( counts );

}
TEST(AIOFifo, ReadSizeInElements)
{    
    AIOFifoCounts *counts = NewAIOFifoCounts( 1000 );
    AIOFifoVolts *volts = NewAIOFifoVolts( 200 );

    EXPECT_EQ( 1000, AIOFifoWriteSizeRemainingNumElements(counts) );
    EXPECT_EQ( 200, AIOFifoWriteSizeRemainingNumElements(volts) );

    for ( int i = 0; i < 100; i ++ ) 
        counts->Push( counts, i );

    EXPECT_EQ( 100, AIOFifoReadSizeNumElements( counts ) );
    EXPECT_EQ( 1000-100, AIOFifoWriteSizeRemainingNumElements(counts)  );

    for ( int i = 0; i < 33; i ++ ) 
        volts->Push( volts, (double)i*3.14159 );

    EXPECT_EQ( 33, AIOFifoReadSizeNumElements( volts ) );
    EXPECT_EQ( 200-33, AIOFifoWriteSizeRemainingNumElements(volts)  );

    AIOFifoReset( counts );
    EXPECT_EQ( 1000, AIOFifoWriteSizeRemainingNumElements(counts));

    DeleteAIOFifoCounts( counts );
    DeleteAIOFifoVolts( volts );
}

/**
 * @brief Simple test,  1000 element buffer. Write 800 in, verify that the first 300 are correct
 * write in an additional 500 and varify that we are at the maximum for the buffer. Then
 * read out the rest and verify that the values were correct.
 */
TEST(AIOFifo, ReadSizeInElementsWithLoop)
{
    AIOFifoCounts *counts = NewAIOFifoCounts( 1000 );
    AIORET_TYPE retval = 0;
    int i,j;
    for ( i = 0,j=0; j < 800; j ++ , i ++ ) 
        counts->Push( counts, i );
    for ( j = 0; j < 300; j ++ ) {
        retval = AIOEitherToInt( counts->Pop( counts ) );
        ASSERT_EQ( j, retval );
    }
    for ( j = 0; j < 500; j ++ , i ++ ) {
        retval = counts->Push( counts, i );
        ASSERT_GE( retval, 0 );
    }

    ASSERT_EQ( 1000, AIOFifoReadSizeNumElements( counts ) );
    ASSERT_EQ( 0, AIOFifoWriteSizeRemainingNumElements( counts ));

    for ( j = 0; j < 1000; j ++ ) {
        retval = AIOEitherToInt( counts->Pop( counts ) );
        ASSERT_EQ( j+300, retval );
    }

    DeleteAIOFifoCounts( counts );
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
