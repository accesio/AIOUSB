/*
 * $RCSfile: DigitalIOSubsystem.hpp,v $
 * $Revision: 1.10 $
 * $Date: 2009/12/20 20:54:52 $
 * jEdit:tabSize=4:collapseFolds=1:
 *
 * class DigitalIOSubsystem declarations
 */

#if ! defined( DigitalIOSubsystem_hpp )
#define DigitalIOSubsystem_hpp

#include "AIOTypes.h"
#include <DeviceSubsystem.hpp>


namespace AIOUSB {

/**
 * Class DigitalIOSubsystem represents the digital I/O subsystem of a device. One accesses
 * this analog output subsystem through its parent object, typically through a method such as
 * <i>dio() (see USB_AI16_Family::dio())</i>.
 */

class DigitalIOSubsystem : public DeviceSubsystem {
	friend class USB_AI16_Family;
	friend class USB_AO16_Family;
	friend class USB_DIO_Family;
	friend class USB_DIO_16_Family;
	friend class USB_DIO_32_Family;


public:


protected:
	int numPorts;								// number of digital I/O ports (bytes)
	int numChannels;							// number of digital I/O channels (bits)
	int numTristateGroups;						// number of tristate groups (bytes)
	int numTristateChannels;					// number of tristate channels (bits)
	UCharArray writeValues;						// last values written to device


public:
	UCharArray &bitsToBytes( UCharArray &dest, int bit, const BoolArray &src );
	BoolArray &bytesToBits( BoolArray &dest, const UCharArray &src, int bit );
	DigitalIOSubsystem( USBDeviceBase &parent );
	virtual ~DigitalIOSubsystem();



public:

	/*
	 * properties
	 */

	virtual std::ostream &print( std::ostream &out );

	/**
	 * Gets the number of digital I/O ports (bytes).
	 * @return Number of ports, numbered 0 to n-1.
	 */

	int getNumPorts() const {
		return numPorts;
	}	// getNumPorts()

	/**
	 * Gets the number of digital I/O channels (bits). The number of "channels" is simply equal
	 * to the number of ports times the number of channels per port, which is eight.
	 * @return Number of channels, numbered 0 to n-1.
	 */

	int getNumChannels() const {
		return numChannels;
	}	// getNumChannels()

	/**
	 * Gets the number of tristate groups (bytes).
	 * @return Number of tristate groups, numbered 0 to n-1.
	 */

	int getNumTristateGroups() const {
		return numTristateGroups;
	}	// getNumTristateGroups()

	/**
	 * Gets the number of tristate channels (bits). The number of "channels" is simply equal
	 * to the number of tristate groups times the number of channels per group, which is eight.
	 * @return Number of tristate channels, numbered 0 to n-1.
	 */

	int getNumTristateChannels() const {
		return numTristateChannels;
	}	// getNumTristateChannels()

	/*
	 * configuration
	 */

	DigitalIOSubsystem &configure( bool tristate, const BoolArray &outputs, const BoolArray &values );
	DigitalIOSubsystem &configure( const BoolArray &tristates, const BoolArray &outputs, const BoolArray &values );
	DigitalIOSubsystem &getConfiguration( BoolArray *tristates, BoolArray *outputs );

	/*
	 * operations
	 */

	bool read( int channel );
	BoolArray read( int startChannel, int numChannels );
	DigitalIOSubsystem &write( int channel, bool value );
	DigitalIOSubsystem &write( int startChannel, const BoolArray &values );
};	// class DigitalIOSubsystem



}	// namespace AIOUSB

#endif

/* end of file */
