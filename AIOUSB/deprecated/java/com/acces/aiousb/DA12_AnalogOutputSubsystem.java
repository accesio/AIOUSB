/*
 * $RCSfile: DA12_AnalogOutputSubsystem.java,v $
 * $Date: 2010/01/25 19:58:14 $
 * $Revision: 1.9 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class DA12_AnalogOutputSubsystem represents the analog output subsystem of a device. One accesses
 * this analog output subsystem through its parent object, typically through a method such as
 * <i>dac() (see {@link USB_DA12_8E_Family#dac()})</i>.
 */

public class DA12_AnalogOutputSubsystem extends AnalogOutputSubsystem {

	// {{{ public constants
	/*
	 * range codes passed to setRange()
	 */
	/** Unipolar, 0-2.5 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_0_2_5V = 0;	// 0-2.5V

	/** Bipolar, -2.5 to +2.5 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_2_5V	= 1;	// +/-2.5V

	/** Unipolar, 0-5 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_0_5V	= 2;	// 0-5V

	/** Bipolar, -5 to +5 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_5V	= 3;	// +/-5V

	/** Unipolar, 0-10 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_0_10V	= 4;	// 0-10V

	/** Bipolar, -10 to +10 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_10V	= 5;	// +/-10V

	/** Minimum number of counts D/A can output. */
	public static final int MIN_COUNTS = 0;

	/** Maximum number of counts D/A can output. */
	public static final int MAX_COUNTS = 0xfff;
	// }}}

	// {{{ protected constants
	protected static final String[] RANGE_TEXT = {
		/*
		 * these strings are indexed by RANGE_* constants above
		 */
		  "0-2.5V"
		, "+/-2.5V"
		, "0-5V"
		, "+/-5V"
		, "0-10V"
		, "+/-10V"
	};	// RANGE_TEXT[]
	// }}}

	// {{{ protected members
	protected DA12_OutputRange[] outputRange;	// device supports a separate range for each D/A channel
	// }}}

	// {{{ protected methods

	protected DA12_AnalogOutputSubsystem( USBDevice parent ) {
		super( parent );
		minCounts = MIN_COUNTS;
		maxCounts = MAX_COUNTS;
		outputRange = new DA12_OutputRange[ numChannels ];
		for( int channel = 0; channel < numChannels; channel++ )
			( outputRange[ channel ] = new DA12_OutputRange( MIN_COUNTS, MAX_COUNTS ) ).setRange( RANGE_0_5V );
	}	// DA12_AnalogOutputSubsystem()

	// }}}

	// {{{ public methods

	/*
	 * properties
	 */

	/**
	 * Gets the textual string for the specified range.
	 * @param range the range for which to obtain the textual string.
	 * @return The textual string for the specified range.
	 * @see #setRange( int channel, int range ) setRange()
	 * @throws IllegalArgumentException
	 */

	public static String getRangeText( int range ) {
		if(
			range < RANGE_0_2_5V
			|| range > RANGE_10V
		)
			throw new IllegalArgumentException( "Invalid range: " + range );
		return RANGE_TEXT[ range ];
	}	// getRangeText()

	/*
	 * configuration
	 */

	/**
	 * Gets the current voltage range of a D/A channel.
	 * @param channel the channel for which to obtain the current range.
	 * @return Current voltage range.
	 * @see #setRange( int channel, int range ) setRange()
	 * @throws IllegalArgumentException
	 */

