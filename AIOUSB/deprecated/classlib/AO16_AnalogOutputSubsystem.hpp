/*
 * $RCSfile: AO16_AnalogOutputSubsystem.hpp,v $
 * $Revision: 1.6 $
 * $Date: 2010/01/29 23:49:53 $
 * jEdit:tabSize=4:collapseFolds=1:
 *
 * class AO16_AnalogOutputSubsystem declarations
 */

#if ! defined( AO16_AnalogOutputSubsystem_hpp )
#define AO16_AnalogOutputSubsystem_hpp


#include <AnalogOutputSubsystem.hpp>
#include <AO16_OutputRange.hpp>
#include <OutputVoltagePoint.hpp>


namespace AIOUSB {

/**
 * Class AO16_AnalogOutputSubsystem represents the analog output subsystem of a device. One accesses
 * this analog output subsystem through its parent object, typically through a method such as
 * <i>dac() (see USB_AO16_Family::dac())</i>.
 */

class AO16_AnalogOutputSubsystem : public AnalogOutputSubsystem {
	friend class USB_AO16_Family;


public:
	/*
	 * range codes passed to setRange(); these values are defined by the device and must not be changed
	 */
	/** Unipolar, 0-5 volt range <i>(see setRange( int range ))</i>. */
	static const int RANGE_0_5V		= 0;		// 0-5V

	/** Bipolar, -5 to +5 volt range <i>(see setRange( int range ))</i>. */
	static const int RANGE_5V		= 1;		// +/-5V

	/** Unipolar, 0-10 volt range <i>(see setRange( int range ))</i>. */
	static const int RANGE_0_10V	= 2;		// 0-10V

	/** Bipolar, -10 to +10 volt range <i>(see setRange( int range ))</i>. */
	static const int RANGE_10V		= 3;		// +/-10V

	/** Minimum number of counts D/A can output. */
	static const int MIN_COUNTS		= 0;

	/** Maximum number of counts D/A can output. */
	static const int MAX_COUNTS		= 0xffff;



protected:
	static const char RANGE_TEXT[][ 10 ];
	AO16_OutputRange outputRange;				// device supports a single range for all D/A channels



protected:
	AO16_AnalogOutputSubsystem( USBDeviceBase &parent );
	virtual ~AO16_AnalogOutputSubsystem();



public:

	/*
	 * properties
	 */

	static std::string getRangeText( int range );

	/*
	 * configuration
	 */

	/**
	 * Gets the current voltage range of the D/A outputs.
	 * @return Current voltage range.
	 * @see setRange( int range )
	 */

	int getRange() const {
		return outputRange.getRange();
	}	// getRange()

	AO16_AnalogOutputSubsystem &setRange( int range );
	AO16_AnalogOutputSubsystem &writeVolts( int channel, double volts );
	AO16_AnalogOutputSubsystem &writeVolts( const OutputVoltagePointArray &points );

	/**
	 * Converts a single D/A count value to volts, based on the current range setting.
	 * @param counts the count value to convert to volts.
	 * @return A voltage value calculated using the current D/A range. The voltage value returned is
	 * constrained to the current minimum-maximum voltage range of the D/A.
	 * <i>(see setRange( int range ))</i>.
	 */

	double countsToVolts( unsigned short counts ) const {
		return outputRange.countsToVolts( counts );
	}	// countsToVolts()

	/**
	 * Converts a single voltage value to D/A counts, based on the current range setting.
	 * @param volts the voltage value to convert to counts.
	 * @return A count value calculated using the current D/A range. The count value returned is
	 * constrained to the current minimum-maximum count range of the D/A.
	 * <i>(see setRange( int range ))</i>.
	 */

	unsigned short voltsToCounts( double volts ) const {
		return ( unsigned short ) outputRange.voltsToCounts( volts );
	}	// voltsToCounts()



};	// class AO16_AnalogOutputSubsystem

}	// namespace AIOUSB

#endif

/* end of file */
