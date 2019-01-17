/*
 * $RCSfile: USB_DA12_8A_Family.hpp,v $
 * $Revision: 1.5 $
 * $Date: 2009/12/23 22:39:10 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class USB_DA12_8A_Family declarations
 */


#if ! defined( USB_DA12_8A_Family_hpp )
#define USB_DA12_8A_Family_hpp


#include <USBDeviceBase.hpp>
#include <DA12_AnalogOutputSubsystem.hpp>
#include <DigitalIOSubsystem.hpp>


namespace AIOUSB {

/**
 * Class USB_DA12_8A_Family represents a USB-DA12-8A-family device, which encompasses the following product IDs:
 * USB_DA12_8A_REV_A, USB_DA12_8A.
 * Instances of class <i>USB_DA12_8A_Family</i> are automatically created by the USB device manager when they are
 * detected on the bus. You should use one of the <i>USBDeviceManager</i> search methods, such as
 * <i>USBDeviceManager::getDeviceByProductID( int productID ) const</i>,
 * to obtain a reference to a <i>USB_DA12_8A_Family</i> instance. You can then cast the <i>USBDeviceBase</i>
 * reference obtained from one of those methods to a <i>USB_DA12_8A_Family</i> and make use of this class' methods, like so:
 * <pre>USBDeviceArray devices = deviceManager.getDeviceByProductID( USB_DA12_8A_REV_A, USB_DA12_8A );
 *if( devices.size() > 0 )
 *  USB_DA12_8A_Family &device = *( USB_DA12_8A_Family * ) devices.at( 0 );</pre>
 */

class USB_DA12_8A_Family : public USBDeviceBase {
	friend class USBDeviceManager;


private:
	static IntArray supportedProductIDs;

	static void initialize();



protected:
	DA12_AnalogOutputSubsystem analogOutputSubsystem;



protected:
	USB_DA12_8A_Family( int productID, int deviceIndex );
	virtual ~USB_DA12_8A_Family();



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

	DA12_AnalogOutputSubsystem &dac() {
		return analogOutputSubsystem;
	}	// adc()



};	// class USB_DA12_8A_Family

}	// namespace AIOUSB

#endif

/* end of file */
