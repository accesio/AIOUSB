/*
 * $RCSfile: USB_AO16_Family.cpp,v $
 * $Revision: 1.4 $
 * $Date: 2009/12/23 22:39:10 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class USB_AO16_Family implementation
 */



#include <iostream>
#include <bits/stl_algo.h>
#include <assert.h>
#include "USBDeviceManager.hpp"
#include "USB_AO16_Family.hpp"


using namespace std;

namespace AIOUSB {



IntArray USB_AO16_Family::supportedProductIDs;

void USB_AO16_Family::initialize() {
	if( supportedProductIDs.empty() ) {
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO16_16A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO16_16 );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO16_12A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO16_12 );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO16_8A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO16_8 );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO16_4A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO16_4 );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO12_16A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO12_16 );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO12_12A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO12_12 );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO12_8A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO12_8 );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO12_4A );
		supportedProductIDs.insert( supportedProductIDs.end(), USB_AO12_4 );
		sort( supportedProductIDs.begin(), supportedProductIDs.end() );
	}	// if( supportedProductIDs.empty() )
}	// USB_AO16_Family::initialize()





USB_AO16_Family::USB_AO16_Family( int productID, int deviceIndex )
		: USBDeviceBase( productID, deviceIndex )
		, analogOutputSubsystem( *this )
		, digitalIOSubsystem( *this ) {
	if( ! isSupportedProductID( productID ) )
		throw IllegalArgumentException( "Invalid product ID" );
}	// USB_AO16_Family::USB_AO16_Family()

USB_AO16_Family::~USB_AO16_Family() {
	// nothing to do
}	// USB_AO16_Family::~USB_AO16_Family()





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

StringArray USB_AO16_Family::getSupportedProductNames() {
	initialize();
	return USBDeviceManager::productIDToName( supportedProductIDs );
}	// USB_AO16_Family::getSupportedProductNames()

/**
 * Gets an array of all the product IDs supported by this USB device family.
 * @return An array of product IDs, sorted in ascending order.
 */

IntArray USB_AO16_Family::getSupportedProductIDs() {
	initialize();
	return IntArray( supportedProductIDs );
}	// USB_AO16_Family::getSupportedProductIDs()

/**
 * Tells if a given product ID is supported by this USB device family.
 * @param productID the product ID to check.
 * @return <i>True</i> if the given product ID is supported by this USB device family; otherwise, <i>false</i>.
 */

bool USB_AO16_Family::isSupportedProductID( int productID ) {
	initialize();
	return binary_search( supportedProductIDs.begin(), supportedProductIDs.end(), productID );
}	// USB_AO16_Family::isSupportedProductID()

/**
 * Prints the properties of this device and all of its subsystems. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &USB_AO16_Family::print( ostream &out ) {
	USBDeviceBase::print( out );
	analogOutputSubsystem.print( out );
	digitalIOSubsystem.print( out );
	return out;
}	// USB_AO16_Family::print()



}	// namespace AIOUSB

/* end of file */
