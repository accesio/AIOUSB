/**
 * @file   AIOUSB_ADC.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  Configuration functions for ADC elements
 *
 */

#include "AIOUSB_ADC.h"
#include "AIOUSB_CTR.h"
#include "AIOTypes.h"
#include "AIODeviceTable.h"
#include "AIOUSB_Core.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>


#ifdef __cplusplus
namespace AIOUSB {
#endif


struct ADRange adRanges[ AD_NUM_GAIN_CODES ] = {
  { 0   , 10 },                 /* AD_GAIN_CODE_0_10V  */
  { -10 , 20 },                 /* AD_GAIN_CODE_10V    */
  { 0   , 5  },                 /* AD_GAIN_CODE_0_5V   */
  { -5  , 10 },                 /* AD_GAIN_CODE_5V     */
  { 0   , 2  },                 /* AD_GAIN_CODE_0_2V   */
  { -2  , 4  },                 /* AD_GAIN_CODE_2V     */
  { 0   , 1  },                 /* AD_GAIN_CODE_0_1V   */
  { -1  , 2  }                  /* AD_GAIN_CODE_1V     */
};

AIORET_TYPE adc_get_bulk_data( ADCConfigBlock *config,USBDevice *usb,  unsigned char endpoint,
                               unsigned char *data, int datasize,int *bytes, unsigned timeout  );


// formerly public in the API
static unsigned long ADC_GetImmediate(
    unsigned long DeviceIndex,
    unsigned long Channel,
    unsigned short *pData);

/*----------------------------------------------------------------------------*/
unsigned long ADC_ResetDevice( unsigned long DeviceIndex  )
{
    AIORESULT result;
    int wValue = 0xe600;
    int wIndex = 1;
    int timeout = 1000;
    unsigned char data[1];
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result ;

    data[0] = 1;
    result = usb->usb_control_transfer(usb,
                                       USB_WRITE_TO_DEVICE,
                                       0xa0,
                                       wValue,
                                       wIndex,
                                       data,
                                       1,
                                       timeout
                                       );
    if ( result <= AIOUSB_SUCCESS )
        goto out_ADC_ResetDevice;

    data[0] = 0;
    sleep(2);
    result = usb->usb_control_transfer(usb,
                                       USB_WRITE_TO_DEVICE,
                                       0xa0,
                                       wValue,
                                       wIndex,
                                       data,
                                       1,
                                       timeout
                                       );
 out_ADC_ResetDevice:
    return result;
}

/*----------------------------------------------------------------------------*/
unsigned long *ADC_GetConfigSize( ADConfigBlock *config )
{
    return &config->size;
}

/*----------------------------------------------------------------------------*/
unsigned char *ADC_GetConfigRegisters( ADConfigBlock *config )
{
    return &config->registers[0];
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE ADC_ReadADConfigBlock( unsigned long DeviceIndex , ADConfigBlock *config )
{
    AIORESULT result;
    AIORET_TYPE retval;
    AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
    if ( result  != AIOUSB_SUCCESS )
        goto out_ADC_ReadADConfigBlock;

    result = GenericVendorRead( DeviceIndex,
                                AUR_ADC_GET_CONFIG ,
                                0 ,
                                0,
                                ADC_GetConfigRegisters( config ),
                                ADC_GetConfigSize( config )
                                );
    if ( result != AIOUSB_SUCCESS )
        retval = -result ;
    else
        retval = AIOUSB_SUCCESS;

out_ADC_ReadADConfigBlock:
    return retval;

}

/*----------------------------------------------------------------------------*/
/**
 * @brief
 * @param DeviceIndex
 * @param forceRead
 */
unsigned long ReadConfigBlock(unsigned long DeviceIndex,
                              AIOUSB_BOOL forceRead
                              )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
    ADCConfigBlock configBlock;
    if ( result  != AIOUSB_SUCCESS )
        return result;

    configBlock = *(ADCConfigBlock*)AIOUSBDeviceGetADCConfigBlock( deviceDesc );
    if ( !configBlock.device )
        configBlock.device = deviceDesc;

    if (forceRead || deviceDesc->cachedConfigBlock.size == 0) {
        USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
        if ( result != AIOUSB_SUCCESS )
            return result;

        ADCConfigBlockInitializeFromAIOUSBDevice( &configBlock, configBlock.device );

        if ( configBlock.testing != AIOUSB_TRUE ) {
            int bytesTransferred = usb->usb_control_transfer(usb,
                                                             USB_READ_FROM_DEVICE,
                                                             AUR_ADC_GET_CONFIG,
                                                             0,
                                                             0,
                                                             configBlock.registers,
                                                             configBlock.size,
                                                             deviceDesc->commTimeout
                                                             );

            if ( bytesTransferred != ( int ) configBlock.size) {
                result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
                goto out_ReadConfigBlock;
            }
            /*
             * check and correct settings read from device
             */
            ADC_VerifyAndCorrectConfigBlock( &configBlock , deviceDesc );
            deviceDesc->cachedConfigBlock = configBlock;
        }
    }
 out_ReadConfigBlock:

    return result;
}



/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 */
unsigned long WriteConfigBlock(unsigned long DeviceIndex)
{

    ADConfigBlock *configBlock;
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
    USBDevice *usb;
    int bytesTransferred;
    if ( result  != AIOUSB_SUCCESS )
        return result;


    configBlock = AIOUSB_GetConfigBlock( AIOUSB_GetDevice( DeviceIndex ));
    if (!configBlock ) {
        result = AIOUSB_ERROR_INVALID_ADCCONFIG_SETTING;
        goto out_WriteConfigBlock;
    }

    if ( configBlock->testing != AIOUSB_TRUE ) {
        usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
        if ( result  != AIOUSB_SUCCESS )
            goto out_WriteConfigBlock;

        bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_WRITE_TO_DEVICE,
                                                     AUR_ADC_SET_CONFIG,
                                                     0,
                                                     0,
                                                     configBlock->registers,
                                                     configBlock->size,
                                                     deviceDesc->commTimeout
                                                     );
        if ( bytesTransferred != ( int )configBlock->size )
            result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
    }

out_WriteConfigBlock:

    return result;
}

/*------------------------------------------------------------------------*/
/**
 * @brief Performs a number of ADC_GetImmediate calls and then averages out the values
 *       to determine adequate values for the Ground and Reference values
 * @param DeviceIndex
 * @param groundCounts
 * @param referenceCounts
 */
AIORESULT ADC_Acquire_Reference_Counts(
                                       unsigned long DeviceIndex,
                                       double *groundCounts,
                                       double *referenceCounts
                                       )
{
    int reading;
    AIORESULT result = AIOUSB_SUCCESS;
    double averageCounts;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    for(reading = 0; reading <= 1; reading++) {

        ADCConfigBlockSetCalMode(&deviceDesc->cachedConfigBlock, (reading == 0) ? AD_CAL_MODE_GROUND : AD_CAL_MODE_REFERENCE );
          result = WriteConfigBlock(DeviceIndex);

          if (result == AIOUSB_SUCCESS) {
            /*
             * average a bunch of readings to get a nice, stable
             * reading
             */
                int AVERAGE_SAMPLES = 256;
                unsigned MAX_GROUND = 0x00ff, MIN_REFERENCE = 0xf000;
                long countsSum = 0;
                int sample;
                unsigned short counts[ MAX_IMM_ADCS ];
                for(sample = 0; sample < AVERAGE_SAMPLES; sample++) {
                      result = ADC_GetImmediate(DeviceIndex, 0, counts);
                      if (result == AIOUSB_SUCCESS)
                          countsSum += counts[ 0 ];
                      else
                          goto RETURN_AIOUSB_GetBulkAcquire;
                  }

                averageCounts = countsSum / ( double )AVERAGE_SAMPLES;

                if (reading == 0) {
                      if (averageCounts <= MAX_GROUND)
                          *groundCounts = averageCounts;
                      else{
                            result = AIOUSB_ERROR_INVALID_DATA;
                            goto RETURN_AIOUSB_GetBulkAcquire;
                        }
                  }else {
                      if (
                          averageCounts >= MIN_REFERENCE &&
                          averageCounts <= AI_16_MAX_COUNTS
                          )
                          *referenceCounts = averageCounts;
                      else{
                            result = AIOUSB_ERROR_INVALID_DATA;
                            goto RETURN_AIOUSB_GetBulkAcquire;
                        }
                  }
            }
      }
RETURN_AIOUSB_GetBulkAcquire:
    return result;
}


                                /* number of samples device can buffer */
#define DEVICE_SAMPLE_BUFFER_SIZE 1024



/*--------------------------------------------------------------------------*/
/**
 * @brief Performs a scan and averages the voltage values.
 * @param DeviceIndex
 * @param counts
 * @return
 * @note In theory, all the A/D functions, including AIOUSB_GetScan(), should work
 * in all measurement modes, including calibration mode; in practice,
 * however, the device will return only a single sample in calibration mode;
 * therefore, users must be careful to select a single channel and set
 * oversample to zero during calibration mode; attempting to read more than
 * one channel or use an oversample setting of more than zero in calibration
 * mode will result in a timeout error; as a convenience to the user we
 * automatically impose this restriction here in AIOUSB_GetScan(); if the
 * device is changed to permit normal use of the A/D functions in
 * calibration mode, we will have to modify this function to somehow
 * recognize which devices support that capability, or simply delete this
 * restriction altogether and rely on the users' good judgment
 *
 * @note The oversample setting dictates how many samples to take _in addition_ to
 * the primary sample; if oversample is zero, we take just one sample for
 * each channel; if oversample is greater than zero then we average the
 * primary sample and all of its over-samples; if the discardFirstSample
 * setting is enabled, then we discard the primary sample, leaving just the
 * over-samples; thus, if discardFirstSample is enabled, we must take at
 * least one over-sample in order to have any data left; there's another
 * complication: the device buffer is limited to a small number of samples,
 * so we have to limit the number of over-samples to what the device buffer
 * can accommodate, so the actual oversample setting depends on the number
 * of channels being scanned; we also preserve and restore the original
 * oversample setting specified by the user; since the user is expecting to
 * average (1 + oversample) samples, then if discardFirstSample is enabled
 * we simply always add one
 */
PRIVATE AIORET_TYPE AIOUSB_GetScan( unsigned long DeviceIndex, unsigned short counts[] )
{
    ADConfigBlock origConfigBlock;
    AIOUSB_BOOL configChanged, discardFirstSample;
    int samplesToAverage = 0, sampleIndex = 0;
    int numChannels, samplesPerChannel, libusbresult;
    unsigned numSamples, overSample;
    int bytesTransferred;

    unsigned short *sampleBuffer;
    AIORET_TYPE result = AIOUSB_SUCCESS;
    unsigned char bcdata[] = {0x05,0x00,0x00,0x00 };

    AIO_ASSERT( counts );

    AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , (AIORESULT*)&result );
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex , (AIORESULT*)&result );

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA_RETVAL( AIOUSB_ERROR_NOT_SUPPORTED , deviceDesc->bADCStream == AIOUSB_TRUE );

    origConfigBlock     = deviceDesc->cachedConfigBlock;

    result = USBDeviceFetchADCConfigBlock( usb, &origConfigBlock );
    AIO_ASSERT_RET( result, result >= AIOUSB_SUCCESS );

    configChanged       = AIOUSB_FALSE;
    discardFirstSample  = deviceDesc->discardFirstSample;
    overSample          = ADCConfigBlockGetOversample(&deviceDesc->cachedConfigBlock);

    numChannels = ADCConfigBlockGetEndChannel( &deviceDesc->cachedConfigBlock ) -
                  ADCConfigBlockGetStartChannel( &deviceDesc->cachedConfigBlock) + 1;

    if ( ADCConfigBlockGetCalMode(&deviceDesc->cachedConfigBlock) == AD_CAL_MODE_GROUND || ADCConfigBlockGetCalMode(&deviceDesc->cachedConfigBlock) == AD_CAL_MODE_REFERENCE) {
        if (numChannels > 1) {
            ADCConfigBlockSetScanRange(&deviceDesc->cachedConfigBlock, ADCConfigBlockGetStartChannel(&deviceDesc->cachedConfigBlock), ADCConfigBlockGetStartChannel(&deviceDesc->cachedConfigBlock) );
            numChannels = 1;
            configChanged = AIOUSB_TRUE;
        }
        if (overSample > 0) {
            ADCConfigBlockSetOversample(&deviceDesc->cachedConfigBlock, 0 );
            configChanged = AIOUSB_TRUE;
        }
        discardFirstSample = AIOUSB_FALSE;           // this feature can't be used in calibration mode either
    }

    /**
     * Turn scan on and turn timer and external trigger off
     */
    ADCConfigBlockSetTriggerMode( &deviceDesc->cachedConfigBlock,
                                  ( ADCConfigBlockGetTriggerMode( &deviceDesc->cachedConfigBlock ) | AD_TRIGGER_SCAN) &
                                  (  ~(AD_TRIGGER_TIMER | AD_TRIGGER_EXTERNAL) ) );
    configChanged = AIOUSB_TRUE;

    samplesPerChannel = 1 + ADCConfigBlockGetOversample(&deviceDesc->cachedConfigBlock );

    if (discardFirstSample)
        samplesPerChannel++;
    if (samplesPerChannel > 256)
        samplesPerChannel = 256;               /* rained by maximum oversample of 255 */

    /**
     * make sure device buffer can accommodate this number
     * of samples
     */
    if ((numChannels * samplesPerChannel) > DEVICE_SAMPLE_BUFFER_SIZE )
        samplesPerChannel = DEVICE_SAMPLE_BUFFER_SIZE / numChannels;

    overSample = samplesPerChannel - 1;
    if (overSample != (unsigned)ADCConfigBlockGetOversample(&deviceDesc->cachedConfigBlock ) ) {
        ADCConfigBlockSetOversample(&deviceDesc->cachedConfigBlock, overSample);
        configChanged = AIOUSB_TRUE;
    }

    /**
     * Needs to be the correct values written out ...
     * Should resemble (04|05) F0 0E
     */
    if ( configChanged )
        result = USBDevicePutADCConfigBlock( usb, &deviceDesc->cachedConfigBlock );


    numSamples = numChannels * samplesPerChannel;

    sampleBuffer = ( unsigned short* )malloc( numSamples * sizeof(unsigned short) );
    if (!sampleBuffer ) {
        result = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
        goto out_AIOUSB_GetScan;
    }

    /* BC */
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE,
                                                 AUR_START_ACQUIRING_BLOCK,
                                                 (numSamples >> 16),           /* High Samples */
                                                 ( unsigned short )numSamples, /* Low samples */
                                                 bcdata,
                                                 sizeof(bcdata),
                                                 deviceDesc->commTimeout
                                                 );
    if ( bytesTransferred != (int)sizeof(bcdata) ) {
        result = -LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);
        goto out_freebuf_AIOUSB_GetScan;
    }

    /* BF */
    bytesTransferred = usb->usb_control_transfer(usb,
                                                 USB_WRITE_TO_DEVICE,
                                                 AUR_ADC_IMMEDIATE,
                                                 0,
                                                 0,
                                                 ( unsigned char* )sampleBuffer,
                                                 0,
                                                 deviceDesc->commTimeout
                                                 );
    if (bytesTransferred == 0) {
        libusbresult = adc_get_bulk_data( &deviceDesc->cachedConfigBlock,
                                         usb,
                                         LIBUSB_ENDPOINT_IN | USB_BULK_READ_ENDPOINT,
                                         ( unsigned char* )sampleBuffer,
                                         numSamples * sizeof(unsigned short),
                                         (int*)&bytesTransferred,
                                         deviceDesc->commTimeout
                                          );

        if (libusbresult != LIBUSB_SUCCESS) {
            result = -LIBUSB_RESULT_TO_AIOUSB_RESULT(libusbresult);
        } else if (bytesTransferred != (int)(numSamples * sizeof(unsigned short)) ) {
            result = -AIOUSB_ERROR_INVALID_DATA;
        } else {
            /**
             * Compute the average of all the samples taken for each channel, discarding
             * the first sample if that option is enabled; each byte in sampleBuffer[] is
             * 1 of 2 bytes for each sample, the first byte being the LSB and the second
             * byte the MSB, in other words, little-endian format; so for convenience we
             * simply declare sampleBuffer[] to be of type 'unsigned short' and the data
             * is already in the correct format; the device returns data only for the
             * channels requested, from startChannel to endChannel; AIOUSB_GetScan()
             * returns the averaged data readings in counts[], putting the reading for
             * startChannel in counts[0], and the reading for endChannel in counts[numChannels-1]
             */
            result = AIOUSB_SUCCESS;
            samplesToAverage = discardFirstSample ? samplesPerChannel - 1 : samplesPerChannel;
            sampleIndex = 0;

            for(int channel = 0; channel < numChannels; channel++) {
                unsigned long sampleSum = 0;
                if (discardFirstSample)
                    sampleIndex++;                 /* skip over first sample */
                int sample;
                for(sample = 0; sample < samplesToAverage; sample++)
                    sampleSum += sampleBuffer[ sampleIndex++ ];
                counts[ channel ] = ( unsigned short )((sampleSum + samplesToAverage / 2) / samplesToAverage);
            }
        }
    }


    out_freebuf_AIOUSB_GetScan:
        free(sampleBuffer);

    if (configChanged) {
        deviceDesc->cachedConfigBlock = origConfigBlock;
	USBDevicePutADCConfigBlock( usb, &deviceDesc->cachedConfigBlock );
    }

 out_AIOUSB_GetScan:

    return result;
}

