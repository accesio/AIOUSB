/*
 * $RCSfile: SampleClass.cpp,v $
 * $Date: 2010/01/30 00:00:31 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

// {{{ build instructions
/*
 * g++ SampleClass.cpp -lclassaiousb -laiousbcpp -lusb-1.0 -o SampleClass
 *   or
 * g++ -ggdb SampleClass.cpp -lclassaiousbdbg -laiousbcppdbg -lusb-1.0 -o SampleClass
 */
// }}}

// {{{ includes
#include <iostream>
#include <iterator>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <USBDeviceManager.hpp>
#include <USB_DA12_8A_Family.hpp>
// }}}

using namespace AIOUSB;
using namespace std;

int main( int argc, char *argv[] ) {
/*API*/	USBDeviceManager deviceManager;
	try {
/*API*/	deviceManager.open();
		cout <<
			"USB-DA12-8A sample program version 1.1, 29 January 2010\n"
/*API*/		"  AIOUSB C++ class library version " << deviceManager.VERSION_NUMBER << ", " << deviceManager.VERSION_DATE << "\n"
/*API*/		"  AIOUSB library version " << deviceManager.getAIOUSBVersion() << ", " << deviceManager.getAIOUSBVersionDate() << "\n"
			"\n"
			"  This program demonstrates controlling a USB-DA12-8A family device on\n"
			"  the USB bus. For simplicity, it uses the first such device found\n"
			"  on the bus and supports these product IDs: ";
/*API*/	const StringArray supportedProductNames = USB_DA12_8A_Family::getSupportedProductNames();
		for( int index = 0; index < ( int ) supportedProductNames.size(); index++ ) {
			if( index > 0 )
				cout << ", ";
			cout << supportedProductNames.at( index );
		}	// for( int index ...
		cout << endl;
/*API*/	deviceManager.printDevices();
/*API*/	USBDeviceArray devices = deviceManager.getDeviceByProductID( USB_DA12_8A_Family::getSupportedProductIDs() );
		if( devices.size() > 0 ) {
			USB_DA12_8A_Family &device = *( USB_DA12_8A_Family * ) devices.at( 0 );	// get first device found
/*API*/		device.setCommTimeout( 1000 );

			/*
			 * demonstrate accessing device properties
			 */
/*API*/		cout << "Found device \'" << device.getName() << "\' with serial number " << hex << device.getSerialNumber() << endl;

			/*
			 * demonstrate writing D/A counts to one D/A channel
			 */
/*API*/		const int numChannels = device.dac().getNumChannels();
			const int TEST_CHANNEL = 0;			// channels are numbered 0 to numChannels - 1
			const unsigned short counts = DA12_AnalogOutputSubsystem::MAX_COUNTS / 2;	// half of full scale
/*API*/		device.dac().writeCounts( TEST_CHANNEL, counts );
			cout << dec << counts << " D/A counts successfully output to channel " << TEST_CHANNEL << endl;

			/*
			 * demonstrate writing D/A counts to multiple D/A channels
			 */
			UShortArray countPoints( numChannels * 2 );		// channel/count pairs
			for( int channel = 0; channel < numChannels; channel++ ) {
				countPoints.at( channel * 2 ) = channel;
				countPoints.at( channel * 2 + 1 ) = ( unsigned short ) ( ( channel + 1 ) * DA12_AnalogOutputSubsystem::MAX_COUNTS / numChannels );
			}	// for( int channel ...
/*API*/		device.dac().writeCounts( countPoints );
			cout << "Multiple D/A counts successfully output to " << numChannels << " channels" << endl;

			/*
			 * demonstrate writing volts to one D/A channel; note that the hardware range is selected
			 * by means of jumpers; this software setting is merely so that the correct D/A count values
			 * are calculated for the current hardware range setting; this demonstration assumes the
			 * hardware jumpers have been set to the +/-10V range
			 */
			const int HARDWARE_RANGE = DA12_AnalogOutputSubsystem::RANGE_10V;
			const double volts = 5;
/*API*/		device.dac().setRange( HARDWARE_RANGE );
/*API*/		device.dac().writeVolts( TEST_CHANNEL, volts );
			cout << volts << " volts (" << device.dac().voltsToCounts( TEST_CHANNEL, volts ) << " D/A counts)"
				<< " successfully output to channel " << TEST_CHANNEL << endl;

			/*
			 * demonstrate writing volts to multiple D/A channels
			 */
/*API*/		OutputVoltagePointArray voltPoints( numChannels );
			for( int channel = 0; channel < numChannels; channel++ ) {
/*API*/			voltPoints.at( channel ).channel = channel;
				voltPoints.at( channel ).volts = ( channel + 1 ) * 10.0 / numChannels;
			}	// for( int channel ...
/*API*/		device.dac().writeVolts( voltPoints );
			cout << "Multiple volts successfully output to " << numChannels << " channels" << endl;
		} else
			cout << "No USB-DA12-8A devices found on USB bus" << endl;
/*API*/	deviceManager.close();
	} catch( exception &ex ) {
		cerr << "Error \'" << ex.what() << "\' occurred while configuring device" << endl;
/*API*/	if( deviceManager.isOpen() )
/*API*/		deviceManager.close();
	}	// catch( ...
}	// main()

/* end of file */
