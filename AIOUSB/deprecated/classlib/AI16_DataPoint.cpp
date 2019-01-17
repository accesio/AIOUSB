/*
 * $RCSfile: AI16_DataPoint.cpp,v $
 * $Revision: 1.4 $
 * $Date: 2009/12/25 16:21:51 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AI16_DataPoint declarations
 */


#include <sstream>
#include <iomanip>
#include "AI16_DataPoint.hpp"
#include "AnalogInputSubsystem.hpp"


using namespace std;

namespace AIOUSB {

AI16_DataPoint::AI16_DataPoint() {
	counts = channel = range = 0;
	differentialMode = false;
}

/**
 * Gets the textual representation of the range that was in effect when this data point was captured.
 * @return The textual representation of the range that was in effect when this data point was captured.
 */

std::string AI16_DataPoint::getRangeText() const {
	return AnalogInputSubsystem::getRangeText( range );
}

/**
 * Gets the captured data in volts.
 * @return The captured data in volts.
 */

double AI16_DataPoint::getVolts() const {
	return ( AI16_InputRange( AnalogInputSubsystem::MIN_COUNTS, AnalogInputSubsystem::MAX_COUNTS ) )
		.setRange( range ).countsToVolts( counts );
}

/**
 * Gets a single-line string summary of this data point. Mainly useful for diagnostic purposes.
 * @return A single-line string summary of this data point.
 */

std::string AI16_DataPoint::toString() const {
	ostringstream out;
	out << "Channel " << dec << setw( 3 ) << setfill( ' ' ) << channel
		<< ", " << setw( 5 ) << counts << " A/D counts"
		<< ", " << setw( 11 ) << setprecision( 7 ) << getVolts() << 'V'
		<< ", " << getRangeText()
		<< ", " << ( differentialMode ? "differential" : "single-ended" );
	return out.str();
}

}

/* end of file */
