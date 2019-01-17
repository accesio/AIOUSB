/*
 * $RCSfile: AI16_DataPoint.java,v $
 * $Date: 2009/12/25 16:21:51 $
 * $Revision: 1.4 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.text.*;
// }}}

/**
 * Class AI16_DataPoint represents a single data point captured from a {@link USB_AI16_Family} device. It encapsulates
 * not only the captured sample, but the channel from which the sample was captured and the range and differential mode
 * in effect when the sample was captured, providing a fairly complete representation of the captured data. This class
 * also provides methods to retrieve the captured data in either A/D counts or volts.
 */

public class AI16_DataPoint {
	protected int counts;						// A/D count data
	protected int channel;						// channel associated with count data
	protected int range;						// range associated with count data
	protected boolean differentialMode;			// differential mode associated with count data

	protected AI16_DataPoint( int counts, int channel, int range, boolean differentialMode ) {
		this.counts = counts;
		this.channel = channel;
		this.range = range;
		this.differentialMode = differentialMode;
	}	// AI16_DataPoint()

	/**
	 * Gets the channel number from which this data point was captured.
	 * @return The channel number from which this data point was captured.
	 */

	public int getChannel() {
		return channel;
	}	// getChannel()

	/**
	 * Gets the range that was in effect when this data point was captured.
	 * @return The range that was in effect when this data point was captured.
	 * @see AnalogInputSubsystem#getRange( int channel ) AnalogInputSubsystem.getRange()
	 */

	public int getRange() {
		return range;
	}	// getRange()

	/**
	 * Gets the textual representation of the range that was in effect when this data point was captured.
	 * @return The textual representation of the range that was in effect when this data point was captured.
	 */

	public String getRangeText() {
		return AnalogInputSubsystem.getRangeText( range );
	}	// getRangeText()

	/**
	 * Gets the differential/single-ended mode that was in effect when this data point was captured.
	 * @return The differential/single-ended mode that was in effect when this data point was captured.
	 * @see AnalogInputSubsystem#isDifferentialMode( int channel ) AnalogInputSubsystem.isDifferentialMode()
	 */

	public boolean isDifferentialMode() {
		return differentialMode;
	}	// isDifferentialMode()

	/**
	 * Gets the captured data in A/D counts.
	 * @return The captured data in A/D counts.
	 */

	public int getCounts() {
		return counts;
	}	// getCounts()

	/**
	 * Gets the captured data in volts.
	 * @return The captured data in volts.
	 */

	public double getVolts() {
		return ( new AI16_InputRange( AnalogInputSubsystem.MIN_COUNTS, AnalogInputSubsystem.MAX_COUNTS ) )
			.setRange( range ).countsToVolts( counts );
	}	// getVolts()

	/**
	 * Gets a single-line string summary of this data point. Mainly useful for diagnostic purposes.
	 * @return A single-line string summary of this data point.
	 */

	public String toString() {
		StringBuffer buffer = new StringBuffer( 100 );
		buffer.append( "Channel " ).append( new DecimalFormat( "000" ).format( channel ) )
			.append( ", " ).append( new DecimalFormat( "00000" ).format( counts ) ).append( " A/D counts" )
			.append( ", " ).append( new DecimalFormat( " 0.0000000V;-0.0000000V" ).format( getVolts() ) )
			.append( ", " ).append( getRangeText() )
			.append( ", " ).append( differentialMode ? "differential" : "single-ended" );
		return buffer.toString();
	}	// toString()

}	// class AI16_DataPoint

/* end of file */
