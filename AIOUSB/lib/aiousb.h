/**
 * @file   aiousb.h
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  General header files for the AIOUSB library
 *
 */

#ifndef AIOUSB_H
#define AIOUSB_H

#include <stdlib.h>
#include <assert.h>
#include "AIOTypes.h"
#include "AIODeviceInfo.h"
#include "AIODeviceQuery.h"
#include "AIODeviceTable.h"
#include "AIOUSBDevice.h"
#include "ADCConfigBlock.h"
#include "AIOUSB_Properties.h"
#include "AIOUSB_DIO.h"
#include "AIOUSB_ADC.h"
#include "AIOUSB_CTR.h"
#include "AIOUSB_DAC.h"
#include "AIOUSB_CustomEEPROM.h"
#include "USBDevice.h"
#include "AIOUSB_Log.h"
#include "AIOCommandLine.h"

#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

#ifndef PUBLIC_EXTERN
#define PUBLIC_EXTERN extern
#endif

#ifdef __aiousb_cplusplus
}
#endif


#endif


/* end of file */
