/*
 * $RCSfile: AI16_InputRange.java,v $
 * $Date: 2009/12/17 20:07:07 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

class AI16_InputRange extends AnalogIORange {

	public AI16_InputRange( int minCounts, int maxCounts ) {
		super( minCounts, maxCounts );
	}	// AI16_InputRange()

	public AnalogIORange setRange( int range ) {
		switch( range ) {
			case AnalogInputSubsystem.RANGE_0_1V:
				setVoltRange( 0, 1 );
				break;

			case AnalogInputSubsystem.RANGE_1V:
				setVoltRange( -1, 1 );
				break;

			case AnalogInputSubsystem.RANGE_0_2V:
				setVoltRange( 0, 2 );
				break;

			case AnalogInputSubsystem.RANGE_2V:
				setVoltRange( -2, 2 );
				break;

			case AnalogInputSubsystem.RANGE_0_5V:
				setVoltRange( 0, 5 );
				break;

			case AnalogInputSubsystem.RANGE_5V:
				setVoltRange( -5, 5 );
				break;

			case AnalogInputSubsystem.RANGE_0_10V:
				setVoltRange( 0, 10 );
				break;

			case AnalogInputSubsystem.RANGE_10V:
				setVoltRange( -10, 10 );
				break;

			default:
				throw new IllegalArgumentException( "Invalid range: " + range );
		}	// switch( range )
		return super.setRange( range );
	}	// setRange()

}	// class AI16_InputRange

/* end of file */
