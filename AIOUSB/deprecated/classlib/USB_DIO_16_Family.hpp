/*
 * $RCSfile: USB_DIO_16_Family.hpp,v $
 * $Revision: 1.5 $
 * $Date: 2009/12/23 22:39:10 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class USB_DIO_16_Family declarations
 */

#if ! defined( USB_DIO_16_Family_hpp )
#define USB_DIO_16_Family_hpp


#include <USBDeviceBase.hpp>
#include <DigitalIOSubsystem.hpp>
#include <DIOStreamSubsystem.hpp>


namespace AIOUSB {

/**
 * Class USB_DIO_16_Family represents a USB-DIO-16-family device, which encompasses the following product IDs:
 * USB_DI16A_REV_A1, USB_DO16A_REV_A1, USB_DI16A_REV_A2, USB_DIO_16H, USB_DI16A, USB_DO16A, USB_DIO_16A.
 * Instances of class <i>USB_DIO_16_Family</i> are automatically created by the USB device manager when they are
 * detected on the bus. You should use one of the <i>USBDeviceManager</i> search methods, such as
 * <i>USBDeviceManager::getDeviceByProductID( int productID ) const</i>,
 * to obtain a reference to a <i>USB_DIO_16_Family</i> instance. You can then cast the <i>USBDeviceBase</i>
 * reference obtained from one of those methods to a <i>USB_DIO_16_Family</i> and make use of this class' methods, like so:
 * <pre>USBDeviceArray devices = deviceManager.getDeviceByProductID( USB_DIO_16H, USB_DIO_16A );
 *if( devices.size() > 0 )
 *  USB_DIO_16_Family &device = *( USB_DIO_16_Family * ) devices.at( 0 );</pre>
 */

class USB_DIO_16_Family : public USBDeviceBase {
	friend class USBDeviceManager;


private:
	static IntArray supportedProductIDs;

	static void initialize();



protected:
	DigitalIOSubsystem digitalIOSubsystem;
	DIOStreamSubsystem dioStreamSubsystem;



protected:
	USB_DIO_16_Family( int productID, int deviceIndex );
	virtual ~USB_DIO_16_Family();



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
	 * Gets a reference to the digital I/O subsystem of this device.
	 * @return A reference to the digital I/O subsystem.
	 */

	DigitalIOSubsystem &dio() {
		return digitalIOSubsystem;
	}	// dio()

	/**
	 * Gets a reference to the digital I/O streaming subsystem of this device.
	 * @return A reference to the digital I/O streaming subsystem.
	 */

	DIOStreamSubsystem &diostream() {
		return dioStreamSubsystem;
	}	// diostream()



};	// class USB_DIO_16_Family

}	// namespace AIOUSB

#endif

/* end of file */
