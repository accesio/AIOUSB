/*
 * $RCSfile: AI16_DataSet.hpp,v $
 * $Revision: 1.4 $
 * $Date: 2009/12/24 20:07:55 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AI16_DataSet declarations
 */

#if ! defined( AI16_DataSet_hpp )
#define AI16_DataSet_hpp


#include <ostream>
#include <time.h>
#include <AI16_DataPoint.hpp>


namespace AIOUSB {

class AnalogInputSubsystem;
class AI16_DataPoint;

/**
 * Class AI16_DataSet represents a data set captured from a USB_AI16_Family device. It comprises a
 * fairly complete snapshot of both the data and the sampling parameters, including a time stamp.
 * @see AnalogInputSubsystem::read( int startChannel, int numChannels )
 */

class AI16_DataSet {
	friend class AnalogInputSubsystem;

protected:
	AnalogInputSubsystem *subsystem;			// subsystem from which this data was obtained
	AI16_DataPointArray points;					// data points taken from device
	long timeStamp;								// approximate time stamp when data was captured
	int calMode;								// calibration mode (AnalogInputSubsystem::CAL_MODE_*)
	int triggerMode;							// trigger mode (AnalogInputSubsystem::TRIG_MODE_*)
	int overSample;								// over-samples
	bool discardFirstSample;					// true == first sample was discarded

	AI16_DataSet( AnalogInputSubsystem &subsystem, int numPoints, long timeStamp
			, int calMode, int triggerMode, int overSample, bool discardFirstSample );

public:
	virtual ~AI16_DataSet();

	/**
	 * Gets the subsystem from which this data set was obtained.
	 * @return The subsystem from which this data set was obtained.
	 */

	AnalogInputSubsystem &getSubsystem() {
		return *subsystem;
	}	// getSubsystem()

	/**
	 * Gets the data point array from this data set.
	 * @return The data point array from this data set.
	 */

	const AI16_DataPointArray &getPoints() {
		return points;
	}	// getPoints()

	/**
	 * Gets the approximate time stamp when this data set was captured. The system time (obtained from
	 * <i>time()</i>) is recorded immediately prior to the sampling of the data, so
	 * it approximately represents the time when the data capture started. This property is not intended
	 * to be precise, but merely to serve as a convenient reference.
	 * @return The approximate time stamp when this data set was captured.
	 */

	long getTimeStamp() {
		return timeStamp;
	}	// getTimeStamp()

	/**
	 * Gets the calibration mode that was in effect when this data set was captured.
	 * @return The calibration mode that was in effect when this data set was captured.
	 * @see AnalogInputSubsystem::getCalMode() const
	 */

	int getCalMode() {
		return calMode;
	}	// getCalMode()

	/**
	 * Gets the trigger mode that was in effect when this data set was captured.
	 * @return The trigger mode that was in effect when this data set was captured.
	 * @see AnalogInputSubsystem::getTriggerMode() const
	 */

	int getTriggerMode() {
		return triggerMode;
	}	// getTriggerMode()

	/**
	 * Gets the over-sample setting that was in effect when this data set was captured.
	 * @return The over-sample setting that was in effect when this data set was captured.
	 * @see AnalogInputSubsystem::getOverSample() const
	 */

	int getOverSample() {
		return overSample;
	}	// getOverSample()

	/**
	 * Gets the sample discard mode that was in effect when this data set was captured.
	 * @return The sample discard mode that was in effect when this data set was captured.
	 * @see AnalogInputSubsystem::isDiscardFirstSample() const
	 */

	bool isDiscardFirstSample() {
		return discardFirstSample;
	}	// isDiscardFirstSample()

	std::ostream &print( std::ostream &out );
};	// class AI16_DataSet

}	// namespace AIOUSB

#endif

/* end of file */
