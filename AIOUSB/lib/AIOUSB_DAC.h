#ifndef _AIOUSB_DAC_H
#define _AIOUSB_DAC_H

#include "AIOTypes.h"


#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

/* BEGIN AIOUSB_API */
PUBLIC_EXTERN unsigned long DACDirect(unsigned long DeviceIndex,unsigned short Channel,unsigned short Value );
PUBLIC_EXTERN unsigned long DACMultiDirect(unsigned long DeviceIndex,unsigned short *pDACData,unsigned long DACDataCount );
PUBLIC_EXTERN unsigned long DACSetBoardRange(unsigned long DeviceIndex,unsigned long RangeCode );
PUBLIC_EXTERN unsigned long DACOutputOpen(unsigned long DeviceIndex,double *pClockHz );
PUBLIC_EXTERN unsigned long DACOutputClose(unsigned long DeviceIndex,unsigned long bWait );
PUBLIC_EXTERN unsigned long DACOutputCloseNoEnd(unsigned long DeviceIndex,unsigned long bWait );
PUBLIC_EXTERN unsigned long DACOutputSetCount(unsigned long DeviceIndex,unsigned long NewCount );
PUBLIC_EXTERN unsigned long DACOutputFrame(unsigned long DeviceIndex,unsigned long FramePoints,unsigned short *FrameData );
PUBLIC_EXTERN unsigned long DACOutputFrameRaw(unsigned long DeviceIndex,unsigned long FramePoints,unsigned short *FrameData );
PUBLIC_EXTERN unsigned long DACOutputStart(unsigned long DeviceIndex );
PUBLIC_EXTERN unsigned long DACOutputSetInterlock(unsigned long DeviceIndex,unsigned long bInterlock );
PUBLIC_EXTERN unsigned long CSA_DACOutputProcess( unsigned long DeviceIndex,double *ClockHz,	unsigned long Samples, unsigned short * sampleData );
/* END AIOUSB_API */


#ifdef __aiousb_cplusplus
}
#endif

#endif
