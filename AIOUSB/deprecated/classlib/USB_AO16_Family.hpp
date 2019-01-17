/*
 * $RCSfile: USB_AO16_Family.hpp,v $
 * $Revision: 1.6 $
 * $Date: 2009/12/23 22:39:10 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class USB_AO16_Family declarations
 */

#if ! defined( USB_AO16_Family_hpp )
#define USB_AO16_Family_hpp


#include <USBDeviceBase.hpp>
#include <AO16_AnalogOutputSubsystem.hpp>
#include <DigitalIOSubsystem.hpp>


namespace AIOUSB {

/**
 * Class USB_AO16_Family represents a USB-AO16-family device, which encompasses the following product IDs:
 * USB_AO16_16A, USB_AO16_16, USB_AO16_12A, USB_AO16_12, USB_AO16_8A, USB_AO16_8, USB_AO16_4A, USB_AO16_4,
 * USB_AO12_16A, USB_AO12_16, USB_AO12_12A, USB_AO12_12, USB_AO12_8A, USB_AO12_8, USB_AO12_4A, USB_AO12_4.
 * Instances of class <i>USB_AO16_Family</i> are automatically created by the USB device manager when they are
 * detected on the bus. You should use one of the <i>USBDeviceManager</i> search methods, such as
 * <i>USBDeviceManager::getDeviceByProductID( int productID ) const</i>,
 * to obtain a reference to a <i>USB_AO16_Family</i> instance. You can then cast the <i>USBDeviceBase</i>
 * reference obtained from one of those methods to a <i>USB_AO16_Family</i> and make use of this class' methods, like so:
 * <pre>USBDeviceArray devices = deviceManager.getDeviceByProductID( USB_AO16_16A, USB_AO16_4 );
 *if( devices.size() > 0 )
 *  USB_AO16_Family &device = *( USB_AO16_Family * ) devices.at( 0 );</pre>
 */

class USB_AO16_Family : public USBDeviceBase {
	friend class USBDeviceManager;


private:
	static IntArray supportedProductIDs;

	static void initialize();



protected:
	AO16_AnalogOutputSubsystem analogOutputSubsystem;
	DigitalIOSubsystem digitalIOSubsystem;



protected:
	USB_AO16_Family( int productID, int deviceIndex );
	virtual ~USB_AO16_Family();



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
	 * Gets a reference to the analog output subsystem of this device.
	 * @return A reference to the analog output subsystem.
	 */

	AO16_AnalogOutputSubsystem &dac() {
		return analogOutputSubsystem;
	}	// adc()

	/**
	 * Gets a reference to the digital I/O subsystem of this device.
	 * @return A reference to the digital I/O subsystem.
	 */

	DigitalIOSubsystem &dio() {
		return digitalIOSubsystem;
	}	// dio()



};	// class USB_AO16_Family

}	// namespace AIOUSB

#endif

/* end of file */
