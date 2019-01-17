/**
 * @file   AIOCountsConverter.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  General header files for the AIOUSB library
 *
 */

#include "AIOTypes.h"
#include "AIOUSB_Core.h"
#include "AIOCountsConverter.h"
#include "AIOUSB_Log.h"
#include <pthread.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif 

int default_out( AIOCountsConverter *cc, unsigned rounded_num_counts )
{
    return cc->converted_count < rounded_num_counts;
}

int enhanced_out( AIOCountsConverter *cc, unsigned rounded_num_counts )
{
    int tmp =  ( cc->converted_count < rounded_num_counts && cc->scan_count < cc->num_scans );
    return tmp;
}


/*----------------------------------------------------------------------------*/
AIOCountsConverter *NewAIOCountsConverterWithBuffer( void *buf, 
                                                     unsigned num_channels, 
                                                     AIOGainRange *ranges, 
                                                     unsigned num_oversamples, 
                                                     unsigned unit_size  
                                                     ) 
{
    AIOCountsConverter *tmp = NewAIOCountsConverter( num_channels, ranges, num_oversamples , unit_size );
    if (!tmp ) 
        return NULL;

    tmp->buf =   buf;

    return tmp;
}


AIOCountsConverter *NewAIOCountsConverterWithScanLimiter( void *buf, 
                                                          unsigned num_scans,
                                                          unsigned num_channels, 
                                                          AIOGainRange *ranges, 
                                                          unsigned num_oversamples, 
                                                          unsigned unit_size  
                                                          ) 
{
    AIOCountsConverter *tmp = NewAIOCountsConverter( num_channels, ranges, num_oversamples , unit_size );
    if (!tmp ) 
        return NULL;
    tmp->buf                  = buf;
    tmp->num_scans            = num_scans;
    tmp->continue_conversion  = enhanced_out;
    return tmp;
}

/*----------------------------------------------------------------------------*/
AIOCountsConverter *NewAIOCountsConverter( unsigned num_channels, AIOGainRange *ranges, unsigned num_oversamples, unsigned unit_size  ) {
    AIOCountsConverter *tmp = (AIOCountsConverter *)calloc(1,sizeof(struct aio_counts_converter) );
    if (!tmp ) 
        return NULL;
    tmp->num_oversamples  = num_oversamples;
    tmp->num_channels     = num_channels;
    tmp->gain_ranges      = ranges; /* Gain ranges should be same length as num_channels */
    tmp->unit_size        = unit_size;
    tmp->Convert          = AIOCountsConverterConvert;
    tmp->ConvertFifo      = AIOCountsConverterConvertFifo;
    tmp->continue_conversion = default_out;
    return tmp;
}

/*----------------------------------------------------------------------------*/
void DeleteAIOCountsConverter( AIOCountsConverter *ccv )
{
    free(ccv);
}

void AIOCountsConverterReset( AIOCountsConverter *cc )
{
    assert(cc);
    cc->scan_count = cc->os_count = cc->channel_count = 0;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOCountsConverterConvertNScans( AIOCountsConverter *ccv, int num_scans )
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOCountsConverterConvertAllAvailableScans( AIOCountsConverter *ccv )
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    return retval;
}

/*----------------------------------------------------------------------------*/
double Convert( AIOGainRange range, unsigned short sum )
{
    return ((double)(range.max - range.min)*sum )/ ((( unsigned short )-1)+1) + range.min;
}



/*----------------------------------------------------------------------------*/
/**
 * @param cc Counts converter object
 * @param tobufptr  ToFifo  ( double )
 * @param frombufptr From Fifo (unsigned short )
 * @param num_counts  number of counts to convert
 * 
 * @return Number of tobufptr objects that have been created
 */
