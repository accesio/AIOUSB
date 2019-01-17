/**
 * @file   AIOUSB_Core.h
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  
 */

#ifndef AIOUSB_CORE_H
#define AIOUSB_CORE_H

#define PUBLIC_EXTERN extern
#define PRIVATE

#include "AIOUSBDevice.h"
#include "libusb.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libusb.h>


#ifdef __aiousb_cplusplus
namespace AIOUSB {
#endif

PUBLIC_EXTERN int aio_errno;

/* parameters passed from ADC_BulkAcquire() to its worker thread */
struct BulkAcquireWorkerParams {
    unsigned long DeviceIndex;
    unsigned long BufSize;
    void *pBuf;
};


typedef struct aiousboption {

} AIOOption;

typedef struct aioerror {
    AIORET_TYPE retval;
    char *error_message;
} AIOError;

#define PROD_NAME_SIZE 40

typedef struct  {
     unsigned int id;
     char name[ PROD_NAME_SIZE + 2 ];
} ProductIDName;


struct ADRange {
  double minVolts;
  double range;
};

extern struct ADRange adRanges[ AD_NUM_GAIN_CODES ];

extern unsigned long AIOUSB_INIT_PATTERN;
extern unsigned long aiousbInit ;

/* BEGIN AIOUSB_API */
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

/* END AIOUSB_API */

#if 0
/*
 * these will be moved to aiousb.h when they are ready to be made public
 */
extern unsigned long DACOutputProcess( unsigned long DeviceIndex, double *ClockHz, unsigned long NumSamples, unsigned short *pSampleData );
#endif



#ifdef __aiousb_cplusplus
}
#endif


#endif


/* end of file */
