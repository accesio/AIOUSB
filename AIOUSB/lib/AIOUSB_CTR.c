/**
 * @file AIOUSB_CTR.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @copy
 * @brief Counter functionality
 */

#include "AIOUSB_CTR.h"
#include "AIODeviceTable.h"
#include "AIOUSB_Log.h"

#include <math.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif

#define RETURN_IF_INVALID_INPUT(d, r, f ) do { \
        if( !d )                                                        \
            return (AIORET_TYPE)-AIOUSB_ERROR_INVALID_INDEX;            \
        if( ( r = f ) != AIOUSB_SUCCESS ) {                             \
            AIOUSB_UnLock();                                            \
            return r;                                                   \
        }                                                               \
    } while (0)

#define JUMP_IF_INVALID_INPUT(d,r,f, g ) do { \
        if ( !d ) { \
            r = -AIOUSB_ERROR_DEVICE_NOT_FOUND;\
            goto g;\
        } else if ( (r = f) != AIOUSB_SUCCESS ) { \
            goto g;\
        } \
    } while (0)
    
#define JUMP_IF_NO_VALID_USB( d, r, f, u, g ) do {              \
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


/*----------------------------------------------------------------------------*/
AIORET_TYPE _check_block_index(AIOUSBDevice *deviceDesc, unsigned long BlockIndex , unsigned long CounterIndex )
{
    if (BlockIndex == 0) {
        /* contiguous counter addressing*/
        BlockIndex = CounterIndex / COUNTERS_PER_BLOCK;
        CounterIndex = CounterIndex % COUNTERS_PER_BLOCK;
        if (BlockIndex >= deviceDesc->Counters) {
            AIOUSB_UnLock();
            return (AIORET_TYPE)-AIOUSB_ERROR_INVALID_PARAMETER;
        }
    } else {
        if ( BlockIndex >= deviceDesc->Counters || CounterIndex >= COUNTERS_PER_BLOCK ) {
            AIOUSB_UnLock();
            return (AIORET_TYPE)-AIOUSB_ERROR_INVALID_PARAMETER;
        }
    }
    return (AIORET_TYPE)AIOUSB_SUCCESS;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE _check_valid_counters( AIOUSBDevice *deviceDesc ) {
     if ( deviceDesc->Counters == 0) {
         return (AIORET_TYPE)-AIOUSB_ERROR_NOT_SUPPORTED;
     }
     return AIOUSB_SUCCESS;
 }
/*----------------------------------------------------------------------------*/
AIORET_TYPE _check_valid_counter_device(AIOUSBDevice *deviceDesc, 
                                        unsigned long BlockIndex,
                                        unsigned long CounterIndex
                                        )
{
    AIORET_TYPE temp;
    if( (temp = _check_valid_counters( deviceDesc )) != AIOUSB_SUCCESS )
        return temp;
    return _check_block_index( deviceDesc, BlockIndex, CounterIndex );
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE _check_valid_counter_device_with_mode(AIOUSBDevice *deviceDesc, 
                                                  unsigned long BlockIndex,
                                                  unsigned long CounterIndex,
                                                  unsigned long Mode
                                                  )
{
    if (Mode >= COUNTER_NUM_MODES)
        return (AIORET_TYPE)-AIOUSB_ERROR_INVALID_PARAMETER;
    return _check_valid_counter_device( deviceDesc, BlockIndex, CounterIndex );
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE _check_valid_counter_device_for_read(AIOUSBDevice *deviceDesc, 
                                                 unsigned short *pData
                                                 )
{
    if ( !pData )
        return (AIORET_TYPE)-AIOUSB_ERROR_INVALID_PARAMETER;
    return (AIORET_TYPE)AIOUSB_SUCCESS;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE _check_valid_counter_device_for_gate(AIOUSBDevice *deviceDesc, unsigned long GateIndex  ) 
{
    AIORET_TYPE retval;
    if( (retval = _check_valid_counters( deviceDesc )) != AIOUSB_SUCCESS ) 
        return retval;
    if ( deviceDesc->bGateSelectable == AIOUSB_FALSE ) {
        return (AIORET_TYPE)-AIOUSB_ERROR_NOT_SUPPORTED;
    }
    if (GateIndex >= deviceDesc->Counters * COUNTERS_PER_BLOCK) {
        return (AIORET_TYPE)-AIOUSB_ERROR_INVALID_PARAMETER;
    }
    return AIOUSB_SUCCESS;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE _check_valid_counter_output_frequency( AIOUSBDevice *deviceDesc, unsigned long BlockIndex, double *pHz ) {
    AIORET_TYPE retval;
    if( (retval = _check_valid_counters( deviceDesc )) != AIOUSB_SUCCESS ) 
        return retval;
    if( BlockIndex >= deviceDesc->Counters || !pHz ) {
        return (AIORET_TYPE)-AIOUSB_ERROR_INVALID_PARAMETER;
    }
    return (AIORET_TYPE)AIOUSB_SUCCESS;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE _check_valid_input_for_modeload( AIOUSBDevice *deviceDesc, 
                                             unsigned long BlockIndex, 
                                             unsigned long CounterIndex, 
                                             unsigned long Mode, 
                                             unsigned long LoadValue, 
                                             unsigned short *pReadValue
                                             )
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    if ( !pReadValue ) {
         return AIOUSB_ERROR_INVALID_PARAMETER;
    }
    if ( (retval = _check_valid_counters( deviceDesc )) != AIOUSB_SUCCESS ) 
        return retval;
    if ( (retval = _check_block_index( deviceDesc, BlockIndex, CounterIndex ) ) != AIOUSB_SUCCESS )
        return retval;

    return retval;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254Mode(
                           unsigned long DeviceIndex,
                           unsigned long BlockIndex,
                           unsigned long CounterIndex,
                           unsigned long Mode
                           ) {
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    USBDevice *usb = NULL;
    unsigned short controlValue;
    int bytesTransferred;
    if ( result != AIOUSB_SUCCESS )
        goto out_CTR_8254Mode;
    if( !deviceDesc ) {
        result = -AIOUSB_ERROR_INVALID_INDEX;
        goto out_CTR_8254Mode;
    }

    if( (retval = _check_valid_counter_device( deviceDesc, BlockIndex, CounterIndex )) != AIOUSB_SUCCESS )
        goto out_CTR_8254Mode;

    usb = AIOUSBDeviceGetUSBHandle( deviceDesc );

    if (!usb ) {
        retval = -AIOUSB_ERROR_INVALID_USBDEVICE;
        goto out_CTR_8254Mode;
    }

    AIOUSB_UnLock();
    controlValue = (( unsigned short )CounterIndex << (6 + 8))  | (0x3u << (4 + 8))  | 
                   (( unsigned short )Mode << (1 + 8))          | ( unsigned short )BlockIndex;
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE,
                                                 AUR_CTR_MODE,
                                                 controlValue,
                                                 0,
                                                 0,
                                                 0,
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred != 0)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

 out_CTR_8254Mode:
    AIOUSB_UnLock();
    return result;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254Load(
                         unsigned long DeviceIndex,
                         unsigned long BlockIndex,
                         unsigned long CounterIndex,
                         unsigned short LoadValue
                         ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    USBDevice *usb;
    unsigned short controlValue;
    int bytesTransferred;

    if ( result != AIOUSB_SUCCESS ) {
        retval = -result;
        goto out_CTR_8254Load;
    }
        
    if( !deviceDesc ) {
         retval = -AIOUSB_ERROR_INVALID_INDEX;
         goto out_CTR_8254Load;
    }

    if( (retval = _check_valid_counter_device( deviceDesc, BlockIndex, CounterIndex )) != AIOUSB_SUCCESS )
        goto out_CTR_8254Load;

    usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    
    if (!usb ) {
        result = -AIOUSB_ERROR_INVALID_USBDEVICE;
        goto out_CTR_8254Load;
    }

    AIOUSB_UnLock();
    controlValue = (( unsigned short )CounterIndex << (6 + 8)) | ( unsigned short )BlockIndex;
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE, 
                                                 AUR_CTR_LOAD,
                                                 controlValue, 
                                                 LoadValue, 
                                                 0, 
                                                 0, 
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred != 0)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

 out_CTR_8254Load:
    AIOUSB_UnLock();
    return result;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254ModeLoad(
                             unsigned long DeviceIndex,
                             unsigned long BlockIndex,
                             unsigned long CounterIndex,
                             unsigned long Mode,
                             unsigned short LoadValue
                             )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    USBDevice *usb;
    unsigned short controlValue;
    int bytesTransferred;

    retval =  _check_valid_counter_device( deviceDesc, BlockIndex, CounterIndex );
    if ( retval != AIOUSB_SUCCESS ) 
        goto out_CTR_8254ModeLoad;

    usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    if ( !usb ) {
        retval = -AIOUSB_ERROR_INVALID_USBDEVICE;
        goto out_CTR_8254ModeLoad;
    }

    AIOUSB_UnLock();
    controlValue    = (( unsigned short )CounterIndex << (6 + 8))    | (0x3u << (4 + 8))  | 
                      (( unsigned short )Mode << (1 + 8))  | ( unsigned short )BlockIndex;
    bytesTransferred = usb->usb_control_transfer( usb,
                                                  USB_WRITE_TO_DEVICE, 
                                                  AUR_CTR_MODELOAD,
                                                  controlValue, 
                                                  LoadValue, 
                                                  0, 
                                                  0, 
                                                  deviceDesc->commTimeout
                                                  );
    if (bytesTransferred != 0)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    
 out_CTR_8254ModeLoad:
    AIOUSB_UnLock();
    return result;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254ReadModeLoad(
                                   unsigned long DeviceIndex,
                                   unsigned long BlockIndex,
                                   unsigned long CounterIndex,
                                   unsigned long Mode,
                                   unsigned short LoadValue,
                                   unsigned short *pReadValue
                                   )
{
    USBDevice *usb;
    unsigned short readValue;
    unsigned short controlValue;
    int bytesTransferred;
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    JUMP_IF_NO_VALID_USB( deviceDesc , retval, _check_valid_input_for_modeload( deviceDesc, BlockIndex, CounterIndex, Mode, LoadValue, pReadValue), usb, out_CTR_8254ReadModeLoad );

    AIOUSB_UnLock();

    controlValue = (( unsigned short )CounterIndex << (6 + 8)) |  (0x3u << (4 + 8)) | 
                   (( unsigned short )Mode << (1 + 8))         |  ( unsigned short )BlockIndex;
    
    bytesTransferred = usb->usb_control_transfer( usb, 
                                                  USB_WRITE_TO_DEVICE, 
                                                  AUR_CTR_MODELOAD,
                                                  controlValue, 
                                                  LoadValue, 
                                                  ( unsigned char* )&readValue, 
                                                  sizeof(readValue), 
                                                  deviceDesc->commTimeout
                                                  );
    if ( bytesTransferred == sizeof(readValue) ) {
        /* TODO: verify endian mode; original code had it reversed*/
        *pReadValue = readValue;
    } else
        result = -LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

 out_CTR_8254ReadModeLoad:
    AIOUSB_UnLock();
    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254Read(
                           unsigned long DeviceIndex,
                           unsigned long BlockIndex,
                           unsigned long CounterIndex,
                           unsigned short *pReadValue
                           )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    USBDevice *usb;
    unsigned short readValue;
    unsigned short controlValue;
    int bytesTransferred;

    JUMP_IF_NO_VALID_USB( deviceDesc, retval, _check_valid_counter_device( deviceDesc, BlockIndex, CounterIndex ), usb, out_CTR_8254Read );

    AIOUSB_UnLock();

    controlValue = (( unsigned short )CounterIndex << 8) | ( unsigned short )BlockIndex;
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_READ_FROM_DEVICE,
                                                 AUR_CTR_READ,
                                                 controlValue,
                                                 0,
                                                 ( unsigned char* )&readValue,
                                                 sizeof(readValue),
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred == sizeof(readValue)) {
        /* TODO: verify endian mode; original code had it reversed*/
        *pReadValue = readValue;
    } else
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

 out_CTR_8254Read:
    AIOUSB_UnLock();
    return result;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254ReadAll(
                              unsigned long DeviceIndex,
                              unsigned short *pData
                              ) {
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    int READ_BYTES;
    int bytesTransferred;
    USBDevice *usb;
    
    JUMP_IF_NO_VALID_USB( deviceDesc, retval, _check_valid_counter_device_for_read( deviceDesc, pData ) , usb, out_CTR_8254ReadAll);

    READ_BYTES = deviceDesc->Counters * COUNTERS_PER_BLOCK * sizeof(unsigned short);
    AIOUSB_UnLock();
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_READ_FROM_DEVICE, 
                                                 AUR_CTR_READALL,
                                                 0, 
                                                 0, 
                                                 ( unsigned char* )pData, 
                                                 READ_BYTES, 
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred != READ_BYTES)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    
 out_CTR_8254ReadAll:
    AIOUSB_UnLock();
    return result;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254ReadStatus(
                                 unsigned long DeviceIndex,
                                 unsigned long BlockIndex,
                                 unsigned long CounterIndex,
                                 unsigned short *pReadValue,
                                 unsigned char *pStatus
                                 ) 
{
    int READ_BYTES = 3; 
    unsigned char readData[ READ_BYTES ];
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    unsigned short controlValue;
    int bytesTransferred;
    AIOUSBDevice * deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    USBDevice *usb;

    JUMP_IF_NO_VALID_USB( deviceDesc, retval, _check_block_index( deviceDesc, BlockIndex, CounterIndex ), usb, out_CTR_8254ReadStatus );  

    AIOUSB_UnLock();

    controlValue = (( unsigned short )CounterIndex << 8) | ( unsigned short )BlockIndex;

    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_READ_FROM_DEVICE, 
                                                 AUR_CTR_READ,
                                                 controlValue, 
                                                 0, 
                                                 readData, 
                                                 READ_BYTES, 
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred == READ_BYTES) {
        *pReadValue = *( unsigned short* )readData;
        *pStatus = readData[ 2 ];
    } else
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

 out_CTR_8254ReadStatus:
    AIOUSB_UnLock();
    return result;
}


/*----------------------------------------------------------------------------*/
/**
 * @brief Calculates the register values for buf->divisora, and buf->divisorb to create
 * an output clock that matches the value stored in buf->hz * 
 * @param hz 
 * @param [out] diva Divisor A to be calculated
 * @param [out] divb Divisor B to be calculated
 * @return >= 0 if succesful, - if failure
 */
AIORET_TYPE CTR_CalculateCountersForClock( int hz , int *diva, int *divb )
{

    AIO_ASSERT( diva );
    AIO_ASSERT( divb );
    float l;
    int divisora, divisorb, divisorab;
    int min_err, err;

    if (  hz == 0 ) {
        return -AIOUSB_ERROR_INVALID_PARAMETER;
    }
    if (   hz * 4 >= ROOTCLOCK ) {
        divisora = 2;
        divisorb = 2;
    } else { 
        divisorab = ROOTCLOCK / hz;
        l = sqrt( divisorab );
        if ( l > 0xffff ) { 
            divisora = 0xffff;
            divisorb = 0xffff;
            min_err  = abs((int)(round(((ROOTCLOCK / hz) - (int)(divisora * l)))));
        } else  { 
            divisora  = round( divisorab / l );
            l         = round(sqrt( divisorab ));
            divisorb  = l;

            min_err = abs(((ROOTCLOCK / hz) - (int)(divisora * l)));
      
            for ( unsigned lv = l ; lv >= 2 ; lv -- ) {
                unsigned olddivisora = (int)round((double)divisorab / lv);
                if (  olddivisora > 0xffff ) { 
                    AIOUSB_DEVEL( "Found value > 0xff..resetting" );
                    break;
                } else { 
                    divisora = olddivisora;
                }

                err = abs((int)((ROOTCLOCK / hz) - (divisora * lv)));
                if (  err <= 0  ) {
                    min_err = 0;
                    AIOUSB_DEVEL("Found zero error: %d\n", lv );
                    divisorb = lv;
                    break;
                } 
                if (  err < min_err  ) {
                    AIOUSB_DEVEL( "Found new error: using lv=%d\n", (int)lv);
                    divisorb = lv;
                    min_err = err;
                }
                divisora = (int)round(divisorab / divisorb);
            }
        }
    }
    *diva = divisora;
    *divb = divisorb;
    return AIOUSB_SUCCESS;
}


/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_StartOutputFreq(unsigned long DeviceIndex, unsigned long BlockIndex, double *pHz ) 
{
    AIO_ASSERT( pHz );
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice * deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    RETURN_IF_INVALID_INPUT( deviceDesc, result, _check_valid_counter_output_frequency( deviceDesc, BlockIndex, pHz ) );

    if (*pHz <= 0) {
                                /* turn off counters */
          AIOUSB_UnLock();
          result = CTR_8254Mode(DeviceIndex, BlockIndex, 1, 2);
          if (result != AIOUSB_SUCCESS)
              return result;
          result = CTR_8254Mode(DeviceIndex, BlockIndex, 2, 3);
          if (result != AIOUSB_SUCCESS)
              return result;
          *pHz = 0;                                                                   /* actual clock speed*/
      } else {
           long rootClock = deviceDesc->RootClock;
          AIOUSB_UnLock();
           long frequency = ( long )*pHz;
           long MIN_DIVISOR = 2;
           long MAX_DIVISOR = 65535;
          long bestHighDivisor = MIN_DIVISOR,
               bestLowDivisor = MIN_DIVISOR,
               minFreqError = 0;
          AIOUSB_BOOL minFreqErrorValid = AIOUSB_FALSE;
           long divisor = ( long )round(( double )rootClock / ( double )frequency);
#if defined(DEBUG_START_CLOCK)
          printf(
              "Calculating divisors (total divisor = %ld)\n"
              "  %8s  %8s  %8s\n",
              divisor, "High", "Low", "Error"
              );
#endif
          if (divisor > MIN_DIVISOR * MIN_DIVISOR) {
                long lowDivisor;
                for(lowDivisor = ( long )sqrt(divisor); lowDivisor >= MIN_DIVISOR; lowDivisor--) {
                      long highDivisor = divisor / lowDivisor;
                       long freqError = labs(frequency - rootClock / (highDivisor * lowDivisor));
#if defined(DEBUG_START_CLOCK)
                      printf("  %8ld  %8ld  %8ld\n", highDivisor, lowDivisor, freqError);
#endif
                      if (highDivisor > MAX_DIVISOR) {
                          /* this divisor would exceed the maximum; use best divisor calculated thus far*/
                            break;
                        } else if (freqError == 0) {
                          /* these divisors have no error; no need to continue searching for divisors*/
                            minFreqErrorValid = AIOUSB_TRUE;
                            minFreqError = freqError;
                            bestHighDivisor = highDivisor;
                            bestLowDivisor = lowDivisor;
                            break;
                        } else if (
                          !minFreqErrorValid ||
                          freqError < minFreqError
                          ) {
                            minFreqErrorValid = AIOUSB_TRUE;
                            minFreqError = freqError;
                            bestHighDivisor = highDivisor;
                            bestLowDivisor = lowDivisor;
                        }
                  }
            }
#if defined(DEBUG_START_CLOCK)
          printf("  %8ld  %8ld  %8ld (final)\n", bestHighDivisor, bestLowDivisor, minFreqError);
#endif
          result = CTR_8254ModeLoad(DeviceIndex, BlockIndex, 1, 2, ( unsigned short )bestHighDivisor);
          if (result != AIOUSB_SUCCESS)
              return result;
          result = CTR_8254ModeLoad(DeviceIndex, BlockIndex, 2, 3, ( unsigned short )bestLowDivisor);
          if (result != AIOUSB_SUCCESS)
              return result;
          *pHz = rootClock / (bestHighDivisor * bestLowDivisor);              /* actual clock speed*/
      }

    return result;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254SelectGate(
                               unsigned long DeviceIndex,
                               unsigned long GateIndex
                               )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc =AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    int bytesTransferred;
    USBDevice *usb = NULL;

    JUMP_IF_NO_VALID_USB( deviceDesc, result, _check_valid_counter_device_for_gate(deviceDesc, GateIndex ), usb, out_CTR_8254SelectGate );

    
    AIOUSB_UnLock();
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE, 
                                                 AUR_CTR_SELGATE,
                                                 GateIndex, 
                                                 0, 
                                                 0, 
                                                 0, 
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred != 0)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

 out_CTR_8254SelectGate:
    AIOUSB_UnLock();
    return result;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE CTR_8254ReadLatched(
                                unsigned long DeviceIndex,
                                unsigned short *pData
                                ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    USBDevice *usb;
    int READ_BYTES;
    int bytesTransferred;

    JUMP_IF_NO_VALID_USB( deviceDesc, result, _check_valid_counters(deviceDesc ), usb, out_CTR_8254ReadLatched );
    
    READ_BYTES = deviceDesc->Counters * COUNTERS_PER_BLOCK * sizeof(unsigned short) + 1 ;/* for "old data" flag */
    
    AIOUSB_UnLock();
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_READ_FROM_DEVICE, 
                                                 AUR_CTR_READLATCHED,
                                                 0, 
                                                 0, 
                                                 ( unsigned char* )pData, 
                                                 READ_BYTES, 
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred != READ_BYTES)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    
 out_CTR_8254ReadLatched:
    AIOUSB_UnLock();
    return retval;
}



#ifdef __cplusplus
}
#endif
