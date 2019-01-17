/**
 * @file   AIOContinuousBuffer.c
 * @author  $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief AIOUSB sample program 
 * @todo Make the number of channels in the ContinuousBuffer match the number of channels in the
 *       config object
 * @brief
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
 * Once libusb is installed properly, and you have sourced sourceme.sh you can compile the sample program
 * using the following command.
 *
 *     make sample AIOUSBLIBDIR=${AIO_LIB_DIR} AIOUSBCLASSLIBDIR=${AIO_CLASSLIB_DIR} DEBUG=1
 *
 * @verbatim
 * Alternatively, one can "manually" compile the sample program using the command:
 *
 *     g++ sample.cpp -laiousb -lusb-1.0 -o sample
 *
 * or, to enable debug features
 *
 *     g++ -ggdb sample.cpp -laiousbdbg -lusb-1.0 -o sample
 * @endverbatim
 *
 */


#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
using namespace AIOUSB;


int main( int argc, char **argv ) {
	printf(
		"USB-DIO-16A sample program version 1.9, 29 January 2010\n"
		"  AIOUSB library version %s, %s\n"
		"  This program demonstrates high speed streaming between 2 USB-DIO-16A\n"
		"  devices on the same USB bus. For simplicity, it uses the first 2 such\n"
		"  devices found on the bus.\n"
       	, AIOUSB_GetVersion(), AIOUSB_GetVersionDate()
	);

	/*
	 * MUST call AIOUSB_Init() before any meaningful AIOUSB functions;
	 * AIOUSB_GetVersion() above is an exception
	 */
       	unsigned long result = AIOUSB_Init();
	if( result == AIOUSB_SUCCESS ) {
		/*
		 * call GetDevices() to obtain "list" of devices found on the bus
		 */
       	unsigned long deviceMask = GetDevices();
		if( deviceMask != 0 ) {
			/*
			 * at least one ACCES device detected, but we want devices of a specific type
			 */
       		AIOUSB_ListDevices();				// print list of all devices found on the bus

			/*
			 * search for two USB-DIO-16A devices; the first one will be the
			 * sender of data, and the second one will be the receiver
			 */
			uint64_t serialNumber1 = 0		// serial number of sender board
				, serialNumber2 = 0;			// serial number of receiver board
			int deviceIndex1 = -1
				, deviceIndex2 = -1
				, index = 0;
			while( deviceMask != 0 ) {
				if( ( deviceMask & 1 ) != 0 ) {
					// found a device, but is it the correct type?
					unsigned long productID;
       				result = QueryDeviceInfo( index, &productID, NULL, NULL, NULL, NULL );
					if( result == AIOUSB_SUCCESS ) {
						if( productID == USB_DIO_16A ) {
							// found a USB-DIO-16A
							if( deviceIndex1 < 0 ) {
       							result = GetDeviceSerialNumber( index, &serialNumber1 );
								if( result == AIOUSB_SUCCESS ) {
									deviceIndex1 = index;
									printf( "Sending device at index %d, serial number %llx\n"
										, deviceIndex1, ( long long ) serialNumber1 );
								} else {
									printf( "Error '%s' getting serial number of device at index %d\n"
       									, AIOUSB_GetResultCodeAsString( result ), index );
									break;
								}
							} else if( deviceIndex2 < 0 ) {
       							result = GetDeviceSerialNumber( index, &serialNumber2 );
								if( result == AIOUSB_SUCCESS ) {
									if( serialNumber2 == serialNumber1 ) {
										printf(
											"Error: device at index %d and device at index %d have the same serial number of %llx\n"
											, deviceIndex1, index, ( long long ) serialNumber1
										);
									} else {
										deviceIndex2 = index;
										printf( "Receiving device at index %d, serial number %llx\n"
											, deviceIndex2, ( long long ) serialNumber2 );
									}
									break;
								} else {
									printf( "Error '%s' getting serial number of device at index %d\n"
       									, AIOUSB_GetResultCodeAsString( result ), index );
									break;
								}
							}
						}
					} else {
						printf( "Error '%s' querying device at index %d\n"
       						, AIOUSB_GetResultCodeAsString( result ), index );
						/*
						 * even though we got an error from this device,
						 * keep searching for other devices
						 */
					}
				}
				index++;
				deviceMask >>= 1;
			}

			if(
				deviceIndex1 >= 0
				&& deviceIndex2 >= 0
				&& serialNumber1 != 0
				&& serialNumber2 != 0
			) {
				/*
				 * allocate buffer for streaming DIO data to send to receiver
				 */
				const double CLOCK_SPEED = 1000000;				// Hz
				const unsigned long FRAME_POINTS = 1024000;		// must be multiple of 256
				unsigned short *const frameData = ( unsigned short * ) malloc( FRAME_POINTS * sizeof( unsigned short ) );
				if( frameData != 0 ) {
					/*
					 * fill buffer with a pattern that the receiver can verify
					 */
					unsigned long point;
					for( point = 0; point < FRAME_POINTS; point++ )
						frameData[ point ] = point & 0xfffful;

					/*
					 * fork receiver process, passing it the serial number of
					 * the receiver board
					 */
					const pid_t pid = fork();
					if( pid < 0 ) {
						printf( "Error creating receiver process\n" );
					} else if( pid == 0 ) {
						/*
						 * we are now the child process, execute the receiver program; its command
						 * line usage is: receiver <hex serial number> <num. points>
						 */
						char serialNumber[ 50 ]
							, numPoints[ 20 ];
						sprintf( serialNumber, "%llx", ( long long ) serialNumber2 );
						sprintf( numPoints, "%lu", FRAME_POINTS );
						execl( "./receiver", "receiver", serialNumber, numPoints, NULL );
					} else {
						/*
						 * receiver process successfully created; proceed to send the data;
						 * configure sending device to send streaming DIO data; sender will
						 * control the clock; in this sample program, the sender and receiver
						 * are wired together; therefore, their I/O ports must have opposite
						 * direction; however, within a device, ports C and D always have
						 * opposite direction; so we must configure one board with A, B and C
						 * as outputs, and port D as input; then we must configure the other
						 * board to be the opposite, or A, B and C as inputs, and port D as
						 * output
						 */
						unsigned char outputMask = 0x07;	// sender: port D is input; C, B and A are output
						unsigned long initialData = 0xffffffff;
						unsigned char tristateMask = 0x00;
						double ReadClockHz
							, WriteClockHz;
						unsigned long transferred;

						/*
						 * set up communication parameters
						 */
       					AIOUSB_SetCommTimeout( deviceIndex1, 1000 );
       					AIOUSB_SetStreamingBlockSize( deviceIndex1, 256 );

						/*
						 * start the clock; the sender (us) will control the clock
						 */
						ReadClockHz = 0;
						WriteClockHz = CLOCK_SPEED;
       					result = DIO_StreamSetClocks( deviceIndex1, &ReadClockHz, &WriteClockHz );
						if( result == AIOUSB_SUCCESS ) {
							printf( "Stream clock for device at index %d set to %0.1f Hz\n"
								, deviceIndex1, WriteClockHz );
						} else {
							printf( "Error '%s' setting stream clock for device at index %d\n"
       							, AIOUSB_GetResultCodeAsString( result ), deviceIndex1 );
							goto abort;
						}

						/*
						 * open stream for writing
						 */
       					result = DIO_StreamOpen( deviceIndex1, AIOUSB_FALSE /* open for writing */ );
						if( result != AIOUSB_SUCCESS ) {
							printf( "Error '%s' opening write stream for device at index %d\n"
       							, AIOUSB_GetResultCodeAsString( result ), deviceIndex1 );
							goto abort;
						}

						/*
						 * configure I/O ports
						 */
       					result = DIO_ConfigureEx( deviceIndex1, &outputMask, &initialData, &tristateMask );
						if( result != AIOUSB_SUCCESS ) {
							printf( "Error '%s' configuring device at index %d\n"
       							, AIOUSB_GetResultCodeAsString( result ), deviceIndex1 );
       						DIO_StreamClose( deviceIndex1 );
							goto abort;
						}

						/*
						 * send frame
						 */
       					result = DIO_StreamFrame( deviceIndex1, FRAME_POINTS, frameData, &transferred );
						if( result == AIOUSB_SUCCESS ) {
							if( transferred == FRAME_POINTS * sizeof( unsigned short ) )
								printf( "%lu point frame successfully written to device at index %d\n"
									, FRAME_POINTS, deviceIndex1 );
							else
								printf(
									"Error: incorrect amount of frame data written to device at index %d\n"
									"  attempted to write %lu bytes, but actually wrote %lu bytes\n"
									, deviceIndex1
									, FRAME_POINTS * sizeof( unsigned short )
									, transferred );
						} else {
							printf( "Error '%s' writing frame to device at index %d\n"
       							, AIOUSB_GetResultCodeAsString( result ), deviceIndex1 );
							AIOUSB_ClearFIFO( deviceIndex1, CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT );
						}

						/*
						 * stop write clock and close stream, but after giving the FIFO time to empty
						 * and the receiver time to finish receiving; after the call to DIO_StreamFrame()
						 * there may still be data in the sending FIFO; an alternative is to simply not
						 * turn off the write clock; the required delay is dependent on the clock rate;
						 * the faster the clock, the smaller the delay required; unfortunately, there's
						 * no handshaking between the software and the hardware to indicate when
						 * transmission is finished
						 */
						sleep( 2 );
						ReadClockHz = WriteClockHz = 0;
       					DIO_StreamSetClocks( deviceIndex1, &ReadClockHz, &WriteClockHz );
       					DIO_StreamClose( deviceIndex1 );
abort:;
					}

					free( frameData );
				} else {
					printf( "Unable to allocate buffer for frame data\n" );
				}
			} else
				printf( "Failed to find 2 USB-DIO-16A devices\n" );
		} else
			printf( "No ACCES devices found on USB bus\n" );

		/*
		 * MUST call AIOUSB_Exit() before program exits,
		 * but only if AIOUSB_Init() succeeded
		 */
       	AIOUSB_Exit();
	}

	/*
	 * pause briefly so receiver can print its output
	 * before command line prompt reappears
	 */
	sleep( 2 );
	return ( int ) result;
}


/* end of file */
