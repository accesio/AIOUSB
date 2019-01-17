#ifndef _AIOCONFIGURATION_H
#define _AIOCONFIGURATION_H

#include "AIOTypes.h"
#include "AIOContinuousBuffer.h"
#include "ADCConfigBlock.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

typedef enum {
    NO_CONFIG = -1,
    ADCCONIGBLOCK_CONFIG = 0,
    AIOCONTINUOUSBUF_CONFIG = 1
} ConfigurationType;

typedef enum {
    AD_SCAN_GETSCAN = 0,
    AD_SCAN_GETSCANV,
    AD_SCAN_GETCHANNEL,
    AD_SCAN_GETCHANNELV,
    AD_SCAN_CONTINUOUS,
    AD_SCAN_BULKACQUIRE
} ADCScanType;

typedef enum {
    AD_NO_SET_CAL = -1,
    AD_SET_CAL_AUTO = 0,
    AD_SET_CAL_NORMAL,
    AD_SET_CAL_MANUAL
} ADCSetCalFunction;


typedef struct configuration {
    ConfigurationType type;
    int timeout;
    int discard_first_sample;
    int device_index;
    int number_scans;
    ADCSetCalFunction calibration;
    ADCScanType scan_type;
    char *calibration_file;
    int debug;
    char *output_file;
    FILE *file_handle;
    char *file_name;
    AIORET_TYPE (*configure)( struct configuration *);
    AIORET_TYPE (*run)( struct configuration *);
} AIOConfiguration;

typedef AIOConfiguration ADCConfiguration;
typedef AIOConfiguration AIOContinousBufConfiguration;


typedef struct {
    AIOUSB_BOOL threaded;
    AIOUSB_BOOL debug;
    int *size;
    int actual_size;
    AIOConfiguration config;
} AIOArgument;

typedef struct {
    AIOArgument *device_args;
    int number_arguments;
} AIOArguments ;

AIORET_TYPE AIOConfigurationSetDebug( AIOConfiguration *config, AIOUSB_BOOL debug );
AIORET_TYPE AIOConfigurationSetTimeout( AIOConfiguration *config, unsigned timeout );
AIOConfiguration *NewAIOConfiguration();
AIORET_TYPE AIOConfigurationInitialize( AIOConfiguration *config );
AIORET_TYPE AIOArgumentInitialize( AIOArgument *arg );


#ifdef __aiousb_cplusplus
}
#endif

#endif

