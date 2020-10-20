#ifndef LIBAIOUSB_H
#define LIBAIOUSB_H


#include <stdint.h>
typedef unsigned long AIORESULT;
typedef int64_t AIORET_TYPE;

#define MATLAB

/* only needed because of mexFunction below and mexPrintf */
#include "AIOUSB_Core.h"
#include "AIOChannelMask.h"
#include "AIOContinuousBuffer.h"
#include "AIOTypes.h"
#include "DIOBuf.h"
#include "AIODeviceInfo.h"
#include "AIODeviceQuery.h"
#include "AIOCommandLine.h"
#include "AIOUSB_Properties.h"


/* #include <pthread.h> */

#include <mex.h>



/* #include "DIOBuf.h" */


PUBLIC_EXTERN DIOBuf *NewDIOBuf ( unsigned size );
PUBLIC_EXTERN DIOBuf *NewDIOBufFromChar( const char *ary , int size_array );
PUBLIC_EXTERN DIOBuf *NewDIOBufFromBinStr( const char *ary );
PUBLIC_EXTERN void DeleteDIOBuf ( DIOBuf  *buf );
PUBLIC_EXTERN DIOBuf *DIOBufReplaceString( DIOBuf *buf, char *ary, int size_array );
PUBLIC_EXTERN DIOBuf *DIOBufReplaceBinString( DIOBuf *buf, char *bitstr );
PUBLIC_EXTERN char *DIOBufToHex( DIOBuf *buf );
PUBLIC_EXTERN char *DIOBufToBinary( DIOBuf *buf );
PUBLIC_EXTERN char *DIOBufToInvertedBinary( DIOBuf *buf );
PUBLIC_EXTERN DIOBuf  *DIOBufResize( DIOBuf  *buf , unsigned size );
PUBLIC_EXTERN unsigned DIOBufSize( DIOBuf  *buf );
PUBLIC_EXTERN unsigned DIOBufByteSize( DIOBuf *buf );
PUBLIC_EXTERN char *DIOBufToString( DIOBuf  *buf );
PUBLIC_EXTERN AIORET_TYPE DIOBufSetIndex( DIOBuf *buf, int index, unsigned value );
PUBLIC_EXTERN AIORET_TYPE DIOBufGetIndex( DIOBuf *buf, int index );
PUBLIC_EXTERN AIORET_TYPE DIOBufGetByteAtIndex( DIOBuf *buf, unsigned index, char *value);
PUBLIC_EXTERN AIORET_TYPE DIOBufSetByteAtIndex( DIOBuf *buf, unsigned index, char  value );


/* #include "AIOUSB_DIO.h" */

PUBLIC_EXTERN AIORESULT DIO_ConfigureWithDIOBuf( unsigned long DeviceIndex, unsigned char bTristate, AIOChannelMask *mask, DIOBuf *buf );
PUBLIC_EXTERN unsigned long DIO_Configure( unsigned long DeviceIndex, unsigned char bTristate, void *pOutMask, void *pData );

PUBLIC_EXTERN unsigned long DIO_ConfigureEx( unsigned long DeviceIndex, void *pOutMask, void *pData, void *pTristateMask );
PUBLIC_EXTERN unsigned long DIO_ConfigurationQuery( unsigned long DeviceIndex, void *pOutMask, void *pTristateMask );
PUBLIC_EXTERN unsigned long DIO_WriteAll( unsigned long DeviceIndex, void *pData );
PUBLIC_EXTERN unsigned long DIO_Write8( unsigned long DeviceIndex, unsigned long ByteIndex, unsigned char Data );

PUBLIC_EXTERN unsigned long DIO_Write1( unsigned long DeviceIndex, unsigned long BitIndex, unsigned char bData );


PUBLIC_EXTERN AIORET_TYPE DIO_ReadAllToDIOBuf( unsigned long DeviceIndex, DIOBuf *buf );
#ifndef SWIG
PUBLIC_EXTERN AIORET_TYPE DIO_ReadIntoDIOBuf( unsigned long DeviceIndex, DIOBuf *buf ) ACCES_DEPRECATED("Please use DIO_ReadAllToDIOBuf");
#endif
PUBLIC_EXTERN AIORESULT DIO_ReadAll( unsigned long DeviceIndex, void *buf );


PUBLIC_EXTERN unsigned long DIO_ReadAllToCharStr( unsigned long DeviceIndex, char *buf, unsigned size );
PUBLIC_EXTERN unsigned long DIO_Read8( unsigned long DeviceIndex, unsigned long ByteIndex, unsigned char *pdat );
PUBLIC_EXTERN unsigned long DIO_Read1( unsigned long DeviceIndex, unsigned long BitIndex, unsigned char *bit );
PUBLIC_EXTERN unsigned long DIO_StreamOpen( unsigned long DeviceIndex, unsigned long bIsRead );
PUBLIC_EXTERN unsigned long DIO_StreamClose( unsigned long DeviceIndex );
PUBLIC_EXTERN unsigned long DIO_StreamSetClocks( unsigned long DeviceIndex, double *ReadClockHz, double *WriteClockHz );
PUBLIC_EXTERN unsigned long DIO_StreamFrame( unsigned long DeviceIndex, unsigned long FramePoints, unsigned short *pFrameData, unsigned long *BytesTransferred );

/* #include "AIOUSB_Core.h" */

PUBLIC_EXTERN unsigned long ADC_ResetDevice( unsigned long DeviceIndex  );
PUBLIC_EXTERN AIORET_TYPE AIOUSB_GetDeviceSerialNumber( unsigned long DeviceIndex );

PUBLIC_EXTERN AIORET_TYPE AIOUSB_ResetChip( unsigned long DeviceIndex );

#ifndef SWIG
PUBLIC_EXTERN AIOUSB_BOOL AIOUSB_Lock(void);
PUBLIC_EXTERN AIOUSB_BOOL AIOUSB_UnLock(void);

PUBLIC_EXTERN AIORESULT AIOUSB_InitTest(void);
PUBLIC_EXTERN AIORESULT AIOUSB_Validate( unsigned long *DeviceIndex );
PUBLIC_EXTERN AIORESULT AIOUSB_Validate_Lock(  unsigned long *DeviceIndex ) ;

PUBLIC_EXTERN DeviceDescriptor *DeviceTableAtIndex( unsigned long DeviceIndex );
PUBLIC_EXTERN DeviceDescriptor *DeviceTableAtIndex_Lock( unsigned long DeviceIndex );
#endif

DeviceDescriptor *AIOUSB_GetDevice( unsigned long DeviceIndex );
ADConfigBlock *AIOUSB_GetConfigBlock( DeviceDescriptor *dev);


PUBLIC_EXTERN AIORESULT AIOUSB_SetMiscClock(unsigned long DeviceIndex,double clockHz );
PUBLIC_EXTERN AIORESULT AIOUSB_GetMiscClock(unsigned long DeviceIndex );