/*--------------------------------------------------------------------------*/
AIORET_TYPE adc_get_bulk_data( ADCConfigBlock *config,
                               USBDevice *usb,
                               unsigned char endpoint,
                               unsigned char *data,
                               int datasize,
                               int *bytes,
                               unsigned timeout
                               )
{
    AIORET_TYPE usbresult;

    usbresult = usb->usb_bulk_transfer( usb,
                                        0x86,
                                        data,
                                        datasize,
                                        bytes,
                                        timeout
                                        );

    return usbresult;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief
 * @param DeviceIndex
 * @param startChannel
 * @param numChannels
 * @param counts
 * @param volts
 * @return
 */
PRIVATE unsigned long AIOUSB_ArrayCountsToVolts(
                                                unsigned long DeviceIndex,
                                                int startChannel,
                                                int numChannels,
                                                const unsigned short counts[],
                                                double volts[]
                                                )
{
    AIORESULT result;
    AIO_ASSERT( volts );
    AIO_ASSERT( counts );
    AIO_ASSERT( startChannel >= 0 );

    AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_PARAMETER, startChannel + numChannels <= ( int )deviceDesc->ADCMUXChannels );

    result = ReadConfigBlock(DeviceIndex, AIOUSB_FALSE);
    if (result == AIOUSB_SUCCESS) {
        for(int channel = 0; channel < numChannels; channel++) {
            int gainCode = ADCConfigBlockGetGainCode( &deviceDesc->cachedConfigBlock , startChannel + channel );
            const struct ADRange *const range = &adRanges[ gainCode ];
            volts[ channel ] = ( (( double )counts[ channel ] / ( double )AI_16_MAX_COUNTS) * range->range ) + range->minVolts;
        }
    }

     return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param startChannel
 * @param numChannels
 * @param volts
 * @param counts
 * @return
 */
PRIVATE AIORET_TYPE AIOUSB_ArrayVoltsToCounts(
                                              unsigned long DeviceIndex,
                                              int startChannel,
                                              int numChannels,
                                              const double volts[],
                                              unsigned short counts[]
                                              )
{
    AIO_ASSERT( volts );
    AIO_ASSERT( counts );
    AIO_ASSERT( startChannel >= 0 );
    AIO_ASSERT( numChannels >= 0 );


    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, (AIORESULT*)&retval );
    AIO_ERROR_VALID_DATA( retval , retval == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_MUX_SETTING, (int)deviceDesc->ADCMUXChannels >= startChannel + numChannels );


    retval = ReadConfigBlock(DeviceIndex, AIOUSB_FALSE);
    AIO_ERROR_VALID_DATA( retval , retval == AIOUSB_SUCCESS );

    int channel;
    for(channel = 0; channel < numChannels; channel++) {
        int gainCode = AIOUSB_GetGainCode(&deviceDesc->cachedConfigBlock, startChannel + channel);

        AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_GAINCODE, VALID_ENUM( ADGainCode, gainCode )  );

        struct ADRange * range = &adRanges[ gainCode ];
        int rawCounts = round(( double )AI_16_MAX_COUNTS * (volts[ channel ] - range->minVolts)/ range->range );

        if (rawCounts < 0)
            rawCounts = 0;
        else if (rawCounts > AI_16_MAX_COUNTS)
            rawCounts = AI_16_MAX_COUNTS;
        counts[ channel ] = ( unsigned short )rawCounts;
    }

    return retval;
}

/*------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param channel
 * @param volts
 * @return
 */
unsigned short AIOUSB_VoltsToCounts(
    unsigned long DeviceIndex,
    unsigned channel,
    double volts
    )
{
    unsigned short counts;

    if(AIOUSB_ArrayVoltsToCounts(DeviceIndex, channel, 1, &volts, &counts) != AIOUSB_SUCCESS)
        counts = 0;
    return counts;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Read one voltage input’s current value
 *
 * @param DeviceIndex DeviceIndex of the card you wish to query;
 *        generally either diOnly or a specific device’s Device Index.
 *
 * @param ChannelIndex number indicating which channel's data you wish to get
 *
 * @param singlevoltage a pointer to a double precision IEEE floating
 *        point num ber which will receive the value read
 *
 * @note This is a slow function
 * @return
 */
AIORET_TYPE ADC_GetChannelV(unsigned long DeviceIndex, unsigned long ChannelIndex, double *singlevoltage )
{
    AIO_ASSERT( singlevoltage );
    AIORET_TYPE result = AIOUSB_SUCCESS;
    unsigned short counts;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, (AIORESULT*)&result );

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, deviceDesc->bADCStream == AIOUSB_TRUE );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_PARAMETER, ChannelIndex < deviceDesc->ADCMUXChannels );

    /**
     * there is no guarantee that ChannelIndex, passed by the user, is
     * within the current channel scan range; if it is not, then valid
     * data cannot be returned; in addition, since we're only returning
     * the data for a single channel, there's no need to scan all the
     * channels; the Windows implementation attempts to improve
     * performance by caching all the values read; but the technique is
     * riddled with problems; first of all, it can easily return extremely
     * stale data, without any indication to the user; secondly, it can
     * return data for channels that weren't even scanned, without any
     * indication to the user; thirdly, caching is unnecessary; if the
     * user wants to read a single channel they can call
     * ADC_GetChannelV(); if the user wants to improve performance by
     * reading multiple channels they can call ADC_GetScanV(); so to
     * address all these issues, we temporarily compress the scan range to
     * just ChannelIndex and then restore it when we're done; so in this
     * implementation all calls to ADC_GetChannelV() return "real-time"
     * data for the specified channel
     */

    result = ReadConfigBlock(DeviceIndex, AIOUSB_FALSE);
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    ADConfigBlock origConfigBlock = deviceDesc->cachedConfigBlock;
    AIOUSB_SetScanRange(&deviceDesc->cachedConfigBlock, ChannelIndex, ChannelIndex);

    result = WriteConfigBlock(DeviceIndex);

    result = AIOUSB_GetScan(DeviceIndex, &counts);

    if (result >= AIOUSB_SUCCESS) {
        double volts;
        result = AIOUSB_ArrayCountsToVolts(DeviceIndex, ChannelIndex, 1, &counts, &volts);
        if (result == AIOUSB_SUCCESS)
            *singlevoltage = volts;
        else
            *singlevoltage = 0.0;
    }

    deviceDesc->cachedConfigBlock = origConfigBlock;
    WriteConfigBlock(DeviceIndex);

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Preferred way to get immediate scan readings. Will Scan all channels ( ie vectored )
 *       perform averaging and culling of data.
 * @param DeviceIndex
 * @param pBuf
 * @return
 */
AIORET_TYPE ADC_GetScanV( unsigned long DeviceIndex, double *pBuf )
{
    AIO_ASSERT( pBuf );
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, deviceDesc->bADCStream );

    /**
     * get raw A/D counts
     */
    unsigned short *counts = ( unsigned short* )calloc(1,deviceDesc->ADCMUXChannels * sizeof(unsigned short));
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_ENOUGH_MEMORY, counts );

    result = ADC_GetScan(DeviceIndex, counts);
    AIO_ERROR_VALID_DATA_W_CODE( result, free(counts),  result == AIOUSB_SUCCESS );

    /**
     * Convert from A/D counts to volts; only
     * the channels from startChannel to
     * endChannel contain valid data, so we
     * only convert those; pBuf[] is expected
     * to contain entries for all the A/D
     * channels; so for cleanliness, we zero
     * out the channels in pBuf[] that aren't
     * going to be filled in with real readings
     */
    unsigned startChannel  = ADCConfigBlockGetStartChannel( &deviceDesc->cachedConfigBlock );
    unsigned endChannel    = ADCConfigBlockGetEndChannel( &deviceDesc->cachedConfigBlock );

    /**
     * convert remaining channels to volts
     */
    result = AIOUSB_ArrayCountsToVolts(DeviceIndex, startChannel, endChannel - startChannel + 1,
                                       counts + startChannel, pBuf + startChannel);

    free(counts);
    return result;
}

/**
 * @brief This simple function takes one scan of A/D data, in counts.
 * @param DeviceIndex DeviceIndex of the card you wish to query; generally either diOnly or a specific
 *        device’s Device Index.
 * @param pBuf Pointer to an array of W ORDs. Each elem ent in the
 * array will receive the value read from the corresponding A/D input
 * channel. The array m ust be at least as large as the num ber of A/D
 * input channels your product contains (16, 32, 64, 96, or 128) - but it
 * is safe to always pass a pointer to an array of 128 W ORDs.  Only elem
 * ents in the array corresponding to A/D channels actually acquired
 * during the scan will be updated: start-channel through end-channel,
 * inclusive. Other values will rem ain unchanged.
 *
 * @return AIORET_TYPE  either AIOUSB_SUCCESS or a failure
 */
AIORET_TYPE ADC_GetScan( unsigned long DeviceIndex,unsigned short *pBuf )
{
    unsigned startChannel;
    AIORET_TYPE result;
    AIORESULT res;
    AIOUSBDevice * deviceDesc;

    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_PARAMETER, pBuf );
    deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, (AIORESULT*)&res );
    AIO_ERROR_VALID_DATA( res , res == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, deviceDesc->bADCStream == AIOUSB_TRUE );

    /**
     * pBuf[] is expected to contain entries for all the A/D channels,
     * even though we may be reading only a few channels; so for
     * cleanliness, we zero out the channels in pBuf[] that aren't
     * going to be filled in with real readings
     */

    startChannel = AIOUSB_GetStartChannel(&deviceDesc->cachedConfigBlock);

    result = AIOUSB_GetScan(DeviceIndex, pBuf + startChannel);

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Determ ine inform ation about the device found at a specific DeviceIndex
 * @param DeviceIndex DeviceIndex of the card you wish to query;
 * generally either diOnly or a specific device’s Device Index.
 * @param ConfigBuf a pointer to the first of an array of bytes for configuration data
 * @param ConfigBufSize a pointer to a variable holding the num ber of
 * configuration bytes to read. W ill be set to the num ber of
 * configuration bytes read
 * @return
 */
unsigned long ADC_GetConfig(
                            unsigned long DeviceIndex,
                            unsigned char *ConfigBuf,
                            unsigned long *ConfigBufSize
                            )
{
    AIORESULT result;
    AIO_ASSERT( ConfigBuf );
    AIO_ASSERT( ConfigBufSize );

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_ENOUGH_MEMORY , *ConfigBufSize >= deviceDesc->ConfigBytes );

    result = ReadConfigBlock(DeviceIndex, AIOUSB_TRUE);
    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_SIZE, deviceDesc->cachedConfigBlock.size > 0 );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_SIZE, deviceDesc->cachedConfigBlock.size <= AD_MAX_CONFIG_REGISTERS );

    memcpy(ConfigBuf, deviceDesc->cachedConfigBlock.registers, deviceDesc->cachedConfigBlock.size);
    *ConfigBufSize = deviceDesc->cachedConfigBlock.size;

    return result;
}

int adcblock_valid_trigger_settings(ADConfigBlock *config )
{
     return (config->registers[ AD_CONFIG_TRIG_COUNT ] & ~AD_TRIGGER_VALID_MASK ) == 0;
}

int adcblock_valid_channel_settings(ADConfigBlock *config , int ADCMUXChannels )
{
     int result = 1;

     for(int channel = 0; channel < AD_NUM_GAIN_CODE_REGISTERS; channel++) {
          if (( config->registers[ AD_CONFIG_GAIN_CODE + channel ] & ~( unsigned char )(AD_DIFFERENTIAL_MODE | AD_GAIN_CODE_MASK)) != 0 ) {
               return 0;
          }
     }

     unsigned endChannel = AIOUSB_GetEndChannel( config );
     if ( endChannel >= ( unsigned )ADCMUXChannels || AIOUSB_GetStartChannel( config ) > endChannel ) {
          result = AIOUSB_ERROR_INVALID_PARAMETER;
     }

     return result;
}

