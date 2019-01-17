/*
 * $RCSfile: OutputVoltagePoint.hpp,v $
 * $Revision: 1.4 $
 * $Date: 2009/12/20 20:54:52 $
 * jEdit:tabSize=4:collapseFolds=1:
 *
 * class OutputVoltagePoint, OutputVoltagePointArray declarations
 */

#if ! defined( OutputVoltagePoint_hpp )
#define OutputVoltagePoint_hpp


#include <vector>


namespace AIOUSB {

/**
 * Class OutputVoltagePoint represents a single analog output data point, consisting of a D/A
 * channel number and a voltage to output to that channel. It is used by methods
 * <i>AO16_AnalogOutputSubsystem::writeVolts( const OutputVoltagePointArray &points )</i> and
 * <i>DA12_AnalogOutputSubsystem::writeVolts( const OutputVoltagePointArray &points )</i> to output a series
 * of voltages to multiple D/A channels.
 */

class OutputVoltagePoint {
public:

	/** Channel number to output voltage to. */
	int channel;

	/** Voltage to output. */
	double volts;

	/**
	 * Default constructor for analog output data point.
	 */

	OutputVoltagePoint() {
		// nothing to do
	}	// OutputVoltagePoint()

	/**
	 * Constructor for analog output data point.
	 * @param channel the channel number to output voltage to.
	 * @param volts the voltage to output.
	 */

	OutputVoltagePoint( int channel, double volts ) {
		this->channel = channel;
		this->volts = volts;
	}	// OutputVoltagePoint()

};	// class OutputVoltagePoint



class OutputVoltagePointArray : public std::vector<OutputVoltagePoint> {
public:
	OutputVoltagePointArray( int size = 0 ) : std::vector<OutputVoltagePoint>( size ) {}
};	// class OutputVoltagePointArray



}	// namespace AIOUSB

#endif

/* end of file */
