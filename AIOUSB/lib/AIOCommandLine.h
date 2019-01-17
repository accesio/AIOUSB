#ifndef _AIO_COMMAND_LINE_H
#define _AIO_COMMAND_LINE_H

#include "AIOTypes.h"
#include "ADCConfigBlock.h"
#include "AIOContinuousBuffer.h"
#include "AIOConfiguration.h"
#include "AIOUSB_Core.h"
#include "AIODeviceTable.h"
#include "AIOUSB_Properties.h"
#include "AIOUSB_Log.h"
#include <getopt.h>
#include <ctype.h>
#include <stdlib.h>


#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

#define DUMP   0x1000
#define CNTS   0x1001
#define JCONF  0x1002
#define REPEAT 0x1003

typedef struct AIOChannelRangeTmp {
    int start_channel;
    int end_channel;
    int gaincode;
} AIOChannelRangeTmp;


typedef struct AIOCommandLineOptions {
    int pass_through;
    int64_t num_scans;
    int64_t default_num_scans;
    int num_channels;
    int default_num_channels;
    int num_oversamples;
    int default_num_oversamples;
    int gain_code;
    int clock_rate;
    int default_clock_rate;
    char *outfile;
    int reset;
    int debug_level;
    int number_ranges;
    int verbose;
    int start_channel;
    int default_start_channel;
    int end_channel;
    int default_end_channel;
    int index;
    int block_size;
    int with_timing;
    int slow_acquire;
    int buffer_size;
    int rate_limit;
    int physical;
    int counts;
    int calibration;            
    int repeat;
    char *aiobuf_json;
    char *default_aiobuf_json;
    char *adcconfig_json;
    AIOChannelRangeTmp **ranges;
 } AIOCommandLineOptions;


typedef enum {
    INDEX_NUM = 0,
    ADCCONFIG_OPT,
    TIMEOUT_OPT,
    DEBUG_OPT,
    SETCAL_OPT,
    COUNT_OPT,
    SAMPLE_OPT,
    FILE_OPT,
    CHANNEL_OPT
} DeviceEnum;

/* BEGIN AIOUSB_API */
PUBLIC_EXTERN AIOCommandLineOptions *NewDefaultAIOCommandLineOptions();
PUBLIC_EXTERN AIOCommandLineOptions *NewAIOCommandLineOptionsFromDefaultOptions(const AIOCommandLineOptions *orig );
PUBLIC_EXTERN AIOCommandLineOptions *AIO_CMDLINE_DEFAULT_OPTIONS();
PUBLIC_EXTERN AIOCommandLineOptions *AIO_CMDLINE_SCRIPTING_OPTIONS();
PUBLIC_EXTERN AIORET_TYPE AIO_CMDLINE_CLEAR_OPTIONS(AIOCommandLineOptions *opts);


PUBLIC_EXTERN AIORET_TYPE AIOProcessCmdline( AIOCommandLineOptions *options, int argc, char **argv );
PUBLIC_EXTERN AIORET_TYPE AIOProcessCommandLine( AIOCommandLineOptions *options, int *argc, char **argv );
PUBLIC_EXTERN AIOChannelRangeTmp *AIOGetChannelRange(char *optarg );
PUBLIC_EXTERN void AIOPrintUsage(int argc, char **argv,  struct option  *options);
PUBLIC_EXTERN AIORET_TYPE DeleteAIOCommandLineOptions( AIOCommandLineOptions *options );


PUBLIC_EXTERN AIORET_TYPE AIOCommandLineOptionsListDevices( AIOCommandLineOptions *options , intlist *indices );
PUBLIC_EXTERN AIORET_TYPE AIOCommandLineOptionsOverrideADCConfigBlock( ADCConfigBlock *config, AIOCommandLineOptions *options );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufOverrideAIOCommandLine( AIOContinuousBuf *buf, AIOCommandLineOptions *options );

 

#if !defined(MATLAB)
static inline AIORET_TYPE AIOCommandLineOptionsGetDeviceIndex( AIOCommandLineOptions *options ) {AIO_ASSERT( options ); return options->index;};
static inline const char *AIOCommandLineOptionsGetDefaultADCJSONConfig( AIOCommandLineOptions *options ) {AIO_ASSERT_RET( NULL, options );return options->adcconfig_json;};
static inline AIORET_TYPE AIOCommandLineOptionsGetIncludeTiming( AIOCommandLineOptions *options ){ AIO_ASSERT(options); return options->with_timing;};
static inline AIORET_TYPE AIOCommandLineOptionsGetCounts( AIOCommandLineOptions *options ) { AIO_ASSERT(options); return options->counts; };
static inline AIORET_TYPE AIOCommandLineOptionsGetScans( AIOCommandLineOptions *options ) { AIO_ASSERT(options); return options->num_scans; };
#endif

PUBLIC_EXTERN const AIOCommandLineOptions *AIO_SCRIPTING_OPTIONS(void);
PUBLIC_EXTERN const AIOCommandLineOptions *AIO_CMDLINE_OPTIONS(void);


/* DEPRECATED FUNCTIONS */
#ifndef SWIG
#if !defined(MATLAB)
PUBLIC_EXTERN AIORET_TYPE AIOCommandLineListDevices( AIOCommandLineOptions *options , int *indices, int num_devices ) /* ACCES_DEPRECATED("Please use AIOCommandLineOptionsListDevices") */;
PUBLIC_EXTERN AIORET_TYPE AIOCommandLineOverrideADCConfigBlock( ADCConfigBlock *config, AIOCommandLineOptions *options ) ACCES_DEPRECATED("Please use AIOCommandLineOptionsOverrideADCConfigBlock");
static inline ACCES_DEPRECATED("Please use AIOCommandLineOptionsGetDefaultADCJSONConfig") const char *AIOCommandLineGetDefaultADCJSONConfig( AIOCommandLineOptions *options )  { return AIOCommandLineOptionsGetDefaultADCJSONConfig(options);};
static inline ACCES_DEPRECATED("Please use AIOCommandLineOptionsGetIncludeTiming") AIORET_TYPE AIOCommandLineGetIncludeTiming( AIOCommandLineOptions *options ){ return AIOCommandLineOptionsGetIncludeTiming(options);};
static inline ACCES_DEPRECATED("Please use AIOCommandLineOptionsGetCounts") AIORET_TYPE AIOCommandLineGetCounts( AIOCommandLineOptions *options ) {return AIOCommandLineOptionsGetCounts(options );};
static inline ACCES_DEPRECATED("Please use AIOCommandLineOptionsGetScans") AIORET_TYPE AIOCommandLineGetScans( AIOCommandLineOptions *options ) { return AIOCommandLineOptionsGetScans(options);};
#endif
#endif
/* END AIOUSB_API */


extern AIOCommandLineOptions AIO_DEFAULT_CMDLINE_OPTIONS;
extern AIOCommandLineOptions AIO_DEFAULT_SCRIPTING_OPTIONS;

#ifdef __aiousb_cplusplus
}
#endif



#endif



