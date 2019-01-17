/*
 * $RCSfile: DA12_AnalogOutputSubsystem.hpp,v $
 * $Revision: 1.7 $
 * $Date: 2010/01/29 23:49:53 $
 * jEdit:tabSize=4:collapseFolds=1:
 *
 * class DA12_AnalogOutputSubsystem declarations
 */


#if ! defined( DA12_AnalogOutputSubsystem_hpp )
#define DA12_AnalogOutputSubsystem_hpp


#include <AnalogOutputSubsystem.hpp>
#include <DA12_OutputRange.hpp>
#include <OutputVoltagePoint.hpp>


namespace AIOUSB {

/**
 * Class DA12_AnalogOutputSubsystem represents the analog output subsystem of a device. One accesses
 * this analog output subsystem through its parent object, typically through a method such as
 * <i>dac() (see USB_DA12_8E_Family::dac())</i>.
 */

class DA12_AnalogOutputSubsystem : public AnalogOutputSubsystem {
	friend class USB_DA12_8A_Family;
	friend class USB_DA12_8E_Family;


public:
	/*
	 * range codes passed to setRange()
	 */
	/** Unipolar, 0-2.5 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_0_2_5V = 0;			// 0-2.5V

	/** Bipolar, -2.5 to +2.5 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_2_5V	= 1;			// +/-2.5V

	/** Unipolar, 0-5 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_0_5V	= 2;			// 0-5V

	/** Bipolar, -5 to +5 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_5V	= 3;			// +/-5V

	/** Unipolar, 0-10 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_0_10V	= 4;		// 0-10V

	/** Bipolar, -10 to +10 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_10V	= 5;			// +/-10V

	/** Minimum number of counts D/A can output. */
	static const int MIN_COUNTS = 0;

	/** Maximum number of counts D/A can output. */
	static const int MAX_COUNTS = 0xfff;



protected:
	static const char RANGE_TEXT[][ 10 ];
	DA12_OutputRange *outputRange;				// device supports a separate range for each D/A channel



protected:
	DA12_AnalogOutputSubsystem( USBDeviceBase &parent );
	virtual ~DA12_AnalogOutputSubsystem();



public:

	/*
	 * properties
	 */

	static std::string getRangeText( int range );

	/*
	 * configuration
	 */

	int getRange( int channel ) const;
	IntArray getRange( int startChannel, int numChannels ) const;
	DA12_AnalogOutputSubsystem &setRange( int channel, int range );
	DA12_AnalogOutputSubsystem &setRange( int startChannel, const IntArray &range );
	DA12_AnalogOutputSubsystem &setRange( int range );

	/*
	 * operations
	 */

	DA12_AnalogOutputSubsystem &writeVolts( int channel, double volts );
	DA12_AnalogOutputSubsystem &writeVolts( const OutputVoltagePointArray &points );
	double countsToVolts( int channel, unsigned short counts ) const;
	unsigned short voltsToCounts( int channel, double volts ) const;


};	// class DA12_AnalogOutputSubsystem

}	// namespace AIOUSB

#endif

/* end of file */
