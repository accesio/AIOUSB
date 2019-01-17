/*
 * $RCSfile: USBDeviceManager.java,v $
 * $Date: 2010/01/18 18:58:39 $
 * $Revision: 1.23 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.lang.*;
import java.util.*;
// }}}

/**
 * Class USBDeviceManager manages all the USB devices on the bus. It scans the bus and builds
 * a list of all the devices found. It also initializes and terminates use of the underlying
 * AIOUSB module.
 */

public class USBDeviceManager {

	// {{{ public constants
	/** The version number of this Java class library. */
	public static final String VERSION_NUMBER = "1.7";

	/** The version date of this Java class library. */
	public static final String VERSION_DATE = "18 January 2010";

	/** Indicates that underlying AIOUSB module operation succeeded. */
	public static final int SUCCESS = 0;
	// }}}

	// {{{ product IDs
	public static final int MIN_PRODUCT_ID				= 0;
	public static final int MAX_PRODUCT_ID				= 0xffff;

	/*
	 * these product IDs are constant
	 */
	 /** Supported by {@link USB_DA12_8A_Family}. */
	public static final int USB_DA12_8A_REV_A			= 0x4001;
	 /** Supported by {@link USB_DA12_8A_Family}. */
	public static final int USB_DA12_8A					= 0x4002;
	 /** Supported by {@link USB_DA12_8E_Family}. */
	public static final int USB_DA12_8E					= 0x4003;

	/*
	 * these are the product IDs after firmware is uploaded to the device; prior
	 * to uploading the firmware, the product ID is that shown below minus 0x8000
	 */
	 /** Supported by {@link USB_DIO_32_Family}. */
	public static final int USB_DIO_32					= 0x8001;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_DIO_48					= 0x8002;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_DIO_96					= 0x8003;
	 /** Supported by {@link USB_DIO_16_Family}. */
	public static final int USB_DI16A_REV_A1			= 0x8008;
	 /** Supported by {@link USB_DIO_16_Family}. */
	public static final int USB_DO16A_REV_A1			= 0x8009;
	 /** Supported by {@link USB_DIO_16_Family}. */
	public static final int USB_DI16A_REV_A2			= 0x800a;
	 /** Supported by {@link USB_DIO_16_Family}. */
	public static final int USB_DIO_16H					= 0x800c;
	 /** Supported by {@link USB_DIO_16_Family}. */
	public static final int USB_DI16A					= 0x800d;
	 /** Supported by {@link USB_DIO_16_Family}. */
	public static final int USB_DO16A					= 0x800e;
	 /** Supported by {@link USB_DIO_16_Family}. */
	public static final int USB_DIO_16A					= 0x800f;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IIRO_16					= 0x8010;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_II_16					= 0x8011;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_RO_16					= 0x8012;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IIRO_8					= 0x8014;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_II_8					= 0x8015;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IIRO_4					= 0x8016;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IDIO_16					= 0x8018;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_II_16_OLD				= 0x8019;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IDO_16					= 0x801a;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IDIO_8					= 0x801c;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_II_8_OLD				= 0x801d;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IDIO_4					= 0x801e;
	 /** Supported by {@link USB_CTR_15_Family}. */
	public static final int USB_CTR_15					= 0x8020;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IIRO4_2SM				= 0x8030;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_IIRO4_COM				= 0x8031;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int USB_DIO16RO8				= 0x8032;
	 /** Supported by {@link USB_DIO_Family}. */
	public static final int PICO_DIO16RO8				= 0x8033;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_16A				= 0x8040;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_16E				= 0x8041;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_16A				= 0x8042;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_16					= 0x8043;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_16E				= 0x8044;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_64MA				= 0x8045;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_64ME				= 0x8046;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_64MA				= 0x8047;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_64M				= 0x8048;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_64ME				= 0x8049;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_32A				= 0x804a;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_32E				= 0x804b;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_32A				= 0x804c;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_32					= 0x804d;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_32E				= 0x804e;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_64A				= 0x804f;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_64E				= 0x8050;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_64A				= 0x8051;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_64					= 0x8052;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_64E				= 0x8053;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_96A				= 0x8054;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_96E				= 0x8055;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_96A				= 0x8056;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_96					= 0x8057;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_96E				= 0x8058;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_128A				= 0x8059;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI16_128E				= 0x805a;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_128A				= 0x805b;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_128				= 0x805c;
	 /** Supported by {@link USB_AI16_Family}. */
	public static final int USB_AI12_128E				= 0x805d;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO16_16A				= 0x8060;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO16_16					= 0x8061;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO16_12A				= 0x8062;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO16_12					= 0x8063;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO16_8A					= 0x8064;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO16_8					= 0x8065;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO16_4A					= 0x8066;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO16_4					= 0x8067;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO12_16A				= 0x8068;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO12_16					= 0x8069;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO12_12A				= 0x806a;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO12_12					= 0x806b;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO12_8A					= 0x806c;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO12_8					= 0x806d;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO12_4A					= 0x806e;
	 /** Supported by {@link USB_AO16_Family}. */
	public static final int USB_AO12_4					= 0x806f;
	// }}}

