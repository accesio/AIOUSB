/**
 * @file   AIOUSB_DIO.c
 * @author  Jimi Damon <jdamon@accesio.com>
 * @date    Thu Jul 20 14:41:44 2017 -0700
 * @version  cfdbc9a
 * @brief Core code to interface with Digital cards
 */

#include "AIOUSB_DIO.h"
#include "AIODeviceTable.h"
#include "AIOUSB_Core.h"
#include "USBDevice.h"
#include <arpa/inet.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif

int MASK_BYTES_SIZE(AIOUSBDevice *device) 
{
    return   ((device->DIOBytes + BITS_PER_BYTE - 1) / BITS_PER_BYTE );
}

int TRISTATE_BYTES_SIZE(AIOUSBDevice *device) 
{
    return  ((device->Tristates + BITS_PER_BYTE - 1) / BITS_PER_BYTE);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Returns the number in Big-Endian format
 */
unsigned short aiousb_htons(unsigned short octaveOffset)
{
#if defined(__BYTE_ORDER__)&&(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return (((0xff) & octaveOffset) << 8 ) | (( 0xff00 & octaveOffset )>>8 );
#else 
    return octaveOffset;
#endif
}


/*----------------------------------------------------------------------------*/
static unsigned short OctaveDacFromFreq(double *Hz) 
{
    AIO_ASSERT( Hz );
    unsigned short octaveOffset = 0;
    if (*Hz > 0) {
          if (*Hz > 40000000.0)
              *Hz = 40000000.0;
          int offset,
              octave = ( int )floor(3.322 * log10(*Hz / 1039));
          if (octave < 0) {
                octave = offset = 0;
            } else {
                offset = ( int )round(2048.0 - (ldexp(2078, 10 + octave) / *Hz));
                octaveOffset = (( unsigned short )octave << 12) | (( unsigned short )offset << 2);
                octaveOffset = aiousb_htons(octaveOffset); // oscillator wants the value in big-endian format
            }
          *Hz = (2078 << octave) / (2.0 - offset / 1024.0);
      }
    return octaveOffset;
}

/*----------------------------------------------------------------------------*/
AIOUSBDevice *_check_dio( unsigned long DeviceIndex, AIORESULT *result ) 
{
    AIO_ASSERT_RET(NULL, result );

    AIOUSBDevice *device = AIODeviceTableGetDeviceAtIndex( DeviceIndex, result );
    
    AIO_ERROR_VALID_DATA(NULL, device );
    AIO_ERROR_VALID_DATA(NULL, *result == AIOUSB_SUCCESS );

    AIO_ERROR_VALID_DATA_W_CODE(NULL, *result = AIOUSB_ERROR_NOT_SUPPORTED, device->DIOBytes != 0 );
    return device;
}

/*----------------------------------------------------------------------------*/
USBDevice *_check_dio_get_device_handle( unsigned long DeviceIndex, 
                                         AIOUSBDevice **device,  
                                         AIORESULT *result ) 
{
    AIO_ASSERT_RET(NULL, device );
    AIO_ASSERT_RET(NULL, result );

    *device = _check_dio( DeviceIndex, result );
    AIO_ERROR_VALID_DATA(NULL, *result == AIOUSB_SUCCESS );

    return AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex , result );
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_ConfigureWithDIOBuf(
                                  unsigned long DeviceIndex,
                                  unsigned char bTristate,
                                  AIOChannelMask *mask,
                                  DIOBuf *buf
                                  )
{
    AIOUSBDevice  *device = NULL;
    AIORESULT result;
    char *dest, *configBuffer;
    int bufferSize;
    int bytesTransferred;

    AIO_ASSERT( bTristate == AIOUSB_FALSE || bTristate == AIOUSB_TRUE );
    AIO_ASSERT_CHANNELMASK( mask );
    AIO_ASSERT_DIOBUF( buf );


    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device, &result );
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    AIO_ERROR_VALID_DATA(-AIOUSB_ERROR_NOT_ENOUGH_MEMORY, device->LastDIOData != 0 );
    char *tmp = DIOBufToBinary(buf);
    memcpy(device->LastDIOData, tmp, DIOBufByteSize( buf ) );

    bufferSize = device->DIOBytes + MASK_BYTES_SIZE(device);

    configBuffer = ( char* )malloc( bufferSize );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_ENOUGH_MEMORY, configBuffer );

    dest = configBuffer;

    memcpy( dest, tmp, device->DIOBytes );


    dest += device->DIOBytes;

    for( int i = 0; i < MASK_BYTES_SIZE( device ) ; i ++ ) {
        char tmpmask;
        if ( (result = AIOChannelMaskGetMaskAtIndex( mask, &tmpmask, i ) != AIOUSB_SUCCESS ) ) { 
            return result;
        }
        memcpy( dest, &tmpmask, 1 );
        dest += 1;
    }

    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE,
                                                 AUR_DIO_CONFIG,
                                                 bTristate,
                                                 0, 
                                                 (unsigned char *)configBuffer,
                                                 bufferSize,
                                                 device->commTimeout 
                                                 );


    if (bytesTransferred != bufferSize )
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    free(configBuffer);

    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_Configure(
                           unsigned long DeviceIndex,
                           unsigned char bTristate,
                           void *pOutMask,
                           void *pData
                           ) 
{
    AIO_ASSERT( bTristate == AIOUSB_FALSE || bTristate == AIOUSB_TRUE );
    AIO_ASSERT( pOutMask );
    AIO_ASSERT( pData );

    AIORESULT result ;
    AIOUSBDevice *device = _check_dio( DeviceIndex, &result );
    AIO_ERROR_VALID_DATA(result, result == AIOUSB_SUCCESS );

    memcpy(device->LastDIOData, pData, device->DIOBytes);

    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device, &result );

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    int bufferSize = device->DIOBytes + 2 * MASK_BYTES_SIZE( device );

    unsigned char *configBuffer = ( unsigned char* )malloc(bufferSize);
    
    AIO_ERROR_VALID_DATA(AIOUSB_ERROR_NOT_ENOUGH_MEMORY, configBuffer );

    unsigned char *dest = configBuffer;
    memcpy(dest, pData, device->DIOBytes);
    dest += device->DIOBytes;
    memcpy(dest, pOutMask, MASK_BYTES_SIZE( device ));
    dest += MASK_BYTES_SIZE( device );
    memset(dest, 0, MASK_BYTES_SIZE( device ) );
    
    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     AUR_DIO_CONFIG,
                                                     bTristate,
                                                     0, 
                                                     (unsigned char *)configBuffer,
                                                     bufferSize,
                                                     device->commTimeout 
                                                     );

    if (bytesTransferred != bufferSize)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    free(configBuffer);
    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_ConfigureEx( 
                          unsigned long DeviceIndex, 
                          void *pOutMask, 
                          void *pData, 
                          void *pTristateMask 
                           ) 
{ 

    AIO_ASSERT( pOutMask );
    AIO_ASSERT( pData );
    AIO_ASSERT( pTristateMask );
  
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    USBDevice * usb = _check_dio_get_device_handle( DeviceIndex, &device, &result );

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_DEVICE_NOT_CONNECTED, result == AIOUSB_SUCCESS );

    memcpy(device->LastDIOData, pData, device->DIOBytes);

    int bufferSize = device->DIOBytes + MASK_BYTES_SIZE( device) + TRISTATE_BYTES_SIZE(device);
    unsigned char *configBuffer = ( unsigned char* )malloc(bufferSize);

    AIO_ERROR_VALID_DATA(AIOUSB_ERROR_NOT_ENOUGH_MEMORY, configBuffer );

    unsigned char *dest = configBuffer;
    memcpy(dest, pData, device->DIOBytes);
    dest += device->DIOBytes;
    memcpy(dest, pOutMask, MASK_BYTES_SIZE( device ) );
    dest += MASK_BYTES_SIZE( device );
    memcpy(dest, pTristateMask, TRISTATE_BYTES_SIZE( device ) );

    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     AUR_DIO_CONFIG,
                                                     0,
                                                     device->DIOBytes,
                                                     configBuffer,
                                                     bufferSize,
                                                     device->commTimeout
                                                     );

    if (bytesTransferred != bufferSize)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    free(configBuffer);

    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_ConfigurationQuery(
                                 unsigned long DeviceIndex,
                                 void *pOutMask,
                                 void *pTristateMask
                                 ) 
{
    AIO_ASSERT( pOutMask );
    AIO_ASSERT( pTristateMask );

    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device;    
    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device, &result );

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    int bufferSize = MASK_BYTES_SIZE( device ) + TRISTATE_BYTES_SIZE( device );
    unsigned char *configBuffer = ( unsigned char* )malloc(bufferSize);

    AIO_ERROR_VALID_DATA(AIOUSB_ERROR_NOT_ENOUGH_MEMORY, configBuffer );

    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_READ_FROM_DEVICE,
                                                     AUR_DIO_CONFIG_QUERY,
                                                     0,
                                                     device->DIOBytes,
                                                     configBuffer,
                                                     bufferSize,
                                                     device->commTimeout
                                                     );


    if ( bytesTransferred == bufferSize ) {
        memcpy(pOutMask, configBuffer, MASK_BYTES_SIZE( device ) );
        memcpy(pTristateMask, configBuffer + MASK_BYTES_SIZE( device ), TRISTATE_BYTES_SIZE( device ) );
    } else {
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    }
    free(configBuffer);
    
    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_WriteAll(
                       unsigned long DeviceIndex,
                       void *pData
                       ) 
{
    AIO_ASSERT( pData );
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device, &result );

    if (!usb ) {
        return AIOUSB_ERROR_DEVICE_NOT_CONNECTED;
    }

    if (device->LastDIOData == 0) {
        result = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
    }

    char foo[10] = {};
    memcpy(foo, pData, device->DIOBytes);
    memcpy(device->LastDIOData, pData, device->DIOBytes);

    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     AUR_DIO_WRITE,
                                                     0,
                                                     0,
                                                     ( unsigned char* )pData,
                                                     device->DIOBytes,
                                                     device->commTimeout
                                                     );


    if (bytesTransferred != (signed)device->DIOBytes )
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_Write8(
                     unsigned long DeviceIndex,
                     unsigned long ByteIndex,
                     unsigned char Data
                     ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device, &result );

    if ( !device->DIOBytes )
        return AIOUSB_ERROR_NOT_ENOUGH_MEMORY;

    if (!usb  )
        return AIOUSB_ERROR_DEVICE_NOT_CONNECTED;

    int dioBytes = device->DIOBytes;


    unsigned char * dataBuffer = ( unsigned char* )malloc(dioBytes);


    if (!dataBuffer )
        return AIOUSB_ERROR_NOT_ENOUGH_MEMORY;

    device->LastDIOData[ ByteIndex ] = Data;
    memcpy(dataBuffer, device->LastDIOData, dioBytes);
    
    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     AUR_DIO_WRITE,
                                                     0,
                                                     0,
                                                     dataBuffer,
                                                     dioBytes,
                                                     device->commTimeout
                                                     );
    if (bytesTransferred != dioBytes)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    free(dataBuffer);

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief 
 * @param DeviceIndex  Device to write to 
 * @param BitIndex  The index of the bit to configure
 * @param bData The value to write
 * @return AIORESULT
 */
