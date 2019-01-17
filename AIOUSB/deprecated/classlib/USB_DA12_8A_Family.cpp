/*
 * $RCSfile: USB_DA12_8A_Family.cpp,v $
 * $Revision: 1.3 $
 * $Date: 2009/12/23 22:39:10 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class USB_DA12_8A_Family implementation
 */


#include <iostream>
#include <bits/stl_algo.h>
#include <assert.h>
#include "USBDeviceManager.hpp"
#include "USB_DA12_8A_Family.hpp"


using namespace std;

namespace AIOUSB {



IntArray USB_DA12_8A_Family::supportedProductIDs;

void USB_DA12_8A_Family::initialize() {
	if( supportedProductIDs.empty() ) {
		supportedProductIDs.insert( supportedProductIDs.end(), USB_DA12_8A_REV_A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_DA12_8A );
		sort( supportedProductIDs.begin(), supportedProductIDs.end() );
	}	// if( supportedProductIDs.empty() )
}	// USB_DA12_8A_Family::initialize()





USB_DA12_8A_Family::USB_DA12_8A_Family( int productID, int deviceIndex )
		: USBDeviceBase( productID, deviceIndex )
		, analogOutputSubsystem( *this ) {
	if( ! isSupportedProductID( productID ) )
		throw IllegalArgumentException( "Invalid product ID" );
}	// USB_DA12_8A_Family::USB_DA12_8A_Family()

USB_DA12_8A_Family::~USB_DA12_8A_Family() {
	// nothing to do
}	// USB_DA12_8A_Family::~USB_DA12_8A_Family()





/*
 * properties
 */

/**
 * Gets an array of all the product names supported by this USB device family.
 * Although this method is <i>static</i>, an instance of USBDeviceManager must be created
 * and be "open" for use before this method can be used. This stipulation is imposed because the
 * underlying library must be initialized in order for product name/ID lookups to succeed, and that
 * initialization occurs only when an instance of USBDeviceManager is created and its
 * <i>USBDeviceManager::open()</i> method is called.
 * @return An array of product names, sorted in ascending order of product ID.
 */

StringArray USB_DA12_8A_Family::getSupportedProductNames() {
	initialize();
	return USBDeviceManager::productIDToName( supportedProductIDs );
}	// USB_DA12_8A_Family::getSupportedProductNames()

/**
 * Gets an array of all the product IDs supported by this USB device family.
 * @return An array of product IDs, sorted in ascending order.
 */

IntArray USB_DA12_8A_Family::getSupportedProductIDs() {
	initialize();
	return IntArray( supportedProductIDs );
}	// USB_DA12_8A_Family::getSupportedProductIDs()

/**
 * Tells if a given product ID is supported by this USB device family.
 * @param productID the product ID to check.
 * @return <i>True</i> if the given product ID is supported by this USB device family; otherwise, <i>false</i>.
 */

bool USB_DA12_8A_Family::isSupportedProductID( int productID ) {
	initialize();
	return binary_search( supportedProductIDs.begin(), supportedProductIDs.end(), productID );
}	// USB_DA12_8A_Family::isSupportedProductID()

/**
 * Prints the properties of this device and all of its subsystems. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &USB_DA12_8A_Family::print( ostream &out ) {
	USBDeviceBase::print( out );
	analogOutputSubsystem.print( out );
	return out;
}	// USB_DA12_8A_Family::print()



}	// namespace AIOUSB

/* end of file */
