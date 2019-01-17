/*
 * $RCSfile: USB_DIO_Family.hpp,v $
 * $Revision: 1.5 $
 * $Date: 2009/12/23 22:39:10 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class USB_DIO_Family declarations
 */


#if ! defined( USB_DIO_Family_hpp )
#define USB_DIO_Family_hpp

#include "CppCommon.h"
#include <USBDeviceBase.hpp>
#include <DigitalIOSubsystem.hpp>


namespace AIOUSB {

/**
 * Class USB_DIO_Family represents a USB-DIO-family device, which performs basic digital I/O and
 * encompasses the following product IDs:
 * USB_DIO_48, USB_DIO_96, USB_IIRO_16, USB_II_16, USB_RO_16, USB_IIRO_8, USB_II_8, USB_IIRO_4,
 * USB_IDIO_16, USB_II_16_OLD, USB_IDO_16, USB_IDIO_8, USB_II_8_OLD, USB_IDIO_4, USB_IIRO4_2SM, USB_IIRO4_COM,
 * USB_DIO16RO8, PICO_DIO16RO8.
 * Instances of class <i>USB_DIO_Family</i> are automatically created by the USB device manager when they are
 * detected on the bus. You should use one of the <i>USBDeviceManager</i> search methods, such as
 * <i>USBDeviceManager::getDeviceByProductID( int productID ) const</i>,
 * to obtain a reference to a <i>USB_DIO_Family</i> instance. You can then cast the <i>USBDeviceBase</i>
 * reference obtained from one of those methods to a <i>USB_DIO_Family</i> and make use of this class' methods, like so:
 * <pre>USBDeviceArray devices = deviceManager.getDeviceByProductID( USB_DIO_48, USB_DIO_96 );
 *if( devices.size() > 0 )
 *  USB_DIO_Family &device = *( USB_DIO_Family * ) devices.at( 0 );</pre>
 */

class USB_DIO_Family : public USBDeviceBase {
	friend class USBDeviceManager;


private:
	static IntArray supportedProductIDs;

	static void initialize();



protected:
	DigitalIOSubsystem digitalIOSubsystem;



protected:
	USB_DIO_Family( int productID, int deviceIndex );
	virtual ~USB_DIO_Family();



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



};	// class USB_DIO_Family

}	// namespace AIOUSB

#endif

/* end of file */