	// {{{ protected members
	protected Vector<USBDevice> deviceList = new Vector<USBDevice>();
	protected static final int OPEN_PATTERN = 0x786938f5;	// unique, random value
	protected int openStatus = 0;				// if openStatus == OPEN_PATTERN, module is initialized
	protected static final String MESSAGE_NOT_OPEN = "Not open, must call open() first";
	// }}}

	// {{{ static initializers
	static {
		System.loadLibrary( "javaaiousb" );		// libjavaaiousb.so
	}	// static
	// }}}

	// {{{ protected methods

	protected native int init();				// => unsigned long AIOUSB_Init()
	protected native void exit();				// => void AIOUSB_Exit()
	protected native int getDeviceByProductID( int minProductID, int maxProductID, int[] devices );
		// => unsigned long AIOUSB_GetDeviceByProductID( int minProductID, int maxProductID, int maxDevices, int *deviceList )
	protected static native String libProductIDToName( int productID );
		// => const char *ProductIDToName( unsigned int productID )
	protected static native int libProductNameToID( String name );
		// => unsigned int ProductNameToID( const char *name )

	protected void finalize() throws Throwable {
		if( isOpen() )
			close();							// in case user forgot
	}	// finalize()

	// }}}

	// {{{ public methods

	/**
	 * Constructor for USB device manager. You must call <i>{@link #open() open()}</i> before using the device manager.
	 */

	public USBDeviceManager() {
		// nothing to do
	}	// USBDeviceManager()

	/*
	 * properties
	 */

	/**
	 * Prints the properties of this device manager and all of the devices found on the bus to
	 * the specified print stream. Mainly useful for diagnostic purposes.
	 * @param stream the print stream where properties will be printed.
	 * @return The print stream.
	 */

	public PrintStream print( PrintStream stream ) {
		if( ! isOpen() )
			throw new OperationFailedException( MESSAGE_NOT_OPEN );
		if( deviceList.size() > 0 ) {
			stream.println( "ACCES devices found:" );
			for( USBDevice device : deviceList )
				device.print( stream );
		} else
			stream.println( "No ACCES devices found" );
		return stream;
	}	// print()

	/**
	 * Prints the properties of this device manager and all of the devices found on the bus to
	 * the standard output device. Mainly useful for diagnostic purposes.
	 * @return This device manager, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public USBDeviceManager printDevices() {
		print( System.out );
		return this;
	}	// printDevices()

	/**
	 * Gets the version number of the underlying AIOUSB module.
	 * @return The AIOUSB module version number as a string with the form, "1.78".
	 */

	public native String getAIOUSBVersion();		// => const char *AIOUSB_GetVersion()

	/**
	 * Gets the version date of the underlying AIOUSB module.
	 * @return The AIOUSB module version date as a string with the form, "15 November 2009".
	 */