AIORESULT DIO_Write1(
                     unsigned long DeviceIndex,
                     unsigned long BitIndex,
                     unsigned char bData
                     ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval;
    AIOUSBDevice *deviceDesc = _check_dio( DeviceIndex, &result );
    USBDevice *usb;
    AIO_ASSERT_AIORET_TYPE( result, result == AIOUSB_SUCCESS );
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_PARAMETER, BYTE_INDEX(BitIndex) < deviceDesc->DIOBytes );
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_PARAMETER, bData == AIOUSB_FALSE || bData == AIOUSB_TRUE );

    AIO_ERROR_VALID_DATA_RETVAL( AIOUSB_ERROR_NOT_ENOUGH_MEMORY, deviceDesc->LastDIOData );
    usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    AIO_ERROR_VALID_DATA_RETVAL( AIOUSB_ERROR_INVALID_USBDEVICE , usb );

    unsigned char value = deviceDesc->LastDIOData[ BYTE_INDEX(BitIndex) ];
    unsigned char bitMask = 1 << (BitIndex % BITS_PER_BYTE);
    if (bData == AIOUSB_FALSE)
        value &= ~bitMask;
    else
        value |= bitMask;

    AIO_ERROR_VALID_DATA_RETVAL( AIOUSB_ERROR_BAD_TOKEN_TYPE,  deviceDesc->DIOBytes );
    AIO_ERROR_VALID_DATA_RETVAL( AIOUSB_ERROR_INVALID_ADDRESS, BYTE_INDEX( BitIndex ) < deviceDesc->DIOBytes );

    if ( bData ) {
        deviceDesc->LastDIOData[BYTE_INDEX(BitIndex)] = deviceDesc->LastDIOData[BYTE_INDEX(BitIndex)] | ( 1 << (BitIndex & 7));
    } else {
        deviceDesc->LastDIOData[BYTE_INDEX(BitIndex)] = deviceDesc->LastDIOData[BYTE_INDEX(BitIndex)] & ( ~(1 << (BitIndex & 7)));
    }
    
    retval = usb->usb_control_transfer( usb,
                                        USB_WRITE_TO_DEVICE,
                                        AUR_DIO_WRITE,
                                        0,
                                        0,
                                        &deviceDesc->LastDIOData[0],
                                        deviceDesc->DIOBytes,
                                        deviceDesc->commTimeout );
    if ( retval < 0 ) {
        result = AIOUSB_ERROR_INTERNAL_ERROR;
    }
    
    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_ReadAll(
                      unsigned long DeviceIndex,
                      void *buf
                      ) 
{
    if ( !buf ) 
        return AIOUSB_ERROR_INVALID_PARAMETER;
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device,  &result );
    if ( !usb || result != AIOUSB_SUCCESS )
        return AIOUSB_ERROR_DEVICE_NOT_FOUND;

    int bytesTransferred = usb->usb_control_transfer( usb,
                                                      USB_READ_FROM_DEVICE, 
                                                      AUR_DIO_READ,
                                                      0,
                                                      0,
                                                      (unsigned char *)buf,
                                                      device->DIOBytes,
                                                      device->commTimeout
                                                      );
    
    if ( bytesTransferred != (int)device->DIOBytes )
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT( bytesTransferred );
    
    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @deprecated You should use the function DIO_ReadAllToDIOBuf instead
 * @param DeviceIndex 
 * @param buf 
 * @return 
 */
