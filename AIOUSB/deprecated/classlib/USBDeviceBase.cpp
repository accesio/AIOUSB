/*
 * $RCSfile: USBDeviceBase.cpp,v $
 * $Revision: 1.15 $
 * $Date: 2010/01/18 19:40:28 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class USBDeviceBase implementation
 */


#include "CppCommon.h"
#include "AIODeviceTable.h"
#include <iomanip>
#include <assert.h>
#include <AIOUSB_Core.h>
#include "USBDeviceBase.hpp"
#include "USBDeviceManager.hpp"


using namespace std;

namespace AIOUSB {


const int USBDeviceBase::CUSTOM_EEPROM_SIZE;
const int USBDeviceBase::CLEAR_FIFO_METHOD_IMMEDIATE;
const int USBDeviceBase::CLEAR_FIFO_METHOD_AUTO;
const int USBDeviceBase::CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT;
const int USBDeviceBase::CLEAR_FIFO_METHOD_WAIT;


USBDeviceBase::USBDeviceBase( int productID, int deviceIndex ) {
	if(
		productID < USBDeviceManager::MIN_PRODUCT_ID
		|| productID > USBDeviceManager::MAX_PRODUCT_ID
		|| deviceIndex < 0
		|| deviceIndex > 127					// arbitrary sanity check
	)
		throw IllegalArgumentException( "Invalid product ID" );
	this->productID = productID;
	this->deviceIndex = deviceIndex;
	serialNumber = 0;
	DeviceProperties properties;
	const int result = AIOUSB_GetDeviceProperties( deviceIndex, &properties );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	name = properties.Name;
	serialNumber = properties.SerialNumber;
}	// USBDeviceBase::USBDeviceBase()

USBDeviceBase::~USBDeviceBase() {
	// nothing to do
}	// USBDeviceBase::~USBDeviceBase()

/*
 * properties
 */

/**
 * Prints the properties of this device and all of its subsystems. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &USBDeviceBase::print( ostream &out ) {
	out
		<< "  Device at index " << left << dec << deviceIndex << ':' << endl
		<< "    Product ID: 0x" << right << setw( 4 ) << setfill( '0' ) << hex << productID << endl
		<< "    Product name: " << left << getName() << endl
		<< "    Serial number: 0x" << right << setw( 16 ) << hex << getSerialNumber() << endl;
	return out;
}	// USBDeviceBase::print()

/*
 * configuration
 */

USBDeviceBase &USBDeviceBase::clearFIFO( FIFO_Method  method ) {
	const int result = AIOUSB_ClearFIFO( deviceIndex, method );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// USBDeviceBase::clearFIFO()

USBDeviceBase &USBDeviceBase::setMiscClock( double clockHz ) {
	if( clockHz < 0 )
		throw IllegalArgumentException( "Invalid clock frequency" );
	const int result = AIOUSB_SetMiscClock( deviceIndex, clockHz );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// USBDeviceBase::setMiscClock()

int USBDeviceBase::getStreamingBlockSize() {
	unsigned long blockSize = AIOUSB_GetStreamingBlockSize( deviceIndex );
	return ( int ) blockSize;
}	// USBDeviceBase::getStreamingBlockSize()

USBDeviceBase &USBDeviceBase::setStreamingBlockSize( int blockSize ) {
	if( blockSize < 0 )
		throw IllegalArgumentException( "Invalid block size" );
	const int result = AIOUSB_SetStreamingBlockSize( deviceIndex, blockSize );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// USBDeviceBase::setStreamingBlockSize()

/**
 * Gets the current timeout setting for USB communications.
 * @return Current timeout setting (in milliseconds).
 * @see setCommTimeout( int timeout )
 */

int USBDeviceBase::getCommTimeout() const {
	return AIOUSB_GetCommTimeout( deviceIndex );
}	// USBDeviceBase::getCommTimeout()

/**
 * Sets the timeout for USB communications.
 * @param timeout the new timeout setting (in milliseconds; default is 5,000).
 * @return This device, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

USBDeviceBase &USBDeviceBase::setCommTimeout( int timeout ) {
	if(
		timeout < 0
		|| timeout > 100000						// arbitrary sanity check
	)
		throw IllegalArgumentException( "Invalid timeout" );
	const int result = AIOUSB_SetCommTimeout( deviceIndex, timeout );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// USBDeviceBase::setCommTimeout()

/*
 * operations
 */

/**
 * Perform a USB port reset to reinitialize the device.
 * @return This device, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

USBDeviceBase &USBDeviceBase::reset() {
	const int result = ( ResultCode ) AIOUSB_Reset( deviceIndex );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// USBDeviceBase::reset()

/**
 * Writes data to the custom programming area of the device EEPROM. <i>Beware that writing to the EEPROM
 * is particularly slow.</i> Writing the entire EEPROM may take several seconds. Before initiating a
 * lengthy EEPROM write procedure, it is recommended that the communication timeout be increased to
 * at least five seconds, if not ten <i>(see setCommTimeout( int timeout ))</i>.
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

USBDeviceBase &USBDeviceBase::customEEPROMWrite( int address, const UCharArray &data ) {
	if(data.size() < 1
           || address < 0
           || address + ( int ) data.size() > CUSTOM_EEPROM_SIZE
	)
		throw IllegalArgumentException( "Invalid data or address" );
	const int result = CustomEEPROMWrite( deviceIndex, address, data.size(), ( void * ) data.data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// USBDeviceBase::customEEPROMWrite()

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

UCharArray USBDeviceBase::customEEPROMRead( int address, int numBytes ) {
	if(
		numBytes < 1
		|| address < 0
		|| address + numBytes > CUSTOM_EEPROM_SIZE
	)
		throw IllegalArgumentException( "Invalid data or address" );
	UCharArray data( numBytes );
	unsigned long size = data.size();
	const int result = CustomEEPROMRead( deviceIndex, address, &size, data.data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return data;
}	// USBDeviceBase::customEEPROMRead()





ostream &operator<<( ostream &out, USBDeviceBase &device ) {
	return device.print( out );
}	// operator<<()


ostream &operator<<( ostream &out, USBDeviceBase *device ) {
	assert( device != 0 );
	if( device != 0 )
		device->print( out );
	return out;
}	// operator<<()



}	// namespace AIOUSB


/* end of file */