AIORET_TYPE AIOCountsConverterConvertFifo( AIOCountsConverter *cc, void *tobufptr, void *frombufptr , unsigned num_counts )
{
    AIOFifoVolts *tofifo     = (AIOFifoVolts*)tobufptr;
    AIOFifoCounts *fromfifo  = (AIOFifoCounts*)frombufptr;

    cc->converted_count = 0;
    double tmpvolt;
    int pos;
    unsigned rounded_num_counts = num_counts;
    unsigned short *tmpbuf = (unsigned short *)malloc( num_counts*sizeof(uint16_t) );

    int tmpval = fromfifo->PopN( fromfifo, tmpbuf, rounded_num_counts );
    if ( tmpval != (int)rounded_num_counts*(int)sizeof(uint16_t ) ) {
        return -3;
    }
    int num_converted = 0;
    int initial = (cc->scan_count *(cc->num_channels)*(cc->num_oversamples + 1)) + 
        cc->channel_count * ( cc->num_oversamples + 1) + cc->os_count;

    for ( int tobuf_pos = 0; cc->continue_conversion( cc, rounded_num_counts) ; cc->scan_count ++ ) {
        for ( ; cc->channel_count < cc->num_channels && cc->converted_count < rounded_num_counts; cc->channel_count ++ , tobuf_pos ++  ) {
            for ( ; cc->os_count < (cc->num_oversamples + 1) && cc->converted_count < rounded_num_counts; cc->os_count ++ ) {

                pos = (cc->scan_count *(cc->num_channels)*(cc->num_oversamples + 1)) + 
                    cc->channel_count * ( cc->num_oversamples + 1) + cc->os_count - initial;

                cc->sum += tmpbuf[pos];

                cc->converted_count ++;
            }
            if ( cc->os_count >= (cc->num_oversamples + 1) ) { 
                cc->os_count = 0;
                cc->sum /= (cc->num_oversamples + 1);
                tmpvolt = (double)Convert( cc->gain_ranges[cc->channel_count], cc->sum );
                tofifo->Push( tofifo, tmpvolt );
                num_converted ++;
                cc->sum = 0;
            } else {
                AIOUSB_DEVEL("Leaving !\n");
                goto done_procssing;
            }
        }
        if ( cc->channel_count >= cc->num_channels ) {
            cc->channel_count = 0;
        } else {
            AIOUSB_DEVEL("other leaving\n");
            goto done_procssing;
        }
    }
 done_procssing:
    free(tmpbuf);
    return num_converted;

}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOCountsConverterConvert( AIOCountsConverter *cc, void *to_buf, void *from_buf, unsigned num_bytes )
{
    int allowed_scans = num_bytes / (cc->num_oversamples * cc->num_channels * cc->unit_size );
    AIORET_TYPE count = 0;

    for ( int scan_count = 0, tobuf_pos = 0; scan_count < allowed_scans ; scan_count ++ ) {
        for ( unsigned ch = 0; ch < cc->num_channels; ch ++ , tobuf_pos ++ ) { 
            unsigned sum = 0;
            for ( unsigned os = 0; os < cc->num_oversamples + 1; os ++ ) {
                sum += ((unsigned short *)from_buf)[ (scan_count*cc->num_channels) + ch + os ];
                count += sizeof(unsigned short);
            }
            sum /= (cc->num_oversamples + 1);
            ((double *)to_buf)[tobuf_pos] = Convert( cc->gain_ranges[ch], sum );
        }
    }

    return count;
}

/*----------------------------------------------------------------------------*/

PUBLIC_EXTERN AIOGainRange* NewAIOGainRangeFromADCConfigBlock( ADCConfigBlock *adc )
{
    AIOGainRange *tmp = (AIOGainRange *)calloc(16,sizeof(AIOGainRange));
    if (!tmp )
        return tmp;

    for ( int i = 0; i < AD_NUM_GAIN_CODE_REGISTERS ; i ++ ) {
        tmp[i].min = adRanges[ adc->registers[i] ].minVolts;
        tmp[i].max = adRanges[ adc->registers[i] ].minVolts + adRanges[ adc->registers[i] ].range;
    }

    return tmp;
}

/*----------------------------------------------------------------------------*/
PUBLIC_EXTERN void DeleteAIOGainRange( AIOGainRange *agr )
{
    free(agr);
}


#ifdef __cplusplus
}
#endif


#ifdef SELF_TEST

#include "AIOUSBDevice.h"
#include "AIOUSB_Core.h"
#include "AIOFifo.h"
#include "gtest/gtest.h"

