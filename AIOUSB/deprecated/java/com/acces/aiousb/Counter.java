/*
 * $RCSfile: Counter.java,v $
 * $Date: 2009/12/20 23:54:47 $
 * $Revision: 1.6 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class Counter represents a single counter/timer. One accesses a counter through its
 * {@link CounterSubsystem} parent object <i>(see {@link CounterSubsystem#getCounter( int counter ) getCounter()})</i>.
 */

public class Counter {

	// {{{ public constants
	/*
	 * 8254 counter/timer modes
	 */
	/** Mode 0: interrupt on terminal count <i>(see {@link #setMode( int mode ) setMode()})</i>. */
	public static final int MODE_TERMINAL_COUNT		= 0;	// mode 0: interrupt on terminal count
	/** Mode 1: hardware retriggerable one-shot <i>(see {@link #setMode( int mode ) setMode()})</i>. */
	public static final int MODE_ONE_SHOT			= 1;	// mode 1: hardware retriggerable one-shot
	/** Mode 2: rate generator <i>(see {@link #setMode( int mode ) setMode()})</i>. */
	public static final int MODE_RATE_GENERATOR		= 2;	// mode 2: rate generator
	/** Mode 3: square wave mode <i>(see {@link #setMode( int mode ) setMode()})</i>. */
	public static final int MODE_SQUARE_WAVE		= 3;	// mode 3: square wave mode
	/** Mode 4: software triggered mode <i>(see {@link #setMode( int mode ) setMode()})</i>. */
	public static final int MODE_SW_TRIGGERED		= 4;	// mode 4: software triggered mode
	/** Mode 5: hardware triggered strobe (retriggerable) <i>(see {@link #setMode( int mode ) setMode()})</i>. */
	public static final int MODE_HW_TRIGGERED		= 5;	// mode 5: hardware triggered strobe (retriggerable)
	// }}}

	// {{{ protected members
	protected CounterSubsystem parent;			// subsystem that this counter belongs to
	protected int counterIndex;					// counter index (using 0-based counter addressing)
	// }}}

	// {{{ protected methods

	protected native int setMode( int deviceIndex, int blockIndex, int counterIndex, int mode );
		// => unsigned long CTR_8254Mode( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned long Mode )
	protected native int setCount( int deviceIndex, int blockIndex, int counterIndex, char count );
		// => unsigned long CTR_8254Load( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned short LoadValue )
	protected native int setModeAndCount( int deviceIndex, int blockIndex, int counterIndex, int mode, char count );
		// => unsigned long CTR_8254ModeLoad( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned long Mode, unsigned short LoadValue )
	protected native int readCount( int deviceIndex, int blockIndex, int counterIndex, char[] count );
		// => unsigned long CTR_8254Read( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned short *pReadValue )
	protected native int readCountAndStatus( int deviceIndex, int blockIndex, int counterIndex, char[] countStatus );
		// => unsigned long CTR_8254ReadStatus( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned short *pReadValue, unsigned char *pStatus )
	protected native int readCountAndSetModeAndCount( int deviceIndex, int blockIndex, int counterIndex, int mode, char count, char[] prevCount );
		// => unsigned long CTR_8254ReadModeLoad( unsigned long DeviceIndex, unsigned long BlockIndex, unsigned long CounterIndex, unsigned long Mode, unsigned short LoadValue, unsigned short *pReadValue )

	protected Counter( CounterSubsystem parent, int counterIndex ) {
		assert parent != null
			&& counterIndex >= 0;
		this.parent = parent;
		this.counterIndex = counterIndex;
	}	// Counter()

	// }}}

	// {{{ public methods

	/**
	 * Gets the index of the parent device on the USB bus. Used internally in calls to the underlying API.
	 * @return The index of the parent device on the USB bus.
	 */

	public int getDeviceIndex() {
		assert parent != null;
		return parent.getDeviceIndex();
	}	// getDeviceIndex()

