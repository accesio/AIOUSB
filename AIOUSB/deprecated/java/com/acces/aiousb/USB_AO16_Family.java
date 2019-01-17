/*
 * $RCSfile: USB_AO16_Family.java,v $
 * $Date: 2009/12/23 22:45:27 $
 * $Revision: 1.12 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class USB_AO16_Family represents a USB-AO16-family device, which encompasses the following product IDs:<br>
 * {@link USBDeviceManager#USB_AO16_16A USB_AO16_16A}, {@link USBDeviceManager#USB_AO16_16 USB_AO16_16},
 * {@link USBDeviceManager#USB_AO16_12A USB_AO16_12A}, {@link USBDeviceManager#USB_AO16_12 USB_AO16_12},
 * {@link USBDeviceManager#USB_AO16_8A USB_AO16_8A}, {@link USBDeviceManager#USB_AO16_8 USB_AO16_8},
 * {@link USBDeviceManager#USB_AO16_4A USB_AO16_4A}, {@link USBDeviceManager#USB_AO16_4 USB_AO16_4},<br>
 * {@link USBDeviceManager#USB_AO12_16A USB_AO12_16A}, {@link USBDeviceManager#USB_AO12_16 USB_AO12_16},
 * {@link USBDeviceManager#USB_AO12_12A USB_AO12_12A}, {@link USBDeviceManager#USB_AO12_12 USB_AO12_12},
 * {@link USBDeviceManager#USB_AO12_8A USB_AO12_8A}, {@link USBDeviceManager#USB_AO12_8 USB_AO12_8},
 * {@link USBDeviceManager#USB_AO12_4A USB_AO12_4A}, {@link USBDeviceManager#USB_AO12_4 USB_AO12_4}.<br><br>
 * Instances of class <i>USB_AO16_Family</i> are automatically created by the USB device manager when they are
 * detected on the bus. You should use one of the <i>{@link USBDeviceManager}</i> search methods, such as
 * <i>{@link USBDeviceManager#getDeviceByProductID( int productID ) USBDeviceManager.getDeviceByProductID()}</i>,
 * to obtain a reference to a <i>USB_AO16_Family</i> instance. You can then cast the <i>{@link USBDevice}</i>
 * reference obtained from one of those methods to a <i>USB_AO16_Family</i> and make use of this class' methods, like so:
 * <pre>USBDevice[] devices = deviceManager.getDeviceByProductID( USBDeviceManager.USB_AO16_16A, USBDeviceManager.USB_AO16_4 );
 *if( devices.length > 0 )
 *  USB_AO16_Family device = ( USB_AO16_Family ) devices[ 0 ];</pre>
 */

public class USB_AO16_Family extends USBDevice {

	// {{{ static members
	private static int[] supportedProductIDs;

	static {
		supportedProductIDs = new int[] {
			  USBDeviceManager.USB_AO16_16A
			, USBDeviceManager.USB_AO16_16
			, USBDeviceManager.USB_AO16_12A
			, USBDeviceManager.USB_AO16_12
			, USBDeviceManager.USB_AO16_8A
			, USBDeviceManager.USB_AO16_8
			, USBDeviceManager.USB_AO16_4A
			, USBDeviceManager.USB_AO16_4
			, USBDeviceManager.USB_AO12_16A
			, USBDeviceManager.USB_AO12_16
			, USBDeviceManager.USB_AO12_12A
			, USBDeviceManager.USB_AO12_12
			, USBDeviceManager.USB_AO12_8A
			, USBDeviceManager.USB_AO12_8
			, USBDeviceManager.USB_AO12_4A
			, USBDeviceManager.USB_AO12_4
		};	// supportedProductIDs[]
		Arrays.sort( supportedProductIDs );
	}	// static
	// }}}

	// {{{ protected members
	protected AO16_AnalogOutputSubsystem analogOutputSubsystem;
	protected DigitalIOSubsystem digitalIOSubsystem;
	// }}}

	// {{{ protected methods

	protected USB_AO16_Family( int productID, int deviceIndex ) {
		super( productID, deviceIndex );
		if( ! isSupportedProductID( productID ) )
			throw new IllegalArgumentException( "Invalid product ID: " + productID );
		analogOutputSubsystem = new AO16_AnalogOutputSubsystem( this );
		digitalIOSubsystem = new DigitalIOSubsystem( this );
	}	// USB_AO16_Family()

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
		analogOutputSubsystem.print( stream );
		digitalIOSubsystem.print( stream );
		return stream;
	}	// print()

	/*
	 * subsystems
	 */

	/**
	 * Gets a reference to the analog output subsystem of this device.
	 * @return A reference to the analog output subsystem.
	 */

	public AO16_AnalogOutputSubsystem dac() {
		return analogOutputSubsystem;
	}	// dac()

	/**
	 * Gets a reference to the digital I/O subsystem of this device.
	 * @return A reference to the digital I/O subsystem.
	 */

	public DigitalIOSubsystem dio() {
		return digitalIOSubsystem;
	}	// dio()

	// }}}

}	// class USB_AO16_Family

/* end of file */
