/*
 * $RCSfile: AnalogOutputSubsystem.cpp,v $
 * $Revision: 1.13 $
 * $Date: 2010/01/25 19:19:21 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AnalogOutputSubsystem implementation
 */

#include "CppCommon.h"
#include <assert.h>
#include <math.h>
#include <aiousb.h>
#include "USBDeviceManager.hpp"
#include "AnalogOutputSubsystem.hpp"


using namespace std;

namespace AIOUSB {



AnalogOutputSubsystem::AnalogOutputSubsystem( USBDeviceBase &parent )
		: DeviceSubsystem( parent ) {
	numChannels = 0;
	minCounts = 0;
	maxCounts = 0xffff;
	DeviceProperties properties;
	const int result = AIOUSB_GetDeviceProperties( getDeviceIndex(), &properties );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	numChannels = properties.DACChannels;
	assert( numChannels > 0 );
}	// AnalogOutputSubsystem::AnalogOutputSubsystem()

AnalogOutputSubsystem::~AnalogOutputSubsystem() {
	// nothing to do
}	// AnalogOutputSubsystem::~AnalogOutputSubsystem()





/*
 * properties
 */

/**
 * Prints the properties of this subsystem. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &AnalogOutputSubsystem::print( ostream &out ) {
	out
		<< "    Number of D/A channels: " << dec << numChannels << endl
		<< "    D/A count range: " << hex << minCounts << "-" << maxCounts << endl;
	return out;
}	// AnalogOutputSubsystem::print()

/*
 * operations
 */

/**
 * Writes a count value to a D/A channel.
 * @param channel the channel to write to.
 * @param counts the D/A count value to output. The number of bits of resolution for the D/A outputs varies from
 * model to model, however it's usually 12 or 16 bits. Moreover, some of the 12-bit models actually accept a
 * 16-bit value and simply truncate the least significant 4 bits. Consult the manual for the specific device to
 * determine the range of D/A values the device will accept. In general, 12-bit devices accept a count range of
 * 0-0xfff, and 16-bit devices accept a count range of 0-0xffff.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

AnalogOutputSubsystem &AnalogOutputSubsystem::writeCounts( int channel, unsigned short counts ) {
	if(
		channel < 0
		|| channel >= numChannels
		|| counts < minCounts
		|| counts > maxCounts
	)
		throw IllegalArgumentException( "Invalid channel or counts" );
	const int result = DACDirect( getDeviceIndex(), channel, counts );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// AnalogOutputSubsystem::writeCounts()

/**
 * Writes a block of count values to one or more D/A channels.
 * @param points an array of 16-bit integers representing channel-count pairs. The first integer of each pair
 * is the D/A channel number and the second integer is the D/A count value to output to the specified channel.
 * Refer to <i>writeCounts( int channel, unsigned short counts )</i> for an explanation of the channel addressing
 * and count values.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

AnalogOutputSubsystem &AnalogOutputSubsystem::writeCounts( const UShortArray &points ) {
	if(points.size() < 2 || ( points.size() & 1 ) != 0)
		throw IllegalArgumentException( "Invalid points array" );

	/*
	 * check all channel/count pairs
	 */
	for( int index = 0; index < ( int ) points.size(); index += 2 ) {
		if(
			points.at( index ) < 0
			|| points.at( index ) >= numChannels
			|| points.at( index + 1 ) < minCounts
			|| points.at( index + 1 ) > maxCounts
		)
			throw IllegalArgumentException( "Invalid channel or counts" );
	}	// for( int index ...

	const int result = DACMultiDirect( getDeviceIndex(), ( unsigned short * ) points.data(), points.size() / 2 /* channel-count pairs */ );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// AnalogOutputSubsystem::writeCounts()



}	// namespace AIOUSB

/* end of file */