#include <iostream>
using namespace AIOUSB;


/**
 * @todo Want the API to set the isInit flag in case the device is ever added
 */
TEST(Initialization,BasicInit )
{
    unsigned short *counts = (unsigned short *)malloc(2000);
    AIOCountsConverter *cc = NewAIOCountsConverterWithBuffer( counts, 16, NULL, 20 , sizeof(unsigned short)  );

    DeleteAIOCountsConverter(cc);
}


TEST(Initialization,Callback )
{

    AIOGainRange *ranges = (AIOGainRange *)malloc(16*sizeof(AIOGainRange));
    int num_channels     = 16;
    int num_oversamples  = 20;
    int num_scans        = 1000;

    int total_size       = num_channels * num_oversamples * num_scans;

    unsigned short *from_buf = (unsigned short *)malloc(total_size*sizeof(unsigned short));
    double *to_buf   = ( double *)malloc(total_size*sizeof(double));

    for ( int i = 0; i < num_channels; i ++ ) {
        ranges[i].max = 10.0;
        ranges[i].min = -10.0;
    }

    AIOCountsConverter *cc = NewAIOCountsConverterWithBuffer( from_buf, num_channels, ranges, num_oversamples , sizeof(unsigned short)  );
    
    for ( int i = 0; i < total_size; i++ )
        from_buf[i] = (((unsigned short)-1)+1) / 2;

    /**
     * @brief When we convert this buffer, we expect the oversmaples to go away ( so total size
     * will be num_channels * num_scans;
     *
     * We also expect that the voltages should be halfway between the min and max since we are
     * using the halfway value of an unsigned short
     */
    int count = cc->Convert( cc, to_buf, from_buf , total_size*sizeof(unsigned short) );
    EXPECT_EQ( num_scans*num_channels*(1+num_oversamples)*sizeof(unsigned short), count ) << "We should remove all of the oversamples and " << 
        "leave just the number_of_scans * number_of_channels";
    
    EXPECT_EQ( to_buf[0], (ranges[0].max + ranges[0].min) / 2 );
    DeleteAIOCountsConverter(cc);
}


/**
 * @brief This test checks whether when we try to convert an incomplete number of 
 *        channels and oversamples ( ie less than integer number of scans) , that
 *        the conversion comes out correct.
 */