	public native String getAIOUSBVersionDate();	// => const char *AIOUSB_GetVersionDate()

	/*
	 * utilities
	 */

	/**
	 * Gets the product name for a product ID. This name is only "approximate," as an actual device
	 * reports its own name. Generally the names reported by the device are the same as those obtained
	 * from this method, but that is not guaranteed. This method provides a name that constitutes a
	 * user-friendly alternative to a product ID number. The complement of this method is
	 * <i>{@link #productNameToID( String name ) productNameToID()}</i>.
	 * <br><br>Although this method is <i>static</i>, an instance of USBDeviceManager must be created
	 * and be "open" for use before this method can be used. This stipulation is imposed because the
	 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
	 * initialization occurs only when an instance of USBDeviceManager is created and its
	 * <i>{@link #open() open()}</i> method is called.
	 * @param productID the product ID to translate to a product name.
	 * @return A string containing the product name, or "UNKNOWN" if the product ID was not found.
	 * @throws IllegalArgumentException
	 */

	public static String productIDToName( int productID ) {
		if(
			productID < MIN_PRODUCT_ID
			|| productID > MAX_PRODUCT_ID
		)
			throw new IllegalArgumentException( "Invalid product ID: " + productID );
		return libProductIDToName( productID );
	}	// productIDToName()

	/**
	 * Gets the product names for an array of product IDs. Functionally identical to
	 * <i>{@link #productIDToName( int productID ) productIDToName()}</i> except that it operates
	 * on an array of product IDs rather than an individual product ID.
	 * <br><br>Although this method is <i>static</i>, an instance of USBDeviceManager must be created
	 * and be "open" for use before this method can be used. This stipulation is imposed because the
	 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
	 * initialization occurs only when an instance of USBDeviceManager is created and its
	 * <i>{@link #open() open()}</i> method is called.
	 * @param productID an array of product IDs to translate to product names.
	 * @return An array of strings containing the product names, or "UNKNOWN" for any product ID that
	 * was not found. The product names are returned in the same order as the product IDs passed in
	 * <i>productID[]</i>.
	 * @throws IllegalArgumentException
	 */

	public static String[] productIDToName( int[] productID ) {
		String[] productName = new String[ productID.length ];
		for( int index = 0; index < productID.length; index++ )
			productName[ index ] = productIDToName( productID[ index ] );
		return productName;
	}	// productIDToName()

	/**
	 * Gets the product ID for a product name. This method is the complement of
	 * <i>{@link #productIDToName( int productID ) productIDToName()}</i> and one should read the notes
	 * for that method. It is not guaranteed that <i>productNameToID()</i> will successfully ascertain the
	 * product ID for a name obtained from a device, although it usually will. <i>ProductNameToID()</i>
	 * will always successfully ascertain the product ID for a name obtained from <i>productIDToName()</i>.
	 * If one has access to a device and its name, then they should obtain the product ID from the device
	 * itself rather than from this method. This method is mainly for easily converting between product
	 * names and IDs, primarily to serve the needs of user interfaces.
	 * <br><br>Although this method is <i>static</i>, an instance of USBDeviceManager must be created
	 * and be "open" for use before this method can be used. This stipulation is imposed because the
	 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
	 * initialization occurs only when an instance of USBDeviceManager is created and its
	 * <i>{@link #open() open()}</i> method is called.
	 * @param productName the product name to translate to a product ID.
	 * @return The product ID for the specified product name, or 0 (zero) if the name was not found.
	 * @throws IllegalArgumentException
	 */

	public static int productNameToID( String productName ) {
		if(
			productName == null
			|| productName.isEmpty()
		)
			throw new IllegalArgumentException( "Invalid product name" );
		return libProductNameToID( productName );
	}	// productNameToID()

