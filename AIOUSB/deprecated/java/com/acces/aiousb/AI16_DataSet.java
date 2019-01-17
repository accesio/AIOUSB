/*
 * $RCSfile: AI16_DataSet.java,v $
 * $Date: 2009/12/20 23:40:31 $
 * $Revision: 1.4 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.lang.*;
import java.util.*;
// }}}

/**
 * Class AI16_DataSet represents a data set captured from a {@link USB_AI16_Family} device. It comprises a
 * fairly complete snapshot of both the data and the sampling parameters, including a time stamp.
 * @see AnalogInputSubsystem#read( int startChannel, int numChannels ) AnalogInputSubsystem.read()
 */

public class AI16_DataSet {
	protected AnalogInputSubsystem subsystem;	// subsystem from which this data was obtained
	protected AI16_DataPoint[] points;			// data points taken from device
	protected long timeStamp;					// approximate time stamp when data was captured
	protected int calMode;						// calibration mode (AnalogInputSubsystem.CAL_MODE_*)
	protected int triggerMode;					// trigger mode (AnalogInputSubsystem.TRIG_MODE_*)
	protected int overSample;					// over-samples
	protected boolean discardFirstSample;		// true == first sample was discarded

	protected AI16_DataSet( AnalogInputSubsystem subsystem, int numPoints, long timeStamp
			, int calMode, int triggerMode, int overSample, boolean discardFirstSample ) {
		assert numPoints > 0;
		points = new AI16_DataPoint[ numPoints ];
		assert points != null;
		this.timeStamp = timeStamp;
		this.subsystem = subsystem;
		this.calMode = calMode;
		this.triggerMode = triggerMode;
		this.overSample = overSample;
		this.discardFirstSample = discardFirstSample;
	}	// AI16_DataSet()

	/**
	 * Gets the subsystem from which this data set was obtained.
	 * @return The subsystem from which this data set was obtained.
	 */

	public AnalogInputSubsystem getSubsystem() {
		return subsystem;
	}	// getSubsystem()

	/**
	 * Gets the data point array from this data set.
	 * @return The data point array from this data set.
	 */

	public AI16_DataPoint[] getPoints() {
		return points;
	}	// getPoints()

	/**
	 * Gets the approximate time stamp when this data set was captured. The system time (obtained from
	 * <i>System.currentTimeMillis()</i>) is recorded immediately prior to the sampling of the data, so
	 * it approximately represents the time when the data capture started. This property is not intended
	 * to be precise, but merely to serve as a convenient reference. The time stamp may be converted to
	 * more useful forms using the java.util.Calendar or java.util.Date classes.
	 * @return The approximate time stamp when this data set was captured.
	 */

	public long getTimeStamp() {
		return timeStamp;
	}	// getTimeStamp()

	/**
	 * Gets the calibration mode that was in effect when this data set was captured.
	 * @return The calibration mode that was in effect when this data set was captured.
	 * @see AnalogInputSubsystem#getCalMode() AnalogInputSubsystem.getCalMode()
	 */

	public int getCalMode() {
		return calMode;
	}	// getCalMode()

	/**
	 * Gets the trigger mode that was in effect when this data set was captured.
	 * @return The trigger mode that was in effect when this data set was captured.
	 * @see AnalogInputSubsystem#getTriggerMode() AnalogInputSubsystem.getTriggerMode()
	 */

	public int getTriggerMode() {
		return triggerMode;
	}	// getTriggerMode()

	/**
	 * Gets the over-sample setting that was in effect when this data set was captured.
	 * @return The over-sample setting that was in effect when this data set was captured.
	 * @see AnalogInputSubsystem#getOverSample() AnalogInputSubsystem.getOverSample()
	 */

	public int getOverSample() {
		return overSample;
	}	// getOverSample()

	/**
	 * Gets the sample discard mode that was in effect when this data set was captured.
	 * @return The sample discard mode that was in effect when this data set was captured.
	 * @see AnalogInputSubsystem#isDiscardFirstSample() AnalogInputSubsystem.isDiscardFirstSample()
	 */

	public boolean isDiscardFirstSample() {
		return discardFirstSample;
	}	// isDiscardFirstSample()

	/**
	 * Prints this data set. Mainly useful for diagnostic purposes.
	 * @param stream the print stream where the data set will be printed.
	 * @return The print stream.
	 */

	public PrintStream print( PrintStream stream ) {
		Date date = new Date( timeStamp );
		stream.println(
			"Analog input data set captured from " + subsystem.parent.getName()
				+ " (S/N 0x" + Long.toString( subsystem.parent.getSerialNumber(), 16 ) + "):\n"
			+ "  Time stamp: " + date + "\n"
			+ "  Calibration mode: " + calMode + "\n"
			+ "  Trigger mode: " + triggerMode + "\n"
			+ "  Over-sample: " + overSample + "\n"
			+ "  Discard first sample: " + discardFirstSample + "\n"
			+ "  " + points.length + " data points:"
		);
		for( AI16_DataPoint point : points )
			stream.println( "    " + point );
		return stream;
	}	// print()

}	// class AI16_DataSet

/* end of file */