unsigned long
valid_config_block( ADConfigBlock *config )
{
     AIORESULT result = 0;
     return result;
}

int
adcblock_valid_size( ADConfigBlock *config )
{
     return config->size > 0 && config->size <= AD_MAX_CONFIG_REGISTERS;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param pConfigBuf
 * @param ConfigBufSize
 *
 * @return
 */
unsigned long ADC_SetConfig(
                            unsigned long DeviceIndex,
                            unsigned char *pConfigBuf,
                            unsigned long *ConfigBufSize
                            )
{

     unsigned endChannel;
     AIORESULT result;
     int retval;
     if ( !pConfigBuf || !ConfigBufSize )
          return AIOUSB_ERROR_INVALID_PARAMETER;

     ADCConfigBlock configBlock = {0};
     AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
     if ( result  != AIOUSB_SUCCESS )
         return result;
     USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
     if ( result  != AIOUSB_SUCCESS )
         return result;

     memcpy( &configBlock.registers, pConfigBuf, *ConfigBufSize );
     configBlock.timeout  = deviceDesc->commTimeout;
     configBlock.size     = *ConfigBufSize;

     /**
      * validate settings
      */
     if (*ConfigBufSize < deviceDesc->ConfigBytes) {
         *ConfigBufSize = deviceDesc->ConfigBytes;
         return AIOUSB_ERROR_INVALID_PARAMETER;
     }

     if ( !adcblock_valid_channel_settings( &configBlock , deviceDesc->ADCMUXChannels )  ) {
          result = AIOUSB_ERROR_INVALID_ADCCONFIG_CHANNEL_SETTING;
          goto out_ADC_SetConfig;
     }

     if ( configBlock.registers[ AD_CONFIG_CAL_MODE ] >= 8 ) {
          result = AIOUSB_ERROR_INVALID_ADCCONFIG_CAL_SETTING;
          goto out_ADC_SetConfig;
     }

     if ( !adcblock_valid_trigger_settings( &configBlock ) )  {
          result = AIOUSB_ERROR_INVALID_ADCCONFIG_SETTING;
          goto out_ADC_SetConfig;
     }

     endChannel = AIOUSB_GetEndChannel( &configBlock );
     if ( endChannel >= ( unsigned )deviceDesc->ADCMUXChannels ||
         AIOUSB_GetStartChannel(&configBlock) > endChannel ) {
          result = AIOUSB_ERROR_INVALID_PARAMETER;
          goto out_ADC_SetConfig;
     }



     retval = usb->usb_put_config( usb, &configBlock );
     result = retval >= 0 ? AIOUSB_SUCCESS : abs(retval);
out_ADC_SetConfig:
     return result;
}


/*----------------------------------------------------------------------------*/
/**
 * @brief Copies the given ADConfig object into the cachedConfigBlock
 * that is used to communicate with the USB device
 * @param DeviceIndex
 * @param config
 *
 * @return
 */
unsigned long ADC_CopyConfig(
    unsigned long DeviceIndex,
    ADConfigBlock *config
    )
{

     AIORESULT result;
     if ( config== NULL )
          return AIOUSB_ERROR_INVALID_PARAMETER;

     AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
     if ( result  != AIOUSB_SUCCESS )
         return result;

     if (deviceDesc->ConfigBytes == 0) {
          result =  AIOUSB_ERROR_NOT_SUPPORTED;
          goto out_ADC_CopyConfig;
     }

     /**
      * validate settings
      */
     if ( config->size < deviceDesc->ConfigBytes) {
          config->size = deviceDesc->ConfigBytes;
          result =  AIOUSB_ERROR_INVALID_PARAMETER;
          goto out_ADC_CopyConfig;
     }

     if ( ! adcblock_valid_size( config ) ) {
          result = AIOUSB_ERROR_INVALID_ADCCONFIG_CHANNEL_SETTING;
          goto out_ADC_CopyConfig;
     }

     if ( !adcblock_valid_channel_settings( config  , deviceDesc->ADCMUXChannels )  ) {
          result = AIOUSB_ERROR_INVALID_ADCCONFIG_CHANNEL_SETTING;
          goto out_ADC_CopyConfig;
     }

     if ( !VALID_ENUM( ADCalMode , config->registers[ AD_CONFIG_CAL_MODE ] ) ) {
          result = AIOUSB_ERROR_INVALID_ADCCONFIG_CAL_SETTING;
          goto out_ADC_CopyConfig;
     }

     if ( !adcblock_valid_trigger_settings( config ) )  {
          result = AIOUSB_ERROR_INVALID_ADCCONFIG_SETTING;
          goto out_ADC_CopyConfig;
     }

     deviceDesc->cachedConfigBlock = *config;


     result = WriteConfigBlock(DeviceIndex);

     if (result == AIOUSB_SUCCESS)
          deviceDesc->cachedConfigBlock.size = config->size;

out_ADC_CopyConfig:

     return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param pGainCodes
 * @param bSingleEnded
 * @return
 */
unsigned long ADC_RangeAll(
                           unsigned long DeviceIndex,
                           unsigned char *pGainCodes,
                           unsigned long bSingleEnded
                           )
{
    unsigned channel;
    AIORESULT result = AIOUSB_SUCCESS;

    if ( !pGainCodes  || ( bSingleEnded != AIOUSB_FALSE && bSingleEnded != AIOUSB_TRUE ))
        return AIOUSB_ERROR_INVALID_PARAMETER;

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;
    if ( deviceDesc->ADCChannels == 0 || deviceDesc->bADCStream == AIOUSB_FALSE )
        return AIOUSB_ERROR_NOT_SUPPORTED;

    /*
     * validate gain codes; they should be just gain codes; single-ended or differential
     * mode is specified by bSingleEnded
     */

    for(channel = 0; channel < deviceDesc->ADCChannels; channel++) {
        if ((pGainCodes[ AD_CONFIG_GAIN_CODE + channel ] & ~AD_GAIN_CODE_MASK) != 0)
              return AIOUSB_ERROR_INVALID_PARAMETER;
    }

    result = ReadConfigBlock(DeviceIndex, AIOUSB_FALSE);
    if (result == AIOUSB_SUCCESS) {

        for(channel = 0; channel < deviceDesc->ADCChannels; channel++) {
            AIOUSB_SetGainCode(&deviceDesc->cachedConfigBlock, channel, pGainCodes[ channel ]);
            ADCConfigBlockSetDifferentialMode( &deviceDesc->cachedConfigBlock, channel,(bSingleEnded == AIOUSB_FALSE) ? AIOUSB_TRUE : AIOUSB_FALSE);
        }

        result = WriteConfigBlock(DeviceIndex);
    }

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param ADChannel
 * @param GainCode
 * @param bSingleEnded
 * @return
 */
unsigned long ADC_Range1(unsigned long DeviceIndex,
                         unsigned long ADChannel,
                         unsigned char GainCode,
                         unsigned long bSingleEnded
                         )
{
    AIORESULT result;
    AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
    if ( result  != AIOUSB_SUCCESS )
        return result;

    result = AIOUSB_ERROR_NOT_SUPPORTED;
    if (deviceDesc->ADCMUXChannels == 0 || deviceDesc->bADCStream == AIOUSB_FALSE)
        goto out_adc_range1;

    result = AIOUSB_ERROR_INVALID_PARAMETER;
    if (
        (GainCode & ~AD_GAIN_CODE_MASK) != 0 ||
        (
            bSingleEnded != AIOUSB_FALSE &&
            bSingleEnded != AIOUSB_TRUE
        ) ||
        ADChannel >= deviceDesc->ADCMUXChannels
        )
        goto out_adc_range1;


    result = ReadConfigBlock(DeviceIndex, AIOUSB_FALSE);
    if (result == AIOUSB_SUCCESS) {

          AIOUSB_SetGainCode(&deviceDesc->cachedConfigBlock, ADChannel, GainCode);
          ADCConfigBlockSetDifferentialMode(&deviceDesc->cachedConfigBlock, ADChannel,
                                     (bSingleEnded == AIOUSB_FALSE) ? AIOUSB_TRUE : AIOUSB_FALSE);

          result = WriteConfigBlock(DeviceIndex);
      }

out_adc_range1:

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param TriggerMode
 * @param CalMode
 * @return
 */
unsigned long ADC_ADMode(
                         unsigned long DeviceIndex,
                         unsigned char TriggerMode,
                         unsigned char CalMode
                         )
{
    AIORESULT result = AIOUSB_SUCCESS;
    ADCConfigBlock tmpblock = {0};
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_TRIGGER_SETTING, (TriggerMode & ~AD_TRIGGER_VALID_MASK) == 0 );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_CAL_SETTING, VALID_ENUM(ADCalMode , CalMode ));

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_NOT_SUPPORTED, deviceDesc->bADCStream == AIOUSB_TRUE );
    USBDevice *usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_USBDEVICE, usb );

    tmpblock.timeout = deviceDesc->commTimeout;

    ADCConfigBlockCopy( &tmpblock, &deviceDesc->cachedConfigBlock );
    result = USBDeviceFetchADCConfigBlock( usb , &tmpblock );

    AIO_ERROR_VALID_DATA( result, result == AIOUSB_SUCCESS );

    ADCConfigBlockSetCalMode(&tmpblock, (ADCalMode)CalMode);
    ADCConfigBlockSetTriggerMode(&tmpblock, TriggerMode);

    ADCConfigBlockCopy( &deviceDesc->cachedConfigBlock, &tmpblock );

    result = USBDevicePutADCConfigBlock( usb, &tmpblock );

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param Oversample
 * @return
 */
AIORESULT ADC_SetOversample(
                            unsigned long DeviceIndex,
                            unsigned char Oversample
                            )
{
    AIORESULT result;
    AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
    if ( result  != AIOUSB_SUCCESS )
        return result;

    if (result != AIOUSB_SUCCESS || !deviceDesc )
        goto out_ADC_SetOversample;
    else if ( deviceDesc->bADCStream == AIOUSB_FALSE) {
        result = AIOUSB_ERROR_NOT_SUPPORTED;
        goto out_ADC_SetOversample;
    }

    result = ReadConfigBlock(DeviceIndex, AIOUSB_FALSE);
    if ( result != AIOUSB_SUCCESS)
        goto out_ADC_SetOversample;

    ADCConfigBlockSetOversample(&deviceDesc->cachedConfigBlock, Oversample);
    result = WriteConfigBlock(DeviceIndex);

out_ADC_SetOversample:

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @return
 */
unsigned ADC_GetOversample( unsigned long DeviceIndex )
{
    AIORESULT result;
    AIOUSBDevice *deviceDesc =  AIODeviceTableGetDeviceAtIndex( DeviceIndex , &result );
    if ( result  != AIOUSB_SUCCESS )
        return result;

    ReadConfigBlock(DeviceIndex, AIOUSB_FALSE);

    result = (unsigned long)ADCConfigBlockGetOversample(&deviceDesc->cachedConfigBlock );

   return result;
}

/*----------------------------------------------------------------------------*/
AIORESULT ADC_SetAllGainCodeAndDiffMode( unsigned long DeviceIndex, unsigned gain, AIOUSB_BOOL differentialMode )
{
    AIORESULT result;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if (result != AIOUSB_SUCCESS )
        goto out_ADC_SetAllGainCodeAndDiffMode;
    else if ( deviceDesc->bADCStream == AIOUSB_FALSE) {
        result = AIOUSB_ERROR_NOT_SUPPORTED;
        goto out_ADC_SetAllGainCodeAndDiffMode;
    }
    AIOUSB_SetAllGainCodeAndDiffMode( &deviceDesc->cachedConfigBlock, gain, differentialMode );
out_ADC_SetAllGainCodeAndDiffMode:

    return result;
}

/*----------------------------------------------------------------------------*/

/**
 * @brief Returns the maximum clock rate for the product in question and
 *        the number of oversamples + number of channels for the device
 * @param ProductID produc we are looking up
 * @param num_channels  Number of channels we will be sampling on
 * @param num_oversamples  Number of oversamples we will be using
 *
 * @return
 */

PUBLIC_EXTERN AIORESULT ADC_GetMaxClockRate( unsigned long ProductID,
                                             unsigned int num_channels,
                                             unsigned int num_oversamples
                                             )
{
    AIORESULT tmp = ADC_ClockRateForADCProduct( ProductID );
    if ( !tmp ) {
        return tmp;
    }
    return tmp / ( num_channels * ( num_oversamples + 1));
}

PUBLIC_EXTERN AIORESULT ADC_ClockRateForADCProduct( unsigned long ProductID )
{
    switch ( ProductID ) {
	case USB_AIO16_16F:
		return 1000000;
	case USB_AI16_16F:
		return 1000000;
    case USB_AIO16_16A:
        return 500000;
    case USB_AIO16_16E:
        return 250000;
    case USB_AI16_16A:
        return 500000;
    case USB_AI16_16E:
        return 250000;
    case USB_AIO12_16A:
        return 500000;
    case USB_AIO12_16:
        return 250000;
    case USB_AIO12_16E:
        return 100000;
    case USB_AI12_16A:
        return 500000;
    case USB_AI12_16:
        return 250000;
    case USB_AI12_16E:
        return 100000;
    case USB_AIO16_64MA:
        return 500000;
    case USB_AIO16_64ME:
        return 250000;
    case USB_AI16_64MA:
        return 500000;
    case USB_AI16_64ME:
        return 250000;
    case USB_AIO12_64MA:
        return 500000;
    case USB_AIO12_64M:
        return 250000;
    case USB_AIO12_64ME:
        return 100000;
    case USB_AI12_64MA:
        return 500000;
    case USB_AI12_64M:
        return 250000;
    case USB_AI12_64ME:
        return 100000;
    case USB_AIO16_128A:
        return 500000;
    case USB_AIO16_128E:
        return 250000;
    case USB_AI16_128A:
        return 500000;
    case USB_AI16_128E:
        return 250000;
    case USB_AIO16_96A:
        return 500000;
    case USB_AIO16_96E:
        return 250000;
    case USB_AI16_96A:
        return 500000;
    case USB_AI16_96E:
        return 250000;
    case USB_AIO16_64A:
        return 500000;
    case USB_AIO16_64E:
        return 250000;
    case USB_AI16_64A:
        return 500000;
    case USB_AI16_64E:
        return 250000;
    case USB_AIO16_32A:
        return 500000;
    case USB_AIO16_32E:
        return 250000;
    case USB_AI16_32A:
        return 500000;
    case USB_AI16_32E:
        return 250000;
    case USB_AIO12_128A:
        return 500000;
    case USB_AIO12_128:
        return 250000;
    case USB_AIO12_128E:
        return 100000;
    case USB_AI12_128A:
        return 500000;
    case USB_AI12_128:
        return 250000;
    case USB_AI12_128E:
        return 100000;
    case USB_AIO12_96A:
        return 500000;
    case USB_AIO12_96:
        return 250000;
    case USB_AIO12_96E:
        return 100000;
    case USB_AI12_96A:
        return 500000;
    case USB_AI12_96:
        return 250000;
    case USB_AI12_96E:
        return 100000;
    case USB_AIO12_64A:
        return 500000;
    case USB_AIO12_64:
        return 250000;
    case USB_AIO12_64E:
        return 100000;
    case USB_AI12_64A:
        return 500000;
    case USB_AI12_64:
        return 250000;
    case USB_AI12_64E:
        return 100000;
    case USB_AIO12_32A:
        return 500000;
    case USB_AIO12_32:
        return 250000;
    case USB_AIO12_32E:
        return 100000;
    case USB_AI12_32A:
        return 500000;
    case USB_AI12_32:
        return 250000;
    case USB_AI12_32E:
        return 100000;
    default:
        return 0;
    };
}

/*----------------------------------------------------------------------------*/
/**
 * @brief
 * @param DeviceIndex
 * @param StartChannel
 * @param EndChannel
 * @return
 */
unsigned long ADC_SetScanLimits(
                                unsigned long DeviceIndex,
                                unsigned long StartChannel,
                                unsigned long EndChannel
                                )
{
    AIORESULT result;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if (!deviceDesc || deviceDesc->bADCStream == AIOUSB_FALSE) {
        result =  AIOUSB_ERROR_NOT_SUPPORTED;
        goto out_ADC_SetScanLimits;
    }

    if ( EndChannel > deviceDesc->ADCMUXChannels || StartChannel > EndChannel ) {
        result = AIOUSB_ERROR_INVALID_PARAMETER;
        goto out_ADC_SetScanLimits;
    }


    if ( (result = ReadConfigBlock(DeviceIndex, AIOUSB_FALSE)) == AIOUSB_SUCCESS ) {
          AIOUSB_SetScanRange(&deviceDesc->cachedConfigBlock, StartChannel, EndChannel);
          result = WriteConfigBlock(DeviceIndex);
    }

 out_ADC_SetScanLimits:
    return result;
}

/**
 * @brief
 * @param DeviceIndex
 * @param CalFileName
 * @return
 */
unsigned long ADC_SetCal(
                         unsigned long DeviceIndex,
                         const char *CalFileName
                         )
{
    AIO_ASSERT( CalFileName );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_DATA, strlen(CalFileName) > 0 );

    AIORESULT result;
    if (strcmp(CalFileName, ":AUTO:") == 0)
        result = AIOUSB_ADC_InternalCal(DeviceIndex, AIOUSB_TRUE, 0, 0);
    else if (
        strcmp(CalFileName, ":NONE:") == 0 ||
        strcmp(CalFileName, ":1TO1:") == 0
        )
        result = AIOUSB_ADC_InternalCal(DeviceIndex, AIOUSB_FALSE, 0, 0);
    else
        result = AIOUSB_ADC_LoadCalTable(DeviceIndex, CalFileName);

    return result;
}

/*----------------------------------------------------------------------------*/

AIOUSB_BOOL ADC_CanCalibrate( unsigned long productID )
{
    AIOUSB_BOOL retval = AIOUSB_TRUE;
    if ( ((productID & 0xff) >= 0x40 ) && ( (productID & 0xff) <= 0x5D )) {
        retval = ( (productID % 5 == 2 ) || ( productID % 5  == 4 ));
    } else {
        retval = AIOUSB_FALSE;
    }
    return retval;
}


/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @return
 */
unsigned long ADC_QueryCal(
                           unsigned long DeviceIndex
                           )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    AIO_ERROR_VALID_DATA( result , result == AIOUSB_SUCCESS );

    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    unsigned char calSupported = 0xff;       // so we can detect if it changes
    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_READ_FROM_DEVICE,
                                                     AUR_PROBE_CALFEATURE,
                                                     0,
                                                     0,
                                                     &calSupported,
                                                     sizeof(calSupported),
                                                     deviceDesc->commTimeout
                                                     );
    if (bytesTransferred == sizeof(calSupported)) {
        if (calSupported != 0xBB) {             // 0xBB == AUR_LOAD_BULK_CALIBRATION_BLOCK
            result = AIOUSB_ERROR_NOT_SUPPORTED;
        }
    } else {
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    }

    return result;
}



