/*
 * $RCSfile: DIOStreamSubsystem.java,v $
 * $Date: 2010/01/29 00:01:37 $
 * $Revision: 1.4 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class DIOStreamSubsystem represents the digital I/O streaming subsystem of a device. One accesses
 * this counter/timer subsystem through its parent object, typically through a method such as
 * <i>diostream() (see {@link USB_DIO_16_Family#diostream()})</i>.
 */

public class DIOStreamSubsystem extends DeviceSubsystem {

	// {{{ protected members
	protected double clockHz = 0;				// last actual frequency set by setClock()
	// }}}

	// {{{ protected methods

	protected native int setClock( int deviceIndex, boolean directionRead, double[] clockHz );
		// => unsigned long DIO_StreamSetClocks( unsigned long DeviceIndex, double *ReadClockHz, double *WriteClockHz )
	protected native int streamOpen( int deviceIndex, boolean directionRead );
		// => unsigned long DIO_StreamOpen( unsigned long DeviceIndex, unsigned long bIsRead )
	protected native int streamClose( int deviceIndex );
		// => unsigned long DIO_StreamClose( unsigned long DeviceIndex )
	protected native int streamFrame( int deviceIndex, char[] values, int[] samplesTransferred );
		// => unsigned long DIO_StreamFrame( unsigned long DeviceIndex, unsigned long FramePoints, unsigned short *pFrameData, unsigned long *BytesTransferred )

	protected DIOStreamSubsystem( USBDevice parent ) {
		super( parent );
	}	// DIOStreamSubsystem()

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
		stream.println( "    Digital I/O streaming capability installed" );
		return stream;
	}	// print()

	/*
	 * configuration
	 */

	/**
	 * Gets the current streaming block size.
	 * @return The current streaming block size. The value returned may not be the same as the value passed to
	 * <i>{@link #setStreamingBlockSize( int blockSize ) setStreamingBlockSize()}</i> because that value is
	 * rounded up to a whole multiple of 256.
	 * @throws OperationFailedException
	 */

	public int getStreamingBlockSize() {
		return parent.getStreamingBlockSize();
	}	// getStreamingBlockSize()

	/**
	 * Sets the streaming block size.
	 * @param blockSize the streaming block size you wish to set. This will be rounded up to the
	 * next multiple of 256.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public DIOStreamSubsystem setStreamingBlockSize( int blockSize ) {
		parent.setStreamingBlockSize( blockSize );
		return this;
	}	// setStreamingBlockSize()

	/**
	 * Gets the current internal read/write clock speed of a digital I/O stream.
	 * @return The actual frequency that will be generated, based on the last call to
	 * <i>{@link #setClock( boolean directionRead, double clockHz ) setClock()}</i>.
	 */

	public double getClock() {
		return clockHz;
	}	// getClock()

	/**
	 * Sets the internal read/write clock speed of a digital I/O stream <i>(see {@link #getClock() getClock()})</i>.
	 * Only one clock - the read or write clock - may be active at a time, so this method <i><b>automatically turns off</b></i>
	 * the clock not being set by this method. Therefore, do not call this method to explicitly turn off one of the
	 * clocks because it will turn off both of them. Also, when streaming between two devices, only one should have an
	 * active internal clock; the other should have its clocks turned off <i>(see {@link #stopClock() stopClock()})</i>.
	 * @param directionRead <i>true</i> sets read clock; <i>false</i> sets write clock.
	 * @param clockHz the frequency at which to stream the samples (in Hertz).
	 * @return The actual frequency that will be generated, limited by the device's capabilities.
	 * @throws OperationFailedException
	 */

	public double setClock( boolean directionRead, double clockHz ) {
		double[] clock = { clockHz };
		final int result = setClock( getDeviceIndex(), directionRead, clock );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return ( this.clockHz = clock[ 0 ] );
	}	// setClock()

	/**
	 * Stops the internal read/write clocks of a digital I/O stream.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public DIOStreamSubsystem stopClock() {
		setClock( false /* which clock doesn't matter */, 0 );
		return this;
	}	// stopClock()

	/*
	 * operations
	 */

	/**
	 * Opens a digital I/O stream. When you are done using the stream, you must close it by calling <i>{@link #close() close()}</i>.
	 * @param directionRead <i>true</i> open the stream for reading; <i>false</i> open the stream for writing.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public DIOStreamSubsystem open( boolean directionRead ) {
		final int result = streamOpen( getDeviceIndex(), directionRead );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// open()

	/**
	 * Closes a digital I/O stream opened by a call to <i>{@link #open( boolean directionRead ) open()}</i>.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public DIOStreamSubsystem close() {
		final int result = streamClose( getDeviceIndex() );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// close()

	/**
	 * Reads a frame from a digital I/O stream opened by a call to <i>{@link #open( boolean directionRead ) open( true )}</i>.
	 * <i>You cannot read from, and write to a stream. A stream may be read-only or write-only.</i>
	 * @param numSamples the number of samples to read.
	 * @return An array containing the samples read. The array may be smaller than the number of samples requested
	 * if fewer samples were received than were requested.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public char[] read( int numSamples ) {
		if( numSamples < 1 )
			throw new IllegalArgumentException( "Invalid number of samples: " + numSamples );
		char[] values = new char[ numSamples ];
		int[] samplesTransferred = new int[ 1 ];
		final int result = streamFrame( getDeviceIndex(), values, samplesTransferred );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		if( samplesTransferred[ 0 ] != numSamples )
			values = Arrays.copyOf( values, samplesTransferred[ 0 ] );
		return values;
	}	// read()

	/**
	 * Writes a frame to a digital I/O stream opened by a call to <i>{@link #open( boolean directionRead ) open( false )}</i>.
	 * <i>You cannot read from, and write to a stream. A stream may be read-only or write-only.</i>
	 * @param values an array containing the samples to write.
	 * @return The number of samples actually written.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public int write( char[] values ) {
		if(
			values == null
			|| values.length < 1
		)
			throw new IllegalArgumentException( "Invalid values array" );
		int[] samplesTransferred = new int[ 1 ];
		final int result = streamFrame( getDeviceIndex(), values, samplesTransferred );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return samplesTransferred[ 0 ];
	}	// write()

	/**
	 * Clears the streaming FIFO, using one of several different methods.
	 * @param method the method to use when clearing the FIFO. May be one of:<br>
	 * <i>{@link USBDevice#CLEAR_FIFO_METHOD_IMMEDIATE}<br>
	 * {@link USBDevice#CLEAR_FIFO_METHOD_AUTO}<br>
	 * {@link USBDevice#CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT}<br>
	 * {@link USBDevice#CLEAR_FIFO_METHOD_WAIT}</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public DIOStreamSubsystem clearFIFO( int method ) {
		parent.clearFIFO( method );
		return this;
	}	// clearFIFO()

	// }}}

}	// class DIOStreamSubsystem

/* end of file */
