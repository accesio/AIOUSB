#ifndef _AIOUSB_CUSTOM_EEPROM_H
#define _AIOUSB_CUSTOM_EEPROM_H

#include "AIOTypes.h"

#ifdef __aiousb_cplusplus       /* Required for header file inclusion and SWIG */
namespace AIOUSB
{
#endif
/* BEGIN AIOUSB_API */
PUBLIC_EXTERN unsigned long CustomEEPROMWrite( unsigned long DeviceIndex, unsigned long StartAddress, unsigned long DataSize, void *Data ); 
PUBLIC_EXTERN unsigned long CustomEEPROMRead( unsigned long DeviceIndex, unsigned long StartAddress, unsigned long *DataSize, void *Data );
/* END AIOUSB_API */

#ifdef __aiousb_cplusplus       /* Required for header file inclusion and SWIG */
}
#endif

#endif
