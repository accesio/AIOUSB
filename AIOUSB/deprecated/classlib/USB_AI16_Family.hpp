/*
 * $RCSfile: USB_AI16_Family.hpp,v $
 * $Revision: 1.18 $
 * $Date: 2009/12/23 22:39:10 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class USB_AI16_Family declarations
 */

#if ! defined( USB_AI16_Family_hpp )
#define USB_AI16_Family_hpp


#include <USBDeviceBase.hpp>
#include <AnalogInputSubsystem.hpp>
#include <DigitalIOSubsystem.hpp>
#include <CounterSubsystem.hpp>


namespace AIOUSB {

/**
 * @verbatim
 * Class USB_AI16_Family represents a USB-AI16-family device, which encompasses the following product IDs:
 * USB_AI16_16A, USB_AI16_16E, USB_AI12_16A, USB_AI12_16, USB_AI12_16E, USB_AI16_64MA, USB_AI16_64ME, USB_AI12_64MA,
 * USB_AI12_64M, USB_AI12_64ME, USB_AI16_32A, USB_AI16_32E, USB_AI12_32A, USB_AI12_32, USB_AI12_32E, USB_AI16_64A,
 * USB_AI16_64E, USB_AI12_64A, USB_AI12_64, USB_AI12_64E, USB_AI16_96A, USB_AI16_96E, USB_AI12_96A, USB_AI12_96,
 * USB_AI12_96E, USB_AI16_128A, USB_AI16_128E, USB_AI12_128A, USB_AI12_128, USB_AI12_128E.
 * Instances of class <i>USB_AI16_Family</i> are automatically created by the USB device manager when they are
 * detected on the bus. You should use one of the <i>USBDeviceManager</i> search methods, such as
 * <i>USBDeviceManager::getDeviceByProductID( int productID ) const</i>,
 * to obtain a reference to a <i>USB_AI16_Family</i> instance. You can then cast the <i>USBDeviceBase</i>
 * reference obtained from one of those methods to a <i>USB_AI16_Family</i> and make use of this class' methods, like so:
 * <pre>USBDeviceArray devices = deviceManager.getDeviceByProductID( USB_AI12_32A, USB_AI12_32E );
 *if( devices.size() > 0 )
 *  USB_AI16_Family &device = *( USB_AI16_Family * ) devices.at( 0 );</pre>
 * @endverbatim
 */

class USB_AI16_Family : public USBDeviceBase {
	friend class USBDeviceManager;


private:
	static IntArray supportedProductIDs;

	static void initialize();



protected:
	AnalogInputSubsystem analogInputSubsystem;
	DigitalIOSubsystem digitalIOSubsystem;
	CounterSubsystem counterSubsystem;



protected:
	USB_AI16_Family( int productID, int deviceIndex );
	virtual ~USB_AI16_Family();



public:

	/*
	 * properties
	 */

	static StringArray getSupportedProductNames();
	static IntArray getSupportedProductIDs();
	static bool isSupportedProductID( int productID );
	virtual std::ostream &print( std::ostream &out );

	/*
	 * subsystems
	 */

	/**
	 * Gets a reference to the analog input subsystem of this device.
	 * @return A reference to the analog input subsystem.
	 */

	AnalogInputSubsystem &adc() {
		return analogInputSubsystem;
	}	// adc()

	/**
	 * Gets a reference to the digital I/O subsystem of this device.
	 * @return A reference to the digital I/O subsystem.
	 */

	DigitalIOSubsystem &dio() {
		return digitalIOSubsystem;
	}	// dio()

	/**
	 * Gets a reference to the counter/timer subsystem of this device.
	 * @return A reference to the counter/timer subsystem.
	 */

	CounterSubsystem &ctr() {
		return counterSubsystem;
	}	// ctr()



};	// class USB_AI16_Family

}	// namespace AIOUSB

#endif

/* end of file */
