/*
 * $RCSfile: DA12_OutputRange.java,v $
 * $Date: 2009/12/17 20:07:07 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

class DA12_OutputRange extends AnalogIORange {

	public DA12_OutputRange( int minCounts, int maxCounts ) {
		super( minCounts, maxCounts );
	}	// DA12_OutputRange()

	public AnalogIORange setRange( int range ) {
		switch( range ) {
			case DA12_AnalogOutputSubsystem.RANGE_0_2_5V:
				setVoltRange( 0, 2.5 );
				break;

			case DA12_AnalogOutputSubsystem.RANGE_2_5V:
				setVoltRange( -2.5, 2.5 );
				break;

			case DA12_AnalogOutputSubsystem.RANGE_0_5V:
				setVoltRange( 0, 5 );
				break;

			case DA12_AnalogOutputSubsystem.RANGE_5V:
				setVoltRange( -5, 5 );
				break;

			case DA12_AnalogOutputSubsystem.RANGE_0_10V:
				setVoltRange( 0, 10 );
				break;

			case DA12_AnalogOutputSubsystem.RANGE_10V:
				setVoltRange( -10, 10 );
				break;

			default:
				throw new IllegalArgumentException( "Invalid range: " + range );
		}	// switch( range )
		return super.setRange( range );
	}	// setRange()

}	// class DA12_OutputRange

/* end of file */
