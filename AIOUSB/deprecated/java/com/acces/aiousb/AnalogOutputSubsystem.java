/*
 * $RCSfile: AnalogOutputSubsystem.java,v $
 * $Date: 2010/01/25 19:14:58 $
 * $Revision: 1.14 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class AnalogOutputSubsystem is the superclass of the analog output subsystem of a device. This
 * class provides basic features, such as writing count values to the D/As. More sophisticated
 * functions are provided by device-specific subclasses. One accesses this analog output subsystem
 * through its parent object, typically through a method such as <i>dac() (see {@link USB_AO16_Family#dac()})</i>.
 */

public class AnalogOutputSubsystem extends DeviceSubsystem {

	// {{{ protected members
	protected int numChannels;					// number of D/A channels
	protected int minCounts = 0;				// minimum counts D/A can output (typically 0)
	protected int maxCounts = 0xffff;			// maximum counts D/A can output (typically 0xfff or 0xffff)
	// }}}

	// {{{ protected methods

	protected native int getNumChannels( int deviceIndex );
	protected native int writeCounts( int deviceIndex, int channel, char counts );
		// => unsigned long DACDirect( unsigned long DeviceIndex, unsigned short Channel, unsigned short Value )
	protected native int writeCounts( int deviceIndex, char[] points );
		// => unsigned long DACMultiDirect( unsigned long DeviceIndex, unsigned short *pDACData, unsigned long DACDataCount )

	protected AnalogOutputSubsystem( USBDevice parent ) {
		super( parent );
		numChannels = getNumChannels( getDeviceIndex() );
		assert numChannels > 0;
	}	// AnalogOutputSubsystem()

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
			  "    Number of D/A channels: " + numChannels + "\n"
			+ "    D/A count range: " + Integer.toHexString( minCounts ) + "-" + Integer.toHexString( maxCounts )
		);
		return stream;
	}	// print()

	/**
	 * Gets the number of analog output channels.
	 * @return Number of channels, numbered 0 to n-1.
	 */

	public int getNumChannels() {
		return numChannels;
	}	// getNumChannels()

	/*
	 * operations
	 */

	/**
	 * Writes a count value to a D/A channel.
	 * @param channel the channel to write to.
	 * @param counts the D/A count value to output. The number of bits of resolution for the D/A outputs varies from
	 * model to model, however it's usually 12 or 16 bits. Moreover, some of the 12-bit models actually accept a
	 * 16-bit value and simply truncate the least significant 4 bits. Consult the manual for the specific device to
	 * determine the range of D/A values the device will accept. In general, 12-bit devices accept a count range of
	 * 0-0xfff, and 16-bit devices accept a count range of 0-0xffff.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public AnalogOutputSubsystem writeCounts( int channel, char counts ) {
		if(
			channel < 0
			|| channel >= numChannels
			|| counts < minCounts
			|| counts > maxCounts
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel + ", or counts: " + counts );
		final int result = writeCounts( getDeviceIndex(), channel, counts );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// writeCounts()

	/**
	 * Writes a block of count values to one or more D/A channels.
	 * @param points an array of 16-bit integers representing channel-count pairs. The first integer of each pair
	 * is the D/A channel number and the second integer is the D/A count value to output to the specified channel.
	 * Refer to <i>{@link #writeCounts( int channel, char counts ) writeCounts()}</i> for an explanation of the channel addressing
	 * and count values.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public AnalogOutputSubsystem writeCounts( char[] points ) {
		if(
			points == null
			|| points.length < 2
			|| ( points.length & 1 ) != 0
		)
			throw new IllegalArgumentException( "Invalid points array" );

		/*
		 * check all channel/count pairs
		 */
		for( int index = 0; index < points.length; index += 2 ) {
			if(
				points[ index ] < 0
				|| points[ index ] >= numChannels
				|| points[ index + 1 ] < minCounts
				|| points[ index + 1 ] > maxCounts
			)
				throw new IllegalArgumentException( "Invalid channel: " + points[ index ] + ", or counts: " + points[ index + 1 ] );
		}	// for( int index ...

		final int result = writeCounts( getDeviceIndex(), points );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// writeCounts()

	// }}}

}	// class AnalogOutputSubsystem

/* end of file */
