/*
 * $RCSfile: AnalogIORange.hpp,v $
 * $Date: 2009/12/10 00:10:51 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AnalogIORange declarations
 */

#if ! defined( AnalogIORange_hpp )
#define AnalogIORange_hpp


#include <USBDeviceManager.hpp>


namespace AIOUSB {

/**
 * Class AnalogIORange helps manage analog I/O range settings and provides voltage-count conversion utilities.
 * A single instance can be used with devices that support just one range, or multiple instances can be used
 * with devices that support multiple ranges, such as a separate range per analog I/O channel. This class also
 * supports changing the range properties. Some devices, for instance, permit the range to be changed at run-time.
 * The class that owns this instance can change the range by calling one or more of the methods of this class.
 * Or, for devices that do not support changing the range, the properties can be set up once and left alone. Or,
 * some properties can be changed and others left alone. For example, devices that permit changing the voltage
 * range usually use a fixed count range. 
 */

class AnalogIORange {


protected:
	int range;									// current range (meaning defined by class that owns this instance)
	int minCounts;								// minimum A/D or D/A counts for current range (typically 0)
	int maxCounts;								// maximum A/D or D/A counts for current range (typically 0xfff or 0xffff)
	int rangeCounts;							// count range (maxCounts - minCounts), to avoid repeatedly calculating this
	double minVolts;							// minimum volts for current range
	double maxVolts;							// maximum volts for current range
	double rangeVolts;							// voltage range (maxVolts - minVolts), to avoid repeatedly calculating this


public:
	AnalogIORange();
	AnalogIORange( int minCounts, int maxCounts );
	virtual ~AnalogIORange();

	/**
	 * Gets the current range ID.
	 * @return Current range ID (defined by class that owns this instance).
	 */

	int getRange() const {
		return range;
	}	// getRange()

	virtual AnalogIORange &setRange( int range );
	AnalogIORange &setCountRange( int minCounts, int maxCounts );
	AnalogIORange &setVoltRange( double minVolts, double maxVolts );
	double countsToVolts( int counts ) const;
	int voltsToCounts( double volts ) const;
};	// class AnalogIORange

}	// namespace AIOUSB

#endif

/* end of file */
