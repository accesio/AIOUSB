#include "AIOPlugNPlay.h"
#include "AIODeviceTable.h"
#include <stdio.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif


#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member)*__mptr = (ptr);    \
             (type *)((char *)__mptr - offsetof(type, member)); })

/**
 * @param pnpentry 
 * 
 * @return 
 */
AIOUSB_BOOL DeviceHasPNPByte(const AIOPlugNPlay *pnpentry )
{
    return ( pnpentry->HasDIOWrite1 != 0 ? 1 : 0 );
}

AIORET_TYPE CheckPNPData( unsigned long DeviceIndex )
{
    AIOUSBDevice *deviceDesc = &deviceTable[DeviceIndex];
    USBDevice *usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIO_ERROR_VALID_DATA_RETVAL( AIOUSB_ERROR_INVALID_USBDEVICE , usb );

    deviceDesc->PNPData.PNPSize = 0;
    
    retval = AIOUSB_CheckFirmware20( DeviceIndex );
    AIO_ERROR_VALID_DATA_RETVAL( AIOUSB_ERROR_INVALID_DATA,  retval == AIOUSB_SUCCESS );

    deviceDesc->PNPData.PNPSize = sizeof(AIOPlugNPlay);
    retval = usb->usb_control_transfer( usb, 0xC0, 0x3F, 0 , 0, (unsigned char *)&deviceDesc->PNPData, deviceDesc->PNPData.PNPSize, deviceDesc->commTimeout  );


    if ( retval != deviceDesc->PNPData.PNPSize || (int8_t)deviceDesc->PNPData.PNPSize == (int8_t)0xFF ) {
        deviceDesc->PNPData.PNPSize = 0;
        retval = AIOUSB_ERROR_INVALID_DATA;
    } else 
        retval = AIOUSB_SUCCESS;

    return retval;
}

#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST

#include "AIOUSBDevice.h"
#include "gtest/gtest.h"


#endif
