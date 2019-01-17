/*
 * $RCSfile: AO16_AnalogOutputSubsystem.java,v $
 * $Date: 2010/01/25 19:14:58 $
 * $Revision: 1.8 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class AO16_AnalogOutputSubsystem represents the analog output subsystem of a device. One accesses
 * this analog output subsystem through its parent object, typically through a method such as
 * <i>dac() (see {@link USB_AO16_Family#dac()})</i>.
 */

public class AO16_AnalogOutputSubsystem extends AnalogOutputSubsystem {

	// {{{ public constants
	/*
	 * range codes passed to setRange(); these values are defined by the device and must not be changed
	 */
	/** Unipolar, 0-5 volt range <i>(see {@link #setRange( int range ) setRange()})</i>. */
	public static final int RANGE_0_5V	= 0;	// 0-5V

	/** Bipolar, -5 to +5 volt range <i>(see {@link #setRange( int range ) setRange()})</i>. */
	public static final int RANGE_5V	= 1;	// +/-5V

	/** Unipolar, 0-10 volt range <i>(see {@link #setRange( int range ) setRange()})</i>. */
	public static final int RANGE_0_10V	= 2;	// 0-10V

	/** Bipolar, -10 to +10 volt range <i>(see {@link #setRange( int range ) setRange()})</i>. */
	public static final int RANGE_10V	= 3;	// +/-10V

	/** Minimum number of counts D/A can output. */
	public static final int MIN_COUNTS = 0;

	/** Maximum number of counts D/A can output. */
	public static final int MAX_COUNTS = 0xffff;
	// }}}

	// {{{ protected constants
	protected static final String[] RANGE_TEXT = {
		/*
		 * these strings are indexed by RANGE_* constants above
		 */
		  "0-5V"
		, "+/-5V"
		, "0-10V"
		, "+/-10V"
	};	// RANGE_TEXT[]
	// }}}

	// {{{ protected members
	protected AO16_OutputRange outputRange;		// device supports a single range for all D/A channels
	// }}}

	// {{{ protected methods

	protected native int setBoardRange( int deviceIndex, int range );
		// => unsigned long DACSetBoardRange( unsigned long DeviceIndex, unsigned long RangeCode )

	protected AO16_AnalogOutputSubsystem( USBDevice parent ) {
		super( parent );
		minCounts = MIN_COUNTS;
		maxCounts = MAX_COUNTS;
		outputRange = new AO16_OutputRange( MIN_COUNTS, MAX_COUNTS );
		setRange( RANGE_0_5V );					// also sets device to this range
	}	// AO16_AnalogOutputSubsystem()

	// }}}

	// {{{ public methods

	/*
	 * properties
	 */

	/**
	 * Gets the textual string for the specified range.
	 * @param range the range for which to obtain the textual string.
	 * @return The textual string for the specified range.
	 * @see #setRange( int range ) setRange()
	 * @throws IllegalArgumentException
	 */

	public static String getRangeText( int range ) {
		if(
			range < RANGE_0_5V
			|| range > RANGE_10V
		)
			throw new IllegalArgumentException( "Invalid range: " + range );
		return RANGE_TEXT[ range ];
	}	// getRangeText()

	/*
	 * configuration
	 */

	/**
	 * Gets the current voltage range of the D/A outputs.
	 * @return Current voltage range.
	 * @see #setRange( int range ) setRange()
	 */

	public int getRange() {
		return outputRange.getRange();
	}	// getRange()

	/**
	 * Sets the voltage range of the D/A outputs.
	 * @param range the voltage range to select. May be one of:<br>
	 * <i>{@link #RANGE_0_5V}<br>
	 * {@link #RANGE_5V}<br>
	 * {@link #RANGE_0_10V}<br>
	 * {@link #RANGE_10V}</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public AO16_AnalogOutputSubsystem setRange( int range ) {
		outputRange.setRange( range );
		final int result = setBoardRange( getDeviceIndex(), range );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setRange()

	/**
	 * Writes a voltage value to a D/A channel.
	 * @param channel the channel to write to.
	 * @param volts the voltage value to output.
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public AO16_AnalogOutputSubsystem writeVolts( int channel, double volts ) {
		writeCounts( channel, voltsToCounts( volts ) );
		return this;
	}	// writeVolts()

	/**
	 * Writes a block of voltage values to one or more D/A channels.
	 * @param points an array of {@link OutputVoltagePoint} points representing channel-voltage pairs.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AO16_AnalogOutputSubsystem writeVolts( OutputVoltagePoint[] points ) {
		if(
			points == null
			|| points.length < 1
		)
			throw new IllegalArgumentException( "Invalid points array" );

		/*
		 * convert channel/voltage pairs to channel/count pairs
		 */
		char[] countPoints = new char[ points.length * 2 ];
		for( int index = 0; index < points.length; index++ ) {
			if(
				points[ index ].channel >= 0
				&& points[ index ].channel < numChannels
			) {
				countPoints[ index * 2 ] = ( char ) points[ index ].channel;
				countPoints[ index * 2 + 1 ] = voltsToCounts( points[ index ].volts );
			} else
				throw new IllegalArgumentException( "Invalid channel: " + points[ index ].channel );
		}	// for( int index ...

		writeCounts( countPoints );
		return this;
	}	// writeVolts()

	/**
	 * Converts a single D/A count value to volts, based on the current range setting.
	 * @param counts the count value to convert to volts.
	 * @return A voltage value calculated using the current D/A range. The voltage value returned is
	 * constrained to the current minimum-maximum voltage range of the D/A.
	 * <i>(see {@link #setRange( int range ) setRange()})</i>.
	 */

	public double countsToVolts( char counts ) {
		return outputRange.countsToVolts( counts );
	}	// countsToVolts()

	/**
	 * Converts a single voltage value to D/A counts, based on the current range setting.
	 * @param volts the voltage value to convert to counts.
	 * @return A count value calculated using the current D/A range. The count value returned is
	 * constrained to the current minimum-maximum count range of the D/A.
	 * <i>(see {@link #setRange( int range ) setRange()})</i>.
	 */

	public char voltsToCounts( double volts ) {
		return ( char ) outputRange.voltsToCounts( volts );
	}	// voltsToCounts()

	// }}}

}	// class AO16_AnalogOutputSubsystem

/* end of file */
