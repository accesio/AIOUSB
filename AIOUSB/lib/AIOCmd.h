#ifndef _AIOCMD_H
#define _AIOCMD_H

#include "AIOTypes.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __aiousb_cplusplus
namespace AIOUSB {
#endif

typedef struct AIOCmd {
    int stop_scan;
    int stop_scan_arg;
    int channel;
    unsigned long num_scans;
    unsigned num_channels;
    unsigned num_samples;
} AIOCmd;

/* BEGIN AIOUSB_API */
PUBLIC_EXTERN AIOCmd *NewAIOCmdFromJSON( const char *str );
PUBLIC_EXTERN AIOCmd *NewAIOCmd();
PUBLIC_EXTERN AIORET_TYPE DeleteAIOCmd( AIOCmd *cmd );
/* END AIOUSB_API */

#ifdef __aiousb_cplusplus
}
#endif

#endif