AIORET_TYPE DIO_ReadIntoDIOBuf(
                                unsigned long DeviceIndex,
                                DIOBuf *buf
                               ) 
{
    return DIO_ReadAllToDIOBuf( DeviceIndex, buf );
}


/*----------------------------------------------------------------------------*/
AIORET_TYPE DIO_ReadAllToDIOBuf(
                                unsigned long DeviceIndex,
                                DIOBuf *buf
                               ) 
{
    AIO_ASSERT( buf );

    AIORET_TYPE result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    int bytesTransferred;
    char *tmpbuf;

    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device,  (AIORESULT*)&result );
    AIO_ASSERT_AIORET_TYPE( AIOUSB_ERROR_INVALID_DEVICE , result == AIOUSB_SUCCESS );

    tmpbuf = (char*)malloc( sizeof(char)*device->DIOBytes );

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_ENOUGH_MEMORY, tmpbuf );
    
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_READ_FROM_DEVICE, 
                                                 AUR_DIO_READ,
                                                 0, 
                                                 0, 
                                                 (unsigned char *)tmpbuf,
                                                 device->DIOBytes,
                                                 device->commTimeout
                                                 );

    if ( bytesTransferred < 0 || bytesTransferred != (int)device->DIOBytes ) {
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
        goto cleanup_DIO_ReadAll;
    }

    if ( DIOBufResize( buf, device->DIOBytes ) == NULL ) {
        result = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
        goto cleanup_DIO_ReadAll;
    }
    
    DIOBufReplaceString( buf, tmpbuf, device->DIOBytes );
    cleanup_DIO_ReadAll:
    free(tmpbuf);

    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_ReadAllToCharStr(
                               unsigned long DeviceIndex,
                               char *buf,
                               unsigned size
                               ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device,  &result );
    AIO_ERROR_VALID_DATA( result , result == AIOUSB_SUCCESS );

    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_READ_FROM_DEVICE, 
                                                     AUR_DIO_READ,
                                                     0, 
                                                     0, 
                                                     (unsigned char *)buf,
                                                     MIN( size, device->DIOBytes ),
                                                     device->commTimeout
                                                     );
    if ( bytesTransferred < 0 || bytesTransferred != (int)device->DIOBytes )
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    
    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_Read8(
                    unsigned long DeviceIndex,
                    unsigned long ByteIndex,
                    unsigned char *pdat
                    ) 
{
    AIO_ASSERT( pdat );
    
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    DIOBuf *readBuffer;

    _check_dio_get_device_handle( DeviceIndex, &device, &result );

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );
 
    readBuffer = NewDIOBuf( device->DIOBytes * BITS_PER_BYTE );

    AIO_ERROR_VALID_DATA(AIOUSB_ERROR_NOT_ENOUGH_MEMORY, readBuffer );

    if ( (result = DIO_ReadAll(DeviceIndex, readBuffer->buffer)) == AIOUSB_SUCCESS ) {
        *pdat = (unsigned char)readBuffer->buffer[ByteIndex];
    }

    DeleteDIOBuf( readBuffer );

    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_Read1(
                    unsigned long DeviceIndex,
                    unsigned long BitIndex,
                    unsigned char *bit
                    ) 
{
    char result = AIOUSB_SUCCESS;
    unsigned char value = 0;
    
    AIO_ASSERT( bit );

    if ((result = DIO_Read8(DeviceIndex, BitIndex / BITS_PER_BYTE, &value )) >= AIOUSB_SUCCESS) {
         unsigned char bitMask = 1 << (( int )BitIndex % BITS_PER_BYTE);
        if ((value & bitMask) != 0)
          *bit = AIOUSB_TRUE;
        else
          *bit = AIOUSB_FALSE;
    }
    return result;
}

