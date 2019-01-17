/*
 * $RCSfile: USB_DIO_Family.java,v $
 * $Date: 2009/12/23 22:45:27 $
 * $Revision: 1.9 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class USB_DIO_Family represents a USB-DIO-family device, which performs basic digital I/O and
 * encompasses the following product IDs:<br>
 * {@link USBDeviceManager#USB_DIO_48 USB_DIO_48}, {@link USBDeviceManager#USB_DIO_96 USB_DIO_96},
 * {@link USBDeviceManager#USB_IIRO_16 USB_IIRO_16}, {@link USBDeviceManager#USB_II_16 USB_II_16},
 * {@link USBDeviceManager#USB_RO_16 USB_RO_16}, {@link USBDeviceManager#USB_IIRO_8 USB_IIRO_8},
 * {@link USBDeviceManager#USB_II_8 USB_II_8}, {@link USBDeviceManager#USB_IIRO_4 USB_IIRO_4},<br>
 * {@link USBDeviceManager#USB_IDIO_16 USB_IDIO_16}, {@link USBDeviceManager#USB_II_16_OLD USB_II_16_OLD},
 * {@link USBDeviceManager#USB_IDO_16 USB_IDO_16}, {@link USBDeviceManager#USB_IDIO_8 USB_IDIO_8},
 * {@link USBDeviceManager#USB_II_8_OLD USB_II_8_OLD}, {@link USBDeviceManager#USB_IDIO_4 USB_IDIO_4},
 * {@link USBDeviceManager#USB_IIRO4_2SM USB_IIRO4_2SM}, {@link USBDeviceManager#USB_IIRO4_COM USB_IIRO4_COM},<br>
 * {@link USBDeviceManager#USB_DIO16RO8 USB_DIO16RO8}, {@link USBDeviceManager#PICO_DIO16RO8 PICO_DIO16RO8}.<br><br>
 * Instances of class <i>USB_DIO_Family</i> are automatically created by the USB device manager when they are
 * detected on the bus. You should use one of the <i>{@link USBDeviceManager}</i> search methods, such as
 * <i>{@link USBDeviceManager#getDeviceByProductID( int productID ) USBDeviceManager.getDeviceByProductID()}</i>,
 * to obtain a reference to a <i>USB_DIO_Family</i> instance. You can then cast the <i>{@link USBDevice}</i>
 * reference obtained from one of those methods to a <i>USB_DIO_Family</i> and make use of this class' methods, like so:
 * <pre>USBDevice[] devices = deviceManager.getDeviceByProductID( USBDeviceManager.USB_DIO_48, USBDeviceManager.USB_DIO_96 );
 *if( devices.length > 0 )
 *  USB_DIO_Family device = ( USB_DIO_Family ) devices[ 0 ];</pre>
 */

public class USB_DIO_Family extends USBDevice {

	// {{{ static members
	private static int[] supportedProductIDs;

	static {
		supportedProductIDs = new int[] {
			  USBDeviceManager.USB_DIO_48
			, USBDeviceManager.USB_DIO_96
			, USBDeviceManager.USB_IIRO_16
			, USBDeviceManager.USB_II_16
			, USBDeviceManager.USB_RO_16
			, USBDeviceManager.USB_IIRO_8
			, USBDeviceManager.USB_II_8
			, USBDeviceManager.USB_IIRO_4
			, USBDeviceManager.USB_IDIO_16
			, USBDeviceManager.USB_II_16_OLD
			, USBDeviceManager.USB_IDO_16
			, USBDeviceManager.USB_IDIO_8
			, USBDeviceManager.USB_II_8_OLD
			, USBDeviceManager.USB_IDIO_4
			, USBDeviceManager.USB_IIRO4_2SM
			, USBDeviceManager.USB_IIRO4_COM
			, USBDeviceManager.USB_DIO16RO8
			, USBDeviceManager.PICO_DIO16RO8
		};	// supportedProductIDs[]
		Arrays.sort( supportedProductIDs );
	}	// static
	// }}}

	// {{{ protected members
	protected DigitalIOSubsystem digitalIOSubsystem;
	// }}}

	// {{{ protected methods

	protected USB_DIO_Family( int productID, int deviceIndex ) {
		super( productID, deviceIndex );
		if( ! isSupportedProductID( productID ) )
			throw new IllegalArgumentException( "Invalid product ID: " + productID );
		digitalIOSubsystem = new DigitalIOSubsystem( this );
	}	// USB_DIO_Family()

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
		digitalIOSubsystem.print( stream );
		return stream;
	}	// print()

	/*
	 * subsystems
	 */

	/**
	 * Gets a reference to the digital I/O subsystem of this device.
	 * @return A reference to the digital I/O subsystem.
	 */

	public DigitalIOSubsystem dio() {
		return digitalIOSubsystem;
	}	// dio()

	// }}}

}	// class USB_DIO_Family

/* end of file */
