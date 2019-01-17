/*
 * $RCSfile: AI16_DataPoint.hpp,v $
 * $Revision: 1.3 $
 * $Date: 2009/12/20 23:38:48 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AI16_DataPoint, AI16_DataPointArray declarations
 */

#if ! defined( AI16_DataPoint_hpp )
#define AI16_DataPoint_hpp


#include <string>
#include <vector>


namespace AIOUSB {

/**
 * Class AI16_DataPoint represents a single data point captured from a
 * USB_AI16_Family device. It encapsulates not only the captured
 * sample, but the channel from which the sample was captured and the
 * range and differential mode in effect when the sample was captured,
 * providing a fairly complete representation of the captured
 * data. This class also provides methods to retrieve the captured
 * data in either A/D counts or volts.
 */

class AI16_DataPoint {
	friend class AI16_DataPointArray;
	friend class AI16_DataSet;
	friend class AnalogInputSubsystem;
	friend class std::vector<AI16_DataPoint>;

public:
	int counts;            // A/D count data
	int channel;           // channel associated with count data
	int range;             // range associated with count data
	bool differentialMode; // differential mode associated with count data

	AI16_DataPoint();



	/**
	 * Gets the channel number from which this data point was captured.
	 * @return The channel number from which this data point was captured.
	 */

	int getChannel() const {
		return channel;
	}	// getChannel()

	/**
	 * Gets the range that was in effect when this data point was captured.
	 * @return The range that was in effect when this data point was captured.
	 * @see AnalogInputSubsystem::getRange( int channel ) const
	 */

	int getRange() const {
		return range;
	}	// getRange()

	std::string getRangeText() const;

	/**
	 * Gets the differential/single-ended mode that was in effect when this data point was captured.
	 * @return The differential/single-ended mode that was in effect when this data point was captured.
	 * @see AnalogInputSubsystem::isDifferentialMode( int channel ) const
	 */

	bool isDifferentialMode() const {
		return differentialMode;
	}	// isDifferentialMode()

	/**
	 * Gets the captured data in A/D counts.
	 * @return The captured data in A/D counts.
	 */

	int getCounts() const {
		return counts;
	}	// getCounts()

	double getVolts() const;
	std::string toString() const;
};	// class AI16_DataPoint



class AI16_DataPointArray : public std::vector<AI16_DataPoint> {
public:
	AI16_DataPointArray( int size = 0 ) : std::vector<AI16_DataPoint>( size ) {}
};



}

#endif

/* end of file */
