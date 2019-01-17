/*
 * $RCSfile: DigitalIOSubsystem.java,v $
 * $Date: 2010/01/27 00:11:32 $
 * $Revision: 1.14 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class DigitalIOSubsystem represents the digital I/O subsystem of a device. One accesses
 * this analog output subsystem through its parent object, typically through a method such as
 * <i>dio() (see {@link USB_AI16_Family#dio()})</i>.
 */

public class DigitalIOSubsystem extends DeviceSubsystem {

	// {{{ public constants
	/** Number of bits in one byte. A handy number. */
	public static final int BITS_PER_BYTE = 8;
	// }}}

	// {{{ protected members
	protected int numPorts;						// number of digital I/O ports (bytes)
	protected int numChannels;					// number of digital I/O channels (bits)
	protected int numTristateGroups;			// number of tristate groups (bytes)
	protected int numTristateChannels;			// number of tristate channels (bits)
	protected byte[] writeValues;				// last values written to device
	// }}}

	// {{{ protected methods

	protected native int getNumPorts( int deviceIndex );
	protected native int getNumTristateGroups( int deviceIndex );
	protected native int configure( int deviceIndex, boolean tristate, byte[] outputMask, byte[] values );
		// => unsigned long DIO_Configure( unsigned long DeviceIndex, unsigned char bTristate, void *pOutMask, void *pData )
	protected native int configure( int deviceIndex, byte[] tristateMask, byte[] outputMask, byte[] values );
		// => unsigned long DIO_ConfigureEx( unsigned long DeviceIndex, void *pOutMask, void *pData, void *pTristateMask )
	protected native int getConfiguration( int deviceIndex, byte[] tristateMask, byte[] outputMask );
		// => unsigned long DIO_ConfigurationQuery( unsigned long DeviceIndex, void *pOutMask, void *pTristateMask )
	protected native int read1( int deviceIndex, int channel, boolean[] values );
		// => unsigned long DIO_Read1( unsigned long DeviceIndex, unsigned long BitIndex, unsigned char *pBuffer )
	protected native int readAll( int deviceIndex, byte[] values );
		// => unsigned long DIO_ReadAll( unsigned long DeviceIndex, void *Buffer )
	protected native int writeAll( int deviceIndex, byte[] values );
		// => unsigned long DIO_WriteAll( unsigned long DeviceIndex, void *pData )

	protected byte[] bitsToBytes( byte[] dest, int bit, boolean[] src ) {
		assert dest != null
			&& dest.length > 0
			&& src != null
			&& src.length > 0
			&& bit >= 0
			&& ( bit + src.length ) <= dest.length * BITS_PER_BYTE;
		int destByte = bit / BITS_PER_BYTE;
		byte destBit = ( byte ) ( 1 << ( bit % BITS_PER_BYTE ) );
		final byte maxDestBit = ( byte ) ( 1 << ( BITS_PER_BYTE - 1 ) );
		for( int index = 0; index < src.length; index++ ) {
			if( src[ index ] )
				dest[ destByte ] |= destBit;
			else
				dest[ destByte ] &= ~destBit;
			if( ( destBit & maxDestBit ) == 0 ) {
				destBit <<= 1;
			} else {
				// advance to next byte
				destBit = 1;
				destByte++;
			}	// if( ( destBit ...
		}	// for( int index ...
		return dest;
	}	// bitsToBytes()

	protected boolean[] bytesToBits( boolean[] dest, byte[] src, int bit ) {
		assert dest != null
			&& dest.length > 0
			&& src != null
			&& src.length > 0
			&& bit >= 0
			&& ( bit + dest.length ) <= src.length * BITS_PER_BYTE;
		int srcByte = bit / BITS_PER_BYTE;
		byte srcBit = ( byte ) ( 1 << ( bit % BITS_PER_BYTE ) );
		final byte maxSrcBit = ( byte ) ( 1 << ( BITS_PER_BYTE - 1 ) );
		for( int index = 0; index < dest.length; index++ ) {
			dest[ index ] = ( src[ srcByte ] & srcBit ) != 0;
			if( ( srcBit & maxSrcBit ) == 0 ) {
				srcBit <<= 1;
			} else {
				// advance to next byte
				srcBit = 1;
				srcByte++;
			}	// if( ( srcBit ...
		}	// for( int index ...
		return dest;
	}	// bytesToBits()

