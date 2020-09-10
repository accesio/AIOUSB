/**
 * @file   AIOUSB_DAC.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief Core code to handle DACs on AIOUSB devices.
 */

#include "AIOUSB_Core.h"
#include "AIODeviceTable.h"
#include <math.h>
#include <string.h>

#ifdef __cplusplus
namespace AIOUSB {
#endif

unsigned long DACDirect(unsigned long DeviceIndex,unsigned short Channel,unsigned short Value)
{
    AIORESULT result = AIOUSB_SUCCESS;

    DeviceDescriptor *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    result=AIOUSB_EnsureOpen(DeviceIndex);
    if ( result != AIOUSB_SUCCESS )
        return result;

    if (deviceDesc->ImmDACs == 0) {
        return AIOUSB_ERROR_NOT_SUPPORTED;
    }

    if ( deviceDesc->bDACStream && (deviceDesc->bDACOpen || deviceDesc->bDACClosing )) {
        return AIOUSB_ERROR_OPEN_FAILED;
    }

    if (Channel >= deviceDesc->ImmDACs) {
        return AIOUSB_ERROR_INVALID_PARAMETER;
    }
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;


    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     AUR_DAC_IMMEDIATE,
                                                     Value,
                                                     Channel,
                                                     0,
                                                     0, /* wLength */
                                                     deviceDesc->commTimeout
                                                     );
    if (bytesTransferred != 0)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);


    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief pDACData is an array of DACDataCount channel/count 16-bit word pairs:
 * @verbatim
 *   +----------------+
 *   |    channel     | word 0
 *   |----------------|
 *   |     count      | word 1
 *   |----------------|
 *          ...
 *   |----------------|
 *   |    channel     |
 *   |----------------|
 *   |     count      | word ( DACDataCount * 2 ) - 1
 *   +----------------+
 *
 * this array has to be converted to a different format when passed to the board:
 *        Block 0
 *   +----------------+
 *   |   chan mask    | byte 0
 *   |----------------|
 *   |  chan 0 count  | bytes 1-2
 *   |----------------|
 *          ...
 *   |----------------|
 *   |  chan 6 count  | bytes 13-14
 *   |----------------|
 *   |  chan 7 count  | bytes 15-16
 *   +----------------+
 *        Block 1
 *   +----------------+
 *   |   chan mask    | byte 17
 *   |----------------|
 *   |  chan 0 count  | bytes 18-19
 *   |----------------|
 *          ...
 *   |----------------|
 *   |  chan 6 count  | bytes 30-31
 *   |----------------|
 *   |  chan 7 count  | bytes 32-33
 *   +----------------+
 *          ...
 *        Block n
 *   +----------------+
 *   |   chan mask    |
 *   |----------------|
 *   |  chan 0 count  |
 *   |----------------|
 *          ...
 *   |----------------|
 *   |  chan 6 count  |
 *   |----------------|
 *   |  chan 7 count  | bytes ( ( 17 * n ) - 2 ) - ( ( 17 * n ) - 1 )
 *   +----------------+
 *@endverbatim
 * the channel mask (the first byte of each block) has a bit set to one for each
 * channel whose output is to be set; the count values are zero for channels that
 * aren't to be set; for example, a mask of 0x01 would write to only channel 0 on
 * a given block; a mask of 0x80 would write to only channel 7
 *
 * since the DAC configuration blocks are contiguous, the byte offset to a channel's
 * count within the buffer containing all the configuration blocks can be calculated as:
 *   offset = ( channel * sizeof( unsigned short ) ) + ( channel / 8 ) + 1;
 * although this calculation is correct, it's difficult to follow, so the code below
 * uses a slightly less efficient calculation that's easier to understand
 *
 * when sending the DAC configuration blocks to the device we have to send all the
 * blocks from block 0 up to the block containing the highest channel number being set
 */
