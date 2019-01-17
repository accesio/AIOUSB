/**
 * @file   AIOCountsConverter.h
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  
 *
 */

#ifndef _AIO_COUNTS_CONVERTER_H
#define _AIO_COUNTS_CONVERTER_H

#include "AIOTypes.h"
#include "ADCConfigBlock.h"
#include "AIOContinuousBuffer.h"
#include "AIOFifo.h"
#include "ADCConfigBlock.h"


#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

/* BEGIN AIOUSB_API */
typedef struct {
    double min;
    double max;
} AIOGainRange;

typedef struct aio_counts_converter {
    unsigned num_oversamples;
    unsigned num_channels;
    unsigned num_scans;
    unsigned unit_size;
    unsigned scan_count;
    unsigned channel_count;
    unsigned os_count;
    unsigned converted_count;
    unsigned sum;
    void *buf;
    int (*continue_conversion)( struct aio_counts_converter*cc, unsigned rounded_num_counts );
    AIOGainRange *gain_ranges;
    AIORET_TYPE (*Convert)( struct aio_counts_converter *cc, void *tobuf, void *frombuf, unsigned num_bytes );
    AIORET_TYPE (*ConvertFifo)( struct aio_counts_converter *cc, void *tobuf, void *frombuf , unsigned num_bytes );
    AIOUSB_BOOL discardFirstSample;
} AIOCountsConverter;



PUBLIC_EXTERN AIOCountsConverter *NewAIOCountsConverterWithBuffer( void *buf, unsigned num_channels, AIOGainRange *ranges, unsigned num_oversamples,unsigned unit_size  );
PUBLIC_EXTERN AIOCountsConverter *NewAIOCountsConverter( unsigned num_channels, AIOGainRange *ranges, unsigned num_oversamples,unsigned unit_size  );
PUBLIC_EXTERN AIOCountsConverter *NewAIOCountsConverterFromAIOContinuousBuf( void *buf);

PUBLIC_EXTERN AIOCountsConverter *NewAIOCountsConverterWithScanLimiter( void *buf, unsigned num_scans, unsigned num_channels,  AIOGainRange *ranges, unsigned num_oversamples,unsigned unit_size );

PUBLIC_EXTERN void AIOCountsConverterReset( AIOCountsConverter *cc );
PUBLIC_EXTERN void DeleteAIOCountsConverter( AIOCountsConverter *ccv );
PUBLIC_EXTERN AIORET_TYPE AIOCountsConverterConvertNScans( AIOCountsConverter *cc, int num_scans );
PUBLIC_EXTERN AIORET_TYPE AIOCountsConverterConvertAllAvailableScans( AIOCountsConverter *cc );
PUBLIC_EXTERN AIORET_TYPE AIOCountsConverterConvert( AIOCountsConverter *cc, void *tobuf, void *frombuf, unsigned num_bytes );
PUBLIC_EXTERN AIORET_TYPE AIOCountsConverterConvertFifo( AIOCountsConverter *cc, void *tobuf, void *frombuf , unsigned num_bytes );

PUBLIC_EXTERN AIOGainRange* NewAIOGainRangeFromADCConfigBlock( ADCConfigBlock *adc );
PUBLIC_EXTERN void  DeleteAIOGainRange( AIOGainRange* );
/* END AIOUSB_API */
#ifdef __aiousb_cplusplus
}
#endif


#endif