	protected DigitalIOSubsystem( USBDevice parent ) {
		super( parent );
		numPorts = getNumPorts( getDeviceIndex() );
		numTristateGroups = getNumTristateGroups( getDeviceIndex() );
		assert numPorts > 0;
		numChannels = numPorts * BITS_PER_BYTE;
		numTristateChannels = numTristateGroups * BITS_PER_BYTE;
		writeValues = new byte[ numPorts ];
	}	// DigitalIOSubsystem()

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
			  "    Number of digital I/O ports: " + numPorts + "\n"
			+ "    Number of digital I/O channels: " + numChannels + "\n"
			+ "    Number of tristate groups: " + numTristateGroups + "\n"
			+ "    Number of tristate channels: " + numTristateChannels
		);
		return stream;
	}	// print()

	/**
	 * Gets the number of digital I/O ports (bytes).
	 * @return Number of ports, numbered 0 to n-1.
	 */

	public int getNumPorts() {
		return numPorts;
	}	// getNumPorts()

	/**
	 * Gets the number of digital I/O channels (bits). The number of "channels" is simply equal
	 * to the number of ports times the number of channels per port, which is eight.
	 * @return Number of channels, numbered 0 to n-1.
	 */

	public int getNumChannels() {
		return numChannels;
	}	// getNumChannels()

	/**
	 * Gets the number of tristate groups (bytes).
	 * @return Number of tristate groups, numbered 0 to n-1.
	 */

	public int getNumTristateGroups() {
		return numTristateGroups;
	}	// getNumTristateGroups()

	/**
	 * Gets the number of tristate channels (bits). The number of "channels" is simply equal
	 * to the number of tristate groups times the number of channels per group, which is eight.
	 * @return Number of tristate channels, numbered 0 to n-1.
	 */

	public int getNumTristateChannels() {
		return numTristateChannels;
	}	// getNumTristateChannels()

	/*
	 * configuration
	 */

	/**
	 * Configures the digital I/O ports.
	 * @param tristate <i>true</i> causes all bits on the device to enter tristate (high-impedance) mode;
	 * <i>false</i> removes tristate mode. All devices with this feature power up in tristate mode, and tristate
	 * mode is changed after the remainder of the configuration has occurred.
	 * @param outputs an array of boolean values, one per digital I/O <i>port</i>. Each <i>true</i> value in the array
	 * configures the entire corresponding I/O port as an output port; each <i>false</i> value configures the entire
	 * corresponding I/O port as an input port.
	 * @param values an array of boolean values, one per digital I/O <i>bit</i>, starting with bit 0 of the device (that is,
	 * the least significant bit on the lowest numbered port). Each <i>true</i> value in the array sets the corresponding
	 * output bit to a "1"; each <i>false</i> value sets the corresponding output bit to a "0." The values are written to
	 * the digital output ports before the ports are taken out of tristate mode.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public DigitalIOSubsystem configure( boolean tristate, boolean[] outputs, boolean[] values ) {
		if(
			outputs == null
			|| outputs.length < 1
			|| outputs.length > numPorts
			|| values == null
			|| values.length < 1
			|| values.length > numChannels
		)
			throw new IllegalArgumentException( "Invalid outputs or values" );
		byte[] outputMask = new byte[ ( numPorts + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE ];
		final int result = configure( getDeviceIndex()
			, tristate
			, bitsToBytes( outputMask, 0, outputs )
			, bitsToBytes( writeValues, 0, values ) );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// configure()

	/**
	 * Configures the digital I/O ports. <i>If the device does not support the per-port tristate feature, then
	 * {@link #configure( boolean tristate, boolean[] outputs, boolean[] values ) configure( boolean tristate, ...)}
	 * should be used instead, otherwise an exception will be thrown.</i>
	 * @param tristates an array of boolean values, one per tristate group. Each <i>true</i> value in the array
	 * puts the entire corresponding I/O port into tristate (high-impedance) mode; each <i>false</i> value takes
	 * the entire corresponding I/O port out of tristate mode. All devices with this feature power up in tristate
	 * mode, and tristate mode is changed after the remainder of the configuration has occurred.
	 * @param outputs an array of boolean values, one per digital I/O <i>port</i>. Each <i>true</i> value in the array
	 * configures the entire corresponding I/O port as an output port; each <i>false</i> value configures the entire
	 * corresponding I/O port as an input port.
	 * @param values an array of boolean values, one per digital I/O <i>bit</i>, starting with bit 0 of the device (that is,
	 * the least significant bit on the lowest numbered port). Each <i>true</i> value in the array sets the corresponding
	 * output bit to a "1"; each <i>false</i> value sets the corresponding output bit to a "0." The values are written to
	 * the digital output ports before the ports are taken out of tristate mode.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public DigitalIOSubsystem configure( boolean[] tristates, boolean[] outputs, boolean[] values ) {
		if(
			tristates == null
			|| tristates.length < 1
			|| tristates.length > numTristateGroups
			|| outputs == null
			|| outputs.length < 1
			|| outputs.length > numPorts
			|| values == null
			|| values.length < 1
			|| values.length > numChannels
		)
			throw new IllegalArgumentException( "Invalid tristates, outputs or values" );
		byte[] outputMask = new byte[ ( numPorts + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE ];
		byte[] tristateMask = new byte[ ( numTristateGroups + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE ];
		final int result = configure( getDeviceIndex()
			, bitsToBytes( tristateMask, 0, tristates )
			, bitsToBytes( outputMask, 0, outputs )
			, bitsToBytes( writeValues, 0, values ) );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// configure()

	/**
	 * Gets the current configuration of the digital I/O ports. <i>If the device does not support the per-port tristate
	 * feature, then this method should not be used, otherwise an exception will be thrown.</i>
	 * @param tristates an array of boolean values, one per tristate group, which will receive the current tristate
	 * mode of each tristate group. Each <i>true</i> value returned in the array indicates that the entire corresponding
	 * I/O port is in tristate (high-impedance) mode; each <i>false</i> value indicates that the entire corresponding I/O
	 * port is not in tristate mode. If this parameter is <i>null</i>, then the tristate configuration is not returned.
	 * @param outputs an array of boolean values, one per digital I/O <i>port</i>, which will receive the current output
	 * mode of each I/O port. Each <i>true</i> value returned in the array indicates that the entire corresponding I/O
	 * port is configured as an output port; each <i>false</i> value indicates that the entire corresponding I/O port is
	 * configured as an input port. If this parameter is <i>null</i>, then the output configuration is not returned.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public DigitalIOSubsystem getConfiguration( boolean[] tristates, boolean[] outputs ) {
		if(
			(
				tristates != null
				&& (
					tristates.length < 1
					|| tristates.length > numTristateGroups
				)
			)
			|| (
				outputs != null
				&& (
					outputs.length < 1
					|| outputs.length > numPorts
				)
			)
		)
			throw new IllegalArgumentException( "Invalid tristates or outputs" );
		byte[] outputMask = new byte[ ( numPorts + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE ];
		byte[] tristateMask = new byte[ ( numTristateGroups + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE ];
		final int result = getConfiguration( getDeviceIndex(), tristateMask, outputMask );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		if( tristates != null )
			bytesToBits( tristates, tristateMask, 0 );
		if( outputs != null )
			bytesToBits( outputs, outputMask, 0 );
		return this;
	}	// getConfiguration()

	/*
	 * operations
	 */

	/**
	 * Reads a single digital input channel.
	 * @param channel the channel to read.
	 * @return <i>True</i> indicates that the bit is set ("1"); <i>false</i> indicates that the bit is clear ("0").
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public boolean read( int channel ) {
		if(
			channel < 0
			|| channel >= numChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		boolean[] values = new boolean[ 1 ];
		final int result = read1( getDeviceIndex(), channel, values );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return values[ 0 ];
	}	// read()

	/**
	 * Reads multiple digital input channels.
	 * @param startChannel the first channel to read.
	 * @param numChannels the number of channels to read.
	 * @return An array containing the values read from the specified channels. For each channel, <i>true</i>
	 * indicates that the bit is set ("1"); <i>false</i> indicates that the bit is clear ("0").
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public boolean[] read( int startChannel, int numChannels ) {
		if(
			numChannels < 1
			|| startChannel < 0
			|| startChannel + numChannels > this.numChannels
		)
			throw new IllegalArgumentException( "Invalid start channel: " + startChannel
				+ ", or number of channels: " + numChannels );
		byte[] ports = new byte[ numPorts ];
		final int result = readAll( getDeviceIndex(), ports );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return bytesToBits( new boolean[ numChannels ], ports, startChannel );
	}	// read()

	/**
	 * Writes a single digital output channel.
	 * @param channel the channel to write.
	 * @param value the value to write to the specified channel. <i>True</i> sets the output bit to a "1" and
	 * <i>false</i> clears the output bit to a "0".
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public DigitalIOSubsystem write( int channel, boolean value ) {
		return write( channel, new boolean[] { value } );
	}	// write()

	/**
	 * Writes multiple digital output channels.
	 * @param startChannel the first channel to write.
	 * @param values an array containing the values to write to the specified channels. For each channel,
	 * <i>true</i> sets the output bit to a "1" and <i>false</i> clears the output bit to a "0".
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public DigitalIOSubsystem write( int startChannel, boolean[] values ) {
		if(
			values == null
			|| values.length < 1
			|| startChannel < 0
			|| startChannel + values.length > numChannels
		)
			throw new IllegalArgumentException( "Invalid values or start channel: " + startChannel );
		final int result = writeAll( getDeviceIndex(), bitsToBytes( writeValues, startChannel, values ) );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// write()

	// }}}

}	// class DigitalIOSubsystem

/* end of file */