unsigned long DACMultiDirect( unsigned long DeviceIndex,
                             unsigned short *pDACData,
                             unsigned long DACDataCount
                             )
{
    AIO_ASSERT( pDACData );
    AIO_ASSERT( DACDataCount <= 10000  );
    AIO_ERROR_VALID_DATA( AIOUSB_SUCCESS, !(DACDataCount == 0) ); /* NOOP */

    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    result=AIOUSB_EnsureOpen(DeviceIndex);
    if ( result != AIOUSB_SUCCESS )
        return result;

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, deviceDesc->ImmDACs );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_OPEN_FAILED,
                          !(
                            ( deviceDesc->bDACDIOStream || deviceDesc->bDACSlowWaveStream || deviceDesc->bDACStream ) &&
                            ( deviceDesc->bDACOpen || deviceDesc->bDACClosing )
                            )
                          );


    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    /**
     * determine highest channel number addressed in pDACData; no checking is
     * performed to ensure that the same channel is not set more than once
     */
    int highestChannel = 0,channel,index;
    for(index = 0; index < ( int )DACDataCount; index++) {
          channel = pDACData[ index * 2 ];            // channel/count pairs
          if (channel > highestChannel)
              highestChannel = channel;
      }

    if (highestChannel >= ( int )deviceDesc->ImmDACs) {
        return AIOUSB_ERROR_INVALID_PARAMETER;
    }

    int DACS_PER_BLOCK = 8;
    int CONFIG_BLOCK_BYTES = 1 /* mask */ + DACS_PER_BLOCK * sizeof(unsigned short) /* 16-bit counts */;
    int numConfigBlocks = (highestChannel / DACS_PER_BLOCK) + 1;
    int configBytes = CONFIG_BLOCK_BYTES * numConfigBlocks;
    unsigned char *configBuffer = ( unsigned char* )malloc(configBytes);

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_ENOUGH_MEMORY, configBuffer );

    /*sparsely populate DAC configuration blocks */
    memset(configBuffer, 0, configBytes);       // zero out channel masks and count values for unused channels
    for(index = 0; index < ( int )DACDataCount; index++) {
        channel = pDACData[ index * 2 ];         // channel/count pairs
        int maskOffset = (channel / DACS_PER_BLOCK) * CONFIG_BLOCK_BYTES;
        int countOffset
            = maskOffset                                    // first byte of block
            + 1                                                   // skip over mask byte
            + (channel % DACS_PER_BLOCK) * sizeof(unsigned short);             // word within block
        configBuffer[ maskOffset ] |= (1u << (channel % DACS_PER_BLOCK));
        *( unsigned short* )&configBuffer[ countOffset ] = pDACData[ index * 2 + 1 ];
    }

    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     AUR_DAC_IMMEDIATE,
                                                     0,
                                                     0,
                                                     configBuffer,
                                                     configBytes,
                                                     deviceDesc->commTimeout
                                                     );
    if (bytesTransferred != configBytes)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    free(configBuffer);

    return result;
}

/*----------------------------------------------------------------------------*/
/*
 * @brief Sets the range code for the DAC
 * @param DeviceIndex
 * @param RangeCode
 * @return
 */
unsigned long DACSetBoardRange(unsigned long DeviceIndex,unsigned long RangeCode )
{
    if ( RangeCode < DAC_RANGE_0_5V || RangeCode > DAC_RANGE_10V )
        return AIOUSB_ERROR_INVALID_PARAMETER;

    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    result=AIOUSB_EnsureOpen(DeviceIndex);
    if ( result != AIOUSB_SUCCESS )
        return result;

    if (deviceDesc->bDACBoardRange == AIOUSB_FALSE)
        return AIOUSB_ERROR_NOT_SUPPORTED;


    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     AUR_DAC_RANGE,
                                                     RangeCode,
                                                     0,
                                                     0,
                                                     0 /* wLength */,
                                                     deviceDesc->commTimeout
                                                     );
    if (bytesTransferred != 0)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    return result;
}



/*----------------------------------------------------------------------------*/
unsigned long DACOutputOpen(unsigned long DeviceIndex,double *pClockHz) {
  // TODO: this function is not yet implemented
    return AIOUSB_ERROR_NOT_SUPPORTED;
}


/*----------------------------------------------------------------------------*/
unsigned long DACOutputClose(unsigned long DeviceIndex,unsigned long bWait) {
  // TODO: this function is not yet implemented
    return AIOUSB_ERROR_NOT_SUPPORTED;
}


/*----------------------------------------------------------------------------*/
unsigned long DACOutputCloseNoEnd( unsigned long DeviceIndex, unsigned long bWait ) {
  // TODO: this function is not yet implemented
    return AIOUSB_ERROR_NOT_SUPPORTED;
}