/**
 * @brief Determ ine information about the device found at a specific DeviceIndex
 * @param DeviceIndex DeviceIndex of the card you wish to control;
 *        generally either diOnly or a specific device’s Device Index.
 * @param pConfigBuf A pointer an array of configuration bytes, identical to that
 *        used in ADC_SetConfig()
 * @param ConfigBufSize a pointer to a variable holding the num ber of configuration bytes to write.
 * @param CalFileName the file nam e of a calibration file, or a com m and string.
          See ADC_SetCal() for details.
 * @return AIOUSB_SUCCESS if successful, error otherwise.
 */
unsigned long ADC_Initialize(
    unsigned long DeviceIndex,
    unsigned char *pConfigBuf,
    unsigned long *ConfigBufSize,
    const char *CalFileName
    )
{
    AIORESULT result = AIOUSB_SUCCESS;

    if (
        pConfigBuf != NULL &&
        ConfigBufSize != NULL
        )
        result = ADC_SetConfig(DeviceIndex, pConfigBuf, ConfigBufSize);

    if (
        result == AIOUSB_SUCCESS &&
        CalFileName != NULL
        )
        result = ADC_SetCal(DeviceIndex, CalFileName);

    return result;
}

static void *BulkAcquireWorker(void *params);

/**
 * @brief Determine inform ation about the device found at a specific DeviceIndex
 * @param DeviceIndex DeviceIndex of the card you wish to control; generally either diOnly or a specific
 *        device’s Device Index.
 * @param BufSize the size, in bytes, of the buffer to receive the data
 * @param pBuf a pointer to the buffer in which to receive data
 * @return AIOUSB_SUCCESS indicates success, failure otherwise
 *
 * @note This function will return im m ediately. A return value of
 * AIOUSB_SUCCESS indicates that bulk data is being acquired
 * in the background, and the buffer should not be deallocated or m
 * oved. Use ADC_BulkPoll() to query this background operation.
 */
unsigned long ADC_BulkAcquire(
                              unsigned long DeviceIndex,
                              unsigned long BufSize,
                              void *pBuf
                              )
{

    if ( pBuf == NULL)
        return AIOUSB_ERROR_INVALID_PARAMETER;
    AIORESULT result = AIOUSB_SUCCESS;

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    if (deviceDesc->bADCStream == AIOUSB_FALSE)
        return AIOUSB_ERROR_NOT_SUPPORTED;

    if (deviceDesc->workerBusy)
        return AIOUSB_ERROR_OPEN_FAILED;


    struct BulkAcquireWorkerParams * acquireParams = ( struct BulkAcquireWorkerParams* )malloc(sizeof(struct BulkAcquireWorkerParams));


    if (acquireParams != 0) {
        /**
         * we initialize the worker thread status here in case the thread doesn't start for some reason,
         * such as an improperly locked mutex; this pre-initialization is necessary so that the thread
         * status doesn't make it appear as though the worker thread has completed successfully
         */

        deviceDesc->workerStatus  = BufSize;       // deviceDesc->workerStatus == bytes remaining to receive
        deviceDesc->workerResult  = AIOUSB_ERROR_INVALID_DATA;
        deviceDesc->workerBusy    = AIOUSB_TRUE;

        acquireParams->DeviceIndex  = DeviceIndex;
        acquireParams->BufSize      = BufSize;
        acquireParams->pBuf         = pBuf;

        struct sched_param schedParam = { sched_get_priority_max(SCHED_FIFO) };
        pthread_attr_t workerThreadAttr;
        pthread_t workerThreadID;

        pthread_attr_init(&workerThreadAttr);
        pthread_attr_setschedpolicy(&workerThreadAttr, SCHED_FIFO);
        pthread_attr_setschedparam(&workerThreadAttr, &schedParam);

        int threadResult = pthread_create( &workerThreadID, NULL, BulkAcquireWorker, acquireParams );

        if (threadResult == 0) {
            sched_yield();
            unsigned char startdata[] = {0x05,0x00,0x00,0x00 };
            usb->usb_control_transfer(usb,
                                      USB_WRITE_TO_DEVICE,
                                      AUR_START_ACQUIRING_BLOCK,
                                      (acquireParams->BufSize >> 17) & 0xffff, /* high */
                                      (acquireParams->BufSize >> 1) & 0xffff,
                                      startdata,
                                      sizeof(startdata),
                                      deviceDesc->commTimeout
                                      );



        } else {
            /*
             * failed to create worker thread, clean up
             */
            deviceDesc->workerStatus = 0;
            deviceDesc->workerResult = AIOUSB_SUCCESS;
            deviceDesc->workerBusy = AIOUSB_FALSE;
            free(acquireParams);
            result = AIOUSB_ERROR_INVALID_THREAD;
        }
        pthread_attr_destroy(&workerThreadAttr);
        pthread_detach(workerThreadID);
    } else {
        result = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
    }

    return result;
}

static void *startAcquire(void *params)
{
    static AIORESULT result = AIOUSB_SUCCESS;
    struct BulkAcquireWorkerParams * acquireParams = ( struct BulkAcquireWorkerParams* )params;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( acquireParams->DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return &result;

    double clockHz = deviceDesc->miscClockHz;
    sched_yield();
    result = CTR_StartOutputFreq( acquireParams->DeviceIndex,  0, &clockHz);
    return &result;
}

#define STREAMING_PNA_DEFINITIONS                                       \
    struct timespec foo , bar;                                          \
    unsigned deltas[16*8192];                                           \
    unsigned transactions[16*8192];                                     \
    int tindex = 0;                                                     \
    int num_reads = 0;



/*----------------------------------------------------------------------------*/
/**
 * @brief we assume the parameters passed to BulkAcquireWorker() have
 * been validated by ADC_BulkAcquire()
 * @param params
 * @return
 */
static void *BulkAcquireWorker(void *params)
{
    static unsigned long result = AIOUSB_SUCCESS;
    AIO_ERROR_VALID_DATA_W_CODE( &result, result = AIOUSB_ERROR_INVALID_PARAMETER, params );
    USBDevice *usb;

    struct BulkAcquireWorkerParams * acquireParams = ( struct BulkAcquireWorkerParams* )params;
    int libusbResult;

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( acquireParams->DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return &result;

    usb = AIOUSBDeviceGetUSBHandle( deviceDesc );
    AIO_ERROR_VALID_DATA_W_CODE( &result, result = AIOUSB_ERROR_INVALID_USBDEVICE , usb );


    pthread_t startAcquireThread;
    unsigned long streamingBlockSize , bytesRemaining;
    int threadResult;

#ifdef PNA_TESTING
    STREAMING_PNA_DEFINITIONS;
#endif

    int bytesTransferred;
    unsigned char *data;

    /* Needed to allow us to start bulk acquire waiting before we signal the board to start collecting data */
    threadResult = pthread_create( &startAcquireThread, NULL, startAcquire , params );
    if ( threadResult != 0 ) {
        deviceDesc->workerResult = AIOUSB_ERROR_INVALID_THREAD;
        goto out_BulkAcquireWorker;
    }
    streamingBlockSize = deviceDesc->StreamingBlockSize;

    bytesRemaining = acquireParams->BufSize;
    deviceDesc->workerStatus = bytesRemaining;       // deviceDesc->workerStatus == bytes remaining to receive
    deviceDesc->workerResult = AIOUSB_SUCCESS;
    deviceDesc->workerBusy = AIOUSB_TRUE;

    data = ( unsigned char* )acquireParams->pBuf;

#ifdef PNA_TESTING
    clock_gettime( CLOCK_MONOTONIC_RAW, &bar );
#endif

    while(bytesRemaining > 0) {
        unsigned long bytesToTransfer = (bytesRemaining < streamingBlockSize) ? bytesRemaining : streamingBlockSize;
#ifdef PNA_TESTING
        clock_gettime( CLOCK_MONOTONIC_RAW, &foo );
        deltas[num_reads++] =  ( foo.tv_sec - bar.tv_sec )*1e9 + (foo.tv_nsec - bar.tv_nsec );
#endif

        libusbResult = usb->usb_bulk_transfer(usb,
                                              LIBUSB_ENDPOINT_IN | USB_BULK_READ_ENDPOINT,
                                              data,
                                              (int)bytesToTransfer,
                                              &bytesTransferred,
                                              4000
                                              );

#ifdef PNA_TESTING
        clock_gettime( CLOCK_MONOTONIC_RAW, &bar );
        transactions[tindex++] = bytesTransferred;
        deltas[num_reads++] = ( bar.tv_sec - foo.tv_sec )*1e9 + (bar.tv_nsec - foo.tv_nsec);
#endif
        if (libusbResult != LIBUSB_SUCCESS) {
            result = LIBUSB_RESULT_TO_AIOUSB_RESULT(libusbResult);
            break;
        } else {
            data += bytesTransferred;
            bytesRemaining -= bytesToTransfer; /* Actually read in bytes */
            deviceDesc->workerStatus = bytesRemaining;
        }
    }
#ifdef PNA_TESTING
    FILE *fp = fopen("delays.dat","w");
    FILE *fp2 = fopen("data.dat","w");
    if ( !fp || !fp2 ) {
        printf("Can't open file delays.dat for writing");
    } else {
        for(int i= 0; i < num_reads ; i ++ ) {
            fprintf(fp,"%u\n", deltas[i] );

        }
        for ( int i = 0 ; i < tindex ; i ++ ) {
            fprintf(fp2,"%u\n", transactions[i] );
        }
        fclose(fp2);
        fclose(fp);
    }
#endif

 out_BulkAcquireWorker:
    deviceDesc->workerStatus = 0;
    deviceDesc->workerResult = result;
    deviceDesc->workerBusy = AIOUSB_FALSE;

    double clockHz = 0;
    CTR_StartOutputFreq(acquireParams->DeviceIndex, 0, &clockHz);

    free(params);
    return 0;
}

/**
 * @brief After setting up your oversamples and such, creates a new
 * AIOBuf object that can be used for BulkAcquiring.
 * @param DeviceIndex
 * @return AIOBuf * new Buffer object for BulkAcquire methods
 * @todo Replace 16 with correct channels returned by probing the device
 */
AIOBuf *CreateSmartBuffer( unsigned long DeviceIndex )
{
  AIORESULT result;
  AIODeviceTableGetDeviceAtIndex(DeviceIndex, &result);
  if ( result != AIOUSB_SUCCESS ) {
       aio_errno = -result;
       return (AIOBuf*)NULL;
  }

  long size = ((1 + ADC_GetOversample( DeviceIndex ) ) * 16 * sizeof( unsigned short ) * AIOUSB_GetStreamingBlockSize(DeviceIndex)) ;
  AIOBuf *tmp = NewAIOBuf( AIO_COUNTS_BUF, size );

  return tmp;
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param BytesLeft
 * @return
 */
unsigned long ADC_BulkPoll(
                           unsigned long DeviceIndex,
                           unsigned long *BytesLeft
                           )
{
    if (BytesLeft == NULL)
        return AIOUSB_ERROR_INVALID_PARAMETER;
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS ){
        return result;
    }

    if (deviceDesc->bADCStream == AIOUSB_FALSE)
        return AIOUSB_ERROR_NOT_SUPPORTED;

    *BytesLeft = deviceDesc->workerStatus;
    result = deviceDesc->workerResult;

    return result;
}



/**
 * @brief
 * o this function is erroneously documented in the API specification, but it should
 *   not be made public; it is useful internally, however, for such things as
 *   calibrating the A/D converter
 *
 * o the specification does not include a Channel parameter, but this implementation
 *   does because the Pascal code does and because it's used by ADC_SetCal()
 *
 * o in a departure from the Pascal code, this function supports two categories of
 *   "immediate" A/Ds: the older products which have a single immediate A/D channel,
 *   and the newer products which have multiple immediate A/D channels; fortunately,
 *   this function accepts a pData parameter, which permits us to return any amount of
 *   data; the caller simply has to make sure that his pData buffer is large enough for
 *   the particular device; that's not an unreasonable demand since this function is
 *   used internally and not intended to be public
 */
static unsigned long ADC_GetImmediate(
                                      unsigned long DeviceIndex,
                                      unsigned long Channel,
                                      unsigned short *pData
                                      )
{
    if (pData == NULL)
        return AIOUSB_ERROR_INVALID_PARAMETER;
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;
    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    if (deviceDesc->ImmADCs == 0)
        return AIOUSB_ERROR_NOT_SUPPORTED;

    int numBytes = sizeof(unsigned short) * deviceDesc->ImmADCs;
    int bytesTransferred = usb->usb_control_transfer(usb,
                                                     USB_READ_FROM_DEVICE,
                                                     AUR_ADC_IMMEDIATE,
                                                     0,
                                                     Channel,
                                                     ( unsigned char* )pData,
                                                     numBytes,
                                                     deviceDesc->commTimeout
                                                     );
    if (bytesTransferred != numBytes)
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(bytesTransferred);

    return result;
}


/*----------------------------------------------------------------------------*/
/**
 * @brief Creates FastIT Config Blocks
 * @param DeviceIndex
 * @param size
 * @return
 */
unsigned long ADC_CreateFastITConfig(unsigned long DeviceIndex,
                                     int size
                                     )
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;


    if (!deviceDesc->FastITConfig || deviceDesc->FastITConfig->size <= 0) {
          deviceDesc->FastITConfig = (ADConfigBlock*)malloc(sizeof(ADConfigBlock));
          deviceDesc->FastITBakConfig = (ADConfigBlock*)malloc(sizeof(ADConfigBlock));
          deviceDesc->FastITConfig->size = size;
          deviceDesc->FastITBakConfig->size = size;
      }
    return AIOUSB_SUCCESS;
}


/*----------------------------------------------------------------------------*/
unsigned char *ADC_GetADConfigBlock_Registers(ADConfigBlock *config)
{
    return &(config->registers[0]);
}



/**
 * @brief Frees memory associated with the FastConfig Config blocks. Use
 *       this call after you are done using the ADC_FastIT* Functions
 * @param DeviceIndex
 */
/*----------------------------------------------------------------------------*/
AIORESULT ADC_ClearFastITConfig(unsigned long DeviceIndex)
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    if (deviceDesc->FastITConfig->size) {
          free(deviceDesc->FastITConfig);
          free(deviceDesc->FastITBakConfig);
          deviceDesc->FastITConfig = NULL;
          deviceDesc->FastITBakConfig = NULL;
      }
    return result;
}



