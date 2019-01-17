/*
 * $RCSfile: CounterSubsystem.java,v $
 * $Date: 2009/12/17 20:07:07 $
 * $Revision: 1.5 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class CounterSubsystem represents the counter/timer subsystem of a device. One accesses
 * this counter/timer subsystem through its parent object, typically through a method such as
 * <i>ctr() (see {@link USB_AI16_Family#ctr()})</i>.
 */

public class CounterSubsystem extends DeviceSubsystem {

	// {{{ protected members
	protected static final int COUNTERS_PER_BLOCK = 3;

	protected int numCounterBlocks;				// number of counter blocks
	protected int numCounters;					// number of individual counters
	protected Counter[] counters;				// list of counters
	// }}}

	// {{{ protected methods

	protected native int getNumCounterBlocks( int deviceIndex );
	protected native int readLatched( int deviceIndex, char[] counts );
		// => unsigned long CTR_8254ReadLatched( unsigned long DeviceIndex, unsigned short *pData )
	protected native int readAll( int deviceIndex, char[] counts );
		// => unsigned long CTR_8254ReadAll( unsigned long DeviceIndex, unsigned short *pData )
	protected native int selectGate( int deviceIndex, int counter );
		// => unsigned long CTR_8254SelectGate( unsigned long DeviceIndex, unsigned long GateIndex )
	protected native int startClock( int deviceIndex, int counterBlock, double[] clock );
		// => unsigned long CTR_StartOutputFreq( unsigned long DeviceIndex, unsigned long BlockIndex, double *pHz )

	protected CounterSubsystem( USBDevice parent ) {
		super( parent );
		numCounterBlocks = getNumCounterBlocks( getDeviceIndex() );
		assert numCounterBlocks > 0;
		if( numCounterBlocks > 20 )
			numCounterBlocks = 20;				// limit to arbitrary, reasonable number
		numCounters = numCounterBlocks * COUNTERS_PER_BLOCK;
		counters = new Counter[ numCounters ];
		for( int index = 0; index < numCounters; index++ )
			counters[ index ] = new Counter( this, index );
	}	// CounterSubsystem()

	// }}}

	// {{{ public methods

	/*
	 * properties
	 */

	/**
	 * Prints the properties of this subsystem. Mainly useful for diagnostic purposes.
	 * @param stream the print stream where properties will be printed.
	 * @return The print stream.
	 */

	public PrintStream print( PrintStream stream ) {
		stream.println(
			  "    Number of counter blocks: " + numCounterBlocks + "\n"
			+ "    Number of counters: " + numCounters
		);
		return stream;
	}	// print()

	/**
	 * Gets the number of counter blocks. Typically there are three counters per counter block.
	 * @return The number of counter blocks.
	 */

	public int getNumCounterBlocks() {
		return numCounterBlocks;
	}	// getNumCounterBlocks()

	/**
	 * Gets the number of individual counters, indexed from 0 to n-1.
	 * @return The number of individual counters.
	 */

	public int getNumCounters() {
		return numCounters;
	}	// getNumCounters()

	/*
	 * operations
	 */

	/**
	 * Gets a reference to an individual counter. You must obtain a reference to a counter
	 * before you can perform counter operations.
	 * @param counter the counter for which to obtain a reference (0 to n-1).
	 * @return A reference to the specified counter.
	 * @throws IllegalArgumentException
	 */

	public Counter getCounter( int counter ) {
		if(
			counter < 0
			|| counter >= numCounters
		)
			throw new IllegalArgumentException( "Invalid counter: " + counter );
		return counters[ counter ];
	}	// getCounter()

	/**
	 * Reads the current count values of all the counters, optionally including an "old data" indication. If <i>oldData</i>
	 * is <i>true</i>, then an extra word will be returned (one word for each counter, plus one extra word) that contains
	 * an "old data" indication, which is useful for optimizing polling rates. If the value of the final word is zero, then
	 * the data is "old data," meaning you are polling the counters faster than your gate signal is running. If <i>oldData</i>
	 * is <i>false</i>, then only the count values are returned.
	 * @param oldData <i>true</i> includes the "old data" indication in the returned data; <i>false</i> returns just the count values.
	 * @return An array containing the current count values for all the counters plus an optional "old data" indication in the final word.
	 * @throws OperationFailedException
	 */

