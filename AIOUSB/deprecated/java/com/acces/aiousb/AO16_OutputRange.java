/*
 * $RCSfile: AO16_OutputRange.java,v $
 * $Date: 2009/12/17 20:07:07 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

class AO16_OutputRange extends AnalogIORange {

	public AO16_OutputRange( int minCounts, int maxCounts ) {
		super( minCounts, maxCounts );
	}	// AO16_OutputRange()

	public AnalogIORange setRange( int range ) {
		switch( range ) {
			case AO16_AnalogOutputSubsystem.RANGE_0_5V:
				setVoltRange( 0, 5 );
				break;

			case AO16_AnalogOutputSubsystem.RANGE_5V:
				setVoltRange( -5, 5 );
				break;

			case AO16_AnalogOutputSubsystem.RANGE_0_10V:
				setVoltRange( 0, 10 );
				break;

			case AO16_AnalogOutputSubsystem.RANGE_10V:
				setVoltRange( -10, 10 );
				break;

			default:
				throw new IllegalArgumentException( "Invalid range: " + range );
		}	// switch( range )
		return super.setRange( range );
	}	// setRange()

}	// class AO16_OutputRange

/* end of file */
