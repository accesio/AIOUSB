/*
 * $RCSfile: AnalogOutputSubsystem.hpp,v $
 * $Revision: 1.11 $
 * $Date: 2010/01/29 23:49:53 $
 * jEdit:tabSize=4:collapseFolds=1:
 *
 * class AnalogOutputSubsystem declarations
 */

#if ! defined( AnalogOutputSubsystem_hpp )
#define AnalogOutputSubsystem_hpp


#include <DeviceSubsystem.hpp>


namespace AIOUSB {

/**
 * Class AnalogOutputSubsystem is the superclass of the analog output subsystem of a device. This
 * class provides basic features, such as writing count values to the D/As. More sophisticated
 * functions are provided by device-specific subclasses. One accesses this analog output subsystem
 * through its parent object, typically through a method such as <i>dac() (see USB_AO16_Family::dac())</i>.
 */

class AnalogOutputSubsystem : public DeviceSubsystem {


protected:
	int numChannels;							// number of D/A channels
	int minCounts;								// minimum counts D/A can output (typically 0)
	int maxCounts;								// maximum counts D/A can output (typically 0xfff or 0xffff)

public:
	AnalogOutputSubsystem( USBDeviceBase &parent );
	virtual ~AnalogOutputSubsystem();

	/*
	 * properties
	 */

	virtual std::ostream &print( std::ostream &out );

	/**
	 * Gets the number of analog output channels.
	 * @return Number of channels, numbered 0 to n-1.
	 */

	int getNumChannels() const {
		return numChannels;
	}	// getNumChannels()

	/*
	 * operations
	 */

	AnalogOutputSubsystem &writeCounts( int channel, unsigned short counts );
	AnalogOutputSubsystem &writeCounts( const UShortArray &points );



};	// class AnalogOutputSubsystem

}	// namespace AIOUSB

#endif

/* end of file */
