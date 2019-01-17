/*
 * $RCSfile: AI16_DataSet.cpp,v $
 * $Revision: 1.2 $
 * $Date: 2009/12/20 23:38:48 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AI16_DataSet declarations
 */


#include <iomanip>
#include <assert.h>
#include "AI16_DataSet.hpp"
#include "AnalogInputSubsystem.hpp"


using namespace std;

namespace AIOUSB {

AI16_DataSet::AI16_DataSet( AnalogInputSubsystem &subsystem, int numPoints, long timeStamp
	, int calMode, int triggerMode, int overSample, bool discardFirstSample )
		: points( numPoints ) {
	assert( numPoints > 0 );
	this->timeStamp = timeStamp;
	this->subsystem = &subsystem;
	this->calMode = calMode;
	this->triggerMode = triggerMode;
	this->overSample = overSample;
	this->discardFirstSample = discardFirstSample;
}	// AI16_DataSet::AI16_DataSet()

/**
 * Destructor for data set. Data sets returned by methods such as <i>AnalogInputSubsystem::read()</i>
 * must be explicitly destroyed.
 */

AI16_DataSet::~AI16_DataSet() {
	// nothing to do
}	// AI16_DataSet::~AI16_DataSet()

/**
 * Prints this data set. Mainly useful for diagnostic purposes.
 * @param out the print stream where the data set will be printed.
 * @return The print stream.
 */

std::ostream &AI16_DataSet::print( std::ostream &out ) {
	assert( sizeof( long ) == sizeof( time_t ) );
	time_t ts = ( time_t ) timeStamp;
	out
		<< "Analog input data set captured from " << subsystem->getParent().getName()
			<< " (S/N 0x" << right << setw( 16 ) << hex << subsystem->getParent().getSerialNumber() << "):\n"
		<< "  Time stamp: " << left << setw( 0 ) << dec << ctime( &ts ) /* << endl - ctime() adds a newline */
		<< "  Calibration mode: " << calMode << endl
		<< "  Trigger mode: " << triggerMode << endl
		<< "  Over-sample: " << overSample << endl
		<< "  Discard first sample: " << ( discardFirstSample ? "true" : "false" ) << endl
		<< "  " << ( int ) points.size() << " data points:\n";
	for( int index = 0; index < ( int ) points.size(); index++ )
		out << "    " << points.at( index ).toString() << endl;
	return out;
}	// AI16_DataSet::print()

}	// namespace AIOUSB

/* end of file */