PUBLIC_EXTERN unsigned long AIOUSB_SetCommTimeout(unsigned long DeviceIndex, unsigned timeout );
PUBLIC_EXTERN unsigned AIOUSB_GetCommTimeout( unsigned long DeviceIndex );

PUBLIC_EXTERN const char *AIOUSB_GetVersion(void); /* returns AIOUSB module version number as a string */
PUBLIC_EXTERN const char *AIOUSB_GetVersionDate(void); /* returns AIOUSB module version date as a string */
PUBLIC_EXTERN const char *AIOUSB_GetResultCodeAsString( unsigned long value ); /* gets string representation of AIOUSB_xxx result code */



PUBLIC_EXTERN unsigned short AIOUSB_VoltsToCounts(unsigned long DeviceIndex, unsigned channel, double volts );

PUBLIC_EXTERN unsigned long AIOUSB_ADC_LoadCalTable(unsigned long DeviceIndex, const char *fileName );

PUBLIC_EXTERN unsigned long AIOUSB_ADC_SetCalTable(unsigned long DeviceIndex, const unsigned short calTable[] );

PUBLIC_EXTERN unsigned long AIOUSB_ClearFIFO(unsigned long DeviceIndex, FIFO_Method Method );

PUBLIC_EXTERN long AIOUSB_GetStreamingBlockSize( unsigned long DeviceIndex );



AIORESULT AIOUSB_InitConfigBlock(ADConfigBlock *config, unsigned long DeviceIndex, AIOUSB_BOOL defaults);


PUBLIC_EXTERN AIORESULT GenericVendorRead( unsigned long deviceIndex, unsigned char Request, unsigned short Value, unsigned short Index, void *bufData , unsigned long *bytes_read );

PUBLIC_EXTERN AIORESULT GenericVendorWrite( unsigned long DeviceIndex, unsigned char Request, unsigned short Value, unsigned short Index, void *bufData, unsigned long *bytes_write );
PUBLIC_EXTERN AIORESULT AIOUSB_Validate_Device( unsigned long DeviceIndex );


/* #include "AIOContinuousBuffer.h" */

/*-----------------------------  Constructors  ------------------------------*/
PUBLIC_EXTERN AIOContinuousBuf *NewAIOContinuousBuf( unsigned long DeviceIndex, unsigned num_channels, unsigned num_oversamples, unsigned base_size );



PUBLIC_EXTERN AIOContinuousBuf *NewAIOContinuousBufForCounts( unsigned long DeviceIndex, unsigned scancounts, unsigned num_channels );
PUBLIC_EXTERN AIOContinuousBuf *NewAIOContinuousBufForVolts( unsigned long DeviceIndex, unsigned scancounts, unsigned num_channels, unsigned num_oversamples );

/*-----------------------------  Destructor   -------------------------------*/

PUBLIC_EXTERN AIORET_TYPE DeleteAIOContinuousBuf( AIOContinuousBuf *buf );

/*-----------------------------  Replacements  ------------------------------*/


PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufInitConfiguration(  AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufInitADCConfigBlock( AIOContinuousBuf *buf, unsigned size,ADGainCode gainCode, AIOUSB_BOOL diffMode, unsigned char os, AIOUSB_BOOL dfs );

PUBLIC_EXTERN AIOUSB_WorkFn AIOContinuousBufGetCallback( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetCallback(AIOContinuousBuf *buf , void *(*work)(void *object ) );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetStreamingBlockSize( AIOContinuousBuf *buf, unsigned sblksize);
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetStreamingBlockSize( AIOContinuousBuf *buf );

PUBLIC_EXTERN ADCConfigBlock *AIOContinuousBufGetADCConfigBlock( AIOContinuousBuf *buf );


PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetNumberChannels( AIOContinuousBuf * buf, unsigned num_channels );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetNumberChannels( AIOContinuousBuf * buf);



PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetOversample( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetOversample( AIOContinuousBuf *buf, unsigned num_oversamples );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufNumberChannels( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetBaseSize( AIOContinuousBuf *buf , size_t newbase );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetBaseSize( AIOContinuousBuf *buf  );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetBufferSize( AIOContinuousBuf *buf  );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetUnitSize( AIOContinuousBuf *buf , uint16_t new_unit_size);
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetUnitSize( AIOContinuousBuf *buf );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetTesting( AIOContinuousBuf *buf, AIOUSB_BOOL testing );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetTesting( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSendPreConfig( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetStartAndEndChannel( AIOContinuousBuf *buf, unsigned startChannel, unsigned endChannel );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetAllGainCodeAndDiffMode( AIOContinuousBuf *buf, ADGainCode gain, AIOUSB_BOOL diff );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetDeviceIndex( AIOContinuousBuf *buf );


PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetDiscardFirstSample(  AIOContinuousBuf *buf , AIOUSB_BOOL discard );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetChannelMask( AIOContinuousBuf *buf, AIOChannelMask *mask );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufNumberSignals( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetChannelRange( AIOContinuousBuf *buf, unsigned startChannel, unsigned endChannel , unsigned gainCode );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSaveConfig( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetDeviceIndex( AIOContinuousBuf *buf , unsigned long DeviceIndex );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufResetDevice(AIOContinuousBuf *buf );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetTimeout( AIOContinuousBuf *buf, unsigned timeout );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetTimeout( AIOContinuousBuf *buf );


PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetDebug( AIOContinuousBuf *buf, AIOUSB_BOOL debug );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetDebug( AIOContinuousBuf *buf );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetNumberScans( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetNumberScans( AIOContinuousBuf *buf , int64_t num_scans );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufNumberWriteSamplesRemaining( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufNumberSamplesAvailable( AIOContinuousBuf *buf );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetNumberSamplesPerScan( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetTotalSamplesExpected(  AIOContinuousBuf *buf );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufReset( AIOContinuousBuf *buf );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufPushN(AIOContinuousBuf *buf ,void  *frombuf, unsigned int N );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufPopN(AIOContinuousBuf *buf , void *tobuf, unsigned int N );


/*-----------------------------  Deprecated / Refactored   -------------------------------*/
#ifndef SWIG

/** @cond INTERNAL_DOCUMENTATION */
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetNumberOfChannels( AIOContinuousBuf * buf, unsigned num_channels ) ACCES_DEPRECATED("Please use AIOContinuousBufSetNumberChannels");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetNumberOfChannels( AIOContinuousBuf * buf) ACCES_DEPRECATED("Please use AIOContinuousBufGetNumberChannels");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_InitConfiguration(  AIOContinuousBuf *buf ) ACCES_DEPRECATED("Please use AIOContinuousBufInitConfiguration");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetCallback(AIOContinuousBuf *buf , void *(*work)(void *object ) ) ACCES_DEPRECATED("Please use AIOContinuousBufSetCallback");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetTesting( AIOContinuousBuf *buf, AIOUSB_BOOL testing ) ACCES_DEPRECATED("Please use AIOContinuousBufSetTesting");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SendPreConfig( AIOContinuousBuf *buf ) ACCES_DEPRECATED("Please use AIOContinuousBufSendPreConfig");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetAllGainCodeAndDiffMode( AIOContinuousBuf *buf, ADGainCode gain, AIOUSB_BOOL diff ) ACCES_DEPRECATED("Please use AIOContinuousBufSetAllGainCodeAndDiffMode");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetStartAndEndChannel( AIOContinuousBuf *buf, unsigned startChannel, unsigned endChannel ) ACCES_DEPRECATED("Please use AIOContinuousBufSetStartAndEndChannel");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_GetDeviceIndex( AIOContinuousBuf *buf ) ACCES_DEPRECATED("Please use AIOContinuousBufGetDeviceIndex");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetChannelRangeGain( AIOContinuousBuf *buf, unsigned startChannel, unsigned endChannel , unsigned gainCode ) ACCES_DEPRECATED("Please use AIOContinuousBufSetChannelRange");
 PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_GetOverSample( AIOContinuousBuf *buf ) ACCES_DEPRECATED("Please use AIOContinuousBufGetOversample");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetOverSample( AIOContinuousBuf *buf, unsigned os ) ACCES_DEPRECATED("Please use AIOContinuousBufSetOverSample");


PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetDiscardFirstSample(  AIOContinuousBuf *buf , AIOUSB_BOOL discard ) ACCES_DEPRECATED("Please use AIOContinuousBufSetDiscardFirstSample");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_NumberChannels( AIOContinuousBuf *buf ) ACCES_DEPRECATED("Please use AIOContinuousBufNumberChannels");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_NumberSignals( AIOContinuousBuf *buf ) ACCES_DEPRECATED("Please use AIOContinuousBufNumberSignals");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SaveConfig( AIOContinuousBuf *buf ) ACCES_DEPRECATED("Please use AIOContinuousBufSaveConfig");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetChannelMask( AIOContinuousBuf *buf, AIOChannelMask *mask ) ACCES_DEPRECATED("Please use AIOContinuousBufSetChannelMask");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_SetDeviceIndex( AIOContinuousBuf *buf , unsigned long DeviceIndex ) ACCES_DEPRECATED("Please use AIOContinuousBufSetDeviceIndex");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_NumberWriteScansInCounts(AIOContinuousBuf *buf ) ACCES_DEPRECATED("Please use AIOContinuousBufNumberWriteScansInCounts");
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBuf_ResetDevice( AIOContinuousBuf *buf)  ACCES_DEPRECATED("Please use AIOContinuousBufResetDevice");
/** @endcond */
#endif
/*---------------------------  Done Deprecated  -----------------------------*/

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufLock( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufUnlock( AIOContinuousBuf *buf );


PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufCallbackStart( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufCallbackStartCallbackWithAcquisitionFunction( AIOContinuousBuf *buf, AIOCmd *cmd, AIORET_TYPE (*callback)( AIOContinuousBuf *buf) );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufStopAcquisition( AIOContinuousBuf *buf );


PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufInitiateCallbackAcquisition( AIOContinuousBuf *buf );



PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetReadPosition( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetWritePosition( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetRemainingSize( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetStatus( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetExitCode( AIOContinuousBuf *buf );
PUBLIC_EXTERN THREAD_STATUS AIOContinuousBufGetRunStatus( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufPending( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetScansRead( AIOContinuousBuf *buf );


PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufReadIntegerScanCounts( AIOContinuousBuf *buf, unsigned short *tmp , unsigned tmpsize, unsigned size );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufReadCompleteScanCounts( AIOContinuousBuf *buf, unsigned short *read_buf, unsigned read_buf_size );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufReadIntegerNumberOfScans( AIOContinuousBuf *buf, unsigned short *read_buf, unsigned tmpbuffer_size, int64_t num_scans );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetCountsBuffer( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetVoltsBuffer( AIOContinuousBuf *buf );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufCountScansAvailable(AIOContinuousBuf *buf);

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSetClock( AIOContinuousBuf *buf, unsigned int hz );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufGetClock( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufEnd( AIOContinuousBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufSimpleSetupConfig( AIOContinuousBuf *buf, ADGainCode gainCode );

PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufRead( AIOContinuousBuf *buf, AIOBufferType *readbuf , unsigned readbufsize, unsigned size);
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufWrite( AIOContinuousBuf *buf, AIOBufferType *writebuf, unsigned wrbufsize, unsigned size, AIOContinuousBufMode flag );
PUBLIC_EXTERN AIORET_TYPE AIOContinuousBufWriteCounts( AIOContinuousBuf *buf, unsigned short *data, unsigned datasize, unsigned size , AIOContinuousBufMode flag );

AIORET_TYPE AIOContinuousBufCleanup( AIOContinuousBuf *buf );


char *AIOContinuousBufToJSON( AIOContinuousBuf *buf );

AIOContinuousBuf *NewAIOContinuousBufFromJSON( const char *json_string );


/* #include "AIOUSB_CTR.h" */

PUBLIC_EXTERN AIORET_TYPE CTR_CalculateCountersForClock( int hz , int *diva, int *divb );
PUBLIC_EXTERN AIORET_TYPE CTR_8254Mode( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned long Mode );
PUBLIC_EXTERN AIORET_TYPE CTR_8254Load( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned short LoadValue );
PUBLIC_EXTERN AIORET_TYPE CTR_8254ModeLoad( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned long Mode, unsigned short LoadValue );
PUBLIC_EXTERN AIORET_TYPE CTR_8254ReadModeLoad( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned long Mode, unsigned short LoadValue, unsigned short *pReadValue );
PUBLIC_EXTERN AIORET_TYPE CTR_8254Read( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned short *pReadValue );
PUBLIC_EXTERN AIORET_TYPE CTR_8254ReadAll( unsigned long DeviceIndex, unsigned short *pData );
PUBLIC_EXTERN AIORET_TYPE CTR_8254ReadStatus( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned short *pReadValue, unsigned char *pStatus );
#ifndef SWIG

PUBLIC_EXTERN AIORET_TYPE CTR_StartOutputFreq( unsigned long DeviceIndex, unsigned long BlockIndex, double *pHz );
#endif

PUBLIC_EXTERN AIORET_TYPE CTR_8254SelectGate( unsigned long DeviceIndex, unsigned long GateIndex );
PUBLIC_EXTERN AIORET_TYPE CTR_8254ReadLatched( unsigned long DeviceIndex, unsigned short *pData );

/* #include "ADCConfigBlock.h" */


PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockInit( ADCConfigBlock *, AIOUSBDevice *deviceDesc, unsigned int );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockInitForCounterScan(ADCConfigBlock *config, AIOUSBDevice *deviceDesc  );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockInitializeDefault( ADCConfigBlock *config );
PUBLIC_EXTERN void ADC_VerifyAndCorrectConfigBlock( ADCConfigBlock *configBlock , AIOUSBDevice *deviceDesc  );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetAllGainCodeAndDiffMode(ADCConfigBlock *config, unsigned gainCode, AIOUSB_BOOL differentialMode);

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetRegister( ADCConfigBlock *config, unsigned reg, unsigned char value );

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetGainCode( const  ADCConfigBlock *config, unsigned channel );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetGainCode(ADCConfigBlock *config, unsigned channel, unsigned char gainCode);

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetClockRate( ADCConfigBlock *config, int clock_rate);
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetClockRate( ADCConfigBlock *config );


PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetScanRange(ADCConfigBlock *config, unsigned startChannel, unsigned endChannel);
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetStartChannel( ADCConfigBlock *config, unsigned char startChannel  );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetEndChannel( ADCConfigBlock *config, unsigned char endChannel  );

/** @cond INTERNAL_DOCUMENTATION */
#define AIO_STRINGIFY(x) #x
#define HIGH_BITS(reg)   ( reg & 0xF0 )
#define LOW_BITS(reg)    ( reg & 0x0F )
/** @endcond INTERNAL_DOCUMENTATION */

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetChannelRange(ADCConfigBlock *config,unsigned startChannel, unsigned endChannel, unsigned gainCode );


PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetCalMode(ADCConfigBlock *config, ADCalMode calMode);
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetCalMode(const ADCConfigBlock *config);

PUBLIC_EXTERN char *ADCConfigBlockToYAML(ADCConfigBlock *config);


PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetStartChannel(const ADCConfigBlock *config);
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetEndChannel(const ADCConfigBlock *config);

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetOversample( const ADCConfigBlock *config );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetOversample( ADCConfigBlock *config, unsigned overSample );

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetTimeout( const ADCConfigBlock *config );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetTimeout( ADCConfigBlock *config, unsigned timeout );

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetTriggerMode(const ADCConfigBlock *config);
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetTriggerMode(ADCConfigBlock *config, unsigned triggerMode);

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetReference( ADCConfigBlock *config, int ref );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetTriggerEdge( ADCConfigBlock *config, AIOUSB_BOOL val );

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetDifferentialMode(ADCConfigBlock *config, unsigned channel, AIOUSB_BOOL differentialMode);
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetRangeSingle(ADCConfigBlock *config, unsigned long channel, unsigned char gainCode);

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockCopy( ADCConfigBlock *to, ADCConfigBlock *from );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetDevice( ADCConfigBlock *obj, AIOUSBDevice *dev );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetAIOUSBDevice( ADCConfigBlock *obj, AIOUSBDevice *dev );
PUBLIC_EXTERN AIOUSBDevice *ADCConfigBlockGetAIOUSBDevice( ADCConfigBlock *obj, AIORET_TYPE *res );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockInitializeFromAIOUSBDevice( ADCConfigBlock *config , AIOUSBDevice *dev);

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetTesting( ADCConfigBlock *obj, AIOUSB_BOOL testing );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetTesting( const ADCConfigBlock *obj );

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetSize( ADCConfigBlock *obj, unsigned size );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetSize( const ADCConfigBlock *obj );

PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockSetDebug( ADCConfigBlock *obj, AIOUSB_BOOL debug );
PUBLIC_EXTERN AIORET_TYPE ADCConfigBlockGetDebug( const ADCConfigBlock *obj );


/* JSON API */
PUBLIC_EXTERN char *ADCConfigBlockToJSON(ADCConfigBlock *config);
PUBLIC_EXTERN ADCConfigBlock *NewADCConfigBlockFromJSON( const char *str );
PUBLIC_EXTERN AIORET_TYPE DeleteADCConfigBlock( ADCConfigBlock *config );

PUBLIC_EXTERN AIOUSB_BOOL is_all_digits( char *str );


#ifndef SWIG
PUBLIC_EXTERN AIORET_TYPE _adccblock_valid_channel_settings(AIORET_TYPE in, ADCConfigBlock *config , int ADCMUXChannels );
PUBLIC_EXTERN AIORET_TYPE _adccblock_valid_size(AIORET_TYPE in, ADCConfigBlock *config );
PUBLIC_EXTERN AIORET_TYPE _adccblock_valid_cal_setting( AIORET_TYPE in, ADCConfigBlock *config );
PUBLIC_EXTERN AIORET_TYPE _adccblock_valid_end_channel( AIORET_TYPE in, ADCConfigBlock *config );
PUBLIC_EXTERN AIORET_TYPE _adccblock_valid_trigger_setting(ADCConfigBlock *config );
PUBLIC_EXTERN AIORET_TYPE _adcblock_valid_channel_settings(AIORET_TYPE in, ADCConfigBlock *config , int ADCMUXChannels );

#endif


/* #include "AIOChannelMask.h" */


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


/* #include "AIOChannelRange.h" */


struct ad_gain_pairs {
    ADGainCode gain;
    const char *name;
};

struct ad_gain_pairs AD_GAIN_CODE_STRINGS[] = {
    {AD_GAIN_CODE_0_10V, "0-10"},
    {AD_GAIN_CODE_10V, "+-10"},
    {AD_GAIN_CODE_0_5V,"0-5"},
    {AD_GAIN_CODE_5V,"+-5"},
    {AD_GAIN_CODE_0_2V,"0-2"},
    {AD_GAIN_CODE_2V,"+-2"},
    {AD_GAIN_CODE_0_1V,"0-1"},
    {AD_GAIN_CODE_1V,"+-1"},
    {ADGainCode_end,0}
};

#define LENGTH_AD_GAIN_CODE_STRINGS ((int)( sizeof(AD_GAIN_CODE_STRINGS)/sizeof(struct ad_gain_pairs) - 1 ))


extern int aio_channel_range_error;

typedef struct aio_channel_range {
    int start;
    int end;
    ADGainCode gain;
} AIOChannelRange;



PUBLIC_EXTERN AIOChannelRange *NewAIOChannelRangeFromStr( const char *str );
PUBLIC_EXTERN void DeleteAIOChannelRange( AIOChannelRange *range );
PUBLIC_EXTERN char *AIOChannelRangeToStr( AIOChannelRange *range );
PUBLIC_EXTERN AIORET_TYPE AIOChannelRangeGetStart( AIOChannelRange *range );
PUBLIC_EXTERN AIORET_TYPE AIOChannelRangeGetEnd( AIOChannelRange *range );
PUBLIC_EXTERN AIORET_TYPE AIOChannelRangeGetGain( AIOChannelRange *range );


/* #include "AIOBuf.h" */


PUBLIC_EXTERN AIOBuf * NewAIOBuf( AIOBufType type , size_t size );
PUBLIC_EXTERN AIORET_TYPE DeleteAIOBuf( AIOBuf *type );
PUBLIC_EXTERN AIORET_TYPE AIOBufGetSize( AIOBuf *buf );
PUBLIC_EXTERN AIOBufType AIOBufGetType( AIOBuf *buf );
PUBLIC_EXTERN void *AIOBufGetRaw( AIOBuf *buf );
PUBLIC_EXTERN AIORET_TYPE AIOBufRead( AIOBuf *buf, void *tobuf, size_t size_tobuf );
PUBLIC_EXTERN AIORET_TYPE AIOBufWrite( AIOBuf *buf, void *frombuf, size_t size_frombuf );
PUBLIC_EXTERN AIOBufIterator AIOBufGetIterator( AIOBuf *buf );
PUBLIC_EXTERN AIOEither AIOBufIteratorGetValue( AIOBufIterator *biter );
PUBLIC_EXTERN AIOUSB_BOOL AIOBufIteratorIsValid( AIOBufIterator *biter );
PUBLIC_EXTERN void AIOBufIteratorNext( AIOBufIterator *biter );


/* #include "AIOCountsConverter.h" */

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

/* #include "AIODeviceInfo.h" */

PUBLIC_EXTERN AIODeviceInfo *NewAIODeviceInfo();
PUBLIC_EXTERN void DeleteAIODeviceInfo( AIODeviceInfo *di );
PUBLIC_EXTERN const char *AIODeviceInfoGetName( AIODeviceInfo *di );
PUBLIC_EXTERN AIODeviceInfo *AIODeviceInfoGet( unsigned long DeviceIndex );
PUBLIC_EXTERN AIORET_TYPE AIODeviceInfoGetCounters( AIODeviceInfo *di );
PUBLIC_EXTERN AIORET_TYPE AIODeviceInfoGetDIOBytes( AIODeviceInfo *di );
PUBLIC_EXTERN AIODeviceInfo *AIODeviceInfoGet( unsigned long DeviceIndex );

/* #include "AIODeviceQuery.h" */


PUBLIC_EXTERN AIODeviceQuery *NewAIODeviceQuery( unsigned long DeviceIndex );
PUBLIC_EXTERN AIORET_TYPE DeleteAIODeviceQuery( AIODeviceQuery *devq );
PUBLIC_EXTERN char *AIODeviceQueryToStr( AIODeviceQuery *devq );
PUBLIC_EXTERN char *AIODeviceQueryToRepr( AIODeviceQuery *devq );
PUBLIC_EXTERN AIORET_TYPE AIODeviceQueryGetProductID( AIODeviceQuery *devq );
PUBLIC_EXTERN AIORET_TYPE AIODeviceQueryNameSize( AIODeviceQuery *devq );
PUBLIC_EXTERN char * AIODeviceQueryGetName( AIODeviceQuery *devq );
PUBLIC_EXTERN AIORET_TYPE AIODeviceQueryGetNumDIOBytes( AIODeviceQuery *devq );
PUBLIC_EXTERN AIORET_TYPE AIODeviceQueryGetNumCounters( AIODeviceQuery *devq );
PUBLIC_EXTERN AIORET_TYPE AIODeviceQueryGetIndex( AIODeviceQuery *devq );



/* #include "AIODeviceTable.h" */

PUBLIC_EXTERN AIORESULT AIODeviceTableAddDeviceToDeviceTable( int *numAccesDevices, unsigned long productID ) ;
PUBLIC_EXTERN AIORESULT AIODeviceTableAddDeviceToDeviceTableWithUSBDevice( int *numAccesDevices, unsigned long productID , USBDevice *usb_dev );
PUBLIC_EXTERN AIORET_TYPE AIODeviceTablePopulateTable(void);
PUBLIC_EXTERN AIORET_TYPE AIODeviceTablePopulateTableTest(unsigned long *products, int length );
PUBLIC_EXTERN AIORESULT AIODeviceTableClearDevices( void );
PUBLIC_EXTERN AIORESULT ClearDevices( void );
PUBLIC_EXTERN AIOUSBDevice *AIODeviceTableGetDeviceAtIndex( unsigned long DeviceIndex , AIORESULT *res );
PUBLIC_EXTERN AIOUSBDevice *AIODeviceTableGetAIOUSBDeviceAtIndex( unsigned long DeviceIndex );
PUBLIC_EXTERN USBDevice *AIODeviceTableGetUSBDeviceAtIndex( unsigned long DeviceIndex, AIORESULT *res );
void _setup_device_parameters( AIOUSBDevice *device , unsigned long productID );

PUBLIC_EXTERN unsigned long QueryDeviceInfo( unsigned long DeviceIndex, unsigned long *pPID, unsigned long *pNameSize, char *pName, unsigned long *pDIOBytes, unsigned long *pCounters );
PUBLIC_EXTERN AIORET_TYPE GetDevices(void);
PUBLIC_EXTERN char *GetSafeDeviceName( unsigned long DeviceIndex );
PUBLIC_EXTERN char *ProductIDToName( unsigned int productID );
PUBLIC_EXTERN AIORET_TYPE ProductNameToID(const char *name);
PUBLIC_EXTERN AIORET_TYPE AIOUSB_Init(void);
PUBLIC_EXTERN AIORET_TYPE AIOUSB_EnsureOpen(unsigned long DeviceIndex);
PUBLIC_EXTERN AIOUSB_BOOL AIOUSB_IsInit();
PUBLIC_EXTERN AIORET_TYPE AIOUSB_Exit();
PUBLIC_EXTERN AIORET_TYPE AIOUSB_Reset( unsigned long DeviceIndex );
PUBLIC_EXTERN void AIODeviceTableInit(void);
PUBLIC_EXTERN AIORET_TYPE ClearAIODeviceTable( int numDevices );

PUBLIC_EXTERN void CloseAllDevices(void);
PUBLIC_EXTERN AIORESULT AIOUSB_GetAllDevices();

PUBLIC_EXTERN unsigned long AIOUSB_INIT_PATTERN;

PUBLIC_EXTERN AIORET_TYPE AIOUSBGetError();


/* #include "AIOFifo.h" */

PUBLIC_EXTERN AIOFifo *NewAIOFifo( unsigned int size , unsigned int refsize );
PUBLIC_EXTERN void DeleteAIOFifo( AIOFifo *fifo );
PUBLIC_EXTERN void AIOFifoReset( void *fifo );
PUBLIC_EXTERN AIORET_TYPE AIOFifoRead( AIOFifo *fifo, void *tobuf , unsigned maxsize );
PUBLIC_EXTERN AIORET_TYPE AIOFifoWrite( AIOFifo *fifo, void *frombuf , unsigned maxsize );
PUBLIC_EXTERN AIORET_TYPE AIOFifoWriteAllOrNone( AIOFifo *fifo, void *frombuf , unsigned maxsize );
PUBLIC_EXTERN AIORET_TYPE AIOFifoReadAllOrNone( AIOFifo *fifo, void *tobuf , unsigned maxsize );
PUBLIC_EXTERN AIORET_TYPE AIOFifoGetRefSize( void *fifo );
PUBLIC_EXTERN AIOFifoTYPE *NewAIOFifoTYPE( unsigned int size );
PUBLIC_EXTERN AIORET_TYPE Push( AIOFifoTYPE *fifo, TYPE a );
PUBLIC_EXTERN AIORET_TYPE PushN( AIOFifoTYPE *fifo, INPUT_TYPE *a, unsigned N );
PUBLIC_EXTERN AIORET_TYPE PopN( AIOFifoTYPE *fifo, INPUT_TYPE *a, unsigned N );
PUBLIC_EXTERN AIORET_TYPE AIOFifoWriteSizeRemaining( void *fifo );
PUBLIC_EXTERN AIORET_TYPE AIOFifoWriteSizeRemainingNumElements( void *fifo );
PUBLIC_EXTERN AIORET_TYPE AIOFifoReadSize( void *tmpfifo );
PUBLIC_EXTERN AIORET_TYPE AIOFifoReadSizeNumElements( void *tmpfifo );
PUBLIC_EXTERN AIORET_TYPE AIOFifoGetSize( void *fifo );
PUBLIC_EXTERN AIORET_TYPE AIOFifoGetSizeNumElements( void *tmpfifo );
PUBLIC_EXTERN AIORET_TYPE AIOFifoResize( AIOFifo *fifo, size_t newsize );
PUBLIC_EXTERN AIORET_TYPE AIOFifoReadPosition( void *nfifo );
PUBLIC_EXTERN AIORET_TYPE AIOFifoWritePosition( void *nfifo );

/* #include "AIOEither.h" */

PUBLIC_EXTERN AIORET_TYPE AIOEitherClear( AIOEither *retval );
PUBLIC_EXTERN AIORET_TYPE AIOEitherSetRight(AIOEither *retval, AIO_EITHER_TYPE val , void *tmp, ... );
PUBLIC_EXTERN AIORET_TYPE AIOEitherGetRight(AIOEither *retval, void *tmp, ... );
PUBLIC_EXTERN AIORET_TYPE AIOEitherSetLeft(AIOEither *retval, int val );
PUBLIC_EXTERN AIORET_TYPE AIOEitherGetLeft(AIOEither *retval );
PUBLIC_EXTERN AIOUSB_BOOL AIOEitherHasError( AIOEither *retval );
PUBLIC_EXTERN char *AIOEitherToString( AIOEither *retval, AIORET_TYPE *result );
PUBLIC_EXTERN int AIOEitherToInt( AIOEither retval);
PUBLIC_EXTERN short AIOEitherToShort( AIOEither *retval, AIORET_TYPE *result );
PUBLIC_EXTERN unsigned AIOEitherToUnsigned( AIOEither *retval, AIORET_TYPE *result );
PUBLIC_EXTERN double AIOEitherToDouble( AIOEither *retval, AIORET_TYPE *result );
PUBLIC_EXTERN AIO_NUMBER AIOEitherToAIONumber( AIOEither *retval, AIORET_TYPE *result );
PUBLIC_EXTERN AIORET_TYPE AIOEitherToAIORetType( AIOEither either );

/* #include "AIOUSB_ADC.h" */


PUBLIC_EXTERN AIORET_TYPE ADC_GetScanV( unsigned long DeviceIndex, double *voltages );
PUBLIC_EXTERN AIORESULT ADC_RangeAll( unsigned long DeviceIndex, unsigned char *pGainCodes, unsigned long bSingleEnded );
PUBLIC_EXTERN AIORET_TYPE ADC_GetChannelV( unsigned long DeviceIndex, unsigned long ChannelIndex, double *singlevoltage );


PUBLIC_EXTERN AIORET_TYPE ADC_GetScan( unsigned long DeviceIndex, unsigned short *pBuf );
PUBLIC_EXTERN AIORET_TYPE AIOUSB_GetScan( unsigned long DeviceIndex, unsigned short counts[] );
PUBLIC_EXTERN AIORESULT ADC_GetConfig( unsigned long DeviceIndex, unsigned char *pConfigBuf, unsigned long *ConfigBufSize );
PUBLIC_EXTERN AIORESULT ADC_SetConfig(unsigned long DeviceIndex, unsigned char *pConfigBuf, unsigned long *ConfigBufSize );
PUBLIC_EXTERN AIORESULT ADC_Range1( unsigned long DeviceIndex, unsigned long ADChannel, unsigned char GainCode, unsigned long bSingleEnded );
PUBLIC_EXTERN AIORESULT ADC_ADMode( unsigned long DeviceIndex, unsigned char TriggerMode, unsigned char CalMode );
PUBLIC_EXTERN AIORESULT ADC_SetScanLimits( unsigned long DeviceIndex, unsigned long StartChannel, unsigned long EndChannel );
PUBLIC_EXTERN AIORESULT ADC_GetMaxClockRate( unsigned long ProductID, unsigned int num_channels, unsigned int num_oversamples                       );
PUBLIC_EXTERN AIORESULT ADC_ClockRateForADCProduct( unsigned long ProductID );
PUBLIC_EXTERN AIORESULT ADC_SetCal( unsigned long DeviceIndex,   const char *CalFileName );
PUBLIC_EXTERN AIORESULT ADC_QueryCal( unsigned long DeviceIndex );

PUBLIC_EXTERN AIOUSB_BOOL ADC_CanCalibrate( unsigned long ProductID );
PUBLIC_EXTERN AIORESULT ADC_Initialize( unsigned long DeviceIndex, unsigned char *pConfigBuf, unsigned long *ConfigBufSize,     const char *CalFileName );
PUBLIC_EXTERN AIORESULT ADC_BulkAcquire( unsigned long DeviceIndex, unsigned long BufSize, void *pBuf );
PUBLIC_EXTERN AIORESULT ADC_BulkPoll( unsigned long DeviceIndex, unsigned long *BytesLeft     );

/* FastScan Functions */
PUBLIC_EXTERN AIORESULT ADC_InitFastITScanV( unsigned long DeviceIndex );
PUBLIC_EXTERN AIORESULT ADC_CreateFastITConfig( unsigned long DeviceIndex, int size );
PUBLIC_EXTERN AIORESULT ADC_ResetFastITScanV( unsigned long DeviceIndex );
PUBLIC_EXTERN AIORESULT ADC_SetFastITScanVChannels( unsigned long DeviceIndex, unsigned long NewChannels);
PUBLIC_EXTERN AIORESULT ADC_GetFastITScanV( unsigned long DeviceIndex, double *pData);
PUBLIC_EXTERN AIORESULT ADC_GetITScanV( unsigned long DeviceIndex, double *pBuf);

PUBLIC_EXTERN unsigned ADC_GetOversample( unsigned long DeviceIndex );
PUBLIC_EXTERN AIORESULT ADC_SetOversample( unsigned long DeviceIndex, unsigned char Oversample );

PUBLIC_EXTERN AIORESULT WriteConfigBlock(unsigned long DeviceIndex);
PUBLIC_EXTERN AIORESULT ReadConfigBlock(unsigned long DeviceIndex,AIOUSB_BOOL forceRead  );

PUBLIC_EXTERN AIORET_TYPE AIOUSB_SetAllGainCodeAndDiffMode( ADConfigBlock *config, unsigned gainCode, AIOUSB_BOOL differentialMode );
PUBLIC_EXTERN AIORET_TYPE AIOUSB_GetGainCode( const ADConfigBlock *config, unsigned channel );
PUBLIC_EXTERN AIORET_TYPE AIOUSB_SetGainCode( ADConfigBlock *config, unsigned channel, unsigned gainCode );
PUBLIC_EXTERN AIORET_TYPE AIOUSB_IsDifferentialMode( const ADConfigBlock *config, unsigned channel );

PUBLIC_EXTERN AIORESULT AIOUSB_ADC_ExternalCal( unsigned long DeviceIndex, const double points[], int numPoints, unsigned short returnCalTable[], const char *saveFileName );

/* DEPRECATED FUNCTIONS */
#ifndef SWIG
PUBLIC_EXTERN AIORESULT ADC_SetAllGainCodeAndDiffMode( unsigned long DeviceIndex, unsigned gain, AIOUSB_BOOL differentialMode ) ACCES_DEPRECATED("Please use ADCConfigBlockSetAllGainCodeAndDiffMode instead");
PUBLIC_EXTERN AIORET_TYPE AIOUSB_SetDifferentialMode( ADConfigBlock *config, unsigned channel, AIOUSB_BOOL differentialMode ) ACCES_DEPRECATED("Please use ADCConfigBlockSetDifferentialMode");
PUBLIC_EXTERN AIORET_TYPE AIOUSB_GetCalMode( const ADConfigBlock *config ) ACCES_DEPRECATED("Please use ADCConfigBlockGetCalMode");
PUBLIC_EXTERN AIORET_TYPE AIOUSB_SetCalMode( ADConfigBlock *config, unsigned calMode ) ACCES_DEPRECATED("Please use ADCConfigBlockSetCalMode");
PUBLIC_EXTERN AIORET_TYPE AIOUSB_SetOversample(ADConfigBlock *config, unsigned overSample) ACCES_DEPRECATED("Please use ADCConfigBlockSetOversample");
PUBLIC_EXTERN AIORET_TYPE AIOUSB_GetOversample(ADConfigBlock *config ) ACCES_DEPRECATED("Please use ADCConfigBlockGetOversample");
PUBLIC_EXTERN unsigned AIOUSB_GetTriggerMode( const ADConfigBlock *config ) ACCES_DEPRECATED("Please use ADCConfigBlockGetTriggerMode");
PUBLIC_EXTERN AIORET_TYPE AIOUSB_SetTriggerMode( ADConfigBlock *config, unsigned triggerMode ) ACCES_DEPRECATED("Please use ADCConfigBlockSetTriggerMode");
#endif

PUBLIC_EXTERN unsigned AIOUSB_GetStartChannel( const ADConfigBlock *config );
PUBLIC_EXTERN unsigned AIOUSB_GetEndChannel( const ADConfigBlock *config );
PUBLIC_EXTERN AIORET_TYPE AIOUSB_SetScanRange( ADConfigBlock *config, unsigned startChannel, unsigned endChannel );
PUBLIC_EXTERN unsigned long AIOUSB_SetStreamingBlockSize( unsigned long DeviceIndex, unsigned long BlockSize );
PUBLIC_EXTERN AIOUSB_BOOL AIOUSB_IsDiscardFirstSample( unsigned long DeviceIndex );
PUBLIC_EXTERN unsigned long AIOUSB_SetDiscardFirstSample(unsigned long DeviceIndex,AIOUSB_BOOL discard );

PUBLIC_EXTERN AIOBuf *CreateSmartBuffer( unsigned long DeviceIndex );
PUBLIC_EXTERN unsigned long AIOUSB_ADC_InternalCal( unsigned long DeviceIndex, AIOUSB_BOOL autoCal, unsigned short returnCalTable[], const char *saveFileName );
PUBLIC_EXTERN unsigned char *ADC_GetADConfigBlock_Registers( ADConfigBlock *config );
PUBLIC_EXTERN void AIOUSB_SetRegister( ADConfigBlock *cb, unsigned int Register, unsigned char value );
PUBLIC_EXTERN unsigned char AIOUSB_GetRegister( ADConfigBlock *cb, unsigned int Register );


/* #include "AIOUSB_DAC.h" */

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
PUBLIC_EXTERN unsigned long CSA_DACOutputProcess(long DeviceIndex, double *ClockHz, long Samples, short * sampleData );
								)
/* #include "AIOUSB_CustomEEPROM.h" */

PUBLIC_EXTERN unsigned long CustomEEPROMWrite( unsigned long DeviceIndex, unsigned long StartAddress, unsigned long DataSize, void *Data );
PUBLIC_EXTERN unsigned long CustomEEPROMRead( unsigned long DeviceIndex, unsigned long StartAddress, unsigned long *DataSize, void *Data );

/* #include "AIOUSBDevice.h" */

PUBLIC_EXTERN char *AIOUSBDeviceToJSON( AIOUSBDevice *device );
PUBLIC_EXTERN AIOUSBDevice *NewAIOUSBDeviceFromJSON( char *str );
PUBLIC_EXTERN AIOUSBDevice *NewAIOUSBDeviceFromJSON( char *str );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceInitializeWithProductID( AIOUSBDevice *device , ProductIDS productID );
PUBLIC_EXTERN USBDevice *AIOUSBDeviceGetUSBHandle( AIOUSBDevice *dev );
PUBLIC_EXTERN USBDevice *AIOUSBDeviceGetUSBHandleFromDeviceIndex( unsigned long DeviceIndex, AIOUSBDevice **dev, AIORESULT *res );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceSetUSBHandle( AIOUSBDevice *dev, USBDevice *usb );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceSetADCConfigBlock( AIOUSBDevice *dev, ADCConfigBlock *conf );
PUBLIC_EXTERN ADCConfigBlock * AIOUSBDeviceGetADCConfigBlock( AIOUSBDevice *dev );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceCopyADCConfigBlock( AIOUSBDevice *dev, ADCConfigBlock *newone );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceSetTesting( AIOUSBDevice *dev, AIOUSB_BOOL testing );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceSize();
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceGetTesting( AIOUSBDevice *dev  );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceGetStreamingBlockSize( AIOUSBDevice *deviceDesc );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceGetDiscardFirstSample( AIOUSBDevice *device );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceSetDiscardFirstSample( AIOUSBDevice *device , AIOUSB_BOOL discard );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceSetTimeout( AIOUSBDevice *device, unsigned timeout );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceGetTimeout( AIOUSBDevice *device );
PUBLIC_EXTERN AIORET_TYPE AIOUSBDeviceWriteADCConfig( AIOUSBDevice *device, ADCConfigBlock *config );

/* #include "AIOUSB_Properties.h" */

PUBLIC_EXTERN AIORESULT AIOUSB_GetDeviceByProductID(int minProductID,int maxProductID,int maxDevices, int *deviceList );
PUBLIC_EXTERN AIORESULT GetDeviceBySerialNumber(uint64_t pSerialNumber);
PUBLIC_EXTERN AIORESULT GetDeviceSerialNumber(unsigned long DeviceIndex, uint64_t *pSerialNumber );
PUBLIC_EXTERN AIORET_TYPE AIOUSB_GetDeviceSerialNumber( unsigned long DeviceIndex );

PUBLIC_EXTERN AIORESULT AIOUSB_GetDeviceProperties(unsigned long DeviceIndex, DeviceProperties *properties );
PUBLIC_EXTERN const char *AIOUSB_GetResultCodeAsString(unsigned long result_value);
PUBLIC_EXTERN AIORET_TYPE AIOUSB_ListDevices();
PUBLIC_EXTERN AIORET_TYPE AIOUSB_ShowDevices( AIODisplayType display_type );

#ifndef SWIG
PUBLIC_EXTERN AIORET_TYPE AIOUSB_FindDevices( int **where, int *length , AIOUSB_BOOL (*is_ok_device)( AIOUSBDevice *dev )  );
PUBLIC_EXTERN AIORET_TYPE AIOUSB_FindDevicesByGroup( int **where, int *length, AIOProductGroup *pg );
#endif
PUBLIC_EXTERN AIORET_TYPE AIOUSB_FindDeviceIndicesByGroup( intlist *indices, AIOProductGroup *pg );


/* #include "AIOUSB_WDG.h" */


typedef struct {
  int bufsize;
  unsigned long L;
  unsigned char *wdgbuf;
  unsigned long timeout;
} AIOWDGConfig ;


AIOWDGConfig *NewWDGConfig(void);
void DeleteWDGConfig( AIOWDGConfig *obj);
AIORET_TYPE WDG_SetConfig( unsigned long DeviceIndex, AIOWDGConfig *obj );
AIORET_TYPE WDG_GetStatus( unsigned long DeviceIndex, AIOWDGConfig *obj );
AIORET_TYPE WDG_Pet( unsigned long DeviceIndex, AIOWDGConfig *obj );

/* #include "USBDevice.h" */

PUBLIC_EXTERN USBDevice * NewUSBDevice(libusb_device *dev, libusb_device_handle *handle );
PUBLIC_EXTERN void DeleteUSBDevice( USBDevice *dev );
PUBLIC_EXTERN USBDevice *CopyUSBDevice( USBDevice *usb );
PUBLIC_EXTERN AIOEither InitializeUSBDevice( USBDevice *usb, LIBUSBArgs *args );
PUBLIC_EXTERN AIORET_TYPE AddAllACCESUSBDevices( libusb_device **deviceList , USBDevice **devs , int *size );
PUBLIC_EXTERN void DeleteUSBDevices( USBDevice *devs);
PUBLIC_EXTERN int USBDeviceClose( USBDevice *dev );

PUBLIC_EXTERN int USBDeviceGetIdProduct( USBDevice *device );
PUBLIC_EXTERN int USBDeviceFetchADCConfigBlock( USBDevice *device, ADCConfigBlock *config );
PUBLIC_EXTERN int USBDevicePutADCConfigBlock( USBDevice *usb, ADCConfigBlock *configBlock );

PUBLIC_EXTERN int usb_control_transfer(USBDevice *dev_handle,
                         uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                                 unsigned char *data, uint16_t wLength, unsigned int timeout);
PUBLIC_EXTERN int usb_bulk_transfer(USBDevice *dev_handle,
                              unsigned char endpoint, unsigned char *data, int length,
                              int *actual_length, unsigned int timeout);
PUBLIC_EXTERN int usb_request(USBDevice *dev_handle,
                        uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                        unsigned char *data, uint16_t wLength, unsigned int timeout);
PUBLIC_EXTERN int usb_reset_device( USBDevice *usb );


PUBLIC_EXTERN libusb_device_handle *get_usb_device( USBDevice *dev );
PUBLIC_EXTERN libusb_device_handle *USBDeviceGetUSBDeviceHandle( USBDevice *usb );

/* #include "AIOCommandLine.h" */

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
PUBLIC_EXTERN AIORET_TYPE AIOCommandLineListDevices( AIOCommandLineOptions *options , int *indices, int num_devices ) ACCES_DEPRECATED("Please use AIOCommandLineOptionsListDevices");
PUBLIC_EXTERN AIORET_TYPE AIOCommandLineOverrideADCConfigBlock( ADCConfigBlock *config, AIOCommandLineOptions *options ) ACCES_DEPRECATED("Please use AIOCommandLineOptionsOverrideADCConfigBlock");
static inline ACCES_DEPRECATED("Please use AIOCommandLineOptionsGetDefaultADCJSONConfig") const char *AIOCommandLineGetDefaultADCJSONConfig( AIOCommandLineOptions *options )  { return AIOCommandLineOptionsGetDefaultADCJSONConfig(options);};
static inline ACCES_DEPRECATED("Please use AIOCommandLineOptionsGetIncludeTiming") AIORET_TYPE AIOCommandLineGetIncludeTiming( AIOCommandLineOptions *options ){ return AIOCommandLineOptionsGetIncludeTiming(options);};
static inline ACCES_DEPRECATED("Please use AIOCommandLineOptionsGetCounts") AIORET_TYPE AIOCommandLineGetCounts( AIOCommandLineOptions *options ) {return AIOCommandLineOptionsGetCounts(options );};
static inline ACCES_DEPRECATED("Please use AIOCommandLineOptionsGetScans") AIORET_TYPE AIOCommandLineGetScans( AIOCommandLineOptions *options ) { return AIOCommandLineOptionsGetScans(options);};
#endif
#endif






#endif