	public char[] readCounts( boolean oldData ) {
		if( ! ( parent instanceof USB_CTR_15_Family ) )
			throw new OperationFailedException( "Supported only in USB-CTR-15" );
		char[] counts = new char[ oldData ? numCounters : ( numCounters + 1 ) ];
		final int result = oldData
			? readLatched( getDeviceIndex(), counts )	// last word of 'counts' is non-zero if data is "old"
			: readAll( getDeviceIndex(), counts );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return counts;
	}	// readCounts()

	/**
	 * Selects the counter to use as a gate in frequency measurement on other counters.
	 * @param counter the counter to select as a gate (0 to n-1).
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public CounterSubsystem selectGate( int counter ) {
		if( ! ( parent instanceof USB_CTR_15_Family ) )
			throw new OperationFailedException( "Supported only in USB-CTR-15" );
		if(
			counter < 0
			|| counter >= numCounters
		)
			throw new IllegalArgumentException( "Invalid counter: " + counter );
		final int result = selectGate( getDeviceIndex(), counter );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// selectGate()

	/**
	 * Selects an output frequency for a counter block and starts the counters.
	 * <i>{@link #selectGate( int counter ) selectGate()}</i> and <i>{@link #readCounts( boolean oldData ) readCounts( true )}</i>
	 * are used in measuring frequency. To measure frequency one must count pulses for a known duration. In simplest terms,
	 * the number of pulses that occur for 1 second translates directly to Hertz. In the USB-CTR-15 and other supported devices,
	 * you can create a known duration by configuring one counter to act as a "gating" signal for any collection of other counters.
	 * The other "measurement" counters will only count during the "high" side of the gate signal, which we can control.<br><br>
	 * So, to measure frequency you:<br>
	 * 1. Create a gate signal of known duration<br>
	 * 2. Connect this gating signal to the gate pins of all the "measurement" counters<br>
	 * 3. Call <i>{@link #selectGate( int counter ) selectGate()}</i> to tell the board which counter is generating that gate<br>
	 * 4. Call <i>{@link #readCounts( boolean oldData ) readCounts( true )}</i> periodically to read the latched count values from
	 * all the "measurement" counters.<br><br>
	 * In practice, it may not be possible to generate a gating signal of sufficient duration from a single counter. Simply concatenate
	 * two or more counters into a series, or daisy-chain, and use the last counter's output as your gating signal. This last counter
	 * in the chain should be selected as the "gate source" using <i>{@link #selectGate( int counter ) selectGate()}</i>.<br><br>
	 * Once a value has been read from a counter using the <i>{@link #readCounts( boolean oldData ) readCounts( true )}</i> call, it
	 * can be translated into actual Hz by dividing the count value returned by the high-side-duration of the gating signal, in seconds.
	 * For example, if your gate is configured for 10Hz, the high-side lasts 0.05 seconds. If you read 1324 counts via the
	 * <i>{@link #readCounts( boolean oldData ) readCounts( true )}</i> call, the frequency would be "1324 / 0.05", or 26.48 KHz.
	 * @param counterBlock the counter block to use to generate the frequency.
	 * @param clockHz the desired output frequency (in Hertz).
	 * @return The actual frequency that will be generated, limited by the device's capabilities.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public double startClock( int counterBlock, double clockHz ) {
		if(
			counterBlock < 0
			|| counterBlock >= numCounterBlocks
			|| clockHz < 0
		)
			throw new IllegalArgumentException( "Invalid counter block: " + counterBlock
				+ ", or frequency: " + clockHz );
		double[] clock = { clockHz };
		final int result = startClock( getDeviceIndex(), counterBlock, clock );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return clock[ 0 ];
	}	// startClock()

	/**
	 * Halts the counter started by <i>{@link #startClock( int counterBlock, double clockHz ) startClock()}</i>
	 * @param counterBlock the counter block to halt generating a frequency.
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public CounterSubsystem stopClock( int counterBlock ) {
		startClock( counterBlock, 0 );
		return this;
	}	// stopClock()

	// }}}

}	// class CounterSubsystem

/* end of file */