/*----------------------------------------------------------------------------*/
AIOUSBDevice *_check_dio_stream( unsigned long DeviceIndex , AIORESULT *result ) 
{
    AIO_ASSERT_RET(NULL, result );
    AIOUSBDevice *device = _check_dio( DeviceIndex, result );

    AIO_ERROR_VALID_DATA_W_CODE( NULL,*result = AIOUSB_ERROR_NOT_SUPPORTED , device->bDIOStream == AIOUSB_TRUE );

    AIO_ERROR_VALID_DATA_W_CODE(NULL, *result = AIOUSB_ERROR_OPEN_FAILED,  device->bDIOOpen == AIOUSB_TRUE );

    return device;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_StreamOpen( unsigned long DeviceIndex,
                          unsigned long bIsRead ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    USBDevice *usb = _check_dio_get_device_handle( DeviceIndex, &device, &result );

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     bIsRead ? AUR_DIO_STREAM_OPEN_INPUT : AUR_DIO_STREAM_OPEN_OUTPUT,
                                                     0, 
                                                     0, 
                                                     0, 
                                                     0, 
                                                     device->commTimeout
                                                     );
    if (bytesTransferred == 0) {
        device->bDIOOpen = AIOUSB_TRUE;
        device->bDIORead = bIsRead ? AIOUSB_TRUE : AIOUSB_FALSE;
    } else {
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    }

    return result;
}


