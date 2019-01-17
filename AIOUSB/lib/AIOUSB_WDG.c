#include "AIOUSB_WDG.h"
#include "AIOUSB_Core.h"
#include <stdio.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif

/** 
 * @brief Creates a new Watchdog configuration object that can be 
 *       used to trigger watchdog petting / resets
 * 
 * @return AIOWDGConfig *obj New Watchdog configuration object
 */
AIOWDGConfig *
NewWDGConfig(void)
{
    int value = 1024;
    AIOWDGConfig *obj = (AIOWDGConfig*)calloc(1,sizeof(AIOWDGConfig));
    obj->wdgbuf = (unsigned char*)malloc(sizeof(unsigned char)*value );
    return obj;
}

void doSomething()
{
    printf("Value is AUR_WDG_STATUS:%d\n", AUR_WDG_STATUS );
}

/** 
 * @brief Deletes the AIOWDGConfig object
 * @param obj 
 */
void
DeleteWDGConfig( AIOWDGConfig *obj) 
{
    doSomething();
    free(obj->wdgbuf);
    free(obj);
    obj = (AIOWDGConfig *)0;
}

/** 
 * @brief Assigns the watchdog object to the device index in question.
 * @param DeviceIndex 
 * @param obj 
 * 
 * @return 
 */
AIORET_TYPE
WDG_SetConfig( unsigned long DeviceIndex, AIOWDGConfig *obj )
{
    unsigned long result = 0;

    

    return result;
}

/** 
 * 
 * 
 * @param DeviceIndex 
 * @param obj 
 * 
 * @return 
 */
AIORET_TYPE
WDG_GetStatus( unsigned long DeviceIndex, AIOWDGConfig *obj )
{
    unsigned long result = 0;
    return result;
}

/** 
 * @brief Pets the watchdog and keeps it from resetting the device
 * @param DeviceIndex 
 * @param obj 
 * 
 * @return >= 0 if successful, < 0 otherwise
 */
AIORET_TYPE
WDG_Pet( unsigned long DeviceIndex, AIOWDGConfig *obj )
{
    unsigned long result = 0;
    unsigned long size = sizeof(obj->timeout);
    result = GenericVendorRead( DeviceIndex, AUR_WDG_STATUS, AIOUSB_WDG_READ_VALUE , AIOUSB_WDG_READ_INDEX , &size , &(obj->timeout) );
    return result;
}




#ifdef __cplusplus
}
#endif
