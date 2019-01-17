/*
 * $RCSfile: sample.cpp,v $
 * $Revision: 1.13 $
 * $Date: 2009/11/26 21:51:22 $
 * :tabSize=4:collapseFolds=1:
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
 *     g++ sample.cpp -laiousb -lusb-1.0 -o sample
 *
 * or, to enable debug features
 *
 *     g++ -ggdb sample.cpp -laiousbdbg -lusb-1.0 -o sample
 */
// }}}

// {{{ includes
#include <aiousb.h>
#include <stdio.h>
using namespace AIOUSB;
// }}}

int main( int argc, char **argv ) {
	printf(
		"USB-AO16-16A sample program version 1.13, 26 November 2009\n"
		"  AIOUSB library version %s, %s\n"
		"  This program demonstrates controlling a USB-AO16-16A device on\n"
		"  the USB bus. For simplicity, it uses the first such device found\n"
		"  on the bus.\n"
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
			 * at least one ACCES device detected, but we want one of a specific type
			 */
       		AIOUSB_ListDevices();				// print list of all devices found on the bus
			const int MAX_NAME_SIZE = 20;
			char name[ MAX_NAME_SIZE + 2 ];
			unsigned long productID, nameSize, numDIOBytes, numCounters;
			unsigned long deviceIndex = 0;
			bool deviceFound = false;
			while( deviceMask != 0 ) {
				if( ( deviceMask & 1 ) != 0 ) {
					// found a device, but is it the correct type?
					nameSize = MAX_NAME_SIZE;
       				result = QueryDeviceInfo( deviceIndex, &productID, &nameSize, name, &numDIOBytes, &numCounters );
					if( result == AIOUSB_SUCCESS ) {
						if(
							productID >= USB_AO16_16A
							&& productID <= USB_AO12_4
						) {
							// found a USB-AO16-16A family device
							deviceFound = true;
							break;				// from while()
						}	// if( productID ...
					} else
						printf( "Error '%s' querying device at index %lu\n"
       						, AIOUSB_GetResultCodeAsString( result ), deviceIndex );
				}	// if( ( deviceMask ...
				deviceIndex++;
				deviceMask >>= 1;
			}	// while( deviceMask ...
			if( deviceFound ) {
       			AIOUSB_SetCommTimeout( deviceIndex, 500 );

				uint64_t serialNumber;
       			result = GetDeviceSerialNumber( deviceIndex, &serialNumber );
				if( result == AIOUSB_SUCCESS )
					printf( "Serial number of device at index %lu: %llx\n", deviceIndex, ( long long ) serialNumber );
				else
					printf( "Error '%s' getting serial number of device at index %lu\n"
       					, AIOUSB_GetResultCodeAsString( result ), deviceIndex );

				/*
				 * demonstrate getting enhanced device properties
				 */
				const int MAX_CHANNELS = 16;
				DeviceProperties properties;
       			result = AIOUSB_GetDeviceProperties( deviceIndex, &properties );
				if( result == AIOUSB_SUCCESS )
					printf( "Device properties successfully retrieved\n" );
				else {
					properties.DACChannels = MAX_CHANNELS;
					printf( "Error '%s' getting device properties\n"
       					, AIOUSB_GetResultCodeAsString( result ) );
				}	// if( result ...

				/*
				 * demonstrate setting output range
				 */
       			result = DACSetBoardRange( deviceIndex, DAC_RANGE_0_5V /* 0-5V */ );
				if( result == AIOUSB_SUCCESS )
					printf( "D/A output range successfully set\n" );
				else
					printf( "Error '%s' setting D/A ourput range\n"
       					, AIOUSB_GetResultCodeAsString( result ) );

				/*
				 * demonstrate writing to one D/A channel
				 */
				const int TEST_CHANNEL = 0;			// channels are numbered 0 to MAX_CHANNELS - 1
				const unsigned MAX_COUNTS = 0xffff;	// the 16-bit and 12-bit devices both support 16-bit count values
				const unsigned counts = MAX_COUNTS / 2;	// half of full scale
       			result = DACDirect( deviceIndex, TEST_CHANNEL, counts );
				if( result == AIOUSB_SUCCESS )
					printf( "%u D/A counts successfully output to channel %d\n", counts, TEST_CHANNEL );
				else
					printf( "Error '%s' attempting to output %u D/A counts successfully to channel %d\n"
       					, AIOUSB_GetResultCodeAsString( result )
						, counts, TEST_CHANNEL );

				/*
				 * demonstrate writing to multiple D/A channels
				 */
				unsigned short dacData[ MAX_CHANNELS * 2 ];		// channel/count pairs
				for( int channel = 0; channel < ( int ) properties.DACChannels; channel++ ) {
					dacData[ channel * 2 ] = channel;
					dacData[ channel * 2 + 1 ] = ( unsigned short ) (
						( unsigned long ) ( channel + 1 )
						* ( unsigned long ) MAX_COUNTS
						/ properties.DACChannels
					);
				}	// for( int channel ...
       			result = DACMultiDirect( deviceIndex, dacData, properties.DACChannels );
				if( result == AIOUSB_SUCCESS )
					printf( "D/A counts successfully output to %u channels simultaneously\n", properties.DACChannels );
				else
					printf( "Error '%s' attempting to output D/A counts to %u channels simultaneously\n"
       					, AIOUSB_GetResultCodeAsString( result )
						, properties.DACChannels );
			} else
				printf( "Failed to find USB-AO16-16A device\n" );
		} else
			printf( "No ACCES devices found on USB bus\n" );

		/*
		 * MUST call AIOUSB_Exit() before program exits,
		 * but only if AIOUSB_Init() succeeded
		 */
       	AIOUSB_Exit();
	}	// if( result ...
	return ( int ) result;
}	// main()


/* end of file */
