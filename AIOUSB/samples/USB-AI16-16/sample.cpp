#include <aiousb.h>
#include <stdio.h>
#include <unistd.h>

using namespace AIOUSB;

int main( int argc, char **argv ) {
    int CAL_CHANNEL = 5;
    int MAX_CHANNELS = 128;
    int NUM_CHANNELS = 16;
    unsigned short counts[ MAX_CHANNELS ];
    double volts[ MAX_CHANNELS ];
    unsigned char gainCodes[ NUM_CHANNELS ];
    unsigned long result;
    unsigned long deviceMask;
    int MAX_NAME_SIZE = 20;
    char name[ MAX_NAME_SIZE + 2 ];
    unsigned long productID, nameSize, numDIOBytes, numCounters;
    unsigned long deviceIndex = 0;
    bool deviceFound = false;
    ADConfigBlock configBlock = {0};
    char *calibration_type;

    printf("USB-AI16-16A sample program version %s, %s\n"
           "  This program demonstrates controlling a USB-AI16-16A device on\n"
           "  the USB bus. For simplicity, it uses the first such device found\n"
           "  on the bus.\n", 
           AIOUSB_GetVersion(), 
           AIOUSB_GetVersionDate()
           );

    /*
     * MUST call AIOUSB_Init() before any meaningful AIOUSB functions;
     * AIOUSB_GetVersion() above is an exception
     */
    result = AIOUSB_Init();
    if ( result != AIOUSB_SUCCESS ) {
        printf( "AIOUSB_Init() error: %d\n" , (int)result);
        goto out_main;
    }

    deviceMask = GetDevices(); /** call GetDevices() to obtain "list" of devices found on the bus */
    if ( !deviceMask  ) {
        printf( "No ACCES devices found on USB bus\n" );
        goto out_after_init;
    }

    /*
     * at least one ACCES device detected, but we want one of a specific type
     */
    AIOUSB_ListDevices(); /** print list of all devices found on the bus */

    while( deviceMask != 0 ) {
        if( ( deviceMask & 1 ) != 0 ) {
            // found a device, but is it the correct type?
            nameSize = MAX_NAME_SIZE;
            result = QueryDeviceInfo( deviceIndex, &productID, &nameSize, name, &numDIOBytes, &numCounters );
            if( result == AIOUSB_SUCCESS ) {
                if( productID >= USB_AI16_16A && productID <= USB_AIO12_128E ) {
                    deviceFound = true;                    // found a USB-AI16-16A family device
                    break;
                }
            } else
                printf( "Error '%s' querying device at index %lu\n", 
                        AIOUSB_GetResultCodeAsString( result ), 
                        deviceIndex 
                        );
        }
        deviceIndex++;
        deviceMask >>= 1;
    }

    if (!deviceFound ) {
        printf( "Failed to find USB-AI16-16A device\n" );
        goto out_after_init;
    }

    AIOUSB_Reset( deviceIndex );

    /*
     * demonstrate A/D configuration; there are two ways to configure the A/D;
     * one way is to create an ADConfigBlock instance and configure it, and then
     * send the whole thing to the device using ADC_SetConfig(); the other way
     * is to use the discrete API functions such as ADC_SetScanLimits(), which
     * send the new settings to the device immediately; here we demonstrate the
     * ADConfigBlock technique; below we demonstrate use of the discrete functions
     */
    AIOUSB_InitConfigBlock( &configBlock, deviceIndex, AIOUSB_FALSE );

    AIOUSB_SetAllGainCodeAndDiffMode( &configBlock, AD_GAIN_CODE_10V, AIOUSB_FALSE );
    ADCConfigBlockSetCalMode( &configBlock, AD_CAL_MODE_NORMAL );
    ADCConfigBlockSetTriggerMode( &configBlock, 0 );
    AIOUSB_SetScanRange( &configBlock, 2, 13 );
    ADCConfigBlockSetOversample( &configBlock, 0 );

    // memset(configBlock.registers,0,20 );
    // configBlock.registers[0x11] = 0x04;
    // configBlock.registers[0x12] = 0xf0;
    // configBlock.registers[0x13] = 0x0e;

    result = ADC_SetConfig( deviceIndex, configBlock.registers, &configBlock.size );

    if ( result != AIOUSB_SUCCESS ) {
        printf( "Error '%s' setting A/D configuration\n", 
                AIOUSB_GetResultCodeAsString( result ) 
                );
        goto out_after_init;
    }
    printf( "A/D settings successfully configured\n" );

    calibration_type = strdup(":1TO1:");
    //calibration_type = strdup(":AUTO:");

    result = ADC_SetCal( deviceIndex, calibration_type ); /**demonstrate automatic A/D calibration */

    if( result == AIOUSB_SUCCESS ) {
        printf( "Calibration completed successfully\n" );
    } else if (result == AIOUSB_ERROR_NOT_SUPPORTED) {
        printf ("Calibration not supported on this device\n");
    } else {
        printf( "Error '%s' performing A/D calibration (code %lx)\n", AIOUSB_GetResultCodeAsString( result ), result );
        goto out_after_init;
    }

    /*
     * verify that A/D ground calibration is correct
     */
    ADC_SetOversample( deviceIndex, 0 );
    ADC_SetScanLimits( deviceIndex, CAL_CHANNEL, CAL_CHANNEL );
    ADC_ADMode( deviceIndex, 0 , AD_CAL_MODE_GROUND );

    result = ADC_GetScan( deviceIndex, counts );

    if( result == AIOUSB_SUCCESS )
        printf( "Ground counts = %x (should be near 0)\n", counts[ CAL_CHANNEL ] );
    else
        printf( "Error '%s' attempting to read ground counts\n", 
                AIOUSB_GetResultCodeAsString( result ) );

    /*
     * verify that A/D reference calibration is correct
     */
    ADC_ADMode( deviceIndex, 0 , AD_CAL_MODE_REFERENCE );
    result = ADC_GetScan( deviceIndex, counts );
    if( result == AIOUSB_SUCCESS )
        printf( "Reference counts = 0x%x (should be near 0x1750)\n", counts[ CAL_CHANNEL ] );
    else
        printf( "Error '%s' attempting to read reference counts\n", 
                AIOUSB_GetResultCodeAsString( result ) );

    /*
     * demonstrate scanning channels and measuring voltages
     */
    for( int channel = 0; channel < NUM_CHANNELS; channel++ )
        gainCodes[ channel ] = AD_GAIN_CODE_0_10V;

    ADC_RangeAll( deviceIndex, gainCodes, AIOUSB_TRUE );
    ADC_SetOversample( deviceIndex, 10 );
    ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );
    ADC_ADMode( deviceIndex, 0 /* TriggerMode */, AD_CAL_MODE_NORMAL );

    result = ADC_GetScanV( deviceIndex, volts );
    if( result == AIOUSB_SUCCESS ) {
        printf( "Volts read (uncalibrated):\n" );
        for( int channel = 0; channel < NUM_CHANNELS; channel++ )
            printf( "  Channel %2d = %f\n", channel, volts[ channel ] );
    } else
        printf( "Error '%s' performing A/D channel scan\n", AIOUSB_GetResultCodeAsString( result ) );



    result = ADC_SetCal(deviceIndex, ":AUTO:");
    if( result == AIOUSB_SUCCESS ) {
        printf( "Automatic calibration completed successfully\n" );
    } else if (result == AIOUSB_ERROR_NOT_SUPPORTED) {
        printf ("Automatic calibration not supported on this device\n");
    } else {
        printf( "Error '%s' performing automatic A/D calibration (code %lx)\n", AIOUSB_GetResultCodeAsString( result ), result );
        goto out_after_init;
    }

    result = ADC_GetScanV( deviceIndex, volts );
    if( result == AIOUSB_SUCCESS ) {
        printf( "Volts read (calibrated):\n" );
        for( int channel = 0; channel < NUM_CHANNELS; channel++ )
            printf( "  Channel %2d = %f\n", channel, volts[ channel ] );
    } else
        printf( "Error '%s' performing A/D channel scan\n", AIOUSB_GetResultCodeAsString( result ) );


    /*
     * demonstrate reading a single channel in volts
     */
    result = ADC_GetChannelV( deviceIndex, CAL_CHANNEL, &volts[ CAL_CHANNEL ] );
    if( result == AIOUSB_SUCCESS )
        printf( "Volts read from A/D channel %d = %f\n", CAL_CHANNEL, volts[ CAL_CHANNEL ] );
    else
        printf( "Error '%s' reading A/D channel %d\n", 
                AIOUSB_GetResultCodeAsString( result ), 
                CAL_CHANNEL );


/*
 * MUST call AIOUSB_Exit() before program exits,
 * but only if AIOUSB_Init() succeeded
 */
 out_after_init:
    AIOUSB_Exit();

 out_main:
    
    return ( int ) result;
}
