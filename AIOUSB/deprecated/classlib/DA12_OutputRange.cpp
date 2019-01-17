/*
 * $RCSfile: DA12_OutputRange.cpp,v $
 * $Date: 2009/12/19 00:27:36 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class DA12_OutputRange implementation
 */


#include "DA12_OutputRange.hpp"
#include "DA12_AnalogOutputSubsystem.hpp"


namespace AIOUSB {

DA12_OutputRange::DA12_OutputRange()
		: AnalogIORange() {
	// nothing to do
}	// DA12_OutputRange::DA12_OutputRange()

DA12_OutputRange::DA12_OutputRange( int minCounts, int maxCounts )
		: AnalogIORange( minCounts, maxCounts ) {
	// nothing to do
}	// DA12_OutputRange::DA12_OutputRange()

DA12_OutputRange::~DA12_OutputRange() {
	// nothing to do
}	// DA12_OutputRange::~DA12_OutputRange()

AnalogIORange &DA12_OutputRange::setRange( int range ) {
	switch( range ) {
		case DA12_AnalogOutputSubsystem::RANGE_0_2_5V:
			setVoltRange( 0, 2.5 );
			break;

		case DA12_AnalogOutputSubsystem::RANGE_2_5V:
			setVoltRange( -2.5, 2.5 );
			break;

		case DA12_AnalogOutputSubsystem::RANGE_0_5V:
			setVoltRange( 0, 5 );
			break;

		case DA12_AnalogOutputSubsystem::RANGE_5V:
			setVoltRange( -5, 5 );
			break;

		case DA12_AnalogOutputSubsystem::RANGE_0_10V:
			setVoltRange( 0, 10 );
			break;

		case DA12_AnalogOutputSubsystem::RANGE_10V:
			setVoltRange( -10, 10 );
			break;

		default:
			throw IllegalArgumentException( "Invalid range" );
	}	// switch( range )
	return AnalogIORange::setRange( range );
}	// DA12_OutputRange::setRange()

}	// namespace AIOUSB

/* end of file */
