/*
 * This source code looks best with a tab width of 4.
 *
 * All the API functions that DO NOT begin "AIOUSB_" are standard API functions, largely
 * documented in http://accesio.com/MANUALS/USB%20Software%20Reference.pdf. The functions
 * that DO begin with "AIOUSB_" are "extended" API functions added to the Linux
 * implementation. Source code lines in this sample program that are prefixed with the
 * comment "/ * API * /" highlight calls to the AIOUSB API.
 *
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
 *     make
 *
 * Alternatively, one can "manually" compile the sample program using the command:
 *
 *     g++ receiver.cpp -laiousb -lusb-1.0 -o receiver
 *
 * or, to enable debug features
 *
 *     g++ -ggdb sample.cpp -laiousbdbg -lusb-1.0 -o sample
 */
// }}}


#include <aiousb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main( int argc, char **argv ) {
    unsigned long result = AIOUSB_SUCCESS;
    double ReadClockHz = 40000, WriteClockHz = 0;

    result = AIOUSB_Init();
    if ( result < AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error running AIOUSB_Init() ...exiting\n");
        return 2;
    }
    AIOUSB_ListDevices();

    if( argc < 3  || argc > 4 ) {
        printf(\
"\nUSB-DIO-16A test program , AIOUSB version:'%s', date:'%s'.\n\
This program demonstrates a simple high speed receiver program\n\
where the device in question performs digital acquisition using\n\
the DIO_StreamFrame() API call. It uses the device specified by the\n\
HEX_SERIAL_NUMBER that the user specifies.\n\n\
\n\
Usage: %s HEX_SERIAL_NUMBER FRAME_POINTS [CLOCKSPEED]\n\n", AIOUSB_GetVersion(), AIOUSB_GetVersionDate(),argv[0]);
        return 1;
    }
    /* parse command line */
    uint64_t targetSerialNumber = strtoll( *( argv + 1 ), 0, 16 );
    unsigned long framePoints = strtol( *( argv + 2 ), 0, 0 );



    if ( framePoints % 512 != 0 ) {
        fprintf(stderr,"\nFRAME_POINTS (%ld) is NOT an integer number of 512 bytes...", framePoints );
        framePoints = (framePoints / 512) * 512;
        fprintf(stderr,"rounding to %ld\n\n", framePoints );
    }

    if ( argc == 4 ) {
        ReadClockHz = strtol( argv[3] , 0, 10 );
    }


    /*
     * find the device with the specified serial number
     */
    unsigned long deviceIndex = GetDeviceBySerialNumber( targetSerialNumber );
    if ( deviceIndex == diNone ) {
        fprintf(stderr,"No USB-DIO_16 devices were found...exiting\n");
        return 1;
    }

    /*
     * found it; allocate buffer in which to receive streaming DIO data
     */
    unsigned short *const frameData = ( unsigned short * ) malloc( framePoints * sizeof( unsigned short ) );
    if( !frameData  ) {
        fprintf(stderr,"Error; Unable to allocate frame Data buffer...exiting\n");
        exit(3);
    }
    unsigned char outputMask = 0x08;    // receiver: port D is output; C, B and A are input
    unsigned long initialData = 0xffffffff;
    unsigned char tristateMask = 0x00;

    unsigned long transferred;

    /* set up communication parameters */
    AIOUSB_SetCommTimeout( deviceIndex, 16000 );
    AIOUSB_SetStreamingBlockSize( deviceIndex, 512 );

    /* First stop the clocks*/
    {
        double tmpclock = 0;
        result = DIO_StreamSetClocks( deviceIndex, &tmpclock, &tmpclock );
        if ( result != AIOUSB_SUCCESS ) {
            fprintf(stderr,"Can't temporarily stop the clocks: %ld\n", result );
            goto abort;
        }
        result = AIOUSB_ClearFIFO( deviceIndex, CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT );
        if ( result != AIOUSB_SUCCESS ) {
            fprintf(stderr,"Can't Clear the FIFO: %ld\n", result );
            goto abort;
        }
    }

    /* configure I/O ports */
    result = DIO_ConfigureEx( deviceIndex, &outputMask, &initialData, &tristateMask );
    if( result < AIOUSB_SUCCESS ) {
        printf( "Error '%s' configuring device at index %lu\n" , AIOUSB_GetResultCodeAsString( result ), deviceIndex );
        DIO_StreamClose( deviceIndex );
        goto abort;
    }

    /* open stream for reading */
    result = DIO_StreamOpen( deviceIndex, AIOUSB_TRUE );
    if( result < AIOUSB_SUCCESS ) {
        fprintf( stderr, "Error '%s' opening read stream for device at index %lu\n" , AIOUSB_GetResultCodeAsString( result ), deviceIndex );
        goto abort;
    }

    /* Set the receiver to  a specific clock frequency */
    result = DIO_StreamSetClocks( deviceIndex, &ReadClockHz, &WriteClockHz );
    if( result < AIOUSB_SUCCESS ) {
        printf( "Error '%s' setting stream clock for device at index %lu\n" , AIOUSB_GetResultCodeAsString( result ), deviceIndex );
        goto abort;
    }

    /* receive frame */
    result = DIO_StreamFrame( deviceIndex, framePoints, frameData, &transferred );
    if( result == AIOUSB_SUCCESS ) {
        if( transferred != framePoints * sizeof( unsigned short ) ) {
            fprintf(stderr,"Received bytes (%ld) was not equal to what was expected (%ld)\n",
                    transferred, framePoints * sizeof( unsigned short ) );
            goto abort;
        }

        printf( "%lu FramePoints successfully read from device at index %lu\n" , framePoints, deviceIndex );
        FILE *fp = fopen("output.dat","w");

        fwrite(frameData,sizeof(frameData[0]),framePoints,fp);

        fclose(fp);


    } else {
        printf("Error running DIO_StreamFrame, result=%ld\n"
               "%lu\n attempted to read %lu bytes, but actually read %lu bytes\n", 
               result,
               deviceIndex, 
               framePoints * sizeof( unsigned short ), 
               transferred 
               );
    }

    DIO_StreamClose( deviceIndex );
    AIOUSB_ClearFIFO( deviceIndex, CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT );

    {
        double tmpclock = 0;
        result = DIO_StreamSetClocks( deviceIndex, &tmpclock, &tmpclock );
        if ( result != AIOUSB_SUCCESS ) {
            fprintf(stderr,"Can't temporarily stop the clocks: %ld\n", result );
            goto abort;
        }
    }


 abort:
    free( frameData );

    /* Call before program exits if AIOUSB_Init() succeeded */
    AIOUSB_Exit();

    return ( int ) result;
}