/*----------------------------------------------------------------------------*/
unsigned long ADC_CreateADBuf(AIOUSBDevice *const deviceDesc,
                              int size
                              )
{
    deviceDesc->ADBuf = (unsigned char*)malloc(sizeof(unsigned char*) * size);
    if (!deviceDesc->ADBuf) {
          return 0;
      }
    deviceDesc->ADBuf_size = size;
    return AIOUSB_SUCCESS;
}
/*----------------------------------------------------------------------------*/
void ADC_ClearADBuf(AIOUSBDevice *deviceDesc)
{
    if (deviceDesc->ADBuf_size) {
          free(deviceDesc->ADBuf);
          deviceDesc->ADBuf_size = 0;
      }
}
/*----------------------------------------------------------------------------*/
unsigned long ADC_InitFastITScanV(
    unsigned long DeviceIndex
    )
{
    int Dat;
    AIORESULT result;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );

    if ( result != AIOUSB_SUCCESS )
        return result;

    if ((result = ADC_CreateFastITConfig(DeviceIndex, 21)) != AIOUSB_SUCCESS)
        goto CLEAR_CONFIG_ADC_InitFastITScanV;


    if (deviceDesc->FastITConfig->size < 20) {
        result = AIOUSB_ERROR_BAD_TOKEN_TYPE;
        goto RETURN_ADC_InitFastITScanV;
    }

    result = ADC_GetConfig(DeviceIndex, &deviceDesc->FastITBakConfig->registers[0], &deviceDesc->FastITBakConfig->size);

    if (result != AIOUSB_SUCCESS)
        goto RETURN_ADC_InitFastITScanV;

    /* Copy old config */
    memcpy(&deviceDesc->FastITConfig->registers[0], &deviceDesc->FastITBakConfig->registers[0], deviceDesc->FastITBakConfig->size);

    /* Make changes to the config block */
    Dat = AIOUSB_GetRegister(deviceDesc->FastITBakConfig, 0x10);
    AIOUSB_SetRegister(deviceDesc->FastITConfig, 0x11, (0x05 | (Dat & 0x10)));
    Dat = (3 > AIOUSB_GetRegister(deviceDesc->FastITBakConfig, 0x13) ? 3 : AIOUSB_GetRegister(deviceDesc->FastITBakConfig, 0x13));

    AIOUSB_SetRegister(deviceDesc->FastITConfig, 0x13, Dat);
    Dat = 64 > deviceDesc->ADCMUXChannels ?  deviceDesc->ADCMUXChannels - 1  : 63;

    AIOUSB_SetRegister(deviceDesc->FastITConfig, 0x12, Dat << 4);
    AIOUSB_SetRegister(deviceDesc->FastITConfig, 0x14, Dat & 0xF0);


    result = ADC_SetConfig(DeviceIndex, &deviceDesc->FastITConfig->registers[0], &deviceDesc->FastITConfig->size);

    if (result != AIOUSB_SUCCESS) {
          ADC_SetConfig(DeviceIndex, ADC_GetADConfigBlock_Registers(deviceDesc->FastITBakConfig), &deviceDesc->FastITConfig_size);
          return result;
      }

    result = 0;

CLEAR_CONFIG_ADC_InitFastITScanV:
    if (result != AIOUSB_SUCCESS) {
        if (deviceDesc->FastITConfig_size)
            ADC_ClearFastITConfig(DeviceIndex);
    }

RETURN_ADC_InitFastITScanV:

    return result;
}
/*----------------------------------------------------------------------------*/
unsigned long ADC_ResetFastITScanV(
    unsigned long DeviceIndex
    )
{
    AIORESULT result = 0;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;


    if (!deviceDesc->bADCStream || deviceDesc->ConfigBytes < 20) {
        result = AIOUSB_ERROR_BAD_TOKEN_TYPE;
        goto RETURN_ADC_ResetFastITScanV;
    }
    result = ADC_SetConfig(DeviceIndex, ADC_GetADConfigBlock_Registers(deviceDesc->FastITBakConfig), &deviceDesc->ConfigBytes);
    if (result != AIOUSB_SUCCESS)
        goto RETURN_ADC_ResetFastITScanV;

    ADC_ClearFastITConfig(DeviceIndex);

RETURN_ADC_ResetFastITScanV:
    return result;
}
/*----------------------------------------------------------------------------*/
unsigned long ADC_SetFastITScanVChannels(
                                         unsigned long DeviceIndex,
                                         unsigned long NewChannels
                                         )
{
    AIORESULT result = 0;
    ADConfigBlock configBlock;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;


    configBlock.device = deviceDesc;
    configBlock.size = deviceDesc->ConfigBytes;

    result = AIOUSB_EnsureOpen(DeviceIndex);
    if (result != AIOUSB_SUCCESS)
        goto RETURN_ADC_SetFastITScanVChannels;

    if (!deviceDesc->bADCStream) {
          result = AIOUSB_ERROR_BAD_TOKEN_TYPE;
          goto RETURN_ADC_SetFastITScanVChannels;
      }

    if (configBlock.size < 20) {
          result = AIOUSB_ERROR_BAD_TOKEN_TYPE;
          goto RETURN_ADC_SetFastITScanVChannels;
      }

    result = ADC_SetConfig(DeviceIndex, ADC_GetADConfigBlock_Registers(deviceDesc->FastITConfig), &deviceDesc->ConfigBytes);

RETURN_ADC_SetFastITScanVChannels:
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Just a debugging function for listing all attributes of a
 *       config object
 **/
void ADC_Debug_Register_Settings(ADConfigBlock *config)
{
    int i;

    for(i = 0; i <= 15; i++) {
          printf("Channel %d:\t", i);
          switch(config->registers[i]) {
            case FIRST_ENUM(ADGainCode):
                printf("0-10V\n");
                break;

            case AD_GAIN_CODE_10V:
                printf("+/-10V\n");
                break;

            case AD_GAIN_CODE_0_5V:
                printf("0-5V\n");
                break;

            case AD_GAIN_CODE_5V:
                printf("+/-5V\n");
                break;

            case AD_GAIN_CODE_0_2V:
                printf("0-2V\n");
                break;

            case AD_GAIN_CODE_2V:
                printf("+/-2V\n");
                break;

            case AD_GAIN_CODE_0_1V:
                printf("0-1V\n");
                break;

            case AD_GAIN_CODE_1V:
                printf("+/-1V\n");
                break;

            default:
                printf("Unknown\n");
            }
      }

    printf("Calibration Mode:\t");

    switch(config->registers[AD_REGISTER_CAL_MODE]) {
      case AD_CAL_MODE_NORMAL:
          printf("Normal\n");
          break;

      case AD_CAL_MODE_GROUND:
          printf("Ground\n");
          break;

      case AD_CAL_MODE_REFERENCE:
          printf("Reference\n");
          break;

      case AD_CAL_MODE_BIP_GROUND:
          printf("BIP Reference\n");
          break;

      default:
          printf("Unknown\n");
      }

    printf("Trig/Counter clk\t");
    if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_CTR0_EXT) {
          printf("(counter)              ");
      }else {
          printf("(externally triggered) ");
      }

    if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_FALLING_EDGE) {
          printf("(triggered by falling edge) ");
      }

    if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_SCAN) {
          printf("(scan all channels) ");
      }else {
          printf("(single channel) ");
      }

    if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_EXTERNAL) {
          printf("(triggered by external ) ");
      }else if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_TIMER) {
          printf("(triggered by counter 2) ");
      }else
        printf("(triggered by sw       ) ");

    printf("\n");

    /* Display the Start end channels */

    printf("Channels:\tstart=%d, end=%d\n", config->registers[AD_CONFIG_START_END] & 0xF, config->registers[AD_CONFIG_START_END] >> 4);
}
/*----------------------------------------------------------------------------*/
/**
 * @verbatim
 * ---
 * config:
 *   channels:
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   - gain: 0-10V
 *   calibration: Normal
 *   trigger:
 *     edge: falling edge
 *     scan: all channels
 *     type: external
 * @endverbatim
 */
char * ADConfigBlockToYAML(ADConfigBlock *config)
{
    int i;
    char tmpbuf[2048] = {0};
    char tbuf[5];
    strcat(tmpbuf,"---\nconfig:\n");
    strcat(tmpbuf,"  channels:\n");
    for(i = 0; i <= 15; i++) {
        strcat(tmpbuf,"  - gain: ");
        switch(config->registers[i]) {
            case FIRST_ENUM(ADGainCode):
                strcat(tmpbuf, "0-10V\n");
                break;
            case AD_GAIN_CODE_10V:
                strcat(tmpbuf, "+/-10V\n");
                break;
            case AD_GAIN_CODE_0_5V:
                strcat(tmpbuf, "0-5V\n");
                break;
            case AD_GAIN_CODE_5V:
                strcat(tmpbuf, "+/-5V\n");
                break;
            case AD_GAIN_CODE_0_2V:
                strcat(tmpbuf, "0-2V\n");
                break;
            case AD_GAIN_CODE_2V:
                strcat(tmpbuf, "+/-2V\n");
                break;
            case AD_GAIN_CODE_0_1V:
                strcat(tmpbuf, "0-1V\n");
                break;
            case AD_GAIN_CODE_1V:
                strcat(tmpbuf, "+/-1V\n");
                break;
            default:
                strcat(tmpbuf, "Unknown\n");
        }
    }

    strcat(tmpbuf,"  calibration: ");
    switch (config->registers[AD_REGISTER_CAL_MODE] ) {
      case AD_CAL_MODE_NORMAL:
          strcat(tmpbuf,"Normal\n");
          break;

      case AD_CAL_MODE_GROUND:
          strcat(tmpbuf,"Ground\n");
          break;

      case AD_CAL_MODE_REFERENCE:
          strcat(tmpbuf, "Reference\n");
          break;

      case AD_CAL_MODE_BIP_GROUND:
          strcat(tmpbuf,"BIP Reference\n");
          break;

      default:
          strcat(tmpbuf, "Unknown\n");
      }

    strcat(tmpbuf, "  trigger: ");
    if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_CTR0_EXT) {
        strcat(tmpbuf, "counter ");
    } else {
        strcat(tmpbuf, "external ");
    }
    if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_FALLING_EDGE) {
        strcat(tmpbuf, "falling edge");
    } else {
        strcat( tmpbuf, "rising edge" );
    }

    if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_SCAN) {
        strcat(tmpbuf, "all channels");
    } else {
        strcat(tmpbuf, "single channel");
    }
    strcat(tmpbuf,"    type: ");
    if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_EXTERNAL) {
        strcat(tmpbuf,"external\n");
    } else if (config->registers[AD_REGISTER_TRIG_COUNT] & AD_TRIGGER_TIMER) {
        strcat(tmpbuf,"counter\n");
    } else {
        strcat(tmpbuf,"sw\n");
    }
    strcat(tmpbuf,  "start_channel: ");
    sprintf(tbuf,"%d\n", config->registers[AD_CONFIG_START_END] & 0xF );
    strcat(tmpbuf, tbuf );
    strcat(tmpbuf,  "end_channel: ");
    sprintf(tbuf,"%d\n", config->registers[AD_CONFIG_START_END] >> 4 );
    strcat(tmpbuf, tbuf );

    return strdup(tbuf);
}