	/**
	 * Gets the product IDs for an array of product names. Functionally identical to
	 * <i>{@link #productNameToID( String name ) productNameToID()}</i> except that it operates
	 * on an array of product names rather than an individual product name.
	 * <br><br>Although this method is <i>static</i>, an instance of USBDeviceManager must be created
	 * and be "open" for use before this method can be used. This stipulation is imposed because the
	 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
	 * initialization occurs only when an instance of USBDeviceManager is created and its
	 * <i>{@link #open() open()}</i> method is called.
	 * @param productName an array of product names to translate to product IDs.
	 * @return An array of integers containing the product IDs, or 0 (zero) for any product name that
	 * was not found. The product IDs are returned in the same order as the product names passed in
	 * <i>name[]</i>.
	 * @throws IllegalArgumentException
	 */

	public static int[] productNameToID( String[] productName ) {
		int[] productID = new int[ productName.length ];
		for( int index = 0; index < productName.length; index++ )
			productID[ index ] = productNameToID( productName[ index ] );
		return productID;
	}	// productNameToID()

	/*
	 * operations
	 */

	/**
	 * Prints the properties of all the devices found on the bus to the standard output device.
	 * This function is similar to <i>{@link #printDevices() printDevices()}</i> but is implemented
	 * by the underlying AIOUSB module and produces different output than <i>printDevices()</i>. Mainly
	 * useful for diagnostic purposes.
	 */

	public native void listDevices();			// => void AIOUSB_ListDevices()

	/**
	 * Gets the string representation of an AIOUSB result code, useful mainly for debugging purposes.
	 * This method is also used to convert an AIOUSB result code to a string when a
	 * {@link OperationFailedException} is thrown in response to an AIOUSB failure.
	 * <br><br>Although this method is <i>static</i>, an instance of USBDeviceManager must be created
	 * and be "open" for use before this method can be used. This stipulation is imposed because the
	 * underlying library must be initialized in order for result code lookups to succeed, and that
	 * initialization occurs only when an instance of USBDeviceManager is created and its
	 * <i>{@link #open() open()}</i> method is called.
	 * @param result an AIOUSB result code.
	 * @return The string representation of <i>result</i>.
	 */

	public static native String getResultCodeAsString( int result );
		// => const char *AIOUSB_GetResultCodeAsString( unsigned long result )

	/**
	 * Tells if the USB device manager has been "opened" for use <i>(see {@link #open() open()})</i>.
	 * @return <i>True</i> indicates that the device manager is open and ready to be used; <i>false</i>
	 * indicates that it is not open.
	 */

	public boolean isOpen() {
		return openStatus == OPEN_PATTERN;
	}	// isOpen()

