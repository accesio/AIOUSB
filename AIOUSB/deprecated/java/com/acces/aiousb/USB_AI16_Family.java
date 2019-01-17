/*
 * $RCSfile: USB_AI16_Family.java,v $
 * $Date: 2009/12/23 22:45:27 $
 * $Revision: 1.15 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class USB_AI16_Family represents a USB-AI16-family device, which encompasses the following product IDs:<br>
 * {@link USBDeviceManager#USB_AI16_16A USB_AI16_16A}, {@link USBDeviceManager#USB_AI16_16E USB_AI16_16E},
 * {@link USBDeviceManager#USB_AI12_16A USB_AI12_16A}, {@link USBDeviceManager#USB_AI12_16 USB_AI12_16},
 * {@link USBDeviceManager#USB_AI12_16E USB_AI12_16E}, {@link USBDeviceManager#USB_AI16_64MA USB_AI16_64MA},
 * {@link USBDeviceManager#USB_AI16_64ME USB_AI16_64ME}, {@link USBDeviceManager#USB_AI12_64MA USB_AI12_64MA},<br>
 * {@link USBDeviceManager#USB_AI12_64M USB_AI12_64M}, {@link USBDeviceManager#USB_AI12_64ME USB_AI12_64ME},
 * {@link USBDeviceManager#USB_AI16_32A USB_AI16_32A}, {@link USBDeviceManager#USB_AI16_32E USB_AI16_32E},
 * {@link USBDeviceManager#USB_AI12_32A USB_AI12_32A}, {@link USBDeviceManager#USB_AI12_32 USB_AI12_32},
 * {@link USBDeviceManager#USB_AI12_32E USB_AI12_32E}, {@link USBDeviceManager#USB_AI16_64A USB_AI16_64A},<br>
 * {@link USBDeviceManager#USB_AI16_64E USB_AI16_64E}, {@link USBDeviceManager#USB_AI12_64A USB_AI12_64A},
 * {@link USBDeviceManager#USB_AI12_64 USB_AI12_64}, {@link USBDeviceManager#USB_AI12_64E USB_AI12_64E},
 * {@link USBDeviceManager#USB_AI16_96A USB_AI16_96A}, {@link USBDeviceManager#USB_AI16_96E USB_AI16_96E},
 * {@link USBDeviceManager#USB_AI12_96A USB_AI12_96A}, {@link USBDeviceManager#USB_AI12_96 USB_AI12_96},<br>
 * {@link USBDeviceManager#USB_AI12_96E USB_AI12_96E}, {@link USBDeviceManager#USB_AI16_128A USB_AI16_128A},
 * {@link USBDeviceManager#USB_AI16_128E USB_AI16_128E}, {@link USBDeviceManager#USB_AI12_128A USB_AI12_128A},
 * {@link USBDeviceManager#USB_AI12_128 USB_AI12_128}, {@link USBDeviceManager#USB_AI12_128E USB_AI12_128E}.<br><br>
 * Instances of class <i>USB_AI16_Family</i> are automatically created by the USB device manager when they are
 * detected on the bus. You should use one of the <i>{@link USBDeviceManager}</i> search methods, such as
 * <i>{@link USBDeviceManager#getDeviceByProductID( int productID ) USBDeviceManager.getDeviceByProductID()}</i>,
 * to obtain a reference to a <i>USB_AI16_Family</i> instance. You can then cast the <i>{@link USBDevice}</i>
 * reference obtained from one of those methods to a <i>USB_AI16_Family</i> and make use of this class' methods, like so:
 * <pre>USBDevice[] devices = deviceManager.getDeviceByProductID( USBDeviceManager.USB_AI12_32A, USBDeviceManager.USB_AI12_32E );
 *if( devices.length > 0 )
 *  USB_AI16_Family device = ( USB_AI16_Family ) devices[ 0 ];</pre>
 */

public class USB_AI16_Family extends USBDevice {

	// {{{ static members
	private static int[] supportedProductIDs;

