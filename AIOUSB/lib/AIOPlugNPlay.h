#ifndef  _AIOPLUGNPLAY_H
#define  _AIOPLUGNPLAY_H

#include "AIOTypes.h"
#include "ADCConfigBlock.h"
#include "USBDevice.h"
#include "cJSON.h"


#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

typedef struct AIOPlugNPlay AIOPlugNPlay;

struct __attribute__((__packed__)) AIOPlugNPlay {
    int8_t  PNPSize;
    int16_t Rev;
    int8_t nADCConfigBytes;
    int8_t nDIOControlBytes;
    int8_t nDIOBytes;
    int8_t nCounterBlocks;
    int8_t nDACWords;
    int8_t HasDeb;
    int8_t HasLatch;
    int8_t PWMTableMaxLen;
    int8_t MapTableMaxLen;
    int8_t MapHighSrcBit;
    int8_t MapHighDstBit;
    int8_t HasDIOWrite1;
};

AIOUSB_BOOL DeviceHasPNPByte(const AIOPlugNPlay *pnpentry );
AIORET_TYPE CheckPNPData( unsigned long DeviceIndex );


#ifdef __aiousb_cplusplus
}
#endif


#endif