/*----------------------------------------------------------------------------*/
unsigned long DACOutputSetCount(unsigned long DeviceIndex, unsigned long NewCount) {
  // TODO: this function is not yet implemented
    return AIOUSB_ERROR_NOT_SUPPORTED;
}



/*----------------------------------------------------------------------------*/
unsigned long DACOutputFrame(unsigned long DeviceIndex,
                             unsigned long FramePoints,
                             unsigned short *FrameData
                             ) {
  // TODO: this function is not yet implemented
    return AIOUSB_ERROR_NOT_SUPPORTED;
}



/*----------------------------------------------------------------------------*/
unsigned long DACOutputFrameRaw(
                                unsigned long DeviceIndex,
                                unsigned long FramePoints,
                                unsigned short *FrameData
                                ) {
  // TODO: this function is not yet implemented
    return AIOUSB_ERROR_NOT_SUPPORTED;
}



/*----------------------------------------------------------------------------*/
unsigned long DACOutputStart(
                             unsigned long DeviceIndex
                             ) {
  // TODO: this function is not yet implemented
    return AIOUSB_ERROR_NOT_SUPPORTED;
}



/*----------------------------------------------------------------------------*/
unsigned long DACOutputSetInterlock(
                                    unsigned long DeviceIndex,
                                    unsigned long bInterlock
                                    ) {
  // TODO: this function is not yet implemented
    return AIOUSB_ERROR_NOT_SUPPORTED;
}


/*----------------------------------------------------------------------------*/
#define SamplesPerDACUploadBlock (31 * 512)
unsigned long CSA_DACOutputProcess(
								unsigned long DeviceIndex,
								double *ClockHz,
								unsigned long Samples,
								unsigned short * sampleData
								) {

  AIORESULT result = AIOUSB_SUCCESS;
  AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
  if ( result != AIOUSB_SUCCESS )
      return result;
  USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
  if ( result != AIOUSB_SUCCESS )
      return result;

  result=AIOUSB_EnsureOpen(DeviceIndex);
    if ( result != AIOUSB_SUCCESS )
        return result;

  // stop ARB and reset read pointer
  int bytesTransferred = usb->usb_control_transfer(usb, USB_WRITE_TO_DEVICE, AUR_DAC_CONTROL, 0x80, 0, 0, 0, deviceDesc->commTimeout);
  if (bytesTransferred != 0)
      return LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

	// write data pointer to zero
	bytesTransferred = usb->usb_control_transfer(usb, USB_WRITE_TO_DEVICE, AUR_DAC_DATAPTR, 0, 0, 0, 0, deviceDesc->commTimeout);
  if (bytesTransferred != 0)
    return LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

	unsigned short CtrDivisor = 12000000.0 / *ClockHz;
	*ClockHz = 12000000.0 / CtrDivisor;

	// set ARB rate
	bytesTransferred = usb->usb_control_transfer(usb, USB_WRITE_TO_DEVICE, AUR_DAC_DIVISOR, 0, CtrDivisor, 0, 0, deviceDesc->commTimeout);
    if (bytesTransferred != 0)
        return LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

  int L;
  unsigned long status;
  unsigned short * ptr = sampleData;
    unsigned long samplesWritten = 0;
    int once = 0;
  while (Samples > SamplesPerDACUploadBlock)
  {
	L = 0;
	status = usb->usb_bulk_transfer(usb, 0x02, (unsigned char *)ptr, SamplesPerDACUploadBlock * 2, &L, 0);
	if (0 != status) return LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
	ptr += SamplesPerDACUploadBlock;
	Samples -= SamplesPerDACUploadBlock;
        samplesWritten += SamplesPerDACUploadBlock;
        if ((samplesWritten > 65536) && (!once))
        {
            once = 1;
	// start (just in the case where not enough data was sent that it auto-started; redundant starts are safe
	bytesTransferred = usb->usb_control_transfer(usb, USB_WRITE_TO_DEVICE, AUR_DAC_CONTROL, 0x01, 0, 0, 0, deviceDesc->commTimeout);
  if (bytesTransferred != 0)
    return LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
        }
    }
    L = 0;
    status = usb->usb_bulk_transfer(usb, 0x02, (unsigned char *)ptr, Samples * 2, &L, 0);
    if (0 != status) return LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

  return AIOUSB_SUCCESS;
}


#ifdef __cplusplus
}
#endif
