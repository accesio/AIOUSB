#ifndef _AIOCHANNEL_MASK_H
#define _AIOCHANNEL_MASK_H

#include "AIOTypes.h"
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

#if defined(signals)
    #undef signals
#endif

typedef char aio_channel_obj;
typedef struct {
    int *signal_indices;
    int signal_index;
    unsigned active_signals;
    aio_channel_obj *signals;
    unsigned number_signals;
    unsigned pos;
    int size;
    char *strrep;
    char *strrepsmall;
} AIOChannelMask;

/* BEGIN AIOUSB_API */

PUBLIC_EXTERN AIOChannelMask *NewAIOChannelMask( unsigned size );
PUBLIC_EXTERN void  DeleteAIOChannelMask( AIOChannelMask *mask );
PUBLIC_EXTERN AIOChannelMask *NewAIOChannelMaskFromStr( const char *bitfields );
PUBLIC_EXTERN AIOChannelMask *NewAIOChannelMaskFromChr( const char bits );


PUBLIC_EXTERN char *AIOChannelMaskToString( AIOChannelMask *mask );
PUBLIC_EXTERN char *AIOChannelMaskToStringAtIndex( AIOChannelMask *obj, unsigned index );
PUBLIC_EXTERN char *AIOChannelMaskGetMask( AIOChannelMask *mask );
PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskGetMaskAtIndex( AIOChannelMask *mask, char *val, unsigned index );
PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskNumberChannels( AIOChannelMask *mask );
PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskNumberSignals( AIOChannelMask *mask );
PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskGetSize( AIOChannelMask *mask );
PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskIndices( AIOChannelMask *mask , int *pos);
PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskNextIndex( AIOChannelMask *mask , int *pos );

PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskSetMaskFromInt( AIOChannelMask *mask, unsigned field );
PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskSetMaskAtIndex( AIOChannelMask *mask, char field, unsigned index  );
PUBLIC_EXTERN AIORET_TYPE AIOChannelMaskSetMaskFromStr( AIOChannelMask *mask, const char *bitfields );

/* END AIOUSB_API */

#define BIT_LENGTH(x) ( sizeof(x) * 8 )

#ifdef __aiousb_cplusplus
}
#endif

#endif
