/*
 * $RCSfile: AO16_AnalogOutputSubsystem.cpp,v $
 * $Revision: 1.6 $
 * $Date: 2010/01/25 19:19:21 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AO16_AnalogOutputSubsystem implementation
 */

#include "CppCommon.h"
#include <assert.h>
#include <aiousb.h>
#include "USBDeviceManager.hpp"
#include "AO16_AnalogOutputSubsystem.hpp"


using namespace std;

namespace AIOUSB {


const int AO16_AnalogOutputSubsystem::RANGE_0_5V;
const int AO16_AnalogOutputSubsystem::RANGE_5V;
const int AO16_AnalogOutputSubsystem::RANGE_0_10V;
const int AO16_AnalogOutputSubsystem::RANGE_10V;
const int AO16_AnalogOutputSubsystem::MIN_COUNTS;
const int AO16_AnalogOutputSubsystem::MAX_COUNTS;
const char AO16_AnalogOutputSubsystem::RANGE_TEXT[][ 10 ] = {
	/*
	 * these strings are indexed by RANGE_* constants above
	 */
	  "0-5V"
	, "+/-5V"
	, "0-10V"
	, "+/-10V"
};	// AO16_AnalogOutputSubsystem::RANGE_TEXT[][]




AO16_AnalogOutputSubsystem::AO16_AnalogOutputSubsystem( USBDeviceBase &parent )
		: AnalogOutputSubsystem( parent )
		, outputRange( MIN_COUNTS, MAX_COUNTS ) {
	minCounts = MIN_COUNTS;
	maxCounts = MAX_COUNTS;
	setRange( RANGE_0_5V );						// also sets device to this range
}	// AO16_AnalogOutputSubsystem::AO16_AnalogOutputSubsystem()

AO16_AnalogOutputSubsystem::~AO16_AnalogOutputSubsystem() {
	// nothing to do
}	// AO16_AnalogOutputSubsystem::~AO16_AnalogOutputSubsystem()





/*
 * properties
 */

/**
 * Gets the textual string for the specified range.
 * @param range the range for which to obtain the textual string.
 * @return The textual string for the specified range.
 * @see setRange( int range )
 * @throws IllegalArgumentException
 */

std::string AO16_AnalogOutputSubsystem::getRangeText( int range ) {
	if(
		range < RANGE_0_5V
		|| range > RANGE_10V
	)
		throw IllegalArgumentException( "Invalid range" );
	return RANGE_TEXT[ range ];
}	// AO16_AnalogOutputSubsystem::getRangeText()

/*
 * configuration
 */

/**
 * Sets the voltage range of the D/A outputs.
 * @param range the voltage range to select. May be one of:
 * <i>AO16_AnalogOutputSubsystem::RANGE_0_5V
 * AO16_AnalogOutputSubsystem::RANGE_5V
 * AO16_AnalogOutputSubsystem::RANGE_0_10V
 * AO16_AnalogOutputSubsystem::RANGE_10V</i>
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

AO16_AnalogOutputSubsystem &AO16_AnalogOutputSubsystem::setRange( int range ) {
	outputRange.setRange( range );
	const int result = DACSetBoardRange( getDeviceIndex(), range );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// AO16_AnalogOutputSubsystem::setRange()

/**
 * Writes a voltage value to a D/A channel.
 * @param channel the channel to write to.
 * @param volts the voltage value to output.
 * @return This subsystem, useful for chaining together multiple operations.
 */

AO16_AnalogOutputSubsystem &AO16_AnalogOutputSubsystem::writeVolts( int channel, double volts ) {
	writeCounts( channel, voltsToCounts( volts ) );
	return *this;
}	// AO16_AnalogOutputSubsystem::writeVolts()

/**
 * Writes a block of voltage values to one or more D/A channels.
 * @param points an array of OutputVoltagePoint points representing channel-voltage pairs.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AO16_AnalogOutputSubsystem &AO16_AnalogOutputSubsystem::writeVolts( const OutputVoltagePointArray &points ) {
	if( points.size() < 1 )
		throw IllegalArgumentException( "Invalid points array" );

	/*
	 * convert channel/voltage pairs to channel/count pairs
	 */
	UShortArray countPoints( points.size() * 2 );
	for( int index = 0; index < ( int ) points.size(); index++ ) {
		if(
			points.at( index ).channel >= 0
			&& points.at( index ).channel < numChannels
		) {
			countPoints.at( index * 2 ) = ( unsigned short ) points.at( index ).channel;
			countPoints.at( index * 2 + 1 ) = voltsToCounts( points.at( index ).volts );
		} else
			throw IllegalArgumentException( "Invalid channel" );
	}	// for( int index ...

	writeCounts( countPoints );
	return *this;
}	// AO16_AnalogOutputSubsystem::writeVolts()



}	// namespace AIOUSB

/* end of file */