/*----------------------------------------------------------------------------*/
AIORESULT DIO_StreamClose(
                          unsigned long DeviceIndex
                          ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device =  _check_dio_stream( DeviceIndex, &result );
    if (result == AIOUSB_SUCCESS ) 
        device->bDIOOpen = AIOUSB_FALSE;

    return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT DIO_StreamSetClocks(
                              unsigned long DeviceIndex,
                              double *ReadClockHz,
                              double *WriteClockHz
                              ) 
{
    AIO_ASSERT( ReadClockHz );
    AIO_ASSERT( WriteClockHz );

    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *device = NULL;
    int CONFIG_BLOCK_SIZE = 5;
    unsigned char configBlock[ CONFIG_BLOCK_SIZE ];
    int bytesTransferred;
    USBDevice *usb = AIOUSBDeviceGetUSBHandleFromDeviceIndex( DeviceIndex, &device, &result );
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    /**
     * @note  
     * @verbatim
     * fill in data for the vendor request
     * byte 0 used enable/disable read and write clocks
     *   bit 0 is write clock
     *   bit 1 is read  clock
     *     1 = off/disable
     *     0 = enable (1000 Khz is default value whenever enabled)
     * bytes 1-2 = write clock value
     * bytes 3-4 = read clock value
     * @endverbatim
     */

    configBlock[ 0 ] = 0x03; /* disable read and write clocks by default */

    if (*WriteClockHz > 0)
        configBlock[ 0 ] &= ~0x01; /* enable write clock */

    if (*ReadClockHz > 0)
        configBlock[ 0 ] &= ~0x02; /* enable read clock */

    *( unsigned short* )&configBlock[ 1 ] = OctaveDacFromFreq(WriteClockHz);
    *( unsigned short* )&configBlock[ 3 ] = OctaveDacFromFreq(ReadClockHz);

    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE, 
                                                 AUR_DIO_SETCLOCKS,
                                                 0, 
                                                 0, 
                                                 configBlock, 
                                                 CONFIG_BLOCK_SIZE, 
                                                 device->commTimeout
                                                 );
    if (bytesTransferred != CONFIG_BLOCK_SIZE)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    return result;
}

