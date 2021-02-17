/**
 * @file   AIOUSB_Core.c
 * 
 * @page bulk_acquire_test bulk_aquire_test.c
 *
 * @deprecated This is a Deprecated sample. Please look at burst_test.c, continuous_mode_callback.c
 * or continuous_mode_callback.c
 *
 */

#include <aiousb.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char **argv)
{
    unsigned long deviceMask;
    int MAX_NAME_SIZE = 20;
    char name[MAX_NAME_SIZE + 2];
    unsigned long productID, nameSize, numDIOBytes, numCounters;
    unsigned long deviceIndex = 0;
    AIOUSB_BOOL deviceFound = AIOUSB_FALSE;
    ADConfigBlock configBlock;
    unsigned long result;

    printf("USB-AIx16-16F 1MHz CH0 2 second log to log.csv \n");

    result = AIOUSB_Init();
    if (result != AIOUSB_SUCCESS)
        return result;

    deviceMask = GetDevices();
    if (!deviceMask != 0)
    {
        printf("No ACCES devices found on USB bus\n");
        goto out_sample;
    }else printf("deviceMask == %08lx\n",deviceMask);

    // AIOUSB_ListDevices(); // print list of all devices found on the bus
    while (deviceMask != 0)
    {
        if ((deviceMask & 1) != 0)
        {
            // found a device, but is it the correct type?
            nameSize = MAX_NAME_SIZE;
            result = QueryDeviceInfo(deviceIndex, &productID, &nameSize, name, &numDIOBytes, &numCounters);
            if (result == AIOUSB_SUCCESS)
            {
                if ((productID >= USB_AI16_16A && productID <= USB_AI16_16F) || (productID >= USB_AIO16_16A && productID <= USB_AIO16_16F))
                {
                    deviceFound = AIOUSB_TRUE;
                    printf("Found %s\n", name);
                    break; // from while()
                }
            }
            else
                printf("Error '%s' querying device at index %lu\n",
                       AIOUSB_GetResultCodeAsString(result), deviceIndex);
        }
        deviceIndex++;
        deviceMask >>= 1;
    }

    if (!deviceFound)
    {
        printf("Failed to find USB-AIx16-16F Family device\n");
        goto out_sample;
    }

    /* AIOUSB_Reset( deviceIndex ); */

    result = ADC_SetCal(deviceIndex, ":NONE:");
    printf("ADC_SetCal() returned %ld\n", result);

    double Hz = 1e6;
    int seconds = 2;
    int samples = seconds * Hz;
    int tobufsize = samples * sizeof(double);
    double *tobuf = (double *)malloc(tobufsize + 8);
    double CLOCK_SPEED = MIN(ADC_GetMaxClockRate(deviceTable[deviceIndex].ProductID, 1, 0), Hz);
    if (CLOCK_SPEED == 0)
    {
        fprintf(stderr, "Got incorrect minimum clock speed of 0 for device. It Looks like this ADC device (id=%#X) is not setup for ADC\n", deviceTable[deviceIndex].ProductID);
        exit(1);
    }
    int BULK_BYTES = samples * sizeof(unsigned short);

    AIOUSB_SetCommTimeout(deviceIndex, 1000);
    AIOUSB_SetDiscardFirstSample(deviceIndex, AIOUSB_TRUE);

    uint64_t serialNumber;
    result = GetDeviceSerialNumber(deviceIndex, &serialNumber);
    if (result == AIOUSB_SUCCESS)
        printf("Serial number of device at index %lu: %llx\n", deviceIndex, (long long)serialNumber);
    else
        printf("Error '%s' getting serial number of device at index %lu\n",
               AIOUSB_GetResultCodeAsString(result), deviceIndex);

    AIOUSB_InitConfigBlock(&configBlock, deviceIndex, AIOUSB_FALSE);
    AIOUSB_SetAllGainCodeAndDiffMode(&configBlock, AD_GAIN_CODE_1V, AIOUSB_FALSE);
    ADCConfigBlockSetCalMode(&configBlock, AD_CAL_MODE_NORMAL);
    ADCConfigBlockSetTriggerMode(&configBlock, 0);
    AIOUSB_SetScanRange(&configBlock, 2, 13);
    ADCConfigBlockSetOversample(&configBlock, 0);
    result = ADC_SetConfig(deviceIndex, configBlock.registers, &configBlock.size);
    if (result < AIOUSB_SUCCESS)
    {
        printf("Error '%s' setting A/D configuration\n", AIOUSB_GetResultCodeAsString(result));
        goto out_sample;
    }
    else
        printf("A/D settings successfully configured\n");

    AIOUSB_Reset(deviceIndex);
    ADC_SetOversample(deviceIndex, 0);
    ADC_SetScanLimits(deviceIndex, 0, 0);

    AIOUSB_SetStreamingBlockSize(deviceIndex, 64 * 1024);

    printf("Allocating %d Bytes to acquire %d samples\n", BULK_BYTES, samples);
    unsigned short *dataBuf = (unsigned short *)malloc(BULK_BYTES);
    if (!dataBuf)
    {
        printf("Error: unable to allocate memory\n");
        goto out_sample;
    }

    /** make sure counter is stopped */
    double clockHz = 0;
    CTR_StartOutputFreq(deviceIndex, 0, &clockHz);

    /*
     * configure A/D for timer-triggered acquisition
     */
    ADC_ADMode(deviceIndex, AD_TRIGGER_SCAN | AD_TRIGGER_TIMER, AD_CAL_MODE_NORMAL);

    /*
     * start bulk acquire; ADC_BulkAcquire() will take care of starting
     * and stopping the counter; but we do have to tell it what clock
     * speed to use, which is why we call AIOUSB_SetMiscClock()
     */
    printf("Using Clock speed %d to acquire data\n", (int)CLOCK_SPEED);
    AIOUSB_SetMiscClock(deviceIndex, Hz);
    /* exit(0); */
    result = ADC_BulkAcquire(deviceIndex, BULK_BYTES, dataBuf);

    if (result == AIOUSB_SUCCESS)
        printf("Started bulk acquire of %d samples\n", samples);
    else
        printf("Error '%s' attempting to start bulk acquire of %d bytes\n",
               AIOUSB_GetResultCodeAsString(result),
               BULK_BYTES);
    /*
     * use bulk poll to monitor progress
     */
    if (result == AIOUSB_SUCCESS)
    {
        unsigned long bytesRemaining = BULK_BYTES;
        for (int s = 0; s < 100; s++)
        {
            sleep(1);
            result = ADC_BulkPoll(deviceIndex, &bytesRemaining);
            if (result == AIOUSB_SUCCESS)
            {
                printf("  %lu bytes remaining\n", bytesRemaining);
                if (bytesRemaining == 0)
                    break;
            }
            else
            {
                printf("Error '%s' polling bulk acquire progress\n",
                       AIOUSB_GetResultCodeAsString(result));
                sleep(1);
                break;
            }
        }

        /*
         * turn off timer-triggered mode
         */
        ADC_ADMode(deviceIndex, 0, AD_CAL_MODE_NORMAL);

        printf("logging to log.csv\n");

        FILE *fp = fopen("log.csv", "w");
        if (result == AIOUSB_SUCCESS && bytesRemaining == 0)
        {
            AIOUSB_BOOL anyZeroData = AIOUSB_FALSE;
            int zeroIndex = -1;
            for (int index = 0; index < samples; index++)
            {
                tobuf[index] = 2.0 * ((double)dataBuf[index] / 65536.0) - 1.0;
                fprintf(fp, "%04x,%lf\n", dataBuf[index], tobuf[index]);

                if (!((1 + index) % (samples / 10)))
                    printf("logged %d samples\n", index + 1);
            }
        }
        else
        {
            printf("Failed to bulk acquire %d bytes: %d\n", BULK_BYTES, (int)result);
            result = -3;
        }
        printf("\n");
        fclose(fp);
    }

    free(dataBuf);

    /*
     * MUST call AIOUSB_Exit() before program exits,
     * but only if AIOUSB_Init() succeeded
     */
out_sample:
    AIOUSB_Exit();
}