/*
 * $RCSfile: USBDeviceBase.hpp,v $
 * $Revision: 1.14 $
 * $Date: 2010/01/18 19:40:28 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class BoolArray, UCharArray, UShortArray, IntArray, DoubleArray, USBDeviceArray, USBDeviceBase declarations
 */

#if ! defined( USBDevice_hpp )
#define USBDevice_hpp


#include <vector>
#include <string>
#include <iostream>
#include <aiousb.h>


namespace AIOUSB {



class BoolArray : public std::vector<bool> {
public:
	BoolArray( int size = 0 ) : std::vector<bool>( size ) {}
};	// class BoolArray

class UCharArray : public std::vector<unsigned char> {
public:
	UCharArray( int size = 0 ) : std::vector<unsigned char>( size ) {}
};	// class UCharArray

class UShortArray : public std::vector<unsigned short> {
public:
	UShortArray( int size = 0 ) : std::vector<unsigned short>( size ) {}
};	// class UShortArray

class IntArray : public std::vector<int> {
public:
	IntArray( int size = 0 ) : std::vector<int>( size ) {}
};	// class IntArray

class DoubleArray : public std::vector<double> {
public:
	DoubleArray( int size = 0 ) : std::vector<double>( size ) {}
};	// class DoubleArray

class StringArray : public std::vector<std::string> {
public:
	StringArray( int size = 0 ) : std::vector<std::string>( size ) {}
};	// class StringArray

class USBDeviceBase;
class USBDeviceArray : public std::vector<USBDeviceBase*> {
public:
	USBDeviceArray( int size = 0 ) : std::vector<USBDeviceBase*>( size ) {}
};	// class USBDeviceArray





/**
 * Class USBDeviceBase is the abstract super class of all USB device families.
 */

class USBDeviceBase {
	friend class USBDeviceManager;
	friend class DIOStreamSubsystem;
	friend class AnalogInputSubsystem;


public:
	/** Size of custom EEPROM area (bytes). */
	static const int CUSTOM_EEPROM_SIZE = 0x200;

	/** Clear FIFO as soon as command received (and disable auto-clear). */
	static const int CLEAR_FIFO_METHOD_IMMEDIATE			= 0;
	/** Enable auto-clear FIFO every falling edge of DIO port D bit 1 (on digital boards, analog boards treat as 0). */
	static const int CLEAR_FIFO_METHOD_AUTO					= 1;
	/** Clear FIFO as soon as command received (and disable auto-clear), and abort stream. */
	static const int CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT	= 5;
	/** Clear FIFO and wait for it to be emptied. */
	static const int CLEAR_FIFO_METHOD_WAIT					= 86;



protected:
	int deviceIndex;							// device index on bus
	int productID;								// 16-bit product ID
	std::string name;							// device name
	__uint64_t serialNumber;					// 64-bit serial number or 0



protected:
	USBDeviceBase( int productID, int deviceIndex );
	virtual ~USBDeviceBase();
	USBDeviceBase &clearFIFO( FIFO_Method method );

	double getMiscClock() {
		return AIOUSB_GetMiscClock( deviceIndex );
	}	// getMiscClock()

	USBDeviceBase &setMiscClock( double clockHz );
	int getStreamingBlockSize();
	USBDeviceBase &setStreamingBlockSize( int blockSize );



public:

	/*
	 * properties
	 */

	virtual std::ostream &print( std::ostream &out );

	/**
	 * Gets the device's index on the USB bus. The device index isn't used within this Java class library,
	 * but is used in the underlying AIOUSB library. The device index is somewhat useful within this Java
	 * class library to differentiate between multiple devices of the same type.
	 * @return The index of the device on the USB bus.
	 */

	int getDeviceIndex() const {
		return deviceIndex;
	}	// getDeviceIndex()

	/**
	 * Gets the device's product ID.
	 * @return The device product ID.
	 */

	int getProductID() const {
		return productID;
	}	// getProductID()

	/**
	 * Gets the device's name.
	 * @return The device name.
	 */

	const std::string &getName() const {
		return name;
	}	// getName()

	/**
	 * Gets the device's serial number.
	 * @return The device serial number (a 64-bit integer).
	 * @throws OperationFailedException
	 */

	__uint64_t getSerialNumber() const {
		return serialNumber;
	}	// getSerialNumber()

	/*
	 * configuration
	 */

	int getCommTimeout() const;
	USBDeviceBase &setCommTimeout( int timeout );

	/*
	 * operations
	 */

	USBDeviceBase &reset();
	USBDeviceBase &customEEPROMWrite( int address, const UCharArray &data );
	UCharArray customEEPROMRead( int address, int numBytes );



};	// class USBDeviceBase





extern std::ostream &operator<<( std::ostream &out, USBDeviceBase &device );
extern std::ostream &operator<<( std::ostream &out, USBDeviceBase *device );



}	// namespace AIOUSB

#endif

/* end of file */