#define GET_ENDPOINT( isread )  ( isread ? (LIBUSB_ENDPOINT_IN | USB_BULK_READ_ENDPOINT) : (LIBUSB_ENDPOINT_OUT | USB_BULK_WRITE_ENDPOINT) )

int pow_of_minsize( int val  ) 
{
    return ((val / 512)+1)*512;
}


/*----------------------------------------------------------------------------*/
AIORESULT DIO_StreamFrame(
                          unsigned long DeviceIndex,
                          unsigned long FramePoints,
                          unsigned short *pFrameData,
                          unsigned long *BytesTransferred
                          ) 
{

    AIO_ASSERT( pFrameData );
    AIO_ASSERT( BytesTransferred );
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_PARAMETER, FramePoints );

    AIOUSBDevice *device = NULL;
    AIORESULT result = AIOUSB_SUCCESS;
    USBDevice *deviceHandle = _check_dio_get_device_handle( DeviceIndex, &device, &result );

    AIO_ERROR_VALID_DATA(AIOUSB_ERROR_DEVICE_NOT_CONNECTED, deviceHandle ); 
    AIO_ERROR_VALID_DATA(result, result == AIOUSB_SUCCESS );

    int streamingBlockSize = ( int )device->StreamingBlockSize;

    /**
     * @note convert parameter types to those that libusb likes
     */
    unsigned char *data = ( unsigned char* )pFrameData;
    int remaining = ( int )FramePoints * sizeof(unsigned short);
    int total = 0;
    while (remaining > 0) {
        int bytes;
        int libusbResult = deviceHandle->usb_bulk_transfer(deviceHandle, 
                                                           GET_ENDPOINT( device->bDIORead ),
                                                           data,
                                                           (remaining < streamingBlockSize) ? remaining : streamingBlockSize,
                                                           &bytes, 
                                                           device->commTimeout
                                                           );

        if (libusbResult == LIBUSB_SUCCESS || libusbResult == LIBUSB_ERROR_OVERFLOW ) {
            if (bytes > 0) {
                int TransferredBytes = MIN(bytes,remaining);
                total += TransferredBytes;
                data += TransferredBytes;
                remaining -= TransferredBytes;
            }
        } else {
            result = LIBUSB_RESULT_TO_AIOUSB_RESULT(libusbResult);
            break;
        }
    }
    if (result == AIOUSB_SUCCESS)
        *BytesTransferred = total;

    return result;
}


#ifdef __cplusplus
}
#endif

/*****************************************************************************
 * Self-test 
 * @note This section is for stress testing the DIO code without using the USB 
 * features
 *
 ****************************************************************************/ 

#ifdef SELF_TEST

#include "AIOUSBDevice.h"
#include "gtest/gtest.h"

#include <iostream>
using namespace AIOUSB;