/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param pData buffer we will write data into
 * @return
 */
unsigned long ADC_GetFastITScanV(unsigned long DeviceIndex, double *pData)
{
    AIORESULT result = 0;

    int StartChannel;
    int EndChannel;
    int Channels;
    unsigned long BytesLeft;
    unsigned short *thisDataBuf;

    int bufsize;
    double clockHz = 0;
    double *pBuf;
    int numsleep = 100;
    double CLOCK_SPEED = 100000;
    int i, ch;

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        goto out_ADC_GetFastITScanV;

    if (!deviceDesc->bADCStream) {
          result = AIOUSB_ERROR_BAD_TOKEN_TYPE;
          goto out_ADC_GetFastITScanV;
      }

    if (deviceDesc->ConfigBytes < 20) {
          result = AIOUSB_ERROR_BAD_TOKEN_TYPE;
          goto out_ADC_GetFastITScanV;
      }

    StartChannel  = AIOUSB_GetRegister(deviceDesc->FastITConfig, 0x12) & 0x0F;
    EndChannel    = AIOUSB_GetRegister(deviceDesc->FastITConfig, 0x12) >> 4;

    if ( deviceDesc->ConfigBytes >= 21 ) {
        StartChannel = StartChannel  | ((AIOUSB_GetRegister(deviceDesc->FastITConfig, 20 ) & 0xF ) << 4);
        EndChannel   = EndChannel    | ( AIOUSB_GetRegister( deviceDesc->FastITConfig, 20 ) & 0xF0 );
    }


    Channels      = EndChannel - StartChannel + 1;

    CTR_8254Mode(DeviceIndex, 0, 2, 0);
    CTR_8254Mode(DeviceIndex, 0, 2, 1);
    bufsize = Channels * sizeof(unsigned short) * (AIOUSB_GetRegister(deviceDesc->FastITConfig, 0x13) + 1); /* 1 sample + 3 oversamples */;

    clockHz = 0;

    ADC_SetScanLimits(DeviceIndex, 0, Channels - 1);

    CLOCK_SPEED = 100000;       // Hz
    AIOUSB_SetStreamingBlockSize(DeviceIndex, 100000);
    thisDataBuf = ( unsigned short* )malloc(bufsize + 100);
    memset(thisDataBuf, 0, bufsize + 100);

    clockHz = 0;
    CTR_StartOutputFreq(DeviceIndex, 0, &clockHz);
    ADC_ADMode(DeviceIndex, AD_TRIGGER_SCAN | AD_TRIGGER_TIMER, AD_CAL_MODE_NORMAL);
    AIOUSB_SetMiscClock(DeviceIndex, CLOCK_SPEED);


    result = ADC_BulkAcquire(DeviceIndex, bufsize, thisDataBuf);

    if (result != AIOUSB_SUCCESS)
        goto CLEANUP_ADC_GetFastITScanV;

    BytesLeft = bufsize;
    numsleep = 0;
    usleep(0);
    while(BytesLeft) {
          result = ADC_BulkPoll(DeviceIndex, &BytesLeft);
          if (result != AIOUSB_SUCCESS) {
                break;
            }else {
                numsleep++;
                usleep(10);
                if (numsleep > 100) {
                      result = AIOUSB_ERROR_TIMEOUT;
                      break;
                  }
            }
      }

    if (result != AIOUSB_SUCCESS)
        goto CLEANUP_ADC_GetFastITScanV;

    pBuf = pData;

    for(i = 0, ch = StartChannel; ch <= EndChannel; i++, ch++) {
          int RangeCode = AIOUSB_GetRegister(deviceDesc->FastITConfig, ch >> deviceDesc->RangeShift);
          int Tot = 0, Wt = 0;
          float V;
          int j;
          int numsamples = AIOUSB_GetRegister(deviceDesc->FastITConfig, 0x13) + 1;

          for(j = 1; j < numsamples; j++) {
                Tot += thisDataBuf[i * (numsamples) + j];
                Wt++;
          }
          V = Tot / Wt / (float)65536;
          if ((RangeCode & 1) != 0)
              V = V * 2 - 1;
          if ((RangeCode & 2) == 0)
              V = V * 2;
          if ((RangeCode & 4) == 0)
              V = V * 5;

          *pBuf = (double)V;
          pBuf++;

          fflush(stdout);
      }

CLEANUP_ADC_GetFastITScanV:
    free(thisDataBuf);

out_ADC_GetFastITScanV:
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief
 * @param DeviceIndex
 * @param pBuf
 * @return
 */
unsigned long ADC_GetITScanV(unsigned long DeviceIndex,
                             double *pBuf
                             )
{
    unsigned result = ADC_InitFastITScanV(DeviceIndex);

    result = ADC_GetFastITScanV(DeviceIndex, pBuf);
    if (result != AIOUSB_SUCCESS)
        result = ADC_ResetFastITScanV(DeviceIndex);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @return
 */
AIOUSB_BOOL AIOUSB_IsDiscardFirstSample(
                                        unsigned long DeviceIndex
                                        )
{
    AIOUSB_BOOL discard = AIOUSB_FALSE;
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

        discard = deviceDesc->discardFirstSample;

    return discard;
}
/*----------------------------------------------------------------------------*/
/**
 * @param DeviceIndex
 * @param discard
 * @return
 */
unsigned long AIOUSB_SetDiscardFirstSample(
                                           unsigned long DeviceIndex,
                                           AIOUSB_BOOL discard
                                           )
{
    AIORESULT result;
    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    deviceDesc->discardFirstSample = discard;

    return result;
}

/*----------------------------------------------------------------------------*/
void AIOUSB_Copy_Config_Block(ADConfigBlock *to, ADConfigBlock *from)
{
    to->device = from->device;
    to->size = from->size;
    memcpy(&to->registers[0], &from->registers[0], to->size);
}

/*----------------------------------------------------------------------------*/
unsigned long AIOUSB_Validate_ADC_Device(unsigned long DeviceIndex)
{
    AIORESULT result;

    if ( (result = AIOUSB_Validate_Device( DeviceIndex ) ) != AIOUSB_SUCCESS )
        goto RETURN_AIOUSB_Validate_ADC_Device;

    result = ADC_QueryCal(DeviceIndex);

RETURN_AIOUSB_Validate_ADC_Device:

    return result;
}

/**
 * @param deviceIndex
 * @return the factory-stored RefV calibration value, if present
 */
/*----------------------------------------------------------------------------*/
double GetHiRef(unsigned long deviceIndex)
{
    unsigned short RefData = 0xFFFF;
    unsigned long DataSize = sizeof(RefData);
    unsigned long Status = GenericVendorRead(deviceIndex, 0xA2, 0x1DF2, 0, &RefData, &DataSize );

    if (Status != AIOUSB_SUCCESS)
        RefData = 0;
    if (DataSize != sizeof(RefData))
        RefData = 0;
    if ((RefData == 0xFFFF) || (RefData == 0x0000))
        RefData =  0;
    return RefData;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Loads the Cal table for Automatic internal calibration
 * @param calTable
 * @param DeviceIndex
 * @param groundCounts
 * @param referenceCounts
 */
void DoLoadCalTable(
                    unsigned short *const calTable,
                    unsigned long DeviceIndex,
                    double groundCounts,
                    double referenceCounts
                    )
{
    const double TARGET_GROUND_COUNTS = 0;      // == 0.0V on 0-10V range (0.0V / 10.0V * 65535 = 0.0)
    const double TARGET_REFERENCE_COUNTS = GetHiRef(DeviceIndex);
    const double slope
        = (TARGET_REFERENCE_COUNTS - TARGET_GROUND_COUNTS)
          / (referenceCounts - groundCounts);
    const double offset = TARGET_GROUND_COUNTS - slope * groundCounts;
    int index;

    for(index = 0; index < CAL_TABLE_WORDS; index++) {
          long value = ( long )round(slope * index + offset);
          if (value < 0)
              value = 0;
          else if (value > AI_16_MAX_COUNTS)
              value = AI_16_MAX_COUNTS;
          calTable[ index ] = ( unsigned short )value;
      }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief
 * @param config
 * @param channel
 * @param gainCode
 */
AIORESULT AIOUSB_SetRangeSingle(ADConfigBlock *config, unsigned long channel, unsigned char gainCode)
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    if ( !config || !config->size || gainCode > LAST_ENUM( ADGainCode ) )
        return AIOUSB_ERROR_INVALID_PARAMETER;

    AIOUSBDevice *deviceDesc = ADCConfigBlockGetAIOUSBDevice( config, &retval );
    if ( retval != AIOUSB_SUCCESS )
        return (AIORESULT)retval;

    if ( channel < AD_MAX_CHANNELS && channel < deviceDesc->ADCMUXChannels ) {

        if ( !deviceDesc->ADCChannelsPerGroup )
            return AIOUSB_ERROR_INVALID_CHANNELS_PER_GROUP_SETTING;

        int reg = AD_CONFIG_GAIN_CODE + channel / deviceDesc->ADCChannelsPerGroup;

        if ( reg > AD_NUM_GAIN_CODE_REGISTERS )
            return AIOUSB_ERROR_INVALID_ADCCONFIG_REGISTER_SETTING;
        config->registers[ reg ] = gainCode;
    }

    return result;
}

int dRef = 3;
// code as original designed
AIORET_TYPE ConfigureAndBulkAcquire1( unsigned long DeviceIndex, ADConfigBlock *config )
{
    unsigned short *ADData;
    int addata_num_bytes,  total = 0;
    uint16_t bytesTransferred;
    int numBytes = 256;
    unsigned char bcdata[] = {0x05,0x00,0x00,0x00};
    int libusbresult;
    AIORESULT result;

    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return (AIORET_TYPE)result;

    /* Set Config */
    ADC_SetConfig( DeviceIndex, config->registers, &config->size );

    ADData = (unsigned short *)malloc(sizeof(unsigned short)*numBytes );
    if (!ADData ) {
        return -AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
    }
    addata_num_bytes = numBytes * sizeof(unsigned short);

    /* Write BC data */
    libusbresult = usb->usb_control_transfer( usb, USB_WRITE_TO_DEVICE, 0xBC, 0, numBytes, bcdata, sizeof(bcdata), 1000 );

    /* Get Immediate */
    libusbresult = usb->usb_control_transfer( usb, USB_WRITE_TO_DEVICE, 0xBF, 0, 0, bcdata, 0, 1000 );

    /* Bulk read */
    libusbresult = usb->usb_bulk_transfer( usb,
                                           LIBUSB_ENDPOINT_IN | USB_BULK_READ_ENDPOINT,
                                           (unsigned char *)ADData,
                                           addata_num_bytes,
                                           (int *)&bytesTransferred,
                                           1000 );

    if (libusbresult != LIBUSB_SUCCESS) {
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(libusbresult);
    } else if (bytesTransferred != addata_num_bytes ) {
        result = AIOUSB_ERROR_INVALID_DATA;
    } else {
        for ( int i = 0; i < numBytes; i ++ )
            total += ADData[i];
        result = total / numBytes;
    }

    return (AIORET_TYPE)result;
}

double ConfigureAndBulkAcquire( unsigned long DeviceIndex, ADConfigBlock *config )
{
    uint16_t ADData[256]; // TODO: adjust for low-speed USB packet size support (64 bytes)
    int addata_num_bytes,  total = 0;
    uint16_t bytesTransferred;
    int numSamples = 256;
    unsigned char bcdata[] = {0x05,0x00,0x00,0x00};
    int libusbresult;
    double result;
    AIORESULT code;

    USBDevice *usb = AIODeviceTableGetUSBDeviceAtIndex( DeviceIndex, &code );
    if ( code != AIOUSB_SUCCESS )
        return (double)code;

    ADC_SetConfig( DeviceIndex, config->registers, &config->size );

    addata_num_bytes = numSamples * sizeof(unsigned short);

    /* Write BC data */
    libusbresult = usb->usb_control_transfer( usb, USB_WRITE_TO_DEVICE, 0xBC, 0, numSamples, bcdata, sizeof(bcdata), 1000 );

    /* Get Immediate */
    libusbresult = usb->usb_control_transfer( usb, USB_WRITE_TO_DEVICE, 0xBF, 0, 0, bcdata, 0, 1000 );

    /* Bulk read */
    libusbresult = usb->usb_bulk_transfer( usb,
                                           LIBUSB_ENDPOINT_IN | USB_BULK_READ_ENDPOINT,
                                           (unsigned char *)ADData,
                                           addata_num_bytes,
                                           (int *)&bytesTransferred,
                                           1000 ); 

    if (libusbresult != LIBUSB_SUCCESS) {
        result = LIBUSB_RESULT_TO_AIOUSB_RESULT(libusbresult);
    } else if (bytesTransferred != addata_num_bytes ) {
        result = AIOUSB_ERROR_INVALID_DATA;
    } else {
        for ( int i = 0; i < numSamples; i ++ )
            total += ADData[i];
        result = total / (double)numSamples;
    }

    return result;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Performs automatic calibration of the ADC
 * @param DeviceIndex
 * @param autoCal
 * @param returnCalTable
 * @param saveFileName
 * @return
 */
unsigned long AIOUSB_ADC_InternalCal(
                                     unsigned long DeviceIndex,
                                     AIOUSB_BOOL autoCal,
                                     unsigned short returnCalTable[],
                                     const char *saveFileName
                                     )
{
    ADConfigBlock oConfig, nConfig;

    AIORET_TYPE retval = AIOUSB_SUCCESS;

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, (AIORESULT*)&retval );
    if ( retval != AIOUSB_SUCCESS )
        return retval;

    if (deviceDesc->bADCStream == AIOUSB_FALSE)
        return AIOUSB_ERROR_NOT_SUPPORTED;

    oConfig = deviceDesc->cachedConfigBlock;

    if ((retval = ADC_QueryCal(DeviceIndex)) != AIOUSB_SUCCESS)
        return retval;

    unsigned short * calTable = ( unsigned short* )malloc(CAL_TABLE_WORDS * sizeof(unsigned short));

    if (!calTable) {
        retval = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
        goto out_AIOUSB_ADC_InternalCal;
    }

    retval = ADC_GetConfig( DeviceIndex, oConfig.registers, &oConfig.size );

    if ( autoCal ) {
        double lowRefRef = 0.0 * 6553.6;
        double hiRefRef = 9.9339 * 6553.6;
        double LoRef, HiRef, dRef, ThisRef, LoRead, HiRead, dRead;
        double F;
        /*
         * create calibrated calibration table
         */

        LoRef = lowRefRef;
        HiRef = GetHiRef(DeviceIndex);
        if (HiRef == 0) {
            HiRef = hiRefRef;
            // printf("  -- ERROR: HiRef returned zero\n");
            }
        dRef = HiRef - LoRef;

        memset(nConfig.registers, 0, 21 );
        nConfig.size = 21;
        nConfig.registers[AD_REGISTER_CAL_MODE] = 0x07;
        nConfig.registers[0x00] = AD_GAIN_CODE_10V; // this is bipolar
        nConfig.registers[AD_REGISTER_TRIG_COUNT] = 0x04;   // software start scan mode
        nConfig.registers[AD_REGISTER_START_END] = 0x00;    // just channel "0" (all channels will reprort selected calibration input)
        nConfig.registers[AD_REGISTER_OVERSAMPLE] = 0xff; // TODO: add handling for low-speed (64-byte) packet sizes

        for ( int k = 0;  k <= 1 ; k ++ ) {
            ADC_SetConfig( DeviceIndex, nConfig.registers, &deviceDesc->cachedConfigBlock.size );

            /* Setup 1-to-1 caltable */
            for(int index = 0; index < 65536; index++) // TODO: CAL_TABLE_WORDS used in two ways, fix
                calTable[ index ] = index;
            AIOUSB_ADC_SetCalTable(DeviceIndex, calTable);

            ThisRef = HiRef;
            if (k==0) 
                ThisRef = 0.5 * (ThisRef + 0x10000);
            nConfig.registers[AD_REGISTER_CAL_MODE] |= 2; // cal high reference

            HiRead = ConfigureAndBulkAcquire( DeviceIndex, &nConfig );
            //printf("AIOUSB - HiRef: %X, HiRead: %X, ThisRef: %X, HiRead-ThisRef: %X\n", (int)HiRef, (int)HiRead, (int)ThisRef, (int)(HiRead-ThisRef));
            if (abs(HiRead-ThisRef) > 0x1000)
            {
                retval = AIOUSB_ERROR_INVALID_DATA; 


                goto free_AIOUSB_ADC_InternalCal;
            }
            usleep(10000); // cargo cult copy-pasted from Windows DLL source

            ThisRef = LoRef;
            if (k==0)
                ThisRef = 0.5 * (ThisRef + 0x10000);
            nConfig.registers[AD_REGISTER_CAL_MODE] = nConfig.registers[AD_REGISTER_CAL_MODE] & (~0x02); // cal low reference
            LoRead = ConfigureAndBulkAcquire( DeviceIndex, &nConfig );

            //printf("AIOUSB - LoRef: %f, LoRead: %F, ThisRef: %f, LoRead-ThisRef: %F\n",  LoRef, LoRead, ThisRef, (LoRead-ThisRef));

            if (abs(LoRead - ThisRef) > 0x200)
            {
                retval = AIOUSB_ERROR_INVALID_DATA;


                goto free_AIOUSB_ADC_InternalCal;
            }
            usleep(10000);

            dRead = HiRead - LoRead;

            for( int i = 0, j  = 0; i < 65536; i++) { // TODO: CAL_TABLE_WORDS used in two ways, fix
                F = (double)( i - LoRead ) / dRead;
                F = (double)(lowRefRef + F * dRef);
                if ( k == 0 ) {
                    F = 0.5 * (F + 0x10000 );
                }
                j = round(F);
                if ( j <= 0 )
                    j = 0;
                else if ( j >= 0xffff )
                    j = 0xffff;
                calTable[i] = j;
            }

            AIOUSB_ADC_SetCalTable(DeviceIndex, calTable);
            /* Save caltable to file if specified */
            nConfig.registers[AD_REGISTER_CAL_MODE] = 0x01;
            nConfig.registers[0x00] = AD_GAIN_CODE_0_10V;
        }

    } else {                  /* 1TO1 calibration table */
      /*
       * create default (1:1) calibration table; that is, each output
       * word equals the input word
       */
        ADConfigBlock tmpconfig = deviceDesc->cachedConfigBlock;

        for ( int i = 0; i < CAL_TABLE_WORDS; i ++ ) {
            calTable[i] = i;
        }

        memset(&tmpconfig.registers,0,tmpconfig.size);
        tmpconfig.size = 20;
        tmpconfig.registers[0x10] = 0x05;

        for ( int k = 0; k <= 1 ; k ++ ) {

            retval = ADC_SetConfig( DeviceIndex, tmpconfig.registers, &tmpconfig.size );
            if ( retval != AIOUSB_SUCCESS )
                break;

            retval = AIOUSB_ADC_SetCalTable(DeviceIndex, calTable);
            if ( retval != AIOUSB_SUCCESS )
                break;
            tmpconfig.registers[0x10] = 0x01;
        }
    }



    if (retval == AIOUSB_SUCCESS && autoCal ) {
      /*
       * optionally return calibration table to caller
       */
          if (returnCalTable != 0)
              memcpy(returnCalTable, calTable, CAL_TABLE_WORDS * sizeof(unsigned short));

          /*
           * optionally save calibration table to a file
           */
          if (saveFileName != 0) {
                FILE * calFile = fopen(saveFileName, "w");
                if (calFile != NULL) {
                       size_t wordsWritten = fwrite(calTable, sizeof(unsigned short), CAL_TABLE_WORDS, calFile);
                      fclose(calFile);
                      if (wordsWritten != ( size_t )CAL_TABLE_WORDS) {
                            remove(saveFileName);             // file is likely corrupt or incomplete
                            retval = AIOUSB_ERROR_FILE_NOT_FOUND;
                        }
                  } else
                    retval = AIOUSB_ERROR_FILE_NOT_FOUND;
            }
      }
 free_AIOUSB_ADC_InternalCal:
    free(calTable);

    deviceDesc->cachedConfigBlock = oConfig;
    ADC_SetConfig( DeviceIndex, oConfig.registers, &oConfig.size );

out_AIOUSB_ADC_InternalCal:

    return retval;
}

/*----------------------------------------------------------------------------*/
void AIOUSB_SetRegister(ADConfigBlock *cb, unsigned int Register, unsigned char value)
{
    cb->registers[Register] = value;
}

/*----------------------------------------------------------------------------*/
unsigned char AIOUSB_GetRegister(ADConfigBlock *cb, unsigned int Register)
{
    unsigned char tmpval;

    tmpval = cb->registers[Register];

    return tmpval;
}
/*
 * we have to lock some of these functions because they access the device table; we don't
 * have to lock functions that don't access the device table
 */
/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_SetAllGainCodeAndDiffMode(ADConfigBlock *config, unsigned gainCode, AIOUSB_BOOL differentialMode)
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIO_ASSERT( config );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_SETTING, config->device );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_SETTING, config->size );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_GAINCODE, gainCode >= FIRST_ENUM( ADGainCode) && gainCode <= LAST_ENUM(ADGainCode) );

    if (differentialMode)
        gainCode |= AD_DIFFERENTIAL_MODE;
    unsigned channel;
    for(channel = 0; channel < AD_NUM_GAIN_CODE_REGISTERS; channel++)
        config->registers[ AD_CONFIG_GAIN_CODE + channel ] = gainCode;

    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_GetGainCode(const ADConfigBlock *config, unsigned channel)
{
    AIORET_TYPE retval = FIRST_ENUM(ADGainCode);             // return reasonable value on error
    AIO_ASSERT( config );

    if ( config != 0 && config->device != 0 &&   config->size != 0 ) {
        const AIOUSBDevice *const deviceDesc = ( DeviceDescriptor* )config->device;
        if (channel < AD_MAX_CHANNELS && channel < deviceDesc->ADCMUXChannels) {

            AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_DEVICE_SETTING, deviceDesc->ADCChannelsPerGroup != 0 );

            retval = (config->registers[ AD_CONFIG_GAIN_CODE + channel / deviceDesc->ADCChannelsPerGroup ]
                        & ( unsigned char )AD_GAIN_CODE_MASK
                        );
        }
    }
    return retval;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief
 * @param config
 * @param channel
 * @param gainCode
 */
