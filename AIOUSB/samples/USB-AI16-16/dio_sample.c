/*
 * sample program to write out the calibration table and then 
 * reload it again, verify that the data is in fact reversed
 */

#include <aiousb.h>
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>
#include <string.h>
#include "aiocommon.h"


AIOUSB_BOOL find_ai_board( AIOUSBDevice *dev ) { 
    if ( dev->ProductID >= USB_AI16_16A && dev->ProductID <= USB_AI12_128E ) { 
        return AIOUSB_TRUE;
    } else if ( dev->ProductID >=  USB_AIO16_16A && dev->ProductID <= USB_AIO12_128E ) {
        return AIOUSB_TRUE;
    } else {
            return AIOUSB_FALSE;
    }
}


int main( int argc, char **argv ) 
{
    struct opts options = AIO_OPTIONS;
    AIORET_TYPE result = AIOUSB_SUCCESS;
    int *indices;
    int num_devices;
    ADCConfigBlock *config;
    AIOUSBDevice *dev;

    process_aio_cmd_line( &options, argc, argv );

    result = AIOUSB_Init();
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error calling AIOUSB_Init(): %d\n", (int)result );
        exit(result );
    }

    AIOUSB_ListDevices();

    process_aio_cmd_line( &options, argc, argv );

    AIOUSB_FindDevices( &indices, &num_devices, find_ai_board );
    
    if( (result = aio_list_devices( &options, indices, num_devices ) != AIOUSB_SUCCESS )) 
        exit(result);

    if ( (config = NewADCConfigBlockFromJSON( options.adcconfig_json )) == NULL )
        exit(AIOUSB_ERROR_INVALID_ADCCONFIG);

    if ( (result = aio_override_adcconfig_settings( config, &options )) != AIOUSB_SUCCESS )
        exit(result);


    /* Save the config for the device index  in question */
    dev = AIODeviceTableGetDeviceAtIndex( options.index , (AIORESULT*)&result );
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error getting device at index %d\n", options.index );
        exit(result);
    }

    unsigned short tmp = 0;
    unsigned short mask = 0xffff; 
    
    /**
     * @brief Make all ports outputs and 0 value as the initial tristate
     */

    printf("Configuring all ports for output\n" );
    result = DIO_Configure( options.index, AIOUSB_FALSE, &mask, &tmp );

    result = DIO_ReadAll( options.index, &tmp );     /* Verify that all the values are initially 0 */
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error performing DIO_ReadAll(): %d\n", (int)result );
        exit(1);
    }
    if ( tmp != 0 )  {
        fprintf(stderr, "Error, by default all ports should have value 0, and not %u\n", tmp );
    } else {
        printf("DIO_ReadAll() gave value %u == %u(expected)\n", tmp, 0 );
    }

    for ( int i = 0 ; i < dev->DIOBytes*8 ; i ++ ) { 
        result = DIO_Write1(options.index,i, (i % 2 == 0 ? AIOUSB_FALSE : AIOUSB_TRUE ));
        if ( result != AIOUSB_SUCCESS ) {
            fprintf(stderr,"Error running on index %d , retval=%d\n", i, (int)result );
            exit(1);
        }
    }
    printf("Successfully performed DIO_Write1()\n");
    tmp = 0;

    result = DIO_ReadAll( options.index, &tmp );
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error performing DIO_ReadAll(): %d\n", (int)result );
    } else if ( tmp != 0xaaaa ) {
        fprintf(stderr,"Values expected from DIO_ReadAll() %#x != %#x\n", 0xaaaa, tmp );
    } else {
        printf("DIO_ReadAll() gave value %#x == %#x(expected)\n", tmp, 0xaaaa );
    }

    return result;
}



