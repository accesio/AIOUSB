/**
 * @file   AIOUSB_Properties.h
 * @author $Author$
 * @date   $Date$
 * @copy
 * @brief
 */
#ifndef _AIOUSB_PROPERTIES_H
#define _AIOUSB_PROPERTIES_H

#include "AIOUSB_Core.h"
#include "AIOTypes.h"
#include "AIOProductTypes.h"
#include "AIOList.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#ifdef __aiousb_cplusplus       /* Required for header file inclusion and SWIG */
namespace AIOUSB
{
#endif

typedef enum {
    BASIC = 0,
    TERSE = 1,
    JSON  = 2,
    YAML  = 3
} AIODisplayType;

/* BEGIN AIOUSB_API */
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

/* END AIOUSB_API */

#ifdef __aiousb_cplusplus       /* Required for header file inclusion and SWIG */
}
#endif

#endif