	public int getRange( int channel ) {
		if(
			channel < 0
			|| channel >= numChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		return outputRange[ channel ].getRange();
	}	// getRange()

	/**
	 * Gets the current voltage range of multiple D/A channels.
	 * @param startChannel the first channel for which to obtain the current range.
	 * @param numChannels the number of channels for which to obtain the current range.
	 * @return Array containing the current range for each of the specified channels.
	 * @see #setRange( int startChannel, int[] range ) setRange()
	 * @throws IllegalArgumentException
	 */

	public int[] getRange( int startChannel, int numChannels ) {
		if(
			numChannels < 1
			|| startChannel < 0
			|| startChannel + numChannels > this.numChannels
		)
			throw new IllegalArgumentException( "Invalid start channel: " + startChannel
				+ ", or number of channels: " + numChannels );
		int[] range = new int[ numChannels ];
		for( int index = 0; index < numChannels; index++ )
			range[ index ] = outputRange[ startChannel + index ].getRange();
		return range;
	}	// getRange()

	/**
	 * Sets the voltage range of a D/A channel. The ranges in this device are selected by means of
	 * hardware jumpers, so these range settings here do not affect the hardware. However, they are
	 * used to perform conversions between volts and counts. Moreover, the range setting is per D/A
	 * channel, so care must be taken when setting the ranges to ensure that the software setting
	 * matches the hardware jumper configuration, otherwise the voltage-count conversions will be
	 * incorrect.
	 * @param channel the channel for which to set the range.
	 * @param range the voltage range to select. May be one of:<br>
	 * <i>{@link #RANGE_0_2_5V}<br>
	 * {@link #RANGE_2_5V}<br>
	 * {@link #RANGE_0_5V}<br>
	 * {@link #RANGE_5V}<br>
	 * {@link #RANGE_0_10V}<br>
	 * {@link #RANGE_10V}</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public DA12_AnalogOutputSubsystem setRange( int channel, int range ) {
		if(
			channel < 0
			|| channel >= numChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		outputRange[ channel ].setRange( range );
		return this;
	}	// setRange()

	/**
	 * Sets the current voltage range of multiple D/A channels.
	 * @param startChannel the first channel for which to set the range.
	 * @param range an array of voltage ranges to select, one per channel. The length of this array
	 * implicitly specifies the number of channels to configure.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @see #setRange( int channel, int range ) setRange()
	 * @throws IllegalArgumentException
	 */

	public DA12_AnalogOutputSubsystem setRange( int startChannel, int[] range ) {
		if(
			range == null
			|| range.length < 1
			|| startChannel < 0
			|| startChannel + range.length > numChannels
		)
			throw new IllegalArgumentException( "Invalid range array or start channel: " + startChannel );
		for( int index = 0; index < range.length; index++ )
			outputRange[ startChannel + index ].setRange( range[ index ] );
		return this;
	}	// setRange()

	/**
	 * Sets the current voltage range of all D/A channels to the same value.
	 * @param range the voltage range to select.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @see #setRange( int channel, int range ) setRange()
	 */

	public DA12_AnalogOutputSubsystem setRange( int range ) {
		for( int channel = 0; channel < numChannels; channel++ )
			outputRange[ channel ].setRange( range );
		return this;
	}	// setRange()

	/**
	 * Writes a voltage value to a D/A channel.
	 * @param channel the channel to write to.
	 * @param volts the voltage value to output.
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public DA12_AnalogOutputSubsystem writeVolts( int channel, double volts ) {
		writeCounts( channel, voltsToCounts( channel, volts ) );
		return this;
	}	// writeVolts()

	/**
	 * Writes a block of voltage values to one or more D/A channels.
	 * @param points an array of {@link OutputVoltagePoint} points representing channel-voltage pairs.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public DA12_AnalogOutputSubsystem writeVolts( OutputVoltagePoint[] points ) {
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
				final int channel = points[ index ].channel;
				countPoints[ index * 2 ] = ( char ) channel;
				countPoints[ index * 2 + 1 ] = voltsToCounts( channel, points[ index ].volts );
			} else
				throw new IllegalArgumentException( "Invalid channel: " + points[ index ].channel );
		}	// for( int index ...

		writeCounts( countPoints );
		return this;
	}	// writeVolts()

	/**
	 * Converts a single D/A count value to volts, based on the current range setting.
	 * @param channel the channel whose current range will be used to perform the conversion.
	 * @param counts the count value to convert to volts.
	 * @return A voltage value calculated using the current D/A range. The voltage value returned is
	 * constrained to the current minimum-maximum voltage range of the D/A.
	 * <i>(see {@link #setRange( int channel, int range ) setRange()})</i>.
	 * @throws IllegalArgumentException
	 */

	public double countsToVolts( int channel, char counts ) {
		if(
			channel < 0
			|| channel >= numChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		return outputRange[ channel ].countsToVolts( counts );
	}	// countsToVolts()

	/**
	 * Converts a single voltage value to D/A counts, based on the current range setting.
	 * @param channel the channel whose current range will be used to perform the conversion.
	 * @param volts the voltage value to convert to counts.
	 * @return A count value calculated using the current D/A range. The count value returned is
	 * constrained to the current minimum-maximum count range of the D/A.
	 * <i>(see {@link #setRange( int channel, int range ) setRange()})</i>.
	 * @throws IllegalArgumentException
	 */

	public char voltsToCounts( int channel, double volts ) {
		if(
			channel < 0
			|| channel >= numChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		return ( char ) outputRange[ channel ].voltsToCounts( volts );
	}	// voltsToCounts()

	// }}}

}	// class DA12_AnalogOutputSubsystem

/* end of file */
