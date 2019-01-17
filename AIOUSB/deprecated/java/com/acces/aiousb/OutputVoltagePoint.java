/*
 * $RCSfile: OutputVoltagePoint.java,v $
 * $Date: 2009/12/08 19:28:00 $
 * $Revision: 1.4 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

/**
 * Class OutputVoltagePoint represents a single analog output data point, consisting of a D/A
 * channel number and a voltage to output to that channel. It is used by method
 * <i>{@link AO16_AnalogOutputSubsystem#writeVolts( OutputVoltagePoint[] points )}</i> to output a series
 * of voltages to multiple D/A channels.
 */

public class OutputVoltagePoint {

	/** Channel number to output voltage to. */
	public int channel;

	/** Voltage to output. */
	public double volts;

	/**
	 * Default constructor for analog output data point.
	 */

	public OutputVoltagePoint() {
		// nothing to do
	}	// OutputVoltagePoint()

	/**
	 * Constructor for analog output data point.
	 * @param channel the channel number to output voltage to.
	 * @param volts the voltage to output.
	 */

	public OutputVoltagePoint( int channel, double volts ) {
		this.channel = channel;
		this.volts = volts;
	}	// OutputVoltagePoint()

}	// class OutputVoltagePoint

/* end of file */
