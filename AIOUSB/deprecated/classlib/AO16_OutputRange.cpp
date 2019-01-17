/*
 * $RCSfile: AO16_OutputRange.cpp,v $
 * $Date: 2009/12/19 00:27:36 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AO16_OutputRange implementation
 */


#include "AO16_OutputRange.hpp"
#include "AO16_AnalogOutputSubsystem.hpp"


namespace AIOUSB {

AO16_OutputRange::AO16_OutputRange()
		: AnalogIORange() {
	// nothing to do
}	// AO16_OutputRange::AO16_OutputRange()

AO16_OutputRange::AO16_OutputRange( int minCounts, int maxCounts )
		: AnalogIORange( minCounts, maxCounts ) {
	// nothing to do
}	// AO16_OutputRange::AO16_OutputRange()

AO16_OutputRange::~AO16_OutputRange() {
	// nothing to do
}	// AO16_OutputRange::~AO16_OutputRange()

AnalogIORange &AO16_OutputRange::setRange( int range ) {
	switch( range ) {
		case AO16_AnalogOutputSubsystem::RANGE_0_5V:
			setVoltRange( 0, 5 );
			break;

		case AO16_AnalogOutputSubsystem::RANGE_5V:
			setVoltRange( -5, 5 );
			break;

		case AO16_AnalogOutputSubsystem::RANGE_0_10V:
			setVoltRange( 0, 10 );
			break;

		case AO16_AnalogOutputSubsystem::RANGE_10V:
			setVoltRange( -10, 10 );
			break;

		default:
			throw IllegalArgumentException( "Invalid range" );
	}	// switch( range )
	return AnalogIORange::setRange( range );
}	// AO16_OutputRange::setRange()

}	// namespace AIOUSB

/* end of file */
