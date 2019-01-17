/*
 * $RCSfile: receiver.cpp,v $
 * $Revision: 1.9 $
 * $Date: 2010/01/29 18:32:46 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * AIOUSB library sample program
 */


// {{{ notes and build instructions
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
using namespace AIOUSB;


int main( int argc, char **argv ) {
	unsigned long result = AIOUSB_SUCCESS;

	if( argc != 3 ) {
		printf(
			"USB-DIO-16A sample program version 1.9, 29 January 2010\n"
			"  AIOUSB library version %s, %s\n"
			"  This program demonstrates high speed streaming between 2 USB-DIO-16A\n"
			"  devices on the same USB bus. For simplicity, it uses the first 2 such\n"
			"  devices found on the bus.\n"
			"\n"
			"  This program is not intended to be executed by itself, but as a child\n"
			"  process of ./sample. Please run ./sample instead.\n"
			"\n"
			"  Usage: receiver <hex serial number> <num. points>\n"
      	, AIOUSB_GetVersion(), AIOUSB_GetVersionDate()
		);
	} else {
		/*
		 * parse command line
		 */
		uint64_t targetSerialNumber = strtoll( *( argv + 1 ), 0, 16 );
		unsigned long framePoints = strtol( *( argv + 2 ), 0, 0 );

		/*
		 * initialize AIOUSB library; MUST do before any meaningful AIOUSB functions
		 */
      	unsigned long result = AIOUSB_Init();
		if( result == AIOUSB_SUCCESS ) {
			/*
			 * find the device with the specified serial number
			 */
      		unsigned long deviceIndex = GetDeviceBySerialNumber( targetSerialNumber );
			if( deviceIndex != diNone ) {
				/*
				 * found it; allocate buffer in which to receive streaming DIO data
				 */
				unsigned short *const frameData = ( unsigned short * ) malloc( framePoints * sizeof( unsigned short ) );
				if( frameData != 0 ) {
					unsigned char outputMask = 0x08;	// receiver: port D is output; C, B and A are input
					unsigned long initialData = 0xffffffff;
					unsigned char tristateMask = 0x00;
					double ReadClockHz
						, WriteClockHz;
					unsigned long transferred;

					/*
					 * set up communication parameters
					 */
      				AIOUSB_SetCommTimeout( deviceIndex, 1000 );
      				AIOUSB_SetStreamingBlockSize( deviceIndex, 256 );

					/*
					 * turn off the clocks; the sender will control the clock
					 */
					ReadClockHz = WriteClockHz = 0;
      				result = DIO_StreamSetClocks( deviceIndex, &ReadClockHz, &WriteClockHz );
					if( result != AIOUSB_SUCCESS ) {
						printf( "Error '%s' setting stream clock for device at index %lu\n"
      						, AIOUSB_GetResultCodeAsString( result ), deviceIndex );
						goto abort;
					}	// if( result ...

					/*
					 * open stream for reading
					 */
      				result = DIO_StreamOpen( deviceIndex, AIOUSB_TRUE /* open for reading */ );
					if( result != AIOUSB_SUCCESS ) {
						printf( "Error '%s' opening read stream for device at index %lu\n"
      						, AIOUSB_GetResultCodeAsString( result ), deviceIndex );
						goto abort;
					}	// if( result ...

					/*
					 * configure I/O ports
					 */
      				result = DIO_ConfigureEx( deviceIndex, &outputMask, &initialData, &tristateMask );
					if( result != AIOUSB_SUCCESS ) {
						printf( "Error '%s' configuring device at index %lu\n"
      						, AIOUSB_GetResultCodeAsString( result ), deviceIndex );
      					DIO_StreamClose( deviceIndex );
						goto abort;
					}	// if( result ...

					/*
					 * fill buffer with a known pattern so we can verify data received
					 */
					memset( frameData, 0x55, framePoints * sizeof( unsigned short ) );

					/*
					 * receive frame
					 */
      				result = DIO_StreamFrame( deviceIndex, framePoints, frameData, &transferred );
					if( result == AIOUSB_SUCCESS ) {
						if( transferred == framePoints * sizeof( unsigned short ) ) {
							/*
							 * it looks like we read the correct amount of data, but
							 * verify the data received
							 */
							AIOUSB_BOOL correct = AIOUSB_TRUE;
							unsigned long point;
							for( point = 0; point < framePoints; point++ ) {
								if( frameData[ point ] != ( point & 0xfffful ) ) {
									printf( "Error: frame word %lu = %u; expected %u\n"
										, point, frameData[ point ]
										, ( unsigned short ) ( point & 0xfffful ) );
									correct = AIOUSB_FALSE;
									break;
								}	// if( frameData[ ...
							}	// for( point ...
							if( correct )
								printf( "%lu point frame successfully read from device at index %lu\n"
									, framePoints, deviceIndex );
							else
								printf( "Error: frame read from device at index %lu contained data errors\n", deviceIndex );
						} else {
							printf(
								"Error: incorrect amount of frame data read from device at index %lu\n"
								"  attempted to read %lu bytes, but actually read %lu bytes\n"
								, deviceIndex
								, framePoints * sizeof( unsigned short )
								, transferred );
							AIOUSB_ClearFIFO( deviceIndex, CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT );
						}	// if( transferred ...
					} else {
						printf( "Error '%s' reading frame from device at index %lu\n"
      						, AIOUSB_GetResultCodeAsString( result ), deviceIndex );
						AIOUSB_ClearFIFO( deviceIndex, CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT );
					}	// if( result ...
      				DIO_StreamClose( deviceIndex );

abort:
					free( frameData );
				} else {
					printf( "Unable to allocate buffer for frame data\n" );
				}	// if( frameData ...
			} else {
				printf( "Unable to find device with serial number %llx\n", ( long long ) targetSerialNumber );
			}	// if( deviceIndex ...

			/*
			 * MUST be called before program exits, but only if AIOUSB_Init() succeeded
			 */
      		AIOUSB_Exit();
		}
	}

	return ( int ) result;
}	// main()
