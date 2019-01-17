/**
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  
 * @see Compilation 
 * @see CmakeCompilation
 */

#include "AIOTypes.h"
#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "AIOChannelMask.h"
#include "aiousb.h"


typedef enum { 
    SUCCESS = 0,
    USB_ERROR = -1,
    NO_DEVICE_FOUND = -2
} EXIT_CODE;

AIOUSB_BOOL  find_dio_96( AIOUSBDevice *dev ) 
{ 
    if ( dev->ProductID == USB_DIO_96 )
        return AIOUSB_TRUE;
    else
        return AIOUSB_FALSE;
}

#undef BITS_PER_BYTE
#define BITS_PER_BYTE 8
#define DEVICES_REQUIRED 1
#define MAX_DIO_BYTES 12
#define MASK_BYTES  (( MAX_DIO_BYTES + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE)
#define MAX_NAME_SIZE 20

typedef struct {
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
}  DeviceInfo;


int main( int argc, char **argv ) {

    int index = 0;
    int devicesFound = 0;
    unsigned port;
    int deviceIndex;
    EXIT_CODE exit_code = SUCCESS;

    DeviceInfo DEVICES[ DEVICES_REQUIRED ];

    DeviceInfo *device;

    printf("USB-DIO-96 sample program version \nAIOUSB library version:'%s' date:'%s'\nThis program demonstrates configuring a USB-DIO-96 device\n",
           AIOUSB_GetVersion(),
           AIOUSB_GetVersionDate()
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

    while( deviceMask != 0 && devicesFound < DEVICES_REQUIRED ) {
        if( ( deviceMask & 1 ) != 0 ) {
            // found a device, but is it the correct type?
            device = &DEVICES[ devicesFound ];
            device->nameSize = MAX_NAME_SIZE;
            result = QueryDeviceInfo( index, &device->productID,
                                      &device->nameSize,
                                      device->name,
                                      &device->numDIOBytes,
                                      &device->numCounters
                                      );
            if( result == AIOUSB_SUCCESS ) {
                if( device->productID == USB_DIO_96 ) { // found a USB-DIO-96
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


   
    for( index = 0; index < devicesFound; index++ ) {
        /* device = &deviceTable[ devicesFound ]; */
        device->nameSize = MAX_NAME_SIZE;
        deviceIndex = index;
        result = QueryDeviceInfo( index, &device->productID,
                                  &device->nameSize,
                                  device->name,
                                  &device->numDIOBytes,
                                  &device->numCounters
                                  );

        device = &DEVICES[ index ];
        memset(device->writeBuffer,0,sizeof(device->writeBuffer) );
        memset(device->readBuffer,0,sizeof(device->readBuffer) );

        result = GetDeviceSerialNumber( device->index, &device->serialNumber );
        if( result == AIOUSB_SUCCESS )
            printf( "Serial number of device at index %d: %llx\n", device->index, ( long long ) device->serialNumber );
        else
            printf( "Error '%s' getting serial number of device at index %d\n", AIOUSB_GetResultCodeAsString( result ), device->index );
    }


    AIOUSB_SetCommTimeout( device->index, 1000 ); /* set timeout for all USB operations */
    /*
     * set all ports to output mode (we could just write "device->outputMask[ 0 ] = 0x0f"
     * here since there are only 4 ports)
     */
    memset( device->outputMask, 0xff, MASK_BYTES );
  
    device->outputMask[0] = (unsigned char )0xff;

    char *cdat = (char *)calloc(1,device->numDIOBytes );
    int retval;
    DIO_Read8( deviceIndex, 0, (int*)cdat );
    char *outmask = (char *)calloc(1, device->numDIOBytes);
    char *trimask = (char *)calloc(1, 2);
    char *pdata   = (char *)calloc(1, device->numDIOBytes);
    memset(outmask, 0xff, device->numDIOBytes );
    memset(trimask, 0x00, 2 );
    memset(cdat,0,device->numDIOBytes );


    retval = DIO_Configure(deviceIndex, AIOUSB_FALSE, outmask, pdata );
    DIO_WriteAll( deviceIndex, &cdat );

    /* Turn all of the ports on */
    for ( int i = 0 ; i < device->numDIOBytes ; i ++ ) { 
        for( port = 0; port <= 0xff; port ++ ) {
            device->writeBuffer[i] = port;
            result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask, device->writeBuffer );
            usleep(100);
        }
    }
    
    for ( int i = 0 ; i < 96 ; i ++ ) { 
        memset(device->writeBuffer, 0, device->numDIOBytes );
        device->writeBuffer[ i / 8 ] |= ( 1 << ( i % 8 ) );
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask, device->writeBuffer );
        usleep(60000);
    }

    if( result == AIOUSB_SUCCESS )
        printf( "Device at index %d successfully configured\n", device->index );
    else
        printf( "Error '%s' configuring device at index %d\n", AIOUSB_GetResultCodeAsString( result ), device->index );


exit_sample:
    AIOUSB_Exit();
    free(cdat);
    free(outmask );
    free(trimask );
    free(pdata   );

    return ( int ) exit_code;
} 

