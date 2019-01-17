/**
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  
 * @see Compilation 
 * @see CmakeCompilation
 */


#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "AIOTypes.h"

int main( int argc, char **argv ) {
    const int DEVICES_REQUIRED = 1;
    const int MAX_DIO_BYTES = 4;
    const int MASK_BYTES = ( MAX_DIO_BYTES + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE;
    const int MAX_NAME_SIZE = 20;
    int devicesFound = 0;
    int index = 0;
    typedef enum { 
      SUCCESS = 0,
      USB_ERROR = -1,
      NO_DEVICE_FOUND = -2
    } EXIT_CODE;
    EXIT_CODE exit_code = SUCCESS;

    struct DeviceInfo {
        unsigned char outputMask[ MASK_BYTES ];
        unsigned char readBuffer[ MAX_DIO_BYTES ];		// image of data read from board
        unsigned char writeBuffer[ MAX_DIO_BYTES ];		// image of data written to board
        char name[ MAX_NAME_SIZE + 2 ];
        unsigned long productID;
        unsigned long nameSize;
        unsigned long numDIOBytes;
        unsigned long numCounters;
        uint64_t serialNumber;
        int index;
    } deviceTable[ DEVICES_REQUIRED ];
    struct DeviceInfo *device;

    printf(
           "USB-DIO-32 sample program\n"
           "AIOUSB version:'%s',date:'%s'\n"
           "This program demonstrates communicating with %d USB-DIO-32 devices on\n"
           "the same USB bus. For simplicity, it uses the first %d such devices\n"
           "found on the bus.\n",
           AIOUSB_GetVersion(),
           AIOUSB_GetVersionDate(),
           DEVICES_REQUIRED,
           DEVICES_REQUIRED
           );

    unsigned long result = AIOUSB_Init(); /* Call AIOUSB_Init() before any meaningful AIOUSB functions; */
    if( result != AIOUSB_SUCCESS ) {
        printf("Can't initialize AIOUSB USB device\n");
        exit_code = USB_ERROR;
    }

    unsigned long deviceMask = GetDevices(); /**< @ref GetDevices */
    if( deviceMask == 0 ) {
        printf( "No ACCES devices found on USB bus\n" );
        exit_code = USB_ERROR;
        goto exit_sample;
    }

    AIOUSB_ListDevices();

    /*
     * search for required number of USB-DIO-32 devices
     */


    while( deviceMask != 0 && devicesFound < DEVICES_REQUIRED ) {
        if( ( deviceMask & 1 ) != 0 ) {
            // found a device, but is it the correct type?
            device = &deviceTable[ devicesFound ];
            device->nameSize = MAX_NAME_SIZE;
            result = QueryDeviceInfo( index, &device->productID,
                                      &device->nameSize,
                                      device->name,
                                      &device->numDIOBytes,
                                      &device->numCounters
                                      );
            if( result == AIOUSB_SUCCESS ) {
                if( device->productID == 0x8004 ) { // found a USB-DIO-32
                    device->index = index;
                    devicesFound++;
                }
            } else
              printf( "Error '%s' querying device at index %d\n",
                      AIOUSB_GetResultCodeAsString( result ),
                      index
                      );
        }
        index++;
        deviceMask >>= 1;
    }

    if( devicesFound < DEVICES_REQUIRED ) {
        printf( "Error: You need at least %d devices connected to run this sample\n", DEVICES_REQUIRED );
        goto exit_sample;
    }
    unsigned port;

    for( index = 0; index < devicesFound; index++ ) {
        device = &deviceTable[ index ];
        result = GetDeviceSerialNumber( device->index, &device->serialNumber );
        if( result == AIOUSB_SUCCESS )
            printf( "Serial number of device at index %d: %llx\n", device->index, ( long long ) device->serialNumber );
        else
            printf( "Error '%s' getting serial number of device at index %d\n", AIOUSB_GetResultCodeAsString( result ), device->index );
    }

    /*
     * DIO configuration
     */
    device = &deviceTable[ 0 ]; /* select first device */
    AIOUSB_SetCommTimeout( device->index, 1000 ); /* set timeout for all USB operations */
    /*
     * set all ports to output mode (we could just write "device->outputMask[ 0 ] = 0x0f"
     * here since there are only 4 ports)
     */
    memset( device->outputMask, 0xff, MASK_BYTES );
    
    for( port = 0; port < device->numDIOBytes; port++ )
        device->writeBuffer[ port ] = 0x11 * ( port + 1 );
    

    /* for ( port =0 ; port < 0xff ; port ++ ) {  */
    device->outputMask[0] = (unsigned char )0xff;
    /* AIOChannelMask *outmask = NewAIOChannelMaskFromStr("11111111"); */

    for( port = 0; port <= 0xff; port ++ ) { 
        device->writeBuffer[0] = port;
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask, device->writeBuffer );
        if ( result < AIOUSB_SUCCESS ) {
            fprintf(stderr,"Error running DIO_Configure: %ld\n", result );
            exit(1);
        }
        usleep(10000);
    }

    for( port = 0; port <= 0xff; port ++ ) { 
        device->writeBuffer[1] = port;
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask, device->writeBuffer );
        if ( result < AIOUSB_SUCCESS ) {
            fprintf(stderr,"Error running DIO_Configure: %ld\n", result );
            exit(1);
        }
        usleep(10000);
    }

    for( port = 0; port <= 0xff; port ++ ) { 
        device->writeBuffer[2] = port;
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask, device->writeBuffer );
        if ( result < AIOUSB_SUCCESS ) {
            fprintf(stderr,"Error running DIO_Configure: %ld\n", result );
            exit(1);
        }
        usleep(10000);
    }

    for( port = 0; port <= 0xff; port ++ ) { 
        device->writeBuffer[3] = port;
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask, device->writeBuffer );
        if ( result < AIOUSB_SUCCESS ) {
            fprintf(stderr,"Error running DIO_Configure: %ld\n", result );
            exit(1);
        }
        usleep(10000);
    }

    for ( port = 0; port < 4 ; port ++ )  {
        device->writeBuffer[port] = 0xff;
    }

    result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask, device->writeBuffer );
    if ( result < AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error running DIO_Configure: %ld\n", result );
        exit(1);
    }

    for ( int i =0 ; i <= 0xf ; i ++ ) { 
        device->outputMask[0] = (unsigned char )i;
        /* AIOChannelMask *outmask = NewAIOChannelMaskFromChr( (char)(unsigned char)i ); */
            
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask, device->writeBuffer );
        if ( result < AIOUSB_SUCCESS ) {
            fprintf(stderr,"Error running DIO_Configure: %ld\n", result );
            exit(1);
        }
        if ( result != AIOUSB_SUCCESS ) 
            break;
        usleep(100000);
    }

    if( result == AIOUSB_SUCCESS )
        printf( "Device at index %d successfully configured\n", device->index );
    else
        printf( "Error '%s' configuring device at index %d\n", AIOUSB_GetResultCodeAsString( result ), device->index );


exit_sample:
    AIOUSB_Exit();


    return ( int ) exit_code;
} 