AIORET_TYPE AIOUSB_SetGainCode(ADConfigBlock *config, unsigned channel, unsigned gainCode)
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_ADCCONFIG, config );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_SETTING, config->device );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_SETTING, config->size );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_GAINCODE, VALID_ENUM(ADGainCode, gainCode));


    AIOUSBDevice * deviceDesc = ( DeviceDescriptor* )config->device;
    if (channel < AD_MAX_CHANNELS && channel < deviceDesc->ADCMUXChannels) {
        AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_DEVICE_SETTING, deviceDesc->ADCChannelsPerGroup );
        int reg = AD_CONFIG_GAIN_CODE + channel / deviceDesc->ADCChannelsPerGroup;
        AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_DEVICE_SETTING, reg < AD_NUM_GAIN_CODE_REGISTERS )

        config->registers[ reg ] = (config->registers[ reg ] & ~( unsigned char )AD_GAIN_CODE_MASK) |
            ( unsigned char )(gainCode & AD_GAIN_CODE_MASK);
    }

    return retval;
}

/*----------------------------------------------------------------------------*/
/**
 * @param config
 * @param channel
 * @return
 */
AIORET_TYPE AIOUSB_IsDifferentialMode(const ADConfigBlock *config, unsigned channel)
{
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_ADCCONFIG, config );
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_ADCCONFIG, config->device && config->size );

    AIORET_TYPE differentialMode = AIOUSB_FALSE;
    AIOUSBDevice *deviceDesc = ( DeviceDescriptor* )config->device;
    if ( channel < AD_MAX_CHANNELS && channel < deviceDesc->ADCMUXChannels ) {

        AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG, deviceDesc->ADCChannelsPerGroup != 0 );
        differentialMode = ( (
                              config->registers[ AD_CONFIG_GAIN_CODE + channel / deviceDesc->ADCChannelsPerGroup ]
                              & ( unsigned char )AD_DIFFERENTIAL_MODE
                              ) != 0
                             ) ? AIOUSB_TRUE : AIOUSB_FALSE;
    }

    return differentialMode;
}
/*----------------------------------------------------------------------------*/
/**
 * @param config
 * @param channel
 * @param differentialMode
 */
AIORET_TYPE AIOUSB_SetDifferentialMode(ADConfigBlock *config, unsigned channel, AIOUSB_BOOL differentialMode)
{
    AIO_ASSERT( config );
    AIO_ERROR_VALID_DATA(AIOUSB_ERROR_INVALID_ADCCONFIG_DEVICE, config->device );
    AIO_ERROR_VALID_DATA(AIOUSB_ERROR_INVALID_ADCCONFIG_SIZE, config->size );
    AIORET_TYPE retval = AIOUSB_SUCCESS;

    AIOUSBDevice *deviceDesc = ( DeviceDescriptor* )config->device;

    if ( channel < AD_MAX_CHANNELS && channel < deviceDesc->ADCMUXChannels ) {

        AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_CHANNEL_SETTING, deviceDesc->ADCChannelsPerGroup != 0 );
        int reg = AD_CONFIG_GAIN_CODE + channel / deviceDesc->ADCChannelsPerGroup;
        AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_REGISTER_SETTING, reg < AD_NUM_GAIN_CODE_REGISTERS );

        if (differentialMode)
            config->registers[ reg ] |= ( unsigned char )AD_DIFFERENTIAL_MODE;
        else
            config->registers[ reg ] &= ~( unsigned char )AD_DIFFERENTIAL_MODE;
    }
    return retval;
}


/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_GetCalMode(const ADConfigBlock *config)
{
    AIO_ASSERT( config );
    AIORET_TYPE retval = AIOUSB_SUCCESS;

    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_DEVICE , config->device );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG_SIZE , config->size );

    retval = config->registers[ AD_CONFIG_CAL_MODE ];

    return retval;
}
/*----------------------------------------------------------------------------*/
/**
 * @param config
 * @param calMode
 */
AIORET_TYPE AIOUSB_SetCalMode(ADConfigBlock *config, unsigned calMode)
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_ADCCONFIG, config );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG, config->device );
    AIO_ERROR_VALID_DATA( AIOUSB_ERROR_INVALID_ADCCONFIG, config->size );

    AIO_ASSERT_RET( AIOUSB_ERROR_INVALID_CALMODE, VALID_ENUM( ADCalMode, calMode ));

    config->registers[ AD_CONFIG_CAL_MODE ] = ( unsigned char )calMode;
    return retval;
}
/*----------------------------------------------------------------------------*/
/**
 * @param config
 * @return
 */
unsigned AIOUSB_GetTriggerMode(const ADConfigBlock *config)
{
    AIO_ASSERT( config );
    unsigned triggerMode = 0;                               // return reasonable value on error
    if (
        config != 0 &&
        config->device != 0 &&
        config->size != 0
        ) {
          triggerMode = config->registers[ AD_CONFIG_TRIG_COUNT ] & ( unsigned char )AD_TRIGGER_VALID_MASK;
      }
    return triggerMode;
}
/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_SetTriggerMode(ADConfigBlock *config, unsigned triggerMode)
{
    AIO_ASSERT(config);
    AIORET_TYPE retval = AIOUSB_SUCCESS;
    if ( config->device != 0      &&
        config->size   != 0      &&
        (triggerMode & ~AD_TRIGGER_VALID_MASK) == 0
        ) {
        config->registers[ AD_CONFIG_TRIG_COUNT ] = ( unsigned char )triggerMode;
    }
    return retval;
}
/*----------------------------------------------------------------------------*/
unsigned AIOUSB_GetStartChannel(const ADConfigBlock *config)
{
    AIO_ASSERT(config != 0);
    unsigned startChannel = 0;                              // return reasonable value on error
    if (
        config != 0 &&
        config->device != 0 &&
        config->size != 0
        ) {
          if (config->size == AD_MUX_CONFIG_REGISTERS)
              startChannel = ((config->registers[ AD_CONFIG_MUX_START_END ] & 0x0f) << 4) | (config->registers[ AD_CONFIG_START_END ] & 0xf);
          else
              startChannel = (config->registers[ AD_CONFIG_START_END ] & 0xf);
      }
    return startChannel;
}