	/**
	 * "Opens" the USB device manager for use. Before the USB device manager may be used, <i>open()</i> must be
	 * called. <i>Open()</i> initializes the underlying AIOUSB module and scans the bus for devices, building
	 * a list of the devices found. When finished using the USB device manager, <i>{@link #close() close()}</i>
	 * must be called. It is possible to call <i>close()</i> and then call <i>open()</i> again, which effectively
	 * reinitializes everything.
	 * @return This device manager, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public USBDeviceManager open() {
		if( ! isOpen() ) {
			final int result = init();
			if( result == SUCCESS ) {
				openStatus = OPEN_PATTERN;
				scanForDevices();
			} else
				throw new OperationFailedException( "Initialization failed" );	// don't call getResultCodeAsString() if init. unsuccessful
		} else
			throw new OperationFailedException( "Already open" );
		return this;
	}	// open()

	/**
	 * "Closes" the USB device manager for use. When finished using the USB device manager, and assuming
	 * <i>{@link #open() open()}</i> was properly called, <i>close()</i> must be called. <i>Close()</i> terminates
	 * use of the underlying AIOUSB module and discards the list of devices found. <i>You must terminate use of
	 * all USB devices before calling close()!</i> You can call <i>open()</i> again to reinitialize things
	 * and reestablish connections to USB devices.
	 * @return This device manager, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public USBDeviceManager close() {
		if( isOpen() ) {
			deviceList.clear();
			openStatus = 0;
			exit();
		} else
			throw new OperationFailedException( MESSAGE_NOT_OPEN );
		return this;
	}	// close()

	/**
	 * Re-scans the bus for devices. <i>ScanForDevices()</i> is called automatically by
	 * <i>{@link #open() open()}</i>. <i>You must terminate use of all USB devices before calling
	 * scanForDevices()!</i> After calling <i>scanForDevices()</i> you can reestablish
	 * connections to USB devices.
	 * @return This device manager, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public USBDeviceManager scanForDevices() {
		if( isOpen() ) {
			deviceList.clear();
			final int MAX_DEVICES = 100;
			int devices[] = new int[ 1 + MAX_DEVICES * 2 ];		// device index-product ID pairs
			final int result = getDeviceByProductID( MIN_PRODUCT_ID, MAX_PRODUCT_ID, devices );		// get all devices
			if( result != SUCCESS )
				throw new OperationFailedException( result );
			final int numDevices = devices[ 0 ];
			for( int index = 0; index < numDevices; index++ ) {
				USBDevice device = null;
				final int deviceIndex = devices[ 1 + index * 2 ];
				final int productID = devices[ 1 + index * 2 + 1 ];
				if( USB_AI16_Family.isSupportedProductID( productID ) ) {
					device = new USB_AI16_Family( productID, deviceIndex );
				} else if( USB_AO16_Family.isSupportedProductID( productID ) ) {
					device = new USB_AO16_Family( productID, deviceIndex );
				} else if( USB_CTR_15_Family.isSupportedProductID( productID ) ) {
					device = new USB_CTR_15_Family( productID, deviceIndex );
				} else if( USB_DA12_8A_Family.isSupportedProductID( productID ) ) {
					device = new USB_DA12_8A_Family( productID, deviceIndex );
				} else if( USB_DA12_8E_Family.isSupportedProductID( productID ) ) {
					device = new USB_DA12_8E_Family( productID, deviceIndex );
				} else if( USB_DIO_16_Family.isSupportedProductID( productID ) ) {
					device = new USB_DIO_16_Family( productID, deviceIndex );
				} else if( USB_DIO_32_Family.isSupportedProductID( productID ) ) {
					device = new USB_DIO_32_Family( productID, deviceIndex );
				} else if( USB_DIO_Family.isSupportedProductID( productID ) ) {
					device = new USB_DIO_Family( productID, deviceIndex );
				}	// else if( USB_DIO_Family.isSupportedProductID( ...
				if( device != null )
					deviceList.add( device );
			}	// for( int index ...
		} else
			throw new OperationFailedException( MESSAGE_NOT_OPEN );
		return this;
	}	// scanForDevices()

	/**
	 * Gets a list of all the devices found on the bus matching the specified product ID. Only devices exactly
	 * matching the specified product ID will be returned. You can search for devices by product name using
	 * {@link #productNameToID( String productName ) productNameToID()}, like so:
	 * <pre>USBDevice[] devices = deviceManager.getDeviceByProductID( deviceManager.productNameToID( "USB-CTR-15" ) );</pre>
	 * @param productID the product ID to search for.
	 * @return An array of all the devices found. If no devices were found matching the specified
	 * product ID, the array will be empty (i.e. contain zero items).
	 */

	public USBDevice[] getDeviceByProductID( int productID ) {
		if(
			productID < MIN_PRODUCT_ID
			|| productID > MAX_PRODUCT_ID
		)
			throw new IllegalArgumentException( "Invalid product ID: " + productID );
		return getDeviceByProductID( productID, productID );
	}	// getDeviceByProductID()

	/**
	 * Gets a list of all the devices found on the bus matching the specified product ID range. Any device
	 * with a product ID greater than or equal to <i>minProductID</i> and less than or equal to <i>maxProductID</i>
	 * will be returned. You can obtain the entire list of devices detected by passing a value of 0 for
	 * <i>minProductID</i> and a value of 0xffff for <i>maxProductID</i>. Then you can search the list obtained
	 * using your own search criteria.
	 * @param minProductID the minimum product ID to search for.
	 * @param maxProductID the maximum product ID to search for.
	 * @return An array of all the devices found. If no devices were found matching the specified
	 * product ID range, the array will be empty (i.e. contain zero items).
	 * @throws IllegalArgumentException
	 */

