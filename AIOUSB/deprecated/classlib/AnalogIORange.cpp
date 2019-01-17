/*
 * $RCSfile: AnalogIORange.cpp,v $
 * $Date: 2009/12/19 00:27:36 $
 * $Revision: 1.2 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AnalogIORange implementation
 */


#include <assert.h>
#include <math.h>
#include "AnalogIORange.hpp"


namespace AIOUSB {

/**
 * Constructor which uses the default properties.
 */

AnalogIORange::AnalogIORange() {
	range = 0;
	minCounts = 0;
	maxCounts = 0xffff;
	rangeCounts = 0xffff;
	minVolts = -10;
	maxVolts = 10;
	rangeVolts = 20;
}	// AnalogIORange::AnalogIORange()


/**
 * Constructor which sets the count range. The count range is usually constant, so setting it one time
 * in the constructor is convenient.
 * @param minCounts minimum counts for current range.
 * @param maxCounts maximum counts for current range.
 */

AnalogIORange::AnalogIORange( int minCounts, int maxCounts ) {
	range = 0;
	minVolts = -10;
	maxVolts = 10;
	rangeVolts = 20;
	setCountRange( minCounts, maxCounts );
}	// AnalogIORange::AnalogIORange()


AnalogIORange::~AnalogIORange() {
	// nothing to do
}	// AnalogIORange::~AnalogIORange()


/**
 * Sets the range ID.
 * @param range the new range ID (defined by class that owns this instance).
 * @return This subsystem, useful for chaining together multiple operations.
 */

AnalogIORange &AnalogIORange::setRange( int range ) {
	this->range = range;
	return *this;
}	// AnalogIORange::setRange()


/**
 * Sets the A/D or D/A count range.
 * @param minCounts minimum counts for current range.
 * @param maxCounts maximum counts for current range.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogIORange &AnalogIORange::setCountRange( int minCounts, int maxCounts ) {
	if( minCounts >= maxCounts )
		throw IllegalArgumentException( "Invalid count range" );
	this->minCounts = minCounts;
	this->maxCounts = maxCounts;
	rangeCounts = maxCounts - minCounts;
	return *this;
}	// AnalogIORange::setCountRange()


/**
 * Sets the voltage range.
 * @param minVolts minimum volts for current range.
 * @param maxVolts maximum volts for current range.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogIORange &AnalogIORange::setVoltRange( double minVolts, double maxVolts ) {
	if( minVolts >= maxVolts )
		throw IllegalArgumentException( "Invalid voltage range" );
	this->minVolts = minVolts;
	this->maxVolts = maxVolts;
	rangeVolts = maxVolts - minVolts;
	return *this;
}	// AnalogIORange::setVoltRange()


/**
 * Converts a single A/D or D/A count value to volts, based on the current range setting.
 * @param counts the count value to convert to volts.
 * @return A voltage value calculated using the current range. The voltage value returned is
 * constrained to the current minimum-maximum voltage range.
 */

double AnalogIORange::countsToVolts( int counts ) const {
	assert( minVolts < maxVolts
		&& rangeVolts > 0
		&& minCounts < maxCounts
		&& rangeCounts > 0 );
	/*
	 *          ( counts - minCounts )
	 * volts = ------------------------- x ( maxVolts - minVolts ) + minVolts
	 *         ( maxCounts - minCounts )
	 */
	double volts = ( ( ( double ) counts - minCounts ) / rangeCounts ) * rangeVolts + minVolts;
	if( volts < minVolts )
		volts = minVolts;
	else if( volts > maxVolts )
		volts = maxVolts;
	return volts;
}	// AnalogIORange::countsToVolts()


/**
 * Converts a single voltage value to A/D or D/A counts, based on the current range setting.
 * @param volts the voltage value to convert to counts.
 * @return A count value calculated using the current D/A range. The count value returned is
 * constrained to the current minimum-maximum count range.
 */

int AnalogIORange::voltsToCounts( double volts ) const {
	assert( minVolts < maxVolts
		&& rangeVolts > 0
		&& minCounts < maxCounts
		&& rangeCounts > 0 );
	/*
	 *           ( volts - minVolts )
	 * counts = ----------------------- x ( maxCounts - minCounts ) + minCounts
	 *          ( maxVolts - minVolts )
	 */
	int counts = ( int ) round( ( ( volts - minVolts ) / rangeVolts ) * rangeCounts + minCounts );
	if( counts < minCounts )
		counts = minCounts;
	else if( counts > maxCounts )
		counts = maxCounts;
	return counts;
}	// AnalogIORange::voltsToCounts()

}	// namespace AIOUSB

/* end of file */
