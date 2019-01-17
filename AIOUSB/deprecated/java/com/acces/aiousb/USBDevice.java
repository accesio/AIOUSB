/*
 * $RCSfile: USBDevice.java,v $
 * $Date: 2010/01/18 18:58:39 $
 * $Revision: 1.12 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.lang.*;
import java.util.*;
// }}}

/**
 * Class USBDevice is the abstract super class of all USB device families.
 */

public abstract class USBDevice {
	// {{{ public constants
	/** Size of custom EEPROM area (bytes). */
	public static final int CUSTOM_EEPROM_SIZE = 0x200;

	/** Clear FIFO as soon as command received (and disable auto-clear). */
	public static final int CLEAR_FIFO_METHOD_IMMEDIATE				= 0;
	/** Enable auto-clear FIFO every falling edge of DIO port D bit 1 (on digital boards, analog boards treat as 0). */
	public static final int CLEAR_FIFO_METHOD_AUTO					= 1;
	/** Clear FIFO as soon as command received (and disable auto-clear), and abort stream. */
	public static final int CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT	= 5;
	/** Clear FIFO and wait for it to be emptied. */
	public static final int CLEAR_FIFO_METHOD_WAIT					= 86;
	// }}}

	// {{{ protected members
	protected int deviceIndex;					// device index on bus
	protected int productID;					// 16-bit product ID
	protected String name = "unknown";			// device name
	protected boolean nameValid;				// true == name contains valid value
	protected long serialNumber;				// 64-bit serial number or 0
	protected boolean serialNumberValid;		// true == serialNumber contains valid value
	// }}}

	// {{{ protected methods

	protected native String getName( int deviceIndex );
	protected native int getDeviceSerialNumber( int deviceIndex, long[] serialNumbers );
		// => unsigned long GetDeviceSerialNumber( unsigned long DeviceIndex, __uint64_t *pSerialNumber )
	protected native int getCommTimeout( int deviceIndex );
		// => unsigned AIOUSB_GetCommTimeout( unsigned long DeviceIndex )
	protected native int setCommTimeout( int deviceIndex, int timeout );
		// => unsigned long AIOUSB_SetCommTimeout( unsigned long DeviceIndex, unsigned timeout )
	protected native int reset( int deviceIndex );
		// => unsigned long AIOUSB_Reset( unsigned long DeviceIndex )
	protected native int customEEPROMWrite( int deviceIndex, int address, byte[] data );
		// => unsigned long CustomEEPROMWrite( unsigned long DeviceIndex, unsigned long StartAddress, unsigned long DataSize, void *Data )
	protected native int customEEPROMRead( int deviceIndex, int address, byte[] data );
		// => unsigned long CustomEEPROMRead( unsigned long DeviceIndex, unsigned long StartAddress, unsigned long *DataSize, void *Data );
	protected native int clearFIFO( int deviceIndex, int method );
		// => unsigned long AIOUSB_ClearFIFO( unsigned long DeviceIndex, unsigned long Method )
	protected native double getMiscClock( int deviceIndex );
		// => double AIOUSB_GetMiscClock( unsigned long DeviceIndex )
	protected native int setMiscClock( int deviceIndex, double clockHz );
		// => unsigned long AIOUSB_SetMiscClock( unsigned long DeviceIndex, double clockHz )
	protected native int getStreamingBlockSize( int deviceIndex, int[] blockSize );
		// => unsigned long AIOUSB_GetStreamingBlockSize( unsigned long DeviceIndex, unsigned long *BlockSize )
	protected native int setStreamingBlockSize( int deviceIndex, int blockSize );
		// => unsigned long AIOUSB_SetStreamingBlockSize( unsigned long DeviceIndex, unsigned long BlockSize )