	/**
	 * Sets the counter's mode.
	 * @param mode the counter mode. May be one of:<br>
	 * <i>{@link #MODE_TERMINAL_COUNT}<br>
	 * {@link #MODE_ONE_SHOT}<br>
	 * {@link #MODE_RATE_GENERATOR}<br>
	 * {@link #MODE_SQUARE_WAVE}<br>
	 * {@link #MODE_SW_TRIGGERED}<br>
	 * {@link #MODE_HW_TRIGGERED}</i>
	 * @return This counter, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public Counter setMode( int mode ) {
		assert counterIndex >= 0;
		if(
			mode < MODE_TERMINAL_COUNT
			|| mode > MODE_HW_TRIGGERED
		)
			throw new IllegalArgumentException( "Invalid mode: " + mode );
		final int result = setMode( getDeviceIndex(), 0 /* blockIndex */, counterIndex, mode );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setMode()

	/**
	 * Loads a count value into the counter.
	 * @param count the count value (0-65,535) to load into the counter.
	 * @return This counter, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public Counter setCount( char count ) {
		if( ! ( parent.parent instanceof USB_CTR_15_Family ) )
			throw new OperationFailedException( "Supported only in USB-CTR-15" );
		assert counterIndex >= 0;
		final int result = setCount( getDeviceIndex(), 0 /* blockIndex */, counterIndex, count );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setCount()

	/**
	 * Sets a counter mode and loads a count value into the counter.
	 * @param mode the counter mode <i>(see {@link #setMode( int mode ) setMode()})</i>.
	 * @param count the count value (0-65,535) to load into the counter.
	 * @return This counter, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public Counter setModeAndCount( int mode, char count ) {
		assert counterIndex >= 0;
		if(
			mode < MODE_TERMINAL_COUNT
			|| mode > MODE_HW_TRIGGERED
		)
			throw new IllegalArgumentException( "Invalid mode: " + mode );
		final int result = setModeAndCount( getDeviceIndex(), 0 /* blockIndex */, counterIndex, mode, count );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setModeAndCount()

	/**
	 * Reads a counter's current count value.
	 * @return The current count value (0-65,535).
	 * @throws OperationFailedException
	 */

	public char readCount() {
		assert counterIndex >= 0;
		char[] count = new char[ 1 ];
		final int result = readCount( getDeviceIndex(), 0 /* blockIndex */, counterIndex, count );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return count[ 0 ];
	}	// readCount()

	/**
	 * Reads a counter's current count value and status.
	 * @return An array of 2 16-bit integers:<br>
	 * char[ 0 ] contains the current count value (0-65,535)<br>
	 * char[ 1 ] contains the current counter status (0-255)
	 * @throws OperationFailedException
	 */

	public char[] readCountAndStatus() {
		assert counterIndex >= 0;
		char[] countStatus = new char[ 2 ];			// for count and status
		final int result = readCountAndStatus( getDeviceIndex(), 0 /* blockIndex */, counterIndex, countStatus );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return countStatus;
	}	// readCountAndStatus()

	/**
	 * Reads a counter's current count value, <i>then</i> sets a new mode and loads a new count value into the counter.
	 * @param mode the counter mode <i>(see {@link #setMode( int mode ) setMode()})</i>.
	 * @param count the count value (0-65,535) to load into the counter.
	 * @return The 16-bit count value (0-65,535) prior to setting the new mode and count.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public char readCountAndSetModeAndCount( int mode, char count ) {
		assert counterIndex >= 0;
		if(
			mode < MODE_TERMINAL_COUNT
			|| mode > MODE_HW_TRIGGERED
		)
			throw new IllegalArgumentException( "Invalid mode: " + mode );
		char[] prevCount = new char[ 1 ];
		final int result = readCountAndSetModeAndCount( getDeviceIndex(), 0 /* blockIndex */, counterIndex, mode, count, prevCount );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return prevCount[ 0 ];
	}	// readCountAndSetModeAndCount()

	// }}}

}	// class Counter

/* end of file */
