/*
 * $RCSfile: AI16_InputRange.cpp,v $
 * $Date: 2009/12/19 00:27:36 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AI16_InputRange implementation
 */


#include "AnalogInputSubsystem.hpp"


namespace AIOUSB {

AI16_InputRange::AI16_InputRange()
		: AnalogIORange() {
	// nothing to do
}	// AI16_InputRange::AI16_InputRange()

AI16_InputRange::AI16_InputRange( int minCounts, int maxCounts )
		: AnalogIORange( minCounts, maxCounts ) {
	// nothing to do
}	// AI16_InputRange::AI16_InputRange()

AI16_InputRange::~AI16_InputRange() {
	// nothing to do
}	// AI16_InputRange::~AI16_InputRange()

AnalogIORange &AI16_InputRange::setRange( int range ) {
	switch( range ) {
		case AnalogInputSubsystem::RANGE_0_1V:
			setVoltRange( 0, 1 );
			break;

		case AnalogInputSubsystem::RANGE_1V:
			setVoltRange( -1, 1 );
			break;

		case AnalogInputSubsystem::RANGE_0_2V:
			setVoltRange( 0, 2 );
			break;

		case AnalogInputSubsystem::RANGE_2V:
			setVoltRange( -2, 2 );
			break;

		case AnalogInputSubsystem::RANGE_0_5V:
			setVoltRange( 0, 5 );
			break;

		case AnalogInputSubsystem::RANGE_5V:
			setVoltRange( -5, 5 );
			break;

		case AnalogInputSubsystem::RANGE_0_10V:
			setVoltRange( 0, 10 );
			break;

		case AnalogInputSubsystem::RANGE_10V:
			setVoltRange( -10, 10 );
			break;

		default:
			throw IllegalArgumentException( "Invalid range" );
	}	// switch( range )
	return AnalogIORange::setRange( range );
}	// AI16_InputRange::setRange()

}	// namespace AIOUSB

/* end of file */