	public USBDevice[] getDeviceByProductID( int minProductID, int maxProductID ) {
		if(
			minProductID < MIN_PRODUCT_ID
			|| minProductID > MAX_PRODUCT_ID
			|| maxProductID < minProductID
			|| maxProductID > MAX_PRODUCT_ID
		)
			throw new IllegalArgumentException( "Invalid product IDs: " + minProductID + ", " + maxProductID );
		Vector<USBDevice> devices = new Vector<USBDevice>();
		for( int index = 0; index < deviceList.size(); index++ ) {
			final int productID = deviceList.get( index ).getProductID();
			if(
				productID >= minProductID
				&& productID <= maxProductID
			)
				devices.add( deviceList.get( index ) );
		}	// for( int index ...
		return devices.toArray( new USBDevice[ 0 ] );
	}	// getDeviceByProductID()

	/**
	 * Gets a list of all the devices found on the bus matching the specified set of product IDs. Any device
	 * with a product ID equal to one of the products listed in <i>productIDs[]</i> will be returned. You can
	 * search for devices by product name using {@link #productNameToID( String[] productName ) productNameToID()}, like so:
	 * <pre>USBDevice[] devices = deviceManager.getDeviceByProductID(
	 *  deviceManager.productNameToID( new String[] { "USB-AO16-16A", "USB-AO16-16" } ) );</pre>
	 * @param productIDs an array containing one or more product IDs to search for.
	 * @return An array of all the devices found. If no devices were found matching the specified
	 * set of product IDs, the array will be empty (i.e. contain zero items).
	 * @throws IllegalArgumentException
	 */

	public USBDevice[] getDeviceByProductID( int[] productIDs ) {
		if(
			productIDs == null
			|| productIDs.length < 1
		)
			throw new IllegalArgumentException( "Invalid product ID array" );
		for( int index = 0; index < productIDs.length; index++ ) {
			if(
				productIDs[ index ] < MIN_PRODUCT_ID
				|| productIDs[ index ] > MAX_PRODUCT_ID
			)
				throw new IllegalArgumentException( "Invalid product ID: " + productIDs[ index ] );
		}	// for( int index ...
		int[] sortedProductIDs = productIDs.clone();
		Arrays.sort( sortedProductIDs );
		Vector<USBDevice> devices = new Vector<USBDevice>();
		for( int index = 0; index < deviceList.size(); index++ ) {
			final int productID = deviceList.get( index ).getProductID();
			if( Arrays.binarySearch( sortedProductIDs, productID ) >= 0 )
				devices.add( deviceList.get( index ) );
		}	// for( int index ...
		return devices.toArray( new USBDevice[ 0 ] );
	}	// getDeviceByProductID()

	/**
	 * Gets a list of all the devices found on the bus matching the specified serial number. Only devices exactly
	 * matching the specified serial number will be returned. In theory, there ought to be only one device
	 * matching a given serial number, but this method returns a vector in order to be consistent with the other
	 * search methods, and in unlikely event that multiple devices do share the same serial number.
	 * @param serialNumber the serial number to search for.
	 * @return An array of all the devices found. If no devices were found matching the specified
	 * serial number, the array will be empty (i.e. contain zero items).
	 */

	public USBDevice[] getDeviceBySerialNumber( long serialNumber ) {
		Vector<USBDevice> devices = new Vector<USBDevice>();
		for( int index = 0; index < deviceList.size(); index++ ) {
			if( deviceList.get( index ).getSerialNumber() == serialNumber )
				devices.add( deviceList.get( index ) );
		}	// for( int index ...
		return devices.toArray( new USBDevice[ 0 ] );
	}	// getDeviceBySerialNumber()

	// }}}

}	// class USBDeviceManager

/* end of file */