TEST(Composite,IncompleteConversion )
{

    int num_channels     = 16;
    int num_oversamples  = 20;
    int num_scans        = 1000;
    int retval = 0;
    int total_size        = num_channels * (num_oversamples+1) * num_scans;
    AIOFifoCounts *infifo = NewAIOFifoCounts( (unsigned)num_channels*(num_oversamples+1)*num_scans );
    AIOFifoVolts *outfifo = NewAIOFifoVolts( num_channels*(num_oversamples+1)*num_scans );
    unsigned short *from_buf = (unsigned short *)malloc(total_size*sizeof(unsigned short));
    int abspos = 0;
    int channel_count, scan_count, os_count;


    AIOGainRange *ranges = (AIOGainRange *)malloc(num_channels*sizeof(AIOGainRange));
    for ( int i = 0; i < num_channels; i ++ ) {
        ranges[i].max = 10.0;
        ranges[i].min = 0.0;
    }

    AIOCountsConverter *cc = NewAIOCountsConverterWithBuffer( from_buf, num_channels, ranges, num_oversamples , sizeof(unsigned short)  );

    channel_count = 0;
    for ( os_count = 0; os_count < (num_oversamples + 1) / 3; os_count ++ , abspos ++ ) {
        from_buf[abspos] = channel_count * 1000;
    }

    retval = infifo->PushN( infifo, from_buf, abspos );
    retval = cc->ConvertFifo( cc, outfifo, infifo, abspos );
    ASSERT_EQ( 0, cc->scan_count  );
    ASSERT_EQ( 0, cc->channel_count );
    ASSERT_EQ( abspos , cc->os_count );


    retval = infifo->PushN( infifo, from_buf, (num_oversamples + 1) - abspos );
    retval = cc->ConvertFifo( cc, outfifo, infifo, (num_oversamples + 1) - abspos  );
    ASSERT_EQ( 0, cc->scan_count  );
    ASSERT_EQ( 1, cc->channel_count );
    ASSERT_EQ( 0 , cc->os_count );

    retval = infifo->PushN( infifo, from_buf, (num_channels-1)*(num_oversamples+1) );
    retval = cc->ConvertFifo( cc, outfifo, infifo, (num_channels-1)*(num_oversamples+1) );

    ASSERT_EQ( 1, cc->scan_count  );
    ASSERT_EQ( 0, cc->channel_count );
    ASSERT_EQ( 0 , cc->os_count );

    AIOFifoReset( (AIOFifo*)infifo);
    AIOFifoReset( (AIOFifo*)outfifo);
    AIOCountsConverterReset( cc );
    abspos = 0;
    for (  scan_count = 0; scan_count < num_scans / 3; scan_count ++ )  {
        for (  channel_count = 0; channel_count < num_channels ; channel_count ++ ) {
            for (  os_count = 0; os_count < num_oversamples + 1; os_count ++ , abspos ++ ) {
                from_buf[abspos] = channel_count * 1000;
            }
        }
    }

    retval = infifo->PushN( infifo, from_buf, abspos );
    retval = cc->ConvertFifo( cc, outfifo, infifo, abspos );
    ASSERT_EQ( num_scans / 3, cc->scan_count  );
    ASSERT_EQ( num_scans/3 * num_channels , retval ) << "We should have num_scans * num_channels converted";
    


    /*-------------------------------------- Now we just a few whole channels --------------------*/
    abspos = 0;
    /* Now let's add a few extra channels extra */
    for (  channel_count = 0; channel_count < num_channels / 3 ; channel_count ++ ) {
        for ( os_count = 0; os_count < num_oversamples + 1; os_count ++ , abspos ++ ) {
            from_buf[abspos] = channel_count * 1000;
        }
    }
    /*----------------------------  last extra in terms of oversamples ----------------------------*/
    for ( os_count = 0; os_count < (num_oversamples + 1) / 3; os_count ++ , abspos ++ ) {
        from_buf[abspos] = channel_count * 1000;
    }

    retval = infifo->PushN( infifo, from_buf, abspos );
    retval = cc->ConvertFifo( cc, outfifo, infifo , abspos );
    /* Verify that we have the outfifo has had the correct number of counts converted */
    EXPECT_EQ( abspos, cc->converted_count ) << "We should be able to read out " << abspos << " number of bytes which is not an integer number of scans";
    ASSERT_EQ( (((num_scans / 3)*num_channels + channel_count)*sizeof(double)), outfifo->write_pos );
    ASSERT_EQ( retval, num_channels / 3 ) << "We should only have converted the Full number (" << (num_channels/3) << ") number of channels";



    /*----------------------------------------------------------------------------*/

    abspos = 0;
    for ( ; os_count < num_oversamples + 1; os_count ++  , abspos ++ ) {
        from_buf[abspos] = channel_count * 1000;
    }
    retval = infifo->PushN( infifo, from_buf, abspos );
    retval = cc->ConvertFifo( cc, outfifo, infifo, abspos );
    
    ASSERT_EQ( num_channels / 3 + 1, cc->channel_count );
    ASSERT_EQ( 0 , cc->os_count );
    channel_count ++;

    /* Read the remaining channels & oversamples to complete the one scan */
    abspos = 0;
    for ( ; channel_count < num_channels ; channel_count ++ ) {
        for ( os_count = 0; os_count < num_oversamples + 1; os_count ++  , abspos ++ ) {
            from_buf[abspos] = channel_count * 1000;
        }
    }
    /* EXPECT_EQ( (num_channels - (num_channels/3))*(num_oversamples+1) + ( num_oversamples +1 - ((num_oversamples + 1) / 3)), abspos ); */

    /* add the data */
    retval = infifo->PushN( infifo, from_buf, abspos );
    /* Convert it */
    retval = cc->ConvertFifo( cc, outfifo, infifo , abspos );

    /* Now we should (num_scans/3)+1 number of scans in the outfifo */

    EXPECT_EQ( abspos, cc->converted_count ) << "Should have read in the remaining for that one scan";

    /* EXPECT_EQ( 42752, outfifo->write_pos ); */
    EXPECT_EQ( AIOFifoReadSize(outfifo) , ((num_scans / 3)+1)*num_channels*sizeof(double) );
}


