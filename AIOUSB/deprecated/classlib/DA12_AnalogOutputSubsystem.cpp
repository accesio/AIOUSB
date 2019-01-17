/**
 * @file   DA12_AnalogOutputSubsystem.cpp
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @brief class DA12_AnalogOutputSubsystem implementation
 */



#include "CppCommon.h"
#include <assert.h>
#include <aiousb.h>
#include "USBDeviceManager.hpp"
#include "DA12_AnalogOutputSubsystem.hpp"


using namespace std;

namespace AIOUSB {


const int DA12_AnalogOutputSubsystem::RANGE_0_2_5V;
const int DA12_AnalogOutputSubsystem::RANGE_2_5V;
const int DA12_AnalogOutputSubsystem::RANGE_0_5V;
const int DA12_AnalogOutputSubsystem::RANGE_5V;
const int DA12_AnalogOutputSubsystem::RANGE_0_10V;
const int DA12_AnalogOutputSubsystem::RANGE_10V;
const int DA12_AnalogOutputSubsystem::MIN_COUNTS;
const int DA12_AnalogOutputSubsystem::MAX_COUNTS;
const char DA12_AnalogOutputSubsystem::RANGE_TEXT[][ 10 ] = {
	/*
	 * these strings are indexed by RANGE_* constants above
	 */
	  "0-2.5V"
	, "+/-2.5V"
	, "0-5V"
	, "+/-5V"
	, "0-10V"
	, "+/-10V"
};	// DA12_AnalogOutputSubsystem::RANGE_TEXT[][]




DA12_AnalogOutputSubsystem::DA12_AnalogOutputSubsystem( USBDeviceBase &parent )
		: AnalogOutputSubsystem( parent ) {
	minCounts = MIN_COUNTS;
	maxCounts = MAX_COUNTS;
	outputRange = new DA12_OutputRange[ numChannels ];
	for( int channel = 0; channel < numChannels; channel++ )
		outputRange[ channel ].setCountRange( MIN_COUNTS, MAX_COUNTS ).setRange( RANGE_0_5V );
}	// DA12_AnalogOutputSubsystem::DA12_AnalogOutputSubsystem()

DA12_AnalogOutputSubsystem::~DA12_AnalogOutputSubsystem() {
	if( outputRange != 0 )
		delete[] outputRange;
}	// DA12_AnalogOutputSubsystem::~DA12_AnalogOutputSubsystem()





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

std::string DA12_AnalogOutputSubsystem::getRangeText( int range ) {
	if(
		range < RANGE_0_2_5V
		|| range > RANGE_10V
	)
		throw IllegalArgumentException( "Invalid range" );
	return RANGE_TEXT[ range ];
}	// DA12_AnalogOutputSubsystem::getRangeText()

/*
 * configuration
 */

/**
 * Gets the current voltage range of a D/A channel.
 * @param channel the channel for which to obtain the current range.
 * @return Current voltage range.
 * @see setRange( int channel, int range )
 * @throws IllegalArgumentException
 */

int DA12_AnalogOutputSubsystem::getRange( int channel ) const {
	if(
		channel < 0
		|| channel >= numChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	return outputRange[ channel ].getRange();
}	// DA12_AnalogOutputSubsystem::getRange()

/**
 * Gets the current voltage range of multiple D/A channels.
 * @param startChannel the first channel for which to obtain the current range.
 * @param numChannels the number of channels for which to obtain the current range.
 * @return Array containing the current range for each of the specified channels.
 * @see setRange( int startChannel, const IntArray &range )
 * @throws IllegalArgumentException
 */

IntArray DA12_AnalogOutputSubsystem::getRange( int startChannel, int numChannels ) const {
	if(
		numChannels < 1
		|| startChannel < 0
		|| startChannel + numChannels > this->numChannels
	)
		throw IllegalArgumentException( "Invalid start channel or number of channels" );
	IntArray range( numChannels );
	for( int index = 0; index < numChannels; index++ )
		range.at( index ) = outputRange[ startChannel + index ].getRange();
	return range;
}	// DA12_AnalogOutputSubsystem::getRange()

/**
 * Sets the voltage range of a D/A channel. The ranges in this device are selected by means of
 * hardware jumpers, so these range settings here do not affect the hardware. However, they are
 * used to perform conversions between volts and counts. Moreover, the range setting is per D/A
 * channel, so care must be taken when setting the ranges to ensure that the software setting
 * matches the hardware jumper configuration, otherwise the voltage-count conversions will be
 * incorrect.
 * @param channel the channel for which to set the range.
 * @param range the voltage range to select. May be one of:
 * <i>DA12_AnalogOutputSubsystem::RANGE_0_2_5V
 * DA12_AnalogOutputSubsystem::RANGE_2_5V
 * DA12_AnalogOutputSubsystem::RANGE_0_5V
 * DA12_AnalogOutputSubsystem::RANGE_5V
 * DA12_AnalogOutputSubsystem::RANGE_0_10V
 * DA12_AnalogOutputSubsystem::RANGE_10V</i>
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

DA12_AnalogOutputSubsystem &DA12_AnalogOutputSubsystem::setRange( int channel, int range ) {
	if(
		channel < 0
		|| channel >= numChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	outputRange[ channel ].setRange( range );
	return *this;
}	// DA12_AnalogOutputSubsystem::setRange()

/**
 * Sets the current voltage range of multiple D/A channels.
 * @param startChannel the first channel for which to set the range.
 * @param range an array of voltage ranges to select, one per channel. The length of this array
 * implicitly specifies the number of channels to configure.
 * @return This subsystem, useful for chaining together multiple operations.
 * @see setRange( int channel, int range )
 * @throws IllegalArgumentException
 */

DA12_AnalogOutputSubsystem &DA12_AnalogOutputSubsystem::setRange( int startChannel, const IntArray &range ) {
	if(range.size() < 1
           || startChannel < 0
           || startChannel + ( int ) range.size() > numChannels
	)
		throw IllegalArgumentException( "Invalid range array or start channel" );
	for( int index = 0; index < ( int ) range.size(); index++ )
		outputRange[ startChannel + index ].setRange( range.at( index ) );
	return *this;
}	// DA12_AnalogOutputSubsystem::setRange()

/**
 * Sets the current voltage range of all D/A channels to the same value.
 * @param range the voltage range to select.
 * @return This subsystem, useful for chaining together multiple operations.
 * @see setRange( int channel, int range )
 */

DA12_AnalogOutputSubsystem &DA12_AnalogOutputSubsystem::setRange( int range ) {
	for( int channel = 0; channel < numChannels; channel++ )
		outputRange[ channel ].setRange( range );
	return *this;
}	// DA12_AnalogOutputSubsystem::setRange()

/*
 * operations
 */

/**
 * Writes a voltage value to a D/A channel.
 * @param channel the channel to write to.
 * @param volts the voltage value to output.
 * @return This subsystem, useful for chaining together multiple operations.
 */

DA12_AnalogOutputSubsystem &DA12_AnalogOutputSubsystem::writeVolts( int channel, double volts ) {
	writeCounts( channel, voltsToCounts( channel, volts ) );
	return *this;
}	// DA12_AnalogOutputSubsystem::writeVolts()

/**
 * Writes a block of voltage values to one or more D/A channels.
 * @param points an array of OutputVoltagePoint points representing channel-voltage pairs.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

DA12_AnalogOutputSubsystem &DA12_AnalogOutputSubsystem::writeVolts( const OutputVoltagePointArray &points ) {
       if(points.size() < 1)
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
			const int channel = points.at( index ).channel;
			countPoints.at( index * 2 ) = ( unsigned short ) channel;
			countPoints.at( index * 2 + 1 ) = voltsToCounts( channel, points.at( index ).volts );
		} else
			throw IllegalArgumentException( "Invalid channel" );
	}	// for( int index ...

	writeCounts( countPoints );
	return *this;
}	// DA12_AnalogOutputSubsystem::writeVolts()

/**
 * Converts a single D/A count value to volts, based on the current range setting.
 * @param channel the channel whose current range will be used to perform the conversion.
 * @param counts the count value to convert to volts.
 * @return A voltage value calculated using the current D/A range. The voltage value returned is
 * constrained to the current minimum-maximum voltage range of the D/A.
 * <i>(see setRange( int channel, int range ))</i>.
 * @throws IllegalArgumentException
 */

double DA12_AnalogOutputSubsystem::countsToVolts( int channel, unsigned short counts ) const {
	if(
		channel < 0
		|| channel >= numChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	return outputRange[ channel ].countsToVolts( counts );
}	// DA12_AnalogOutputSubsystem::countsToVolts()

/**
 * Converts a single voltage value to D/A counts, based on the current range setting.
 * @param channel the channel whose current range will be used to perform the conversion.
 * @param volts the voltage value to convert to counts.
 * @return A count value calculated using the current D/A range. The count value returned is
 * constrained to the current minimum-maximum count range of the D/A.
 * <i>(see setRange( int channel, int range ))</i>.
 * @throws IllegalArgumentException
 */

unsigned short DA12_AnalogOutputSubsystem::voltsToCounts( int channel, double volts ) const {
	if(
		channel < 0
		|| channel >= numChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	return ( unsigned short ) outputRange[ channel ].voltsToCounts( volts );
}	// DA12_AnalogOutputSubsystem::voltsToCounts()



}	// namespace AIOUSB

/* end of file */
