/**
 * @file   USBDeviceManager.cpp
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @brief class USBDeviceManager implementation
 *
 */

#include "CppCommon.h"
#include <iostream>
#include <iterator>
#include <bits/stl_algo.h>
#include <assert.h>
#include <AIOUSB_Core.h>
#include "AIODeviceTable.h"
#include "USBDeviceManager.hpp"
#include "USB_AI16_Family.hpp"
#include "USB_AO16_Family.hpp"
#include "USB_CTR_15_Family.hpp"
#include "USB_DA12_8A_Family.hpp"
#include "USB_DA12_8E_Family.hpp"
#include "USB_DIO_16_Family.hpp"
#include "USB_DIO_32_Family.hpp"
#include "USB_DIO_Family.hpp"
#include "USB_AIO16_Family.hpp"



using namespace std;

namespace AIOUSB {


const std::string USBDeviceManager::VERSION_NUMBER = "1.8";
const std::string USBDeviceManager::VERSION_DATE = "18 January 2010";
const int USBDeviceManager::MIN_PRODUCT_ID;
const int USBDeviceManager::MAX_PRODUCT_ID;
const unsigned long USBDeviceManager::OPEN_PATTERN;
const std::string USBDeviceManager::MESSAGE_NOT_OPEN = "Not open, must call open() first";




void USBDeviceManager::emptyDeviceList() {
	for( int index = 0; index < ( int ) deviceList.size(); index++ ) {
		assert( deviceList.at( index ) != 0 );
		delete deviceList.at( index );
	}	// for( int index ...
	deviceList.clear();
}	// USBDeviceManager::emptyDeviceList()





USBDeviceManager::USBDeviceManager() {
	openStatus = 0;
}	// USBDeviceManager::USBDeviceManager()

USBDeviceManager::~USBDeviceManager() {
	if( isOpen() )
		close();								// in case user forgot
}	// USBDeviceManager::~USBDeviceManager()

/*
 * properties
 */

/**
 * Prints the properties of this device manager and all of the devices found on the bus to
 * the specified print stream. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &USBDeviceManager::print( ostream &out ) {
	if( ! isOpen() )
		throw OperationFailedException( MESSAGE_NOT_OPEN );
	if( deviceList.size() > 0 ) {
		out << "ACCES devices found:" << endl;
		copy( deviceList.begin(), deviceList.end(), ostream_iterator<USBDeviceBase *>( out ) );
	} else
		out << "No ACCES devices found" << endl;
	return out;
}	// USBDeviceManager::print()

/**
 * Prints the properties of this device manager and all of the devices found on the bus to
 * the standard output device. Mainly useful for diagnostic purposes.
 * @return This device manager, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

USBDeviceManager &USBDeviceManager::printDevices() {
	print( cout );
	return *this;
}	// USBDeviceManager::printDevices()

/*
 * utilities
 */

/**
 * Gets the product name for a product ID. This name is only "approximate," as an actual device
 * reports its own name. Generally the names reported by the device are the same as those obtained
 * from this method, but that is not guaranteed. This method provides a name that constitutes a
 * user-friendly alternative to a product ID number. The complement of this method is
 * <i>productNameToID( const std::string &productName )</i>.
 * Although this method is <i>static</i>, an instance of USBDeviceManager must be created
 * and be "open" for use before this method can be used. This stipulation is imposed because the
 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
 * initialization occurs only when an instance of USBDeviceManager is created and its
 * <i>open()</i> method is called.
 * @param productID the product ID to translate to a product name.
 * @return A string containing the product name, or "UNKNOWN" if the product ID was not found.
 * @throws IllegalArgumentException
 */

std::string USBDeviceManager::productIDToName( int productID ) {
	if(
		productID < MIN_PRODUCT_ID
		|| productID > MAX_PRODUCT_ID
	)
		throw IllegalArgumentException( "Invalid product ID" );
	return ProductIDToName( productID );
}	// USBDeviceManager::productIDToName()

/**
 * Gets the product names for an array of product IDs. Functionally identical to
 * <i>productIDToName( int productID )</i> except that it operates
 * on an array of product IDs rather than an individual product ID.
 * Although this method is <i>static</i>, an instance of USBDeviceManager must be created
 * and be "open" for use before this method can be used. This stipulation is imposed because the
 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
 * initialization occurs only when an instance of USBDeviceManager is created and its
 * <i>open()</i> method is called.
 * @param productID an array of product IDs to translate to product names.
 * @return An array of strings containing the product names, or "UNKNOWN" for any product ID that
 * was not found. The product names are returned in the same order as the product IDs passed in
 * <i>productID[]</i>.
 * @throws IllegalArgumentException
 */

StringArray USBDeviceManager::productIDToName( const IntArray &productID ) {
	StringArray productName( productID.size() );
	for( int index = 0; index < ( int ) productID.size(); index++ )
		productName.at( index ) = productIDToName( productID.at( index ) );
	return productName;
}	// USBDeviceManager::productIDToName()

/**
 * Gets the product ID for a product name. This method is the complement of
 * <i>productIDToName( int productID )</i> and one should read the notes
 * for that method. It is not guaranteed that <i>productNameToID()</i> will successfully ascertain the
 * product ID for a name obtained from a device, although it usually will. <i>ProductNameToID()</i>
 * will always successfully ascertain the product ID for a name obtained from <i>productIDToName()</i>.
 * If one has access to a device and its name, then they should obtain the product ID from the device
 * itself rather than from this method. This method is mainly for easily converting between product
 * names and IDs, primarily to serve the needs of user interfaces.
 * Although this method is <i>static</i>, an instance of USBDeviceManager must be created
 * and be "open" for use before this method can be used. This stipulation is imposed because the
 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
 * initialization occurs only when an instance of USBDeviceManager is created and its
 * <i>open()</i> method is called.
 * @param productName the product name to translate to a product ID.
 * @return The product ID for the specified product name, or 0 (zero) if the name was not found.
 * @throws IllegalArgumentException
 */

int USBDeviceManager::productNameToID( const std::string &productName ) {
	if( productName.empty() )
		throw IllegalArgumentException( "Invalid product name" );
	return ProductNameToID( productName.c_str() );
}	// USBDeviceManager::productNameToID()

/**
 * Gets the product IDs for an array of product names. Functionally identical to
 * <i>productNameToID( const std::string &productName )</i> except that it operates
 * on an array of product names rather than an individual product name.
 * Although this method is <i>static</i>, an instance of USBDeviceManager must be created
 * and be "open" for use before this method can be used. This stipulation is imposed because the
 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
 * initialization occurs only when an instance of USBDeviceManager is created and its
 * <i>open()</i> method is called.
 * @param productName an array of product names to translate to product IDs.
 * @return An array of integers containing the product IDs, or 0 (zero) for any product name that
 * was not found. The product IDs are returned in the same order as the product names passed in
 * <i>productName[]</i>.
 * @throws IllegalArgumentException
 */

IntArray USBDeviceManager::productNameToID( const StringArray &productName ) {
	IntArray productID( productName.size() );
	for( int index = 0; index < ( int ) productName.size(); index++ )
		productID.at( index ) = productNameToID( productName.at( index ) );
	return productID;
}	// USBDeviceManager::productNameToID()

/*
 * operations
 */

/**
 * "Opens" the USB device manager for use. Before the USB device manager may be used, <i>open()</i> must be
 * called. <i>Open()</i> initializes the underlying AIOUSB module and scans the bus for devices, building
 * a list of the devices found. When finished using the USB device manager, <i>close()</i>
 * must be called. It is possible to call <i>close()</i> and then call <i>open()</i> again, which effectively
 * reinitializes everything.
 * @return This device manager, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

USBDeviceManager &USBDeviceManager::open() {
	if( ! isOpen() ) {
		const int result = AIOUSB_Init();
		if( result == AIOUSB_SUCCESS ) {
			openStatus = OPEN_PATTERN;
			scanForDevices();
		} else
			throw OperationFailedException( "Initialization failed" );	// don't call getResultCodeAsString() if init. unsuccessful
	} else
		throw OperationFailedException( "Already open" );
	return *this;
}	// USBDeviceManager::open()

/**
 * "Closes" the USB device manager for use. When finished using the USB device manager, and assuming
 * <i>open()</i> was properly called, <i>close()</i> must be called. <i>Close()</i> terminates
 * use of the underlying AIOUSB module and discards the list of devices found. <i>You must terminate use of
 * all USB devices before calling close()!</i> You can call <i>open()</i> again to reinitialize things
 * and reestablish connections to USB devices.
 * @return This device manager, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

USBDeviceManager &USBDeviceManager::close() {
	if( isOpen() ) {
		emptyDeviceList();
		openStatus = 0;
		AIOUSB_Exit();
	} else
		throw OperationFailedException( MESSAGE_NOT_OPEN );
	return *this;
}	// USBDeviceManager::close()

/**
 * Re-scans the bus for devices. <i>ScanForDevices()</i> is called automatically by
 * <i>open()</i>. <i>You must terminate use of all USB devices before calling
 * scanForDevices()!</i> After calling <i>scanForDevices()</i> you can reestablish
 * connections to USB devices.
 * @return This device manager, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

USBDeviceManager &USBDeviceManager::scanForDevices() {
	if( isOpen() ) {
		emptyDeviceList();
		const int MAX_DEVICES = 100;
		int devices[ 1 + MAX_DEVICES * 2 ];		// device index-product ID pairs
		const int result = AIOUSB_GetDeviceByProductID( MIN_PRODUCT_ID, MAX_PRODUCT_ID, MAX_DEVICES, devices );		// get all devices
		if( result != AIOUSB_SUCCESS )
			throw OperationFailedException( result );
		const int numDevices = devices[ 0 ];
		for( int index = 0; index < numDevices; index++ ) {
			USBDeviceBase *device = 0;
			const int deviceIndex = devices[ 1 + index * 2 ];
			const int productID = devices[ 1 + index * 2 + 1 ];
			if( USB_AI16_Family::isSupportedProductID( productID ) ) {
				device = new USB_AI16_Family( productID, deviceIndex );
			} else if( USB_AO16_Family::isSupportedProductID( productID ) ) {
				device = new USB_AO16_Family( productID, deviceIndex );
			} else if( USB_AIO16_Family::isSupportedProductID( productID ) ) { 
				device = new USB_AIO16_Family( productID, deviceIndex );
                        } else if( USB_CTR_15_Family::isSupportedProductID( productID ) ) {
				device = new USB_CTR_15_Family( productID, deviceIndex );
			} else if( USB_DA12_8A_Family::isSupportedProductID( productID ) ) {
				device = new USB_DA12_8A_Family( productID, deviceIndex );
			} else if( USB_DA12_8E_Family::isSupportedProductID( productID ) ) {
				device = new USB_DA12_8E_Family( productID, deviceIndex );
			} else if( USB_DIO_16_Family::isSupportedProductID( productID ) ) {
				device = new USB_DIO_16_Family( productID, deviceIndex );
			} else if( USB_DIO_32_Family::isSupportedProductID( productID ) ) {
				device = new USB_DIO_32_Family( productID, deviceIndex );
			} else if( USB_DIO_Family::isSupportedProductID( productID ) ) {
				device = new USB_DIO_Family( productID, deviceIndex );
			}	// else if( USB_DIO_Family::isSupportedProductID( ...
			if( device != 0 )
				deviceList.insert( deviceList.end(), device );
		}	// for( int index ...
	} else
		throw OperationFailedException( MESSAGE_NOT_OPEN );
	return *this;
}	// USBDeviceManager::scanForDevices()

/**
 * Gets a list of all the devices found on the bus matching the specified product ID. Only devices exactly
 * matching the specified product ID will be returned. You can search for devices by product name using
 * productNameToID( const std::string &productName ), like so:
 * <pre>USBDeviceArray devices = deviceManager.getDeviceByProductID( deviceManager.productNameToID( "USB-CTR-15" ) );</pre>
 * @param productID the product ID to search for.
 * @return An array of all the devices found. If no devices were found matching the specified
 * product ID, the array will be empty (i.e. contain zero items).
 */

USBDeviceArray USBDeviceManager::getDeviceByProductID( int productID ) const {
	if(
		productID < MIN_PRODUCT_ID
		|| productID > MAX_PRODUCT_ID
	)
		throw IllegalArgumentException( "Invalid product ID" );
	return getDeviceByProductID( productID, productID );
}	// USBDeviceManager::getDeviceByProductID()

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

USBDeviceArray USBDeviceManager::getDeviceByProductID( int minProductID, int maxProductID ) const {
	if(
		minProductID < MIN_PRODUCT_ID
		|| minProductID > MAX_PRODUCT_ID
		|| maxProductID < minProductID
		|| maxProductID > MAX_PRODUCT_ID
	)
		throw IllegalArgumentException( "Invalid product IDs" );
	USBDeviceArray devices;
	for( int index = 0; index < ( int ) deviceList.size(); index++ ) {
		assert( deviceList.at( index ) != 0 );
		const int productID = deviceList.at( index )->getProductID();
		if(
			productID >= minProductID
			&& productID <= maxProductID
		)
			devices.insert( devices.end(), deviceList.at( index ) );
	}	// for( int index ...
	return devices;
}	// USBDeviceManager::getDeviceByProductID()

/**
 * Gets a list of all the devices found on the bus matching the specified set of product IDs. Any device
 * with a product ID equal to one of the products listed in <i>productIDs[]</i> will be returned.
 * @param productIDs an array containing one or more product IDs to search for.
 * @return An array of all the devices found. If no devices were found matching the specified
 * set of product IDs, the array will be empty (i.e. contain zero items).
 * @throws IllegalArgumentException
 */

USBDeviceArray USBDeviceManager::getDeviceByProductID( const IntArray &productIDs ) const {
        if(productIDs.size() < 1)
		throw IllegalArgumentException( "Invalid product ID array" );
	for( int index = 0; index < ( int ) productIDs.size(); index++ ) {
		if(
			productIDs.at( index ) < MIN_PRODUCT_ID
			|| productIDs.at( index ) > MAX_PRODUCT_ID
		)
			throw IllegalArgumentException( "Invalid product ID" );
	}	// for( int index ...
	IntArray sortedProductIDs( productIDs );
	sort( sortedProductIDs.begin(), sortedProductIDs.end() );
	USBDeviceArray devices;
	for( int index = 0; index < ( int ) deviceList.size(); index++ ) {
		assert( deviceList.at( index ) != 0 );
		const int productID = deviceList.at( index )->getProductID();
		if( binary_search( sortedProductIDs.begin(), sortedProductIDs.end(), productID ) )
			devices.insert( devices.end(), deviceList.at( index ) );
	}	// for( int index ...
	return devices;
}	// USBDeviceManager::getDeviceByProductID()

/**
 * Gets a list of all the devices found on the bus matching the specified serial number. Only devices exactly
 * matching the specified serial number will be returned. In theory, there ought to be only one device
 * matching a given serial number, but this method returns a vector in order to be consistent with the other
 * search methods, and in unlikely event that multiple devices do share the same serial number.
 * @param serialNumber the serial number to search for.
 * @return An array of all the devices found. If no devices were found matching the specified
 * serial number, the array will be empty (i.e. contain zero items).
 */
USBDeviceArray USBDeviceManager::getDeviceBySerialNumber( __uint64_t serialNumber ) const {
	USBDeviceArray devices;
	for( int index = 0; index < ( int ) deviceList.size(); index++ ) {
		assert( deviceList.at( index ) != 0 );
		if( deviceList.at( index )->getSerialNumber() == serialNumber )
			devices.insert( devices.end(), deviceList.at( index ) );
	}
	return devices;
}



}	// namespace AIOUSB

/* end of file */
