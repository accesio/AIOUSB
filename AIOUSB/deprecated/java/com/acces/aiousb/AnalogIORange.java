/*
 * $RCSfile: AnalogIORange.java,v $
 * $Date: 2009/12/17 21:01:59 $
 * $Revision: 1.4 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.lang.Math;
import java.util.*;
// }}}

/**
 * Class AnalogIORange helps manage analog I/O range settings and provides voltage-count conversion utilities.
 * A single instance can be used with devices that support just one range, or multiple instances can be used
 * with devices that support multiple ranges, such as a separate range per analog I/O channel. This class also
 * supports changing the range properties. Some devices, for instance, permit the range to be changed at run-time.
 * The class that owns this instance can change the range by calling one or more of the methods of this class.
 * Or, for devices that do not support changing the range, the properties can be set up once and left alone. Or,
 * some properties can be changed and others left alone. For example, devices that permit changing the voltage
 * range usually use a fixed count range. 
 */

class AnalogIORange {

	// {{{ protected members
	protected int range;						// current range (meaning defined by class that owns this instance)
	protected int minCounts = 0;				// minimum A/D or D/A counts for current range (typically 0)
	protected int maxCounts = 0xffff;			// maximum A/D or D/A counts for current range (typically 0xfff or 0xffff)
	protected int rangeCounts = 0xffff;			// count range (maxCounts - minCounts), to avoid repeatedly calculating this
	protected double minVolts = -10;			// minimum volts for current range
	protected double maxVolts = 10;				// maximum volts for current range
	protected double rangeVolts = 20;			// voltage range (maxVolts - minVolts), to avoid repeatedly calculating this
	// }}}

	/**
	 * Constructor which uses the default properties.
	 */

	public AnalogIORange() {
		// nothing to do
	}	// AnalogIORange()

	/**
	 * Constructor which sets the count range. The count range is usually constant, so setting it one time
	 * in the constructor is convenient.
	 * @param minCounts minimum counts for current range.
	 * @param maxCounts maximum counts for current range.
	 */

	public AnalogIORange( int minCounts, int maxCounts ) {
		setCountRange( minCounts, maxCounts );
	}	// AnalogIORange()

	/**
	 * Gets the current range ID.
	 * @return Current range ID (defined by class that owns this instance).
	 */

	public int getRange() {
		return range;
	}	// getRange()

	/**
	 * Sets the range ID.
	 * @param range the new range ID (defined by class that owns this instance).
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public AnalogIORange setRange( int range ) {
		this.range = range;
		return this;
	}	// setRange()

	/**
	 * Sets the A/D or D/A count range.
	 * @param minCounts minimum counts for current range.
	 * @param maxCounts maximum counts for current range.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogIORange setCountRange( int minCounts, int maxCounts ) {
		if( minCounts >= maxCounts )
			throw new IllegalArgumentException( "Invalid count range: " + minCounts + " - " + maxCounts );
		this.minCounts = minCounts;
		this.maxCounts = maxCounts;
		rangeCounts = maxCounts - minCounts;
		return this;
	}	// setCountRange()

	/**
	 * Sets the voltage range.
	 * @param minVolts minimum volts for current range.
	 * @param maxVolts maximum volts for current range.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogIORange setVoltRange( double minVolts, double maxVolts ) {
		if( minVolts >= maxVolts )
			throw new IllegalArgumentException( "Invalid voltage range: " + minVolts + " - " + maxVolts );
		this.minVolts = minVolts;
		this.maxVolts = maxVolts;
		rangeVolts = maxVolts - minVolts;
		return this;
	}	// setVoltRange()

	/**
	 * Converts a single A/D or D/A count value to volts, based on the current range setting.
	 * @param counts the count value to convert to volts.
	 * @return A voltage value calculated using the current range. The voltage value returned is
	 * constrained to the current minimum-maximum voltage range.
	 */

	public double countsToVolts( int counts ) {
		assert minVolts < maxVolts
			&& rangeVolts > 0
			&& minCounts < maxCounts
			&& rangeCounts > 0;
		/*
		 *          ( counts - minCounts )
		 * volts = ------------------------- x ( maxVolts - minVolts ) + minVolts
		 *         ( maxCounts - minCounts )
		 */
		double volts = ( ( ( double ) counts - minCounts ) / rangeCounts ) * rangeVolts + minVolts;
		if( volts < minVolts )
			volts = minVolts;
		else if( volts > maxVolts )
			volts = maxVolts;
		return volts;
	}	// countsToVolts()

	/**
	 * Converts a single voltage value to A/D or D/A counts, based on the current range setting.
	 * @param volts the voltage value to convert to counts.
	 * @return A count value calculated using the current D/A range. The count value returned is
	 * constrained to the current minimum-maximum count range.
	 */

	public int voltsToCounts( double volts ) {
		assert minVolts < maxVolts
			&& rangeVolts > 0
			&& minCounts < maxCounts
			&& rangeCounts > 0;
		/*
		 *           ( volts - minVolts )
		 * counts = ----------------------- x ( maxCounts - minCounts ) + minCounts
		 *          ( maxVolts - minVolts )
		 */
		int counts = ( int ) Math.round( ( ( volts - minVolts ) / rangeVolts ) * rangeCounts + minCounts );
		if( counts < minCounts )
			counts = minCounts;
		else if( counts > maxCounts )
			counts = maxCounts;
		return counts;
	}	// voltsToCounts()

}	// class AnalogIORange

/* end of file */