	protected USBDevice clearFIFO( int method ) {
		if(
			method != CLEAR_FIFO_METHOD_IMMEDIATE
			&& method != CLEAR_FIFO_METHOD_AUTO
			&& method != CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT
			&& method != CLEAR_FIFO_METHOD_WAIT
		)
			throw new IllegalArgumentException( "Invalid method: " + method );
		final int result = clearFIFO( deviceIndex, method );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// clearFIFO()

	protected double getMiscClock() {
		return getMiscClock( deviceIndex );
	}	// getMiscClock()

	protected USBDevice setMiscClock( double clockHz ) {
		if( clockHz < 0 )
			throw new IllegalArgumentException( "Invalid clock frequency: " + clockHz );
		final int result = setMiscClock( deviceIndex, clockHz );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setMiscClock()

	protected int getStreamingBlockSize() {
		int[] blockSize = new int[ 1 ];
		final int result = getStreamingBlockSize( deviceIndex, blockSize );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return blockSize[ 0 ];
	}	// getStreamingBlockSize()

	protected USBDevice setStreamingBlockSize( int blockSize ) {
		if( blockSize < 0 )
			throw new IllegalArgumentException( "Invalid block size: " + blockSize );
		final int result = setStreamingBlockSize( deviceIndex, blockSize );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setStreamingBlockSize()

	protected USBDevice( int productID, int deviceIndex ) {
		if(
			productID < USBDeviceManager.MIN_PRODUCT_ID
			|| productID > USBDeviceManager.MAX_PRODUCT_ID
			|| deviceIndex < 0
			|| deviceIndex > 127				// arbitrary sanity check
		)
			throw new IllegalArgumentException( "Invalid product ID: " + productID + ", or device index: " + deviceIndex );
		this.productID = productID;
		this.deviceIndex = deviceIndex;
	}	// USBDevice()

	// }}}

	// {{{ public methods

	/*
	 * properties
	 */

	/**
	 * Prints the properties of this device and all of its subsystems. Mainly useful for diagnostic purposes.
	 * @param stream the print stream where properties will be printed.
	 * @return The print stream.
	 */

	public PrintStream print( PrintStream stream ) {
		stream.println(
			  "  Device at index " + deviceIndex + "\n"
			+ "    Product ID: 0x" + Integer.toString( productID, 16 ) + "\n"
			+ "    Product name: " + getName() + "\n"
			+ "    Serial number: 0x" + Long.toString( getSerialNumber(), 16 )
		);
		return stream;
	}	// print()

	/**
	 * Gets the device's index on the USB bus. The device index isn't used within this Java class library,
	 * but is used in the underlying AIOUSB library. The device index is somewhat useful within this Java
	 * class library to differentiate between multiple devices of the same type.
	 * @return The index of the device on the USB bus.
	 */

	public int getDeviceIndex() {
		return deviceIndex;
	}	// getDeviceIndex()

	/**
	 * Gets the device's product ID.
	 * @return The device product ID.
	 * @see USBDeviceManager#USB_DA12_8A_REV_A Product IDs
	 */

	public int getProductID() {
		return productID;
	}	// getProductID()

	/**
	 * Gets the device's name.
	 * @return The device name.
	 */

	public String getName() {
		if( ! nameValid ) {
			name = getName( deviceIndex );
			nameValid = true;
		}	// if( ! nameValid )
		return name;
	}	// getName()

	/**
	 * Gets the device's serial number.
	 * @return The device serial number (a 64-bit integer).
	 * @throws OperationFailedException
	 */

	public long getSerialNumber() {
		if( ! serialNumberValid ) {
			long[] serialNumbers = new long[ 1 ];
			final int result = getDeviceSerialNumber( deviceIndex, serialNumbers );
			if( result == USBDeviceManager.SUCCESS ) {
				serialNumber = serialNumbers[ 0 ];
				serialNumberValid = true;
			} else
				throw new OperationFailedException( result );
		}	// if( ! serialNumberValid )
		return serialNumber;
	}	// getSerialNumber()

	/*
	 * configuration
	 */

	/**
	 * Gets the current timeout setting for USB communications.
	 * @return Current timeout setting (in milliseconds).
	 * @see #setCommTimeout( int timeout ) setCommTimeout()
	 */

	public int getCommTimeout() {
		return getCommTimeout( deviceIndex );
	}	// getCommTimeout()

	/**
	 * Sets the timeout for USB communications.
	 * @param timeout the new timeout setting (in milliseconds; default is 5,000).
	 * @return This device, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public USBDevice setCommTimeout( int timeout ) {
		if(
			timeout < 0
			|| timeout > 100000					// arbitrary sanity check
		)
			throw new IllegalArgumentException( "Invalid timeout: " + timeout );
		final int result = setCommTimeout( deviceIndex, timeout );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setCommTimeout()

	/*
	 * operations
	 */

	/**
	 * Perform a USB port reset to reinitialize the device.
	 * @return This device, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public USBDevice reset() {
		final int result = reset( deviceIndex );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// reset()

	/**
	 * Writes data to the custom programming area of the device EEPROM. <i>Beware that writing to the EEPROM
	 * is particularly slow.</i> Writing the entire EEPROM may take several seconds. Before initiating a
	 * lengthy EEPROM write procedure, it is recommended that the communication timeout be increased to
	 * at least five seconds, if not ten <i>(see {@link #setCommTimeout( int timeout ) setCommTimeout()})</i>.
	 * Otherwise, a timeout error will occur before the write procedure finishes. Once the write procedure
	 * is finished, you can restore the timeout to a more reasonable value. If you are writing a smaller amount
	 * of data to the EEPROM, you may reduce the timeout proportionately.
	 * @param address starting address from 0x000 to 0x1FF within the EEPROM.
	 * @param data an array of bytes containing the data to write to the EEPROM, beginning at the starting address.
	 * The starting address plus the data size may not exceed the maximum address of 0x1FF.
	 * @return This device, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public USBDevice customEEPROMWrite( int address, byte[] data ) {
		if(
			data == null
			|| data.length < 1
			|| address < 0
			|| address + data.length > CUSTOM_EEPROM_SIZE
		)
			throw new IllegalArgumentException( "Invalid data or address: " + address );
		final int result = customEEPROMWrite( deviceIndex, address, data );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// customEEPROMWrite()

	/**
	 * Reads data from the custom programming area of the device EEPROM.
	 * @param address starting address from 0x000 to 0x1FF within the EEPROM.
	 * @param numBytes the number of bytes to read from the EEPROM, beginning at the starting address.
	 * The starting address plus the number of bytes to read may not exceed the maximum address of 0x1FF.
	 * @return An array of bytes containing the data read from the EEPROM. The length of the array will
	 * be equal to <i>numBytes</i>.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public byte[] customEEPROMRead( int address, int numBytes ) {
		if(
			numBytes < 1
			|| address < 0
			|| address + numBytes > CUSTOM_EEPROM_SIZE
		)
			throw new IllegalArgumentException( "Invalid data or address: " + address );
		byte[] data = new byte[ numBytes ];
		final int result = customEEPROMRead( deviceIndex, address, data );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return data;
	}	// customEEPROMRead()

	// }}}

}	// class USBDevice

/* end of file */