	static {
		supportedProductIDs = new int[] {
			  USBDeviceManager.USB_AI16_16A
			, USBDeviceManager.USB_AI16_16E
			, USBDeviceManager.USB_AI12_16A
			, USBDeviceManager.USB_AI12_16
			, USBDeviceManager.USB_AI12_16E
			, USBDeviceManager.USB_AI16_64MA
			, USBDeviceManager.USB_AI16_64ME
			, USBDeviceManager.USB_AI12_64MA
			, USBDeviceManager.USB_AI12_64M
			, USBDeviceManager.USB_AI12_64ME
			, USBDeviceManager.USB_AI16_32A
			, USBDeviceManager.USB_AI16_32E
			, USBDeviceManager.USB_AI12_32A
			, USBDeviceManager.USB_AI12_32
			, USBDeviceManager.USB_AI12_32E
			, USBDeviceManager.USB_AI16_64A
			, USBDeviceManager.USB_AI16_64E
			, USBDeviceManager.USB_AI12_64A
			, USBDeviceManager.USB_AI12_64
			, USBDeviceManager.USB_AI12_64E
			, USBDeviceManager.USB_AI16_96A
			, USBDeviceManager.USB_AI16_96E
			, USBDeviceManager.USB_AI12_96A
			, USBDeviceManager.USB_AI12_96
			, USBDeviceManager.USB_AI12_96E
			, USBDeviceManager.USB_AI16_128A
			, USBDeviceManager.USB_AI16_128E
			, USBDeviceManager.USB_AI12_128A
			, USBDeviceManager.USB_AI12_128
			, USBDeviceManager.USB_AI12_128E
		};	// supportedProductIDs[]
		Arrays.sort( supportedProductIDs );
	}	// static
	// }}}

	// {{{ protected members
	protected AnalogInputSubsystem analogInputSubsystem;
	protected DigitalIOSubsystem digitalIOSubsystem;
	protected CounterSubsystem counterSubsystem;
	// }}}

	// {{{ protected methods

	protected USB_AI16_Family( int productID, int deviceIndex ) {
		super( productID, deviceIndex );
		if( ! isSupportedProductID( productID ) )
			throw new IllegalArgumentException( "Invalid product ID: " + productID );
		analogInputSubsystem = new AnalogInputSubsystem( this );
		digitalIOSubsystem = new DigitalIOSubsystem( this );
		counterSubsystem = new CounterSubsystem( this );
	}	// USB_AI16_Family()

	// }}}

	// {{{ public methods

	/*
	 * properties
	 */

	/**
	 * Gets an array of all the product names supported by this USB device family.
	 * <br><br>Although this method is <i>static</i>, an instance of USBDeviceManager must be created
	 * and be "open" for use before this method can be used. This stipulation is imposed because the
	 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
	 * initialization occurs only when an instance of USBDeviceManager is created and its
	 * <i>{@link USBDeviceManager#open() open()}</i> method is called.
	 * @return An array of product names, sorted in ascending order of product ID.
	 */

	public static String[] getSupportedProductNames() {
		return USBDeviceManager.productIDToName( supportedProductIDs );
	}	// getSupportedProductNames()

	/**
	 * Gets an array of all the product IDs supported by this USB device family.
	 * @return An array of product IDs, sorted in ascending order.
	 */

	public static int[] getSupportedProductIDs() {
		return supportedProductIDs.clone();
	}	// getSupportedProductIDs()

	/**
	 * Tells if a given product ID is supported by this USB device family.
	 * @param productID the product ID to check.
	 * @return <i>True</i> if the given product ID is supported by this USB device family; otherwise, <i>false</i>.
	 */

	public static boolean isSupportedProductID( int productID ) {
		return Arrays.binarySearch( supportedProductIDs, productID ) >= 0;
	}	// isSupportedProductID()

	/**
	 * Prints the properties of this device and all of its subsystems. Mainly useful for diagnostic purposes.
	 * @param stream the print stream where properties will be printed.
	 * @return The print stream.
	 */

	public PrintStream print( PrintStream stream ) {
		super.print( stream );
		analogInputSubsystem.print( stream );
		digitalIOSubsystem.print( stream );
		counterSubsystem.print( stream );
		return stream;
	}	// print()

	/*
	 * subsystems
	 */

	/**
	 * Gets a reference to the analog input subsystem of this device.
	 * @return A reference to the analog input subsystem.
	 */

	public AnalogInputSubsystem adc() {
		return analogInputSubsystem;
	}	// adc()

	/**
	 * Gets a reference to the digital I/O subsystem of this device.
	 * @return A reference to the digital I/O subsystem.
	 */

	public DigitalIOSubsystem dio() {
		return digitalIOSubsystem;
	}	// dio()

	/**
	 * Gets a reference to the counter/timer subsystem of this device.
	 * @return A reference to the counter/timer subsystem.
	 */

	public CounterSubsystem ctr() {
		return counterSubsystem;
	}	// ctr()

	// }}}

}	// class USB_AI16_Family

/* end of file */
