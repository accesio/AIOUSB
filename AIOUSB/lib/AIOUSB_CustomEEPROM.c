/**
 * @file   AIOUSB_CustomEEPROM.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  General header files for EEProm functionality
 *
 */

#include "AIOUSB_CustomEEPROM.h"
#include "AIOUSB_Core.h"
#include "AIODeviceTable.h"

#ifdef __cplusplus
namespace AIOUSB {
#endif

#define EXIT_FN_IF_NO_VALID_USB( d, r, f, u, g ) do {           \
        if ( !d ) {                                             \
            r = -AIOUSB_ERROR_DEVICE_NOT_FOUND;                 \
            goto g;                                             \
        } else if ( ( r = f ) != AIOUSB_SUCCESS  ) {            \
            goto g;                                             \
        } else if ( !(u = AIOUSBDeviceGetUSBHandle( d )))  {    \
            r = -AIOUSB_ERROR_INVALID_USBDEVICE;                \
            goto g;                                             \
        }                                                       \
    } while (0 )


static AIORET_TYPE _check_eeprom_data(AIORET_TYPE in,
                               unsigned long DeviceIndex,
                               unsigned long StartAddress,
                               unsigned long DataSize,
                               void *Data
                               ) 
{
    if ( in != AIOUSB_SUCCESS ) {
        return in;
    } else if( StartAddress > EEPROM_CUSTOM_MAX_ADDRESS ||
               (StartAddress + DataSize) > EEPROM_CUSTOM_MAX_ADDRESS + 1 ||
               Data == NULL
               ) {
        return -AIOUSB_ERROR_INVALID_PARAMETER;
    } else {
        return AIOUSB_SUCCESS;
    }
}


/*----------------------------------------------------------------------------*/
/**
* EEPROM layout:
*   program code: 0x0000 -> EEPROM_CUSTOM_BASE_ADDRESS - 1
*   user space  : EEPROM_CUSTOM_BASE_ADDRESS -> EEPROM_CUSTOM_BASE_ADDRESS + EEPROM_CUSTOM_MAX_ADDRESS - 1
*                 (user space is addressed as 0 -> EEPROM_CUSTOM_MAX_ADDRESS - 1)
*/
unsigned long CustomEEPROMWrite(
                                unsigned long DeviceIndex,
                                unsigned long StartAddress,
                                unsigned long DataSize,
                                void *Data
                                )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    USBDevice *usb;
    int bytesTransferred;

    EXIT_FN_IF_NO_VALID_USB( deviceDesc , retval, _check_eeprom_data((AIORET_TYPE)result,DeviceIndex,StartAddress,DataSize,Data ), usb, out_CustomEEPROMWrite );

    AIOUSB_UnLock();
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE, 
                                                 AUR_EEPROM_WRITE,
                                                 EEPROM_CUSTOM_BASE_ADDRESS + StartAddress, 
                                                 0,
                                                 ( unsigned char* )Data, 
                                                 DataSize, 
                                                 deviceDesc->commTimeout
                                                 );
    if(bytesTransferred != ( int )DataSize)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

 out_CustomEEPROMWrite:
    AIOUSB_UnLock();
    return result;
}


/*----------------------------------------------------------------------------*/
/**
 * @brief
 * EEPROM layout:
 *   program code: 0x0000 -> EEPROM_CUSTOM_BASE_ADDRESS - 1
 *   user space  : EEPROM_CUSTOM_BASE_ADDRESS -> EEPROM_CUSTOM_BASE_ADDRESS + EEPROM_CUSTOM_MAX_ADDRESS - 1
 *                 (user space is addressed as 0 -> EEPROM_CUSTOM_MAX_ADDRESS - 1)
 */
unsigned long CustomEEPROMRead(
                               unsigned long DeviceIndex,
                               unsigned long StartAddress,
                               unsigned long *DataSize,
                               void *Data
                               )
{

    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    int bytesTransferred;
    USBDevice *usb;

    EXIT_FN_IF_NO_VALID_USB( deviceDesc , retval, _check_eeprom_data((AIORET_TYPE)result,DeviceIndex,StartAddress,*DataSize,Data ) , usb, out_CustomEEPROMRead );

    AIOUSB_UnLock();
    bytesTransferred  = usb->usb_control_transfer(usb,
                                                  USB_READ_FROM_DEVICE, 
                                                  AUR_EEPROM_READ,
                                                  EEPROM_CUSTOM_BASE_ADDRESS + StartAddress, 
                                                  0,
                                                  ( unsigned char* )Data, 
                                                  *DataSize, 
                                                  deviceDesc->commTimeout
                                                  );
    if(bytesTransferred != ( int )*DataSize)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

 out_CustomEEPROMRead:
    AIOUSB_UnLock();
    return result;
}


#ifdef __cplusplus
}
#endif