/*----------------------------------------------------------------------------*/
unsigned AIOUSB_GetEndChannel(const ADConfigBlock *config)
{
    AIO_ASSERT(config);
    unsigned endChannel = 0;                                // return reasonable value on error
    if (
        config != 0 &&
        config->device != 0 &&
        config->size != 0
        ) {
          if (config->size == AD_MUX_CONFIG_REGISTERS)
              endChannel
                  = (config->registers[ AD_CONFIG_MUX_START_END ] & 0xf0)
                    | (config->registers[ AD_CONFIG_START_END ] >> 4);
          else
              endChannel = (config->registers[ AD_CONFIG_START_END ] >> 4);
      }
    return endChannel;
}
/*----------------------------------------------------------------------------*/
/**
 * @param config
 * @param startChannel
 * @param endChannel
 */
AIORET_TYPE AIOUSB_SetScanRange(ADConfigBlock *config, unsigned startChannel, unsigned endChannel)
{
    AIORET_TYPE retval = AIOUSB_SUCCESS;

    if ( !config->device || !config->size )  {
        return  -(AIOUSB_ERROR_INVALID_DATA);
    }

    AIOUSBDevice *deviceDesc = ADCConfigBlockGetAIOUSBDevice( config, &retval );
    if ( retval != AIOUSB_SUCCESS )
        return retval;

    if ( endChannel < AD_MAX_CHANNELS && endChannel < deviceDesc->ADCMUXChannels &&
         startChannel <= endChannel ) {
        if ( config->size == AD_MUX_CONFIG_REGISTERS) {
            /*
             * this board has a MUX, so support more channels
             */
            config->registers[ AD_CONFIG_START_END ] = ( unsigned char )((endChannel << 4) | (startChannel & 0x0f));
            config->registers[ AD_CONFIG_MUX_START_END ] = ( unsigned char )((endChannel & 0xf0) | ((startChannel >> 4) & 0x0f));
        } else {
            /*
             * this board doesn't have a MUX, so support base
             * number of channels
             */
            config->registers[ AD_CONFIG_START_END ] = ( unsigned char )((endChannel << 4) | startChannel);
        }
    }

    return retval;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_GetOversample(ADConfigBlock *config)
{
    AIO_ASSERT(config);
    return config->registers[ AD_CONFIG_OVERSAMPLE ];
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOUSB_SetOversample(ADConfigBlock *config, unsigned overSample)
{
    AIO_ASSERT( config );
    config->registers[ AD_CONFIG_OVERSAMPLE ] = ( unsigned char )overSample;
    return AIOUSB_SUCCESS;
}

/*----------------------------------------------------------------------------*/
static int CompareVoltage(const void *p1, const void *p2)
{
    const double voltage1 = *( double* )p1, voltage2 = *( double* )p2;
    if (voltage1 < voltage2)
        return -1;
    else if (voltage1 > voltage2)
        return 1;
    else
        return 0;
}

/*----------------------------------------------------------------------------*/
unsigned long AIOUSB_ADC_ExternalCal(
                                     unsigned long DeviceIndex,
                                     const double points[],
                                     int numPoints,
                                     unsigned short returnCalTable[],
                                     const char *saveFileName
                                     )
{
    AIORESULT result = AIOUSB_SUCCESS;

    if ( points == 0 || numPoints < 2 || numPoints > CAL_TABLE_WORDS )
        return AIOUSB_ERROR_INVALID_PARAMETER;

    int INPUT_COLUMNS = 2, COLUMN_VOLTS = 0, COLUMN_COUNTS = 1;
    int index;
    for(index = 0; index < numPoints; index++) {
          if (
              points[ index * INPUT_COLUMNS + COLUMN_COUNTS ] < 0 ||
              points[ index * INPUT_COLUMNS + COLUMN_COUNTS ] > AI_16_MAX_COUNTS
              ) {
#if defined(DEBUG_EXT_CAL)
                printf("Error: invalid count value at point (%0.3f,%0.3f)\n",
                       points[ index * INPUT_COLUMNS + COLUMN_VOLTS ],
                       points[ index * INPUT_COLUMNS + COLUMN_COUNTS ]);
#endif
                return AIOUSB_ERROR_INVALID_PARAMETER;
            }
      }

    AIOUSBDevice *deviceDesc = AIODeviceTableGetDeviceAtIndex( DeviceIndex, &result );
    if ( result != AIOUSB_SUCCESS )
        return result;

    if (deviceDesc->bADCStream == AIOUSB_FALSE)
        return AIOUSB_ERROR_NOT_SUPPORTED;

    if ((result = ADC_QueryCal(DeviceIndex)) != AIOUSB_SUCCESS)
          return result;


    /**
     * @note
     * @verbatim
     * sort table into ascending order by input voltage; then verify that both the
     * input voltages and the measured counts are unique and uniformly increasing;
     * since the user's points[] array is declared to be 'const' we need to allocate
     * a working table that we can sort; in addition, we want to allocate space for
     * a slope and offset between each pair of points; so while points[] is like a
     * table with numPoints rows and two columns (input voltage, measured counts),
     * the working table effectively has the same number of rows, but four columns
     * (input voltage, measured counts, slope, offset)
     *
     *       points[] format:
     *       +-----------------+       +-----------------+
     *   [0] |  input voltage  |   [1] | measured counts |
     *       |=================|       |=================|
     *   [2] |  input voltage  |   [3] | measured counts |
     *       |=================|       |=================|
     *                            ...
     *       |=================|       |=================|
     * [n-2] |  input voltage  | [n-1] | measured counts |
     *       +-----------------+       +-----------------+
     * 'n' is not numPoints, but numPoints*2
     * @endverbatim
     */
    int WORKING_COLUMNS = 4, COLUMN_SLOPE = 2, COLUMN_OFFSET = 3;
    double *workingPoints = ( double* )malloc(numPoints * WORKING_COLUMNS * sizeof(double));

    if (workingPoints != 0) {
      /*
       * copy user's table to our working table and set slope and offset to valid values
       */
          for(index = 0; index < numPoints; index++) {
                workingPoints[ index * WORKING_COLUMNS + COLUMN_VOLTS ] = points[ index * INPUT_COLUMNS + COLUMN_VOLTS ];
                workingPoints[ index * WORKING_COLUMNS + COLUMN_COUNTS ] = points[ index * INPUT_COLUMNS + COLUMN_COUNTS ];
                workingPoints[ index * WORKING_COLUMNS + COLUMN_SLOPE ] = 1.0;
                workingPoints[ index * WORKING_COLUMNS + COLUMN_OFFSET ] = 0.0;
            }

          /*
           * sort working table in ascending order of input voltage
           */
          qsort(workingPoints, numPoints, WORKING_COLUMNS * sizeof(double), CompareVoltage);

          /*
           * verify that input voltages and measured counts are unique and ascending
           */
          for(index = 1 ; index < numPoints; index++) {
                if (
                    workingPoints[ index * WORKING_COLUMNS + COLUMN_VOLTS ] <=
                    workingPoints[ (index - 1) * WORKING_COLUMNS + COLUMN_VOLTS ] ||
                    workingPoints[ index * WORKING_COLUMNS + COLUMN_COUNTS ] <=
                    workingPoints[ (index - 1) * WORKING_COLUMNS + COLUMN_COUNTS ]
                    ) {
#if defined(DEBUG_EXT_CAL)
                      printf("Error: points (%0.3f,%0.3f) and (%0.3f,%0.3f) are not unique or not increasing\n",
                             workingPoints[ (index - 1) * WORKING_COLUMNS + COLUMN_VOLTS ],
                             workingPoints[ (index - 1) * WORKING_COLUMNS + COLUMN_COUNTS ],
                             workingPoints[ index * WORKING_COLUMNS + COLUMN_VOLTS ],
                             workingPoints[ index * WORKING_COLUMNS + COLUMN_COUNTS ]);
#endif
                      result = AIOUSB_ERROR_INVALID_PARAMETER;
                      break;
                  }
            }

          /**
           * @note if table of calibration points looks good, then proceed to calculate slopes and
           * offsets of line segments between points; we verified that no two points in the
           * table are equal, so we should not get any division by zero errors
           */
          if (result == AIOUSB_SUCCESS) {
            /**
             * @note
             * @verbatim the calibration table really only applies to one range if precision is our
             * objective; therefore, we assume that all the channels are configured for the
             * same range during calibration mode, and that the user is still using the same
             * range now as when they collected the calibration data points; if all these
             * assumptions are correct, then we can use the range setting for channel 0
             *
             * the calculations are based on the following model:
             *   mcounts = icounts x slope + offset
             * where,
             *   mcounts is the measured counts (reported by an uncalibrated A/D)
             *   icounts is the input counts from an external voltage source
             *   slope is the gain error inherent in the A/D and associated circuitry
             *   offset is the offset error inherent in the A/D and associated circuitry
             * to reverse the effect of these slope and offset errors, we use this equation:
             *   ccounts = ( mcounts – offset ) / slope
             * where,
             *   ccounts is the corrected counts
             * we calculate the slope and offset using these equations:
             *   slope = ( mcounts[s] – mcounts[z] ) / ( icounts[m] – icounts[z] )
             *   offset = mcounts[z] – icounts[z] x slope
             * where,
             *   [s] is the reading at "span" (the upper reference point)
             *   [z] is the reading at "zero" (the lower reference point)
             * in the simplest case, we would use merely two points to correct the entire voltage
             * range of the A/D; in such a simple case, the "zero" point would be a point near 0V,
             * and the "span" point would be a point near the top of the voltage range, such as 9.9V;
             * however, since this function is actually calculating a whole bunch of slope/offset
             * correction factors, one between each pair of points, "zero" refers to the lower of
             * two points, and "span" refers to the higher of the two points
             * @endverbatim
             */
                for(index = 1 ; index < numPoints; index++) {
                      double counts0 = AIOUSB_VoltsToCounts(DeviceIndex, 0,           /* channel */
                                                                  workingPoints[ (index - 1) * WORKING_COLUMNS + COLUMN_VOLTS ]),
                                   counts1 = AIOUSB_VoltsToCounts(DeviceIndex, 0,           /* channel */
                                                                  workingPoints[ index * WORKING_COLUMNS + COLUMN_VOLTS ]);
                      double slope
                          = (
                          workingPoints[ index * WORKING_COLUMNS + COLUMN_COUNTS ]
                          - workingPoints[ (index - 1) * WORKING_COLUMNS + COLUMN_COUNTS ]
                          )
                            / (counts1 - counts0);
                      double offset
                          = workingPoints[ (index - 1) * WORKING_COLUMNS + COLUMN_COUNTS ]
                            - (counts0 * slope);
                      if (
                          slope >= 0.1 &&
                          slope <= 10.0 &&
                          offset >= -1000.0 &&
                          offset <= 1000.0
                          ) {
                            workingPoints[ index * WORKING_COLUMNS + COLUMN_SLOPE ] = slope;
                            workingPoints[ index * WORKING_COLUMNS + COLUMN_OFFSET ] = offset;
                        }else {
#if defined(DEBUG_EXT_CAL)
                            printf("Error: slope of %0.3f or offset of %0.3f is outside the allowed limits\n", slope, offset);
#endif
                            result = AIOUSB_ERROR_INVALID_DATA;             // slopes and offsets are way off, abort
                            break;                                // from for()
                        }
                  }
            }

          if (result == AIOUSB_SUCCESS) {
#if defined(DEBUG_EXT_CAL)
                printf(
                    "External Calibration Points\n"
                    "     Input    Measured  Calculated  Calculated\n"
                    "     Volts      Counts       Slope      Offset\n"
                    );
                for(index = 0; index < numPoints; index++) {
                      printf("%10.3f  %10.3f  %10.3f  %10.3f\n",
                             workingPoints[ index * WORKING_COLUMNS + COLUMN_VOLTS ],
                             workingPoints[ index * WORKING_COLUMNS + COLUMN_COUNTS ],
                             workingPoints[ index * WORKING_COLUMNS + COLUMN_SLOPE ],
                             workingPoints[ index * WORKING_COLUMNS + COLUMN_OFFSET ]
                             );
                  }
#endif

/**
 * generate calibration table using the equation
 *   ccounts = ( mcounts – offset ) / slope
 * described above; each slope/offset pair in workingPoints[] describes the line
 * segment running between the _previous_ point and the current one; in addition,
 * the first row in workingPoints[] doesn't contain a valid slope/offset pair
 * because there is no previous point before the first one (!), so we stretch the
 * first line segment (between points 0 and 1) backward to the beginning of the A/D
 * count range; similarly, since the highest calibration point is probably not right
 * at the top of the A/D count range, we stretch the highest line segment (between
 * points n-2 and n-1) up to the top of the A/D count range
 */
                unsigned short *calTable = ( unsigned short* )malloc(CAL_TABLE_WORDS * sizeof(unsigned short));
                if (calTable != 0) {
                      int measCounts = 0;                 // stretch first line segment to bottom of A/D count range
                      for(index = 1 ; index < numPoints; index++) {
                            double slope = workingPoints[ index * WORKING_COLUMNS + COLUMN_SLOPE ],
                                         offset = workingPoints[ index * WORKING_COLUMNS + COLUMN_OFFSET ];
                            int maxSegmentCounts
                                = (index == (numPoints - 1))
                                  ? (CAL_TABLE_WORDS - 1)                 // stretch last line segment to top of A/D count range
                                  : ( int )workingPoints[ index * WORKING_COLUMNS + COLUMN_COUNTS ];
                            for(; measCounts <= maxSegmentCounts; measCounts++) {
                                  int corrCounts = round((measCounts - offset) / slope);
                                  if (corrCounts < 0)
                                      corrCounts = 0;
                                  else if (corrCounts > AI_16_MAX_COUNTS)
                                      corrCounts = AI_16_MAX_COUNTS;
                                  calTable[ measCounts ] = corrCounts;
                              }
                        }

/*
 * optionally return calibration table to caller
 */
                      if (returnCalTable != 0)
                          memcpy(returnCalTable, calTable, CAL_TABLE_WORDS * sizeof(unsigned short));

/*
 * optionally save calibration table to a file
 */
                      if (saveFileName != 0) {
                            FILE *calFile = fopen(saveFileName, "w");
                            if (calFile != NULL) {
                                  size_t wordsWritten = fwrite(calTable, sizeof(unsigned short), CAL_TABLE_WORDS, calFile);
                                  fclose(calFile);
                                  if (wordsWritten != ( size_t )CAL_TABLE_WORDS) {
                                        remove(saveFileName);                 // file is likely corrupt or incomplete
                                        result = AIOUSB_ERROR_FILE_NOT_FOUND;
                                    }
                              }else
                                result = AIOUSB_ERROR_FILE_NOT_FOUND;
                        }

/*
 * finally, send calibration table to device
 */
                      result = AIOUSB_ADC_SetCalTable(DeviceIndex, calTable);

                      free(calTable);
                  }else
                    result = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
            }

          free(workingPoints);
      }else
        result = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;

    return result;
}


#ifdef __cplusplus
}
#endif