TEST(DIO,CheckingInternalFunctions)
{
    unsigned long DeviceIndex = 0;
    AIOUSBDevice *device;
    AIORESULT result = AIOUSB_ERROR_INVALID_DEVICE;

    USBDevice *usb;

    int numDevices = 0;
    int tmp;
    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_DIO_32 );

    ASSERT_DEATH( {_check_dio_get_device_handle( DeviceIndex, &device, NULL ); }, "Assertion `result' failed");
    ASSERT_DEATH( {_check_dio_get_device_handle( DeviceIndex, NULL, NULL ); }, "Assertion `device' failed");

    result = AIOUSB_SUCCESS;
    usb = _check_dio_get_device_handle( DeviceIndex, &device, &result );
    ASSERT_FALSE( usb );
    ASSERT_EQ( -AIOUSB_ERROR_INVALID_USBDEVICE , result );


    ASSERT_DEATH( {_check_dio( DeviceIndex, NULL ); },"Assertion `result' failed"); 

    tmp = deviceTable[0].DIOBytes;
    deviceTable[0].DIOBytes = 0;

    /* Verify that DIOBytes is checked */
    device = _check_dio( DeviceIndex, &result);
    ASSERT_FALSE( device );
    ASSERT_EQ( AIOUSB_ERROR_NOT_SUPPORTED, result );
    deviceTable[0].DIOBytes = tmp;

    ASSERT_DEATH( { OctaveDacFromFreq(NULL); }, "Assertion `Hz' failed");

    /* Two settings to induce failure */
    deviceTable[DeviceIndex].bDIOStream = AIOUSB_FALSE;
    deviceTable[DeviceIndex].bDIOOpen = AIOUSB_TRUE;
    
    device = _check_dio_stream( DeviceIndex, &result );

   
    ASSERT_FALSE( device );
    ASSERT_EQ( AIOUSB_ERROR_NOT_SUPPORTED, result );
    deviceTable[DeviceIndex].bDIOStream = AIOUSB_TRUE;

    
    device = _check_dio_stream( DeviceIndex, &result );
    ASSERT_TRUE( device );

 }

TEST(DIO,CheckConfigureEx) 
{   
    unsigned long DeviceIndex = 0;
    AIOUSBDevice *device;
    AIORESULT result = AIOUSB_ERROR_INVALID_DEVICE;
    int numDevices = 0;
    void *pOutMask = NULL;
    void *pData = NULL ;
    void *pTristateMask = NULL;
    unsigned char bitindex = 0;

    ASSERT_DEATH( {DIO_ConfigureEx(DeviceIndex, pOutMask,pData,pTristateMask); },"Assertion `pOutMask' failed"); 
    ASSERT_DEATH( {DIO_ConfigurationQuery(DeviceIndex, pOutMask, pTristateMask ); }, "Assertion `pOutMask' failed" );
    ASSERT_DEATH( {DIO_Read1(DeviceIndex,bitindex, NULL) ; } , "Assertion `bit' failed" );

    ASSERT_DEATH( {DIO_ConfigureEx(DeviceIndex, (void *)42,pData,pTristateMask); },"Assertion `pData' failed"); 

    AIODeviceTableInit();    
    AIODeviceTableAddDeviceToDeviceTable( &numDevices, USB_DIO_32 );

    result = DIO_ConfigureEx(DeviceIndex, (void *)42,(void*)42,(void*)42);
    ASSERT_EQ( AIOUSB_ERROR_DEVICE_NOT_CONNECTED, result );
    result = DIO_StreamOpen( DeviceIndex , bitindex );
    ASSERT_EQ( -AIOUSB_ERROR_INVALID_USBDEVICE, result );

    result = DIO_Read8( DeviceIndex, bitindex , &bitindex);
    ASSERT_EQ( -AIOUSB_ERROR_INVALID_USBDEVICE, result );

}


#include <unistd.h>
#include <stdio.h>


int main(int argc, char *argv[] )
{
  
  AIORET_TYPE retval;

  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
#ifdef GTEST_TAP_PRINT_TO_STDOUT
  delete listeners.Release(listeners.default_result_printer());
#endif

  return RUN_ALL_TESTS();  

}

#endif