TEST(Composite,FifoWriting )
{

    AIOGainRange *ranges = (AIOGainRange *)malloc(16*sizeof(AIOGainRange));
    int num_channels     = 16;
    int num_oversamples  = 20;
    int num_scans        = 1000;
    int retval = 0;
    int total_size       = num_channels * (num_oversamples+1) * num_scans;

    unsigned short *from_buf = (unsigned short *)malloc(total_size*sizeof(unsigned short));
    double *to_buf   = ( double *)malloc(total_size*sizeof(double));

    for ( int i = 0; i < num_channels; i ++ ) {
        ranges[i].max = 10.0;
        ranges[i].min = 0.0;
    }
    for ( int i = 0; i < total_size; i++ )
        from_buf[i] = (((unsigned short)-1)+1) / 2;

    AIOCountsConverter *cc = NewAIOCountsConverterWithBuffer( from_buf, num_channels, ranges, num_oversamples , sizeof(unsigned short)  );

    AIOFifoCounts *infifo = NewAIOFifoCounts( (unsigned)num_channels*(num_oversamples+1)*num_scans );
    AIOFifoVolts *outfifo = NewAIOFifoVolts( num_channels*(num_oversamples+1)*num_scans );

    /**
     * @brief Load the fifo with values
     */

    retval = infifo->PushN( infifo, from_buf, total_size );
    EXPECT_GE( retval, total_size*sizeof(unsigned short) );

    retval = cc->ConvertFifo( cc, outfifo, infifo, total_size );

    EXPECT_GE( retval, 0 );

    outfifo->PopN( outfifo, to_buf, num_channels );

    for ( int i = 0 ; i < num_channels ; i ++ ) {
        EXPECT_EQ( to_buf[i], (ranges[0].max + ranges[0].min) / 2 ) << "For i=" << i << std::endl;
    }
}

class AllGainCode : public ::testing::TestWithParam<ADGainCode> {};
TEST_P( AllGainCode, FromADCConfigBlock )
{
    ADConfigBlock cb = {0};
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    ADCConfigBlockInitializeDefault( &cb );
    ADGainCode gcode = GetParam();

    /* Try several gain codes */
    for ( int i = 0; i < 16 ; i ++ ) {
        retval = ADCConfigBlockSetGainCode( &cb, i, gcode );
        ASSERT_EQ(  0 , retval ) << "Able to set the channel's gain code correctly for channel '" << i << "'" << std::endl;

        int tmp = ADCConfigBlockGetGainCode(&cb, i );
        ASSERT_EQ( gcode, ADCConfigBlockGetGainCode(&cb, i )) << "Able to get the channel's gain code correctly for channel '" << i << "'" << std::endl;
    }

    /* Verify that we can copy these settings to the gaincode */
    AIOGainRange *tmpvals = NewAIOGainRangeFromADCConfigBlock( &cb );
    ASSERT_TRUE( tmpvals );

    for ( int i = 0; i < 16 ; i ++ ) {
        ASSERT_EQ( (adRanges[ gcode ].minVolts), tmpvals[i].min ) << "Able to get the Min value";
        ASSERT_EQ( (adRanges[ gcode ].minVolts + adRanges[ gcode ].range), tmpvals[i].max ) << "Able to get the Max value";
    }

    DeleteAIOGainRange( tmpvals );
}

INSTANTIATE_TEST_CASE_P( TestRangeConversion, AllGainCode, ::testing::Values( AD_GAIN_CODE_0_10V,
                                                                              AD_GAIN_CODE_10V,  
                                                                              AD_GAIN_CODE_0_5V, 
                                                                              AD_GAIN_CODE_5V,   
                                                                              AD_GAIN_CODE_0_2V, 
                                                                              AD_GAIN_CODE_2V,   
                                                                              AD_GAIN_CODE_0_1V, 
                                                                              AD_GAIN_CODE_1V    
                                                                              ));


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
