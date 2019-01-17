/**
 * @file   AIOUSB_Core.h
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @ingroup samples
 * @brief  
 *
 * @note All the API functions that DO NOT begin "AIOUSB_" are standard API functions, largely
 * documented in the <a href="USB Software Reference"> http://accesio.com/MANUALS/USB%20Software%20Reference.pdf</a>. 
 * The functions that DO begin with "AIOUSB_" are "extended" API functions added to the Linux
 * implementation. Source code lines in this sample program that are prefixed with the
 * comment highlight calls to the AIOUSB API.
 *
 * @see Compilation 
 * @see CmakeCompilation
 */

/**
 * @ref libusb 
 * LIBUSB (http://www.libusb.org/) must be installed on the Linux box (the AIOUSB code
 * was developed using libusb version 1.0.3). After installing libusb, it may also be
 * necessary to set an environment variable so that the libusb and aiousb header files can
 * be located:
 *
 *     export CPATH=/usr/local/include/libusb-1.0/:/usr/local/include/aiousb/
 *
 * Once libusb is installed properly, it should be possible to compile the sample program
 * using the simple command:
 *
 *
 *     make
 *
 * Alternatively, one can "manually" compile the sample program using the command:
 *
 *     g++ sample.cpp -laiousb -lusb-1.0 -o sample
 *
 * or, to enable debug features
 *
 *     g++ -ggdb sample.cpp -laiousbdbg -lusb-1.0 -o sample
 */

#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "AIOTypes.h"
#include "AIOChannelMask.h"

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
        /* unsigned char outputMask[ MASK_BYTES ]; */
        AIOChannelMask *outputMask;
        /* unsigned char readBuffer[ MAX_DIO_BYTES ]; */
        /* unsigned char writeBuffer[ MAX_DIO_BYTES ]; */
        DIOBuf *readBuffer;
        DIOBuf *writeBuffer;
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
           "USB-DIO-32 sample program \n"
           "  AIOUSB version:'%s',date:'%s'\n"
           "  This program demonstrates communicating with %d USB-DIO-32 devices on\n"
           "  the same USB bus. For simplicity, it uses the first %d such devices\n"
           "  found on the bus.\n",
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
                if( device->productID == USB_DIO_32 ) { // found a USB-DIO-32
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
    device->outputMask = NewAIOChannelMaskFromStr("1111");
    device->readBuffer = NewDIOBuf(4*8);
    device->writeBuffer = NewDIOBuf(4*8);
    /* device->writeBuffer = NewDIOBufFromChar((char *)"    ",4 ); */

    AIOUSB_SetCommTimeout( device->index, 1000 ); /* set timeout for all USB operations */
    /*
     * set all ports to output mode (we could just write "device->outputMask[ 0 ] = 0x0f"
     * here since there are only 4 ports)
     */
    /* for( port = 0; port < device->numDIOBytes; port++ ) */
    /*     DIOBufSetIndex( device->writeBuffer, port, 0x11 * ( port + 1 ) ); */
    /*     device->writeBuffer[ port ] = ; */
    /* for ( port =0 ; port < 0xff ; port ++ ) {  */
    /* device->outputMask[0] = (unsigned char )0xff; */
    AIOChannelMaskSetMaskFromStr(device->outputMask, "1111" ); /* one for each channel that is selectable
                                                                * (a,b,c,d) */


    for( port = 0; port <= 0x7; port ++ ) { 
        DIOBufSetIndex( device->writeBuffer, port, 1 );
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );
        usleep(10000);
    }
    sleep(1);
    for( port = 0x7; port <= 0xf; port ++ ) { 
        DIOBufSetIndex( device->writeBuffer, port, 1 );
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );
        usleep(10000);
    }
    sleep(1);
    for( port = 0x10; port <= 0x17; port ++ ) { 
        DIOBufSetIndex( device->writeBuffer, port, 1 );
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );
        usleep(10000);
    }

    sleep(1);
    for( port = 0x18; port <= 0x1f; port ++ ) { 
        DIOBufSetIndex( device->writeBuffer, port, 1 );
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );
        usleep(10000);
    }

    /* Mask demonstration....*/
    /* ONLY Port D */
    AIOChannelMaskSetMaskFromStr(device->outputMask, "1000" );
    result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );
    sleep(1);
    /* ONLY Port C */
    AIOChannelMaskSetMaskFromStr(device->outputMask, "0100" );
    result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );
    sleep(1);
    /* ONLY Port B */
    AIOChannelMaskSetMaskFromStr(device->outputMask, "0010" );
    result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );
    sleep(1);
    /* ONLY Port A */
    AIOChannelMaskSetMaskFromStr(device->outputMask, "0001" );
    result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );
    sleep(1);
    /* All Channels */
    AIOChannelMaskSetMaskFromStr(device->outputMask, "1111" );
    result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );

    /* Repeated with a more "byte" manipulation approach */
    /* DIOBufReplaceBinString(device->writeBuffer,"10101010000000001111111100000000" ); */
    AIOChannelMaskSetMaskAtIndex( device->outputMask, 0, 0x7 );     

    result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );    

    /* Now change the bits based on a bitstring */
    char tmpbuf[32] = "00000000000000000000000000000000";
    for( int i = 0; i < 32; i ++ ) {
        if( i >= 1 ) {
            tmpbuf[i-1] = '0';
        }
        tmpbuf[i] = '1';
        DIOBufReplaceBinString(device->writeBuffer, tmpbuf );
        result = DIO_Configure( device->index, AIOUSB_FALSE, device->outputMask , device->writeBuffer );    
        usleep(100000);
    }
    /* All Port A */
    DIOBufSetIndex( device->writeBuffer , 0 , 0x0 | ( 1 << 3 | 1 <<2 | 1 )  );
    /* All Port B */
    DIOBufSetIndex( device->writeBuffer , 1 , 0x0 | ( 1 << 3 | 1 <<2 | 1 )  );
    /* All Port C */
    DIOBufSetIndex( device->writeBuffer , 2 , 0x0 | ( 1 << 3 | 1 <<2 | 1 )  );
    /* All Port D */
    DIOBufSetIndex( device->writeBuffer , 3 , 0x0 | ( 1 << 3 | 1 <<2 | 1 )  );
    

    return(0);

    if( result == AIOUSB_SUCCESS )
        printf( "Device at index %d successfully configured\n", device->index );
    else
        printf( "Error '%s' configuring device at index %d\n", AIOUSB_GetResultCodeAsString( result ), device->index );

exit_sample:
    AIOUSB_Exit();

    return ( int ) exit_code;
} 


