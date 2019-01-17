#ifndef _AIO_DEVICE_TABLE_H
#define _AIO_DEVICE_TABLE_H

#include "AIOTypes.h"
#include "AIOUSBDevice.h"
#include "AIOUSB_Core.h"
#include <string.h>

#include "libusb.h"
#include <stdlib.h>
#include <errno.h>

#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

PUBLIC_EXTERN AIOUSBDevice deviceTable[ MAX_USB_DEVICES ];
/* BEGIN AIOUSB_API */
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

PUBLIC_EXTERN AIORET_TYPE AIOUSB_CheckFirmware20( unsigned long DeviceIndex );

PUBLIC_EXTERN void CloseAllDevices(void);
PUBLIC_EXTERN AIORESULT AIOUSB_GetAllDevices();

PUBLIC_EXTERN unsigned long AIOUSB_INIT_PATTERN;

PUBLIC_EXTERN AIORET_TYPE AIOUSBGetError();

/* END AIOUSB_API */

#ifdef __aiousb_cplusplus
}
#endif

#endif
