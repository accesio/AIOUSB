/*
 * $RCSfile: AnalogInputSubsystem.java,v $
 * $Date: 2010/01/20 18:15:34 $
 * $Revision: 1.30 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
import java.util.*;
// }}}

/**
 * Class AnalogInputSubsystem represents the analog input subsystem of a device. One accesses
 * this analog input subsystem through its parent object, typically through a method such as
 * <i>adc() (see {@link USB_AI16_Family#adc()})</i>.
 */

public class AnalogInputSubsystem extends DeviceSubsystem {

	// {{{ public constants
	/*
	 * A/D calibration modes; if ground or reference mode is selected, only one A/D
	 * sample may be taken at a time; that means, one channel and no oversampling;
	 * attempting to read more than one channel or use an oversample setting of more
	 * than zero will result in a timeout error
	 */
	/** Selects normal measurement mode <i>(see {@link #setCalMode( int calMode ) setCalMode()})</i>. */
	public static final int CAL_MODE_NORMAL			= 0;		// normal measurement
	/** Selects ground calibration mode <i>(see {@link #setCalMode( int calMode ) setCalMode()})</i>. */
	public static final int CAL_MODE_GROUND			= 1;		// measure ground
	/** Selects reference (full scale) calibration mode <i>(see {@link #setCalMode( int calMode ) setCalMode()})</i>. */
	public static final int CAL_MODE_REFERENCE		= 3;		// measure reference

	/*
	 * A/D trigger and counter bits
	 */
	/** If set, counter 0 is externally triggered <i>(see {@link #setTriggerMode( int triggerMode ) setTriggerMode()})</i>. */
	public static final int TRIG_MODE_CTR0_EXT		= 0x10;		// 1 == counter 0 externally triggered
	/** If set, the A/D is triggered by the falling edge of its trigger source, otherwise it's triggered by the rising edge
	 * <i>(see {@link #setTriggerMode( int triggerMode ) setTriggerMode()})</i>.
	 */
	public static final int TRIG_MODE_FALLING_EDGE	= 0x08;		// 1 == triggered by falling edge
	/** If set, each trigger will cause the A/D to scan all the channels, otherwise the A/D will read a single channel with each trigger
	 * <i>(see {@link #setTriggerMode( int triggerMode ) setTriggerMode()})</i>.
	 */
	public static final int TRIG_MODE_SCAN			= 0x04;		// 1 == scan all channels, 0==single channel
	/** If set, the A/D is triggered by an external pin on the board <i>(see {@link #setTriggerMode( int triggerMode ) setTriggerMode()})</i>. */
	public static final int TRIG_MODE_EXTERNAL		= 0x02;		// 1 == triggered by external pin on board
	/** If set, the A/D is triggered by counter 2 <i>(see {@link #setTriggerMode( int triggerMode ) setTriggerMode()})</i>. */
	public static final int TRIG_MODE_TIMER			= 0x01;		// 1 == triggered by counter 2

	/** Unipolar, 0-10 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_0_10V				= 0;		// 0-10V
	/** Bipolar, -10 to +10 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_10V				= 1;		// +/-10V
	/** Unipolar, 0-5 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_0_5V				= 2;		// 0-5V
	/** Bipolar, -5 to +5 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_5V				= 3;		// +/-5V
	/** Unipolar, 0-2 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_0_2V				= 4;		// 0-2V
	/** Bipolar, -2 to +2 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_2V				= 5;		// +/-2V
	/** Unipolar, 0-1 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_0_1V				= 6;		// 0-1V
	/** Bipolar, -1 to +1 volt range <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. */
	public static final int RANGE_1V				= 7;		// +/-1V

	/** Minimum number of counts A/D can read. */
	public static final int MIN_COUNTS 				= 0;

	/** Maximum number of counts A/D can read. */
	public static final int MAX_COUNTS				= 0xffff;

	/** Number of 16-bit words in an A/D calibration table (65,536 16-bit words). */
	public static final int CAL_TABLE_WORDS			= 64 * 1024;
	// }}}

	// {{{ protected constants
	protected static final String[] RANGE_TEXT = {
		/*
		 * these strings are indexed by RANGE_* constants above
		 */
		  "0-10V"
		, "+/-10V"
		, "0-5V"
		, "+/-5V"
		, "0-2V"
		, "+/-2V"
		, "0-1V"
		, "+/-1V"
	};	// RANGE_TEXT[]
	// }}}

	// {{{ protected members
	/*
	 * register indexes
	 */
	protected static final int NUM_CONFIG_REGISTERS	= 20;		// number of "registers" (bytes) in config. block
	protected static final int NUM_MUX_CONFIG_REGISTERS = 21;	// number of "registers" (bytes) in config. block when MUX installed
	protected static final int NUM_GAIN_CODE_REGISTERS = 16;	// number of gain code (range) registers in config. block
	protected static final int REG_GAIN_CODE		= 0;		// gain code (range) for A/D channel 0 (one of RANGE_* settings above)
																// gain codes for channels 1-15 occupy configuration block indexes 1-15
	protected static final int REG_CAL_MODE			= 16;		// calibration mode (one of CAL_MODE_* settings below)
	protected static final int REG_TRIG_MODE		= 17;		// trigger mode (one of TRIG_MODE_* settings below)
	protected static final int REG_START_END		= 18;		// start and end channels for scan (bits 7-4 contain end channel, bits 3-0 contain start channel)
	protected static final int REG_OVERSAMPLE		= 19;		// oversample setting (0-255 samples in addition to single sample)
	protected static final int REG_MUX_START_END	= 20;		// MUX start and end channels for scan (bits 7-4 contain end channel MS-nibble, bits 3-0 contain start channel MS-nibble)

	protected static final int DIFFERENTIAL_MODE	= 8;		// OR with range to enable differential mode for channels 0-7
	protected static final int MAX_OVERSAMPLE		= 0xff;
	protected static final int TRIG_MODE_VALID_MASK	= (
		TRIG_MODE_CTR0_EXT
		| TRIG_MODE_FALLING_EDGE
		| TRIG_MODE_SCAN
		| TRIG_MODE_EXTERNAL
		| TRIG_MODE_TIMER );

	protected static final int AUTO_CAL_UNKNOWN		= 0;
	protected static final int AUTO_CAL_NOT_PRESENT	= 1;
	protected static final int AUTO_CAL_PRESENT		= 2;

	protected static final int MAX_CHANNELS			= 128;		// maximum number of channels supported by this software

	protected int numChannels;									// number of A/D channels
	protected int numMUXChannels;								// number of MUXed A/D channels
	protected int channelsPerGroup;								// number of A/D channels in each config. group (1, 4 or 8 depending on model)
	protected int configBlockSize;								// size of configuration block (bytes)
	protected int autoCalFeature = AUTO_CAL_UNKNOWN;			// cached A/D auto. cal. feature
	protected AI16_InputRange[] inputRange;						// range for each channel group
	protected boolean[] differentialMode;						// differential mode for each channel group
	protected int calMode;										// calibration mode (CAL_MODE_*)
	protected int triggerMode;									// trigger mode (TRIG_MODE_*)
	protected int startChannel;									// starting channel for scans
	protected int endChannel;									// ending channel for scans
	protected int overSample;									// over-samples
	protected int readBulkSamplesRequested;						// total number of samples requested during bulk read procedure
	protected int readBulkSamplesRetrieved;						// number of samples retrieved so far during  bulk read procedure
	protected boolean autoConfig = true;						// true == automatically send modified configuration to device
	// }}}

	// {{{ protected methods

	protected native int getConfigSize( int deviceIndex );
	protected native int getNumChannels( int deviceIndex );
	protected native int getNumMUXChannels( int deviceIndex );
	protected native int getChannelsPerGroup( int deviceIndex );
	protected native int queryCal( int deviceIndex );
		// => unsigned long ADC_QueryCal( unsigned long DeviceIndex )
	protected native boolean isDiscardFirstSample( int deviceIndex );
		// => AIOUSB_BOOL AIOUSB_IsDiscardFirstSample( unsigned long DeviceIndex )
	protected native int setDiscardFirstSample( int deviceIndex, boolean discard );
		// => unsigned long AIOUSB_SetDiscardFirstSample( unsigned long DeviceIndex, AIOUSB_BOOL discard )
	protected native int loadCalTable( int deviceIndex, String fileName );
		// => unsigned long AIOUSB_ADC_LoadCalTable( unsigned long DeviceIndex, const char *fileName )
	protected native int setCalTable( int deviceIndex, char[] calTable );
		// => unsigned long AIOUSB_ADC_SetCalTable( unsigned long DeviceIndex, const unsigned short calTable[] )
	protected native int getConfig( int deviceIndex, byte[] configBlock );
		// => unsigned long ADC_GetConfig( unsigned long DeviceIndex, unsigned char *pConfigBuf, unsigned long *ConfigBufSize )
	protected native int setConfig( int deviceIndex, byte[] configBlock );
		// => unsigned long ADC_SetConfig( unsigned long DeviceIndex, unsigned char *pConfigBuf, unsigned long *ConfigBufSize )
	protected native int internalCal( int deviceIndex, boolean autoCal, char[] returnCalTable, String saveFileName );
		// => unsigned long AIOUSB_ADC_InternalCal( unsigned long DeviceIndex, AIOUSB_BOOL autoCal, unsigned short returnCalTable[], const char *saveFileName )
	protected native int externalCal( int deviceIndex, double[] points, char[] returnCalTable, String saveFileName );
		// => unsigned long AIOUSB_ADC_ExternalCal( unsigned long DeviceIndex, const double points[], int numPoints, unsigned short returnCalTable[], const char *saveFileName )
	protected native int getScan( int deviceIndex, char[] counts );
		// => unsigned long AIOUSB_GetScan( unsigned long DeviceIndex, unsigned short counts[] )
	protected native int readBulkStart( int deviceIndex, int numSamples );
	protected native int readBulkSamplesRemaining( int deviceIndex, int[] samplesRemaining );
	protected native int readBulkNext( int deviceIndex, int offset, char[] counts );
	protected native int readBulkFinish( int deviceIndex );

	protected AnalogInputSubsystem setScanRange( int startChannel, int numChannels ) {
		final int endChannel = startChannel + numChannels - 1;
		if(
			numChannels < 1
			|| startChannel < 0
			|| ( configBlockSize == NUM_MUX_CONFIG_REGISTERS && endChannel >= numMUXChannels )
			|| ( configBlockSize != NUM_MUX_CONFIG_REGISTERS && endChannel >= this.numChannels )
		)
			throw new IllegalArgumentException( "Invalid start channel: " + startChannel
				+ ", or number of channels: " + numChannels );
		boolean configChanged = false;
		if( this.startChannel != startChannel ) {
			this.startChannel = startChannel;
			configChanged = true;
		}	// if( this.startChannel ...
		if( this.endChannel != endChannel ) {
			this.endChannel = endChannel;
			configChanged = true;
		}	// if( this.endChannel ...
		if(
			configChanged
			&& autoConfig
		)
			writeConfig();
		return this;
	}	// setScanRange()

	protected AnalogInputSubsystem( USBDevice parent ) {
		super( parent );
		numMUXChannels = getNumMUXChannels( getDeviceIndex() );
		assert numMUXChannels >= 1
			&& numMUXChannels <= MAX_CHANNELS;
		numChannels = getNumChannels( getDeviceIndex() );
		assert numChannels >= 1
			&& numChannels <= numMUXChannels;
		channelsPerGroup = getChannelsPerGroup( getDeviceIndex() );
		assert channelsPerGroup == 1
			|| channelsPerGroup == 4
			|| channelsPerGroup == 8;
		configBlockSize = getConfigSize( getDeviceIndex() );
		assert configBlockSize == NUM_CONFIG_REGISTERS
			|| configBlockSize == NUM_MUX_CONFIG_REGISTERS;
		inputRange = new AI16_InputRange[ NUM_GAIN_CODE_REGISTERS ];
		for( int group = 0; group < NUM_GAIN_CODE_REGISTERS; group++ )
			inputRange[ group ] = new AI16_InputRange( MIN_COUNTS, MAX_COUNTS );
		differentialMode = new boolean[ NUM_GAIN_CODE_REGISTERS ];
		readConfig();							// initializes remaining class members
	}	// AnalogInputSubsystem()

	// }}}

	// {{{ public methods

	/*
	 * properties
	 */

	/**
	 * Prints the properties of this subsystem. Mainly useful for diagnostic purposes.
	 * @param stream the print stream where properties will be printed.
	 * @return The print stream.
	 */

	public PrintStream print( PrintStream stream ) {
		stream.println( 
			  "    Number of A/D channels: " + numChannels + "\n"
			+ "    Number of MUXed A/D channels: " + numMUXChannels
		);
		return stream;
	}	// print()

	/**
	 * Gets the number of primary analog input channels.
	 * @return The number of channels, numbered 0 to n-1.
	 */

	public int getNumChannels() {
		return numChannels;
	}	// getNumChannels()

	/**
	 * Gets the number of analog input channels available through an optional multiplexer.
	 * @return The number of channels, numbered 0 to n-1.
	 */

	public int getNumMUXChannels() {
		return numMUXChannels;
	}	// getNumMUXChannels()

	/**
	 * Gets the number of analog input channels in each configuration group (1, 4 or 8 depending on the device model).
	 * @return The number of channels per group.
	 */

	public int getChannelsPerGroup() {
		return channelsPerGroup;
	}	// getChannelsPerGroup()

	/**
	 * Tells if automatic calibration is possible with this device.
	 * @param force <i>True</i> forces this class to interrogate the device anew; <i>false</i> returns the previous
	 * result if available, or interrogates the device if a previous result is not available.
	 * @return <i>True</i> indicates that automatic calibration is available.
	 * @see #calibrate( boolean autoCal, boolean returnCalTable, String saveFileName ) calibrate()
	 * @throws OperationFailedException
	 */

	public boolean isAutoCalPresent( boolean force ) {
		if(
			force
			|| autoCalFeature == AUTO_CAL_UNKNOWN
		) {
			final int ERROR_NOT_SUPPORTED = 10;	// AIOUSB result code
			final int result = queryCal( getDeviceIndex() );
			if( result == USBDeviceManager.SUCCESS )
				autoCalFeature = AUTO_CAL_PRESENT;
			else if( result == ERROR_NOT_SUPPORTED )
				autoCalFeature = AUTO_CAL_NOT_PRESENT;
			else {
				/*
				 * an error occurred; leave auto. cal. feature unchanged
				 */
				throw new OperationFailedException( result );
			}	// else if( result ...
		}	// if( force ...
		return autoCalFeature == AUTO_CAL_PRESENT;
	}	// isAutoCalPresent()

	/**
	 * Gets the textual string for the specified range.
	 * @param range the range for which to obtain the textual string.
	 * @return The textual string for the specified range.
	 * @see #setRange( int channel, int range ) setRange()
	 * @throws IllegalArgumentException
	 */

	public static String getRangeText( int range ) {
		if(
			range < RANGE_0_10V
			|| range > RANGE_1V
		)
			throw new IllegalArgumentException( "Invalid range: " + range );
		return RANGE_TEXT[ range ];
	}	// getRangeText()

	/*
	 * configuration
	 */

	/**
	 * Tells whether the modified configuration will be automatically sent to the device.
	 * @return <i>True</i> indicates that the modified configuration will be automatically sent to the device,
	 * <i>false</i> indicates that you will have to explicitly call <i>{@link #writeConfig() writeConfig()}</i>
	 * to send the configuration to the device.
	 * @see #setAutoConfig( boolean autoConfig ) setAutoConfig()
	 */

	public boolean isAutoConfig() {
		return autoConfig;
	}	// isAutoConfig()

	/**
	 * Enables or disables automatically sending the modified configuration to the device. Normally, it's desirable
	 * to send the modified configuration to the device immediately. However, in order to reduce the amount of
	 * communication with the device while setting multiple properties, this automatic sending mechanism can be
	 * disabled and the configuration can be sent by explicitly calling <i>{@link #writeConfig() writeConfig()}</i>
	 * once all the properties have been set, like so:
	 * <pre>device.adc()
	 *   .setAutoConfig( false )
	 *   .setCalMode( AnalogInputSubsystem.CAL_MODE_NORMAL )
	 *   .setTriggerMode( AnalogInputSubsystem.TRIG_MODE_SCAN | AnalogInputSubsystem.TRIG_MODE_TIMER )
	 *   .setOverSample( 50 )
	 *   .writeConfig()
	 *   .setAutoConfig( true );</pre>
	 * <i>Remember to call setAutoConfig( true ) after configuring the properties, otherwise all subsequent
	 * configuration changes will have to be explicitly sent to the device by calling
	 * {@link #writeConfig() writeConfig()}.</i>
	 * @param autoConfig <i>True</i> enables automatically sending modified configuration, <i>false</i> disables it.
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public AnalogInputSubsystem setAutoConfig( boolean autoConfig ) {
		this.autoConfig = autoConfig;
		return this;
	}	// isAutoConfig()

	/**
	 * Reads the A/D configuration from the device. This is done automatically when the class is
	 * instantiated, so it generally does not need to be done again. However, if the A/D configuration
	 * in the device has been changed without using this class (e.g. another program changed it),
	 * <i>readConfig()</i> can be used to copy the device's configuration into this class.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public AnalogInputSubsystem readConfig() {
		byte[] configBlock = new byte[ configBlockSize ];
		final int result = getConfig( getDeviceIndex(), configBlock );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		final int RANGE_VALID_MASK = 7;
		for( int group = 0; group < NUM_GAIN_CODE_REGISTERS; group++ ) {
			inputRange[ group ].setRange( configBlock[ REG_GAIN_CODE + group ] & RANGE_VALID_MASK );
			differentialMode[ group ] = ( configBlock[ REG_GAIN_CODE + group ] & DIFFERENTIAL_MODE ) != 0;
		}	// for( int group ...
		calMode = configBlock[ REG_CAL_MODE ] & 0xff;
		triggerMode = configBlock[ REG_TRIG_MODE ] & TRIG_MODE_VALID_MASK;
		if( configBlockSize == NUM_MUX_CONFIG_REGISTERS ) {
			startChannel
				= ( ( configBlock[ REG_MUX_START_END ] & 0x0f ) << 4 )
				| ( configBlock[ REG_START_END ] & 0xf );
			endChannel
				= ( configBlock[ REG_MUX_START_END ] & 0xf0 )
				| ( ( configBlock[ REG_START_END ] & 0xff ) >>> 4 );
		} else {
			startChannel = configBlock[ REG_START_END ] & 0xf;
			endChannel = ( configBlock[ REG_START_END ] & 0xff ) >>> 4;
		}	// if( configBlockSize ...
		overSample = configBlock[ REG_OVERSAMPLE ] & 0xff;
		return this;
	}	// readConfig()

	/**
	 * Writes the A/D configuration to the device. This is done automatically whenever the pertinent
	 * settings within this class are changed. However, if the A/D configuration in the device has been
	 * changed without using this class (e.g. another program changed it), or if automatic sending of
	 * the configuration has been disabled <i>(see {@link #setAutoConfig( boolean autoConfig ) setAutoConfig()})</i>,
	 * then <i>writeConfig()</i> can be used to copy this class' configuration settings into the device. 
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public AnalogInputSubsystem writeConfig() {
		byte[] configBlock = new byte[ configBlockSize ];
		for( int group = 0; group < NUM_GAIN_CODE_REGISTERS; group++ ) {
			int range = inputRange[ group ].getRange();
			if( differentialMode[ group ] )
				range |= DIFFERENTIAL_MODE;
			configBlock[ REG_GAIN_CODE + group ] = ( byte ) range;
		}	// for( int group ...
		configBlock[ REG_CAL_MODE ] = ( byte ) calMode;
		configBlock[ REG_TRIG_MODE ] = ( byte ) triggerMode;
		configBlock[ REG_START_END ] = ( byte ) ( ( endChannel << 4 ) | ( startChannel & 0x0f ) );
		if( configBlockSize == NUM_MUX_CONFIG_REGISTERS )
			configBlock[ REG_MUX_START_END ] = ( byte ) ( ( endChannel & 0xf0 ) | ( ( startChannel >>> 4 ) & 0x0f ) );
		configBlock[ REG_OVERSAMPLE ] = ( byte ) overSample;
		final int result = setConfig( getDeviceIndex(), configBlock );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// writeConfig()

	/**
	 * Tells if the <i>read()</i>, <i>readCounts()</i> and <i>readVolts()</i> functions will discard the first A/D sample taken.
	 * @return <i>False</i> indicates that no samples will be discarded; <i>true</i> indicates that the first sample will be discarded.
	 */

	public boolean isDiscardFirstSample() {
		return isDiscardFirstSample( getDeviceIndex() );
	}	// isDiscardFirstSample()

	/**
	 * Specifies whether the <i>read()</i>, <i>readCounts()</i> and <i>readVolts()</i> functions will discard the first A/D sample
	 * taken. This setting does <b>not</b> pertain to the <i>readBulkNext()</i> function which returns all of the raw data captured.
	 * Discarding the first sample may be useful in cases in which voltage "residue" from reading a different channel affects the
	 * channel currently being read.
	 * @param discard <i>false</i> indicates that no samples will be discarded; <i>true</i> indicates that the first sample will be discarded.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	public AnalogInputSubsystem setDiscardFirstSample( boolean discard ) {
		final int result = setDiscardFirstSample( getDeviceIndex(), discard );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setDiscardFirstSample()

	/**
	 * Gets the current calibration mode.
	 * @return Current calibration mode (<i>CAL_MODE_NORMAL</i>, <i>CAL_MODE_GROUND</i> or <i>CAL_MODE_REFERENCE</i>).
	 * @see #setCalMode( int calMode ) setCalMode()
	 */

	public int getCalMode() {
		return calMode;
	}	// getCalMode()

	/**
	 * Sets the A/D calibration mode. If ground or reference mode is selected, only one A/D sample may be taken at a time.
	 * That means, one channel and no oversampling. Attempting to read more than one channel or use an oversample setting
	 * of more than zero will result in a timeout error because the device will not send more than one sample. In order to
	 * protect users from accidentally falling into this trap, the <i>read*()</i> functions automatically and temporarily
	 * correct the scan parameters, restoring them when they complete.
	 * @param calMode the calibration mode. May be one of:<br>
	 * <i>{@link #CAL_MODE_NORMAL}<br>
	 * {@link #CAL_MODE_GROUND}<br>
	 * {@link #CAL_MODE_REFERENCE}</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogInputSubsystem setCalMode( int calMode ) {
		if(
			calMode != CAL_MODE_NORMAL
			&& calMode != CAL_MODE_GROUND
			&& calMode != CAL_MODE_REFERENCE
		)
			throw new IllegalArgumentException( "Invalid cal. mode: " + calMode );
		if( this.calMode != calMode ) {
			this.calMode = calMode;
			if( autoConfig )
				writeConfig();
		}	// if( this.calMode ...
		return this;
	}	// setCalMode()

	/**
	 * Gets the current trigger mode.
	 * @return Current trigger mode (bitwise OR of <i>TRIG_MODE_CTR0_EXT</i>, <i>TRIG_MODE_FALLING_EDGE</i>,
	 * <i>TRIG_MODE_SCAN</i>, <i>TRIG_MODE_EXTERNAL</i> or <i>TRIG_MODE_TIMER</i>).
	 * @see #setTriggerMode( int triggerMode ) setTriggerMode()
	 */

	public int getTriggerMode() {
		return triggerMode;
	}	// getTriggerMode()

	/**
	 * Sets the trigger mode.
	 * @param triggerMode a bitwise OR of these flags:<br>
	 * <i>{@link #TRIG_MODE_CTR0_EXT}<br>
	 * {@link #TRIG_MODE_FALLING_EDGE}<br>
	 * {@link #TRIG_MODE_SCAN}<br>
	 * {@link #TRIG_MODE_EXTERNAL}<br>
	 * {@link #TRIG_MODE_TIMER}</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogInputSubsystem setTriggerMode( int triggerMode ) {
		if( ( triggerMode & ~TRIG_MODE_VALID_MASK ) != 0 )
			throw new IllegalArgumentException( "Invalid trigger mode: " + triggerMode );
		if( this.triggerMode != triggerMode ) {
			this.triggerMode = triggerMode;
			if( autoConfig )
				writeConfig();
		}	// if( this.triggerMode ...
		return this;
	}	// setTriggerMode()

	/**
	 * Gets the current range for <i>channel</i>.
	 * @param channel the channel for which to obtain the current range.
	 * @return Current range for <i>channel</i>.
	 * @see #setRange( int channel, int range ) setRange()
	 * @throws IllegalArgumentException
	 */

	public int getRange( int channel ) {
		if(
			channel < 0
			|| channel >= numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		return inputRange[ channel / channelsPerGroup ].getRange();
	}	// getRange()

	/**
	 * Gets the current range for multiple A/D channels.
	 * @param startChannel the first channel for which to obtain the current range.
	 * @param numChannels the number of channels for which to obtain the current range.
	 * @return Array containing the current ranges for the specified channels.
	 * @see #setRange( int startChannel, int[] range ) setRange()
	 * @throws IllegalArgumentException
	 */

	public int[] getRange( int startChannel, int numChannels ) {
		if(
			numChannels < 1
			|| startChannel < 0
			|| startChannel + numChannels > numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid start channel: " + startChannel
				+ ", or number of channels: " + numChannels );
		int[] range = new int[ numChannels ];
		for( int index = 0; index < numChannels; index++ )
			range[ index ] = inputRange[ ( startChannel + index ) / channelsPerGroup ].getRange();
		return range;
	}	// getRange()

	/**
	 * Sets the range for a single A/D channel.
	 * @param channel the channel for which to set the range.
	 * @param range the range (voltage range) for the channel. May be one of:<br>
	 * <i>{@link #RANGE_0_1V}<br>
	 * {@link #RANGE_1V}<br>
	 * {@link #RANGE_0_2V}<br>
	 * {@link #RANGE_2V}<br>
	 * {@link #RANGE_0_5V}<br>
	 * {@link #RANGE_5V}<br>
	 * {@link #RANGE_0_10V}<br>
	 * {@link #RANGE_10V}</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @see #setDifferentialMode( int channel, boolean differentialMode ) setDifferentialMode()
	 */

	public AnalogInputSubsystem setRange( int channel, int range ) {
		if(
			channel < 0
			|| channel >= numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		final int group = channel / channelsPerGroup;
		if( inputRange[ group ].getRange() != range ) {
			inputRange[ group ].setRange( range );
			if( autoConfig )
				writeConfig();
		}	// if( inputRange[ ...
		return this;
	}	// setRange()

	/**
	 * Sets the range for multiple A/D channels.
	 * @param startChannel the first channel for which to set the range.
	 * @param range an array of ranges, one per channel <i>(see {@link #setRange( int channel, int range ) setRange()})</i>.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogInputSubsystem setRange( int startChannel, int[] range ) {
		if(
			range == null
			|| range.length < 1
			|| startChannel < 0
			|| startChannel + range.length > numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid range array or start channel: " + startChannel );
		boolean configChanged = false;
		for( int index = 0; index < range.length; index++ ) {
			final int group = ( startChannel + index ) / channelsPerGroup;
			if( inputRange[ group ].getRange() != range[ index ] ) {
				inputRange[ group ].setRange( range[ index ] );
				configChanged = true;
			}	// if( inputRange[ ...
		}	// for( int index ...
		if(
			configChanged
			&& autoConfig
		)
			writeConfig();
		return this;
	}	// setRange()

	/**
	 * Tells if <i>channel</i> is configured for single-ended or differential mode.
	 * @param channel the channel for which to obtain the current differential mode.
	 * @return Current range for <i>channel</i>.
	 * @return <i>False</i> indicates single-ended mode; <i>true</i> indicates differential mode.
	 * @see #setDifferentialMode( int channel, boolean differentialMode ) setDifferentialMode()
	 * @throws IllegalArgumentException
	 */

	public boolean isDifferentialMode( int channel ) {
		if(
			channel < 0
			|| channel >= numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		return differentialMode[ channel / channelsPerGroup ];
	}	// isDifferentialMode()

	/**
	 * Tells if multiple A/D channels are configured for single-ended or differential mode.
	 * @param startChannel the first channel for which to obtain the current differential mode.
	 * @param numChannels the number of channels for which to obtain the current differential mode.
	 * @return Array containing the current differential modes for the specified channels. <i>False</i> indicates
	 * channel is configured for single-ended mode and <i>true</i> indicates channel is configured for differential mode.
	 * @see #setDifferentialMode( int startChannel, boolean[] differentialMode ) setDifferentialMode()
	 * @throws IllegalArgumentException
	 */

	public boolean[] isDifferentialMode( int startChannel, int numChannels ) {
		if(
			numChannels < 1
			|| startChannel < 0
			|| startChannel + numChannels > numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid start channel: " + startChannel
				+ ", or number of channels: " + numChannels );
		boolean[] differentialMode = new boolean[ numChannels ];
		for( int index = 0; index < numChannels; index++ )
			differentialMode[ index ] = this.differentialMode[ ( startChannel + index ) / channelsPerGroup ];
		return differentialMode;
	}	// isDifferentialMode()

	/**
	 * Sets a single A/D channel to differential or single-ended mode.
	 * <br><br>When using differential mode, one should have a good understanding of how the hardware implements it.
	 * Considering the simple case of a device with only sixteen input channels, when differential mode is enabled
	 * for a channel, that channel is paired with another channel, eight higher than the one for which differential
	 * mode is enabled. For instance, if differential mode is enabled for channel 1, then it is paired with channel 9,
	 * meaning that channel 1 will return the voltage difference between channels 1 and 9, and channel 9 will no longer
	 * return a meaningful reading.
	 * <br><br>This scheme also means that enabling differential mode for channels 8-15 has no effect. In fact, if
	 * one attempts to enable differential mode for channels 8-15, nothing happens and if the differential mode setting
	 * is read back from the device for those channels, it will likely no longer be enabled! Further confusing matters is
	 * that some newer firmware does not clear the differential mode setting for channels 8-15, meaning that it will be
	 * returned from the device exactly as set even though it has no effect. So ... one should not rely on the
	 * differential mode setting for channels 8-15 to behave in a consistent or predictable manner.
	 * <br><br>For consistency and simplicity, one may read counts or volts from channels 8-15 even while differential
	 * mode is enabled, but the readings will not be meaningful. In differential mode, only the base channel (0-7) of the
	 * pair that's enabled for differential mode will return a meaningful reading. Channels 8-15 which are not enabled
	 * for differential mode will continue to return meaningful readings. For example, if differential mode is enabled
	 * for channel 1, then channel 1 will return a meaningful reading, channel 9 will not, and channels 8 and
	 * 10-15 will.
	 * <br><br>Considering the more complex case of a device such as the USB-AI16-64MA, which has an additional MUX
	 * affording 32 differential, or 64 single-ended inputs, things are a bit more complex. In this case, channels
	 * 0-3 share the same differential mode (and range) setting; channels 4-7 share the same setting; and so on. For the
	 * sake of simplicity and to support future designs which may have distinct settings for all channels, this software
	 * permits the differential mode (and range) to be specified for <i>any</i> MUXed channel, even though ultimately
	 * multiple channels may share the same setting. For example, on such a device as this, setting the differential
	 * mode (or range) of channel 1 also sets the differential mode (or range) of channels 0, 2 and 3.
	 * <br><br>There is yet another case to consider, that of devices such as the USB-AI16-128A. This device may have
	 * up to 128 channels, which share settings in groups of eight rather than four on the USB-AI16-64MA. Method
	 * <i>{@link #getChannelsPerGroup() getChannelsPerGroup()}</i> tells how many channels are grouped together on each device,
	 * and this topic is discussed more thoroughly in
	 * <a href="http://accesio.com/MANUALS/USB-AI%20FAMILY.PDF">http://accesio.com/MANUALS/USB-AI FAMILY.PDF</a>. The foregoing
	 * description also applies to the range setting, so one should refer to <i>{@link #setRange( int channel, int range ) setRange()}</i>
	 * as well.
	 * @param channel the channel for which to set differential or single-ended mode.
	 * @param differentialMode <i>false</i> selects single-ended mode; <i>true</i> selects differential mode.
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public AnalogInputSubsystem setDifferentialMode( int channel, boolean differentialMode ) {
		if(
			channel < 0
			|| channel >= numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		final int group = channel / channelsPerGroup;
		if( this.differentialMode[ group ] != differentialMode ) {
			this.differentialMode[ group ] = differentialMode;
			if( autoConfig )
				writeConfig();
		}	// if( this.differentialMode[ ...
		return this;
	}	// setDifferentialMode()

	/**
	 * Sets multiple A/D channels to differential or single-ended mode.
	 * @param startChannel the first channel for which to set differential or single-ended mode.
	 * @param differentialMode an array of mode selectors, one per channel. For each element in the array,
	 * <i>false</i> selects single-ended mode for that channel and <i>true</i> selects differential mode.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogInputSubsystem setDifferentialMode( int startChannel, boolean[] differentialMode ) {
		if(
			differentialMode == null
			|| differentialMode.length < 1
			|| startChannel < 0
			|| startChannel + differentialMode.length > numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid differential mode array or start channel: " + startChannel );
		boolean configChanged = false;
		for( int index = 0; index < differentialMode.length; index++ ) {
			final int group = ( startChannel + index ) / channelsPerGroup;
			if( this.differentialMode[ group ] != differentialMode[ index ] ) {
				this.differentialMode[ group ] = differentialMode[ index ];
				configChanged = true;
			}	// if( this.differentialMode[ ...
		}	// for( int index ...
		if(
			configChanged
			&& autoConfig
		)
			writeConfig();
		return this;
	}	// setDifferentialMode()

	/**
	 * Sets the range and differential mode for a single A/D channel.
	 * @param channel the channel for which to set the range.
	 * @param range the range (voltage range) for the channel <i>(see {@link #setRange( int channel, int range ) setRange()})</i>.
	 * @param differentialMode <i>false</i> selects single-ended mode; <i>true</i> selects differential mode.
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public AnalogInputSubsystem setRangeAndDiffMode( int channel, int range, boolean differentialMode ) {
		if(
			channel < 0
			|| channel >= numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		boolean configChanged = false;
		final int group = channel / channelsPerGroup;
		if( inputRange[ group ].getRange() != range ) {
			inputRange[ group ].setRange( range );
			configChanged = true;
		}	// if( inputRange[ ...
		if( this.differentialMode[ group ] != differentialMode ) {
			this.differentialMode[ group ] = differentialMode;
			configChanged = true;
		}	// if( this.differentialMode[ ...
		if(
			configChanged
			&& autoConfig
		)
			writeConfig();
		return this;
	}	// setRangeAndDiffMode()

	/**
	 * Sets the range and differential mode for multiple A/D channels.
	 * @param startChannel the first channel for which to set the range and differential mode.
	 * @param range an array of ranges, one per channel <i>(see {@link #setRange( int channel, int range ) setRange()})</i>.
	 * @param differentialMode an array of mode selectors, one per channel. For each element in the array,
	 * <i>false</i> selects single-ended mode for that channel and <i>true</i> selects differential mode.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogInputSubsystem setRangeAndDiffMode( int startChannel, int[] range, boolean[] differentialMode ) {
		if(
			range == null
			|| range.length < 1
			|| differentialMode == null
			|| differentialMode.length < 1
			|| startChannel < 0
			|| startChannel + range.length > numMUXChannels
			|| startChannel + differentialMode.length > numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid range array, differential mode array or start channel: " + startChannel );
		boolean configChanged = false;
		for( int index = 0; index < range.length; index++ ) {
			final int group = ( startChannel + index ) / channelsPerGroup;
			if( inputRange[ group ].getRange() != range[ index ] ) {
				inputRange[ group ].setRange( range[ index ] );
				configChanged = true;
			}	// if( inputRange[ ...
		}	// for( int index ...
		for( int index = 0; index < differentialMode.length; index++ ) {
			final int group = ( startChannel + index ) / channelsPerGroup;
			if( this.differentialMode[ group ] != differentialMode[ index ] ) {
				this.differentialMode[ group ] = differentialMode[ index ];
				configChanged = true;
			}	// if( this.differentialMode[ ...
		}	// for( int index ...
		if(
			configChanged
			&& autoConfig
		)
			writeConfig();
		return this;
	}	// setRangeAndDiffMode()

	/**
	 * Sets all the A/D channels to the same range and differential mode.
	 * @param range the range (voltage range) for the channels <i>(see {@link #setRange( int channel, int range ) setRange()})</i>.
	 * @param differentialMode <i>false</i> selects single-ended mode; <i>true</i> selects differential mode.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogInputSubsystem setRangeAndDiffMode( int range, boolean differentialMode ) {
		boolean configChanged = false;
		for( int group = 0; group < NUM_GAIN_CODE_REGISTERS; group++ ) {
			if( inputRange[ group ].getRange() != range ) {
				inputRange[ group ].setRange( range );
				configChanged = true;
			}	// if( inputRange[ ...
			if( this.differentialMode[ group ] != differentialMode ) {
				this.differentialMode[ group ] = differentialMode;
				configChanged = true;
			}	// if( this.differentialMode[ ...
		}	// for( int group ...
		if(
			configChanged
			&& autoConfig
		)
			writeConfig();
		return this;
	}	// setRangeAndDiffMode()

	/**
	 * Gets the current number of over-samples.
	 * @return Current number of over-samples (0-255).
	 * @see #setOverSample( int oversample ) setOverSample()
	 */

	public int getOverSample() {
		return overSample;
	}	// getOverSample()

	/**
	 * Sets the number of over-samples for all A/D channels.
	 * @param overSample number of over-samples (0-255).
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 */

	public AnalogInputSubsystem setOverSample( int overSample ) {
		if(
			overSample < 0
			|| overSample > MAX_OVERSAMPLE
		)
			throw new IllegalArgumentException( "Invalid over-sample: " + overSample );
		if( this.overSample != overSample ) {
			this.overSample = overSample;
			if( autoConfig )
				writeConfig();
		}	// if( this.overSample ...
		return this;
	}	// setOverSample()

	/**
	 * Loads a calibration table from a file into the A/D.
	 * @param fileName the name of a file containing the calibration table. A calibration table must consist
	 * of exactly 65,536 16-bit unsigned integers
	 * <i>(see {@link #calibrate( boolean autoCal, boolean returnCalTable, String saveFileName ) calibrate()})</i>.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public AnalogInputSubsystem setCalibrationTable( String fileName ) {
		if(
			fileName == null
			|| fileName.isEmpty()
		)
			throw new IllegalArgumentException( "Invalid file name" );
		final int result = loadCalTable( getDeviceIndex(), fileName );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setCalibrationTable()

	/**
	 * Sets the calibration table in the A/D to the contents of <i>calTable</i>.
	 * @param calTable the calibration table to load. A calibration table must consist
	 * of exactly 65,536 16-bit unsigned integers
	 * <i>(see {@link #calibrate( boolean autoCal, boolean returnCalTable, String saveFileName ) calibrate()})</i>.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public AnalogInputSubsystem setCalibrationTable( char[] calTable ) {
		if(
			calTable == null
			|| calTable.length != CAL_TABLE_WORDS
		)
			throw new IllegalArgumentException( "Invalid cal. table" );
		final int result = setCalTable( getDeviceIndex(), calTable );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return this;
	}	// setCalibrationTable()

	/**
	 * Gets the current streaming block size.
	 * @return The current streaming block size. The value returned may not be the same as the value passed to
	 * <i>{@link #setStreamingBlockSize( int blockSize ) setStreamingBlockSize()}</i> because that value is
	 * rounded up to a whole multiple of 512.
	 * @throws OperationFailedException
	 */

	public int getStreamingBlockSize() {
		return parent.getStreamingBlockSize();
	}	// getStreamingBlockSize()

	/**
	 * Sets the streaming block size.
	 * @param blockSize the streaming block size you wish to set. This will be rounded up to the
	 * next multiple of 512.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public AnalogInputSubsystem setStreamingBlockSize( int blockSize ) {
		parent.setStreamingBlockSize( blockSize );
		return this;
	}	// setStreamingBlockSize()

	/**
	 * Gets the current clock frequency for timer-driven bulk reads <i>(see
	 * {@link #setClock( double clockHz ) setClock()})</i>.
	 * @return The current frequency at which to take the samples (in Hertz).
	 */

	public double getClock() {
		return parent.getMiscClock();
	}	// getClock()

	/**
	 * Sets the clock frequency for timer-driven bulk reads <i>(see {@link #getClock() getClock()} and
	 * {@link #readBulkStart( int startChannel, int numChannels, int numSamples ) readBulkStart()})</i>.
	 * @param clockHz the frequency at which to take the samples (in Hertz).
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public AnalogInputSubsystem setClock( double clockHz ) {
		parent.setMiscClock( clockHz );
		return this;
	}	// setClock()

	/**
	 * Calibrates the A/D, generating either a default table or using the internal voltage references
	 * to generate a calibration table.
	 * @param autoCal <i>true</i> uses the internal voltage references to automatically calibrate the A/D; <i>false</i>
	 * generates a default (uncalibrated) table.
	 * @param returnCalTable <i>true</i> causes <i>calibrate()</i> to return the generated calibration table; <i>false</i>
	 * returns an empty table.
	 * @param saveFileName the name of the file in which to save the generated calibration table. If <i>null</i> or
	 * empty, the generated calibration table is not saved to a file.
	 * @return If <i>returnCalTable</i> is <i>true</i>, an array of 65,536 16-bit unsigned integers representing the generated
	 * calibration table is returned; otherwise, an empty table (containing zero elements) is returned.
	 * @throws OperationFailedException
	 */

	public char[] calibrate( boolean autoCal, boolean returnCalTable, String saveFileName ) {
		char[] calTable = new char[ returnCalTable ? CAL_TABLE_WORDS : 0 ];
		final int result = internalCal( getDeviceIndex(), autoCal, returnCalTable ? calTable : null, saveFileName );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return calTable;
	}	// calibrate()

	/**
	 * Permits the A/D to be calibrated using an external voltage source. The proper way to use this function is to configure
	 * the A/D with a default calibration table (such as by calling
	 * <i>{@link #calibrate( boolean autoCal, boolean returnCalTable, String saveFileName ) calibrate( false ... )}</i>). Then
	 * inject a series of voltages into one of the A/D input channels, recording the count values reported by the A/D (by calling
	 * <i>{@link #readCounts( int channel ) readCounts()}</i>). It's also a good idea to enable oversampling while recording these
	 * values in order to obtain the most stable readings. Alternatively, since <i>points</i> is an array of <i>double</i> values,
	 * you can obtain individual A/D count measurements and average them yourself, producing a <i>double</i> average, and put that
	 * value into the <i>points</i> array.<br><br>
	 * The <i>points</i> array consists of voltage-count pairs; <i>points[0]</i> is the first input voltage; <i>points[1]</i> is
	 * the corresponding count value measured by the A/D; <i>points[2]</i> and <i>points[3]</i> contain the second pair of
	 * voltage-count values; and so on. You can provide any number of pairs, although more than a few dozen is probably overkill,
	 * not to mention would take a lot of effort to acquire.<br><br>
	 * This calibration procedure uses the current gain A/D setting for channel 0, so it must be the same as that used to collect
	 * the measured A/D counts. It's recommended that all the channels be set to the same gain, the one that will be used during
	 * normal operation. The calibration is gain dependent, so switching the gain after calibrating may introduce slight offset or
	 * gain changes. So for best results, the A/D should be calibrated on the same gain setting that will be used during normal
	 * operation. You can create any number of calibration tables. If your application needs to switch between ranges, you may wish
	 * to create a separate calibration table for each range your application will use. Then when switching to a different range,
	 * the application can load the appropriate calibration table.<br><br>
	 * Although calibrating in this manner does take some effort, it produces the best results, eliminating all sources of error
	 * from the input pins onward. Furthermore, the calibration table can be saved to a file and reloaded into the A/D, ensuring consistency.
	 * @param points array of voltage-count pairs to calibrate the A/D with.
	 * @param returnCalTable <i>true</i> causes <i>calibrate()</i> to return the generated calibration table; <i>false</i>
	 * returns an empty table.
	 * @param saveFileName the name of the file in which to save the generated calibration table. If <i>null</i> or
	 * empty, the generated calibration table is not saved to a file.
	 * @return If <i>returnCalTable</i> is <i>true</i>, an array of 65,536 16-bit unsigned integers representing the generated
	 * calibration table is returned; otherwise, an empty table (containing zero elements) is returned.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public char[] calibrate( double[] points, boolean returnCalTable, String saveFileName ) {
		if(
			points == null
			|| points.length < 4				// at least 2 voltage-count pairs
			|| ( points.length & 1 ) != 0
		)
			throw new IllegalArgumentException( "Invalid points array" );
		char[] calTable = new char[ returnCalTable ? CAL_TABLE_WORDS : 0 ];
		final int result = externalCal( getDeviceIndex(), points, returnCalTable ? calTable : null, saveFileName );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );
		return calTable;
	}	// calibrate()

	/**
	 * Reads from multiple A/D channels and returns a data set containing both the data captured and the parameters
	 * in effect at the time the data was captured. Whereas {@link #readCounts( int startChannel, int numChannels ) readCounts()}
	 * and {@link #readVolts( int startChannel, int numChannels ) readVolts()} also read data from multiple channels,
	 * they return only the raw data. <i>read()</i> returns a richer snapshot of the data.
	 * @param startChannel the first channel to read.
	 * @param numChannels the number of channels to read.
	 * @return A data set containing the samples and the sampling parameters.
	 * @throws OperationFailedException
	 */

	public AI16_DataSet read( int startChannel, int numChannels ) {
		final long timeStamp = System.currentTimeMillis();	// record time when data capture starts
		char[] counts = readCounts( startChannel, numChannels );
		AI16_DataSet dataSet = new AI16_DataSet( this, numChannels, timeStamp
			, calMode, triggerMode, overSample, isDiscardFirstSample() );
		for( int index = 0; index < numChannels; index++ ) {
			final int channel = startChannel + index;
			dataSet.points[ index ] = new AI16_DataPoint( counts[ index ], channel, getRange( channel ), isDifferentialMode( channel ) );
		}	// for( int index ...
		return dataSet;
	}	// read()

	/**
	 * Reads the A/D count value from a single channel.
	 * @param channel the channel to read.
	 * @return A/D counts (0-65,535). The meaning of these counts depends on the current A/D range of the channel
	 * <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. The count value may be converted to
	 * a voltage value using <i>{@link #countsToVolts( int channel, char counts ) countsToVolts()}</i>.
	 */

	public char readCounts( int channel ) {
		return readCounts( channel, 1 )[ 0 ];
	}	// readCounts()

	/**
	 * Reads the A/D count values from multiple channels.
	 * @param startChannel the first channel to read.
	 * @param numChannels the number of channels to read.
	 * @return An array of A/D counts (0-65,535), one per channel read. The meaning of these counts depends on the
	 * current A/D range of each channel <i>(see {@link #setRange( int channel, int range ) setRange()})</i>.
	 * The array of count values may be converted to an array of voltage values using
	 * <i>{@link #countsToVolts( int startChannel, char[] counts ) countsToVolts()}</i>.
	 * @throws OperationFailedException
	 */

	public char[] readCounts( int startChannel, int numChannels ) {
		/*
		 * save current configuration and restore it when done; we don't need to restore
		 * the scan range because it is explicitly set during every read operation
		 */
		final boolean origDiscardFirstSample = isDiscardFirstSample();
		final int origOverSample = overSample;

		/*
		 * if calibration mode is enabled, then don't permit more than one
		 * sample to be taken; otherwise "bad" things will happen
		 */
		if(
			calMode == CAL_MODE_GROUND
			|| calMode == CAL_MODE_REFERENCE
		) {
			setDiscardFirstSample( false );
			setOverSample( 0 );
			numChannels = 1;
		}	// if( calMode ...
		setScanRange( startChannel, numChannels );
		char[] counts = new char[ numChannels ];
		final int result = getScan( getDeviceIndex(), counts );
		if( result != USBDeviceManager.SUCCESS )
			throw new OperationFailedException( result );

		/*
		 * restore original configuration
		 */
		setDiscardFirstSample( origDiscardFirstSample );
		setOverSample( origOverSample );

		return counts;
	}	// readCounts()

	/**
	 * Reads the voltage from a single channel.
	 * @param channel the channel to read.
	 * @return A voltage value, limited to the current A/D range of the channel <i>(see
	 * {@link #setRange( int channel, int range ) setRange()})</i>. The voltage value may be converted to a
	 * count value using <i>{@link #voltsToCounts( int channel, double volts ) voltsToCounts()}</i>.
	 */

	public double readVolts( int channel ) {
		return readVolts( channel, 1 )[ 0 ];
	}	// readVolts()

	/**
	 * Reads the voltage from multiple channels.
	 * @param startChannel the first channel to read.
	 * @param numChannels the number of channels to read.
	 * @return An array of voltage values, one per channel read, each limited to the current A/D range of each channel
	 * <i>(see {@link #setRange( int channel, int range ) setRange()})</i>. The array of voltage values may be
	 * converted to an array of count values using <i>{@link #voltsToCounts( int startChannel, double[] volts ) voltsToCounts()}</i>.
	 */

	public double[] readVolts( int startChannel, int numChannels ) {
		return countsToVolts( startChannel, readCounts( startChannel, numChannels ) );
	}	// readVolts()

	/**
	 * Starts a large A/D acquisition process in a background thread and returns immediately. The status of the
	 * acquisition process can be monitored using <i>{@link #readBulkSamplesAvailable readBulkSamplesAvailable()}</i>, which
	 * returns the number of samples available to be retrieved by <i>{@link #readBulkNext( int numSamples ) readBulkNext()}</i>.
	 * When the last of the data has been retrieved using <i>readBulkNext()</i>, the bulk acquisition process is
	 * automatically terminated and becomes ready to be used again.<br><br>
	 * <i>While a bulk acquisition process is in progress, no functions of the device other than readBulkSamplesAvailable()
	 * or readBulkNext() should be used.</i><br><br>
	 * This example shows the proper way to configure the device for a large A/D acquisition process using the internal timer.
	 * <pre>device.adc()
	 *   .setStreamingBlockSize( 100000 )
	 *   .setCalMode( AnalogInputSubsystem.CAL_MODE_NORMAL )
	 *   .setTriggerMode( AnalogInputSubsystem.TRIG_MODE_SCAN | AnalogInputSubsystem.TRIG_MODE_TIMER )
	 *   .setClock( 100000 )
	 *   .readBulkStart( 1, 1, numSamples );
	 * do {
	 *   char[] data = device.adc().readBulkNext( 20000 );
	 *   ... do something with data ...
	 * } while( ...more data is available... );</pre>
	 * @param startChannel the first channel to read.
	 * @param numChannels the number of channels to read.
	 * @param numSamples the total number of <i>samples</i> to read.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public AnalogInputSubsystem readBulkStart( int startChannel, int numChannels, int numSamples ) {
		if( numSamples < 1 )
			throw new IllegalArgumentException( "Invalid number of samples: " + numSamples );
		setScanRange( startChannel, numChannels );
		final int result = readBulkStart( getDeviceIndex(), numSamples );
		if( result == USBDeviceManager.SUCCESS ) {
			readBulkSamplesRequested = numSamples;
			readBulkSamplesRetrieved = 0;
		} else {
			readBulkSamplesRequested = readBulkSamplesRetrieved = 0;
			throw new OperationFailedException( result );
		}	// if( result ...
		return this;
	}	// readBulkStart()

	/**
	 * Gets the number of samples available to be retrieved during a bulk acquisition process initiated by
	 * <i>{@link #readBulkStart( int startChannel, int numChannels, int numSamples ) readBulkStart()}</i>.
	 * @return The number of samples available.
	 * @throws OperationFailedException
	 */

	public int readBulkSamplesAvailable() {
		int samplesAvailable = 0;
		if( readBulkSamplesRequested >= 0 ) {
			int[] samplesRemaining = new int[ 1 ];
			final int result = readBulkSamplesRemaining( getDeviceIndex(), samplesRemaining );
			if( result != USBDeviceManager.SUCCESS )
				throw new OperationFailedException( result );
			samplesAvailable = readBulkSamplesRequested - readBulkSamplesRetrieved - samplesRemaining[ 0 ];
			if( samplesAvailable < 0 )
				samplesAvailable = 0;
		}	// if( readBulkSamplesRequested ...
		return samplesAvailable;
	}	// readBulkSamplesAvailable()

	/**
	 * Retrieves the next set of samples acquired during a bulk acquisition process initiated by
	 * <i>{@link #readBulkStart( int startChannel, int numChannels, int numSamples ) readBulkStart()}</i>.
	 * @param numSamples the number of samples to retrieve.
	 * @return An array containing the number of samples requested, or all that are available. If fewer samples
	 * are available than are requested, only the samples available are returned. If zero samples are available,
	 * a zero-length array is returned.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public char[] readBulkNext( int numSamples ) {
		if( numSamples < 1 )
			throw new IllegalArgumentException( "Invalid number of samples: " + numSamples );
		char[] counts = new char[ 0 ];
		if( readBulkSamplesRequested >= 0 ) {
			final int samplesAvailable = readBulkSamplesAvailable();
			if( samplesAvailable > 0 ) {
				if( numSamples > samplesAvailable )
					numSamples = samplesAvailable;
				counts = new char[ numSamples ];
				int result = readBulkNext( getDeviceIndex(), readBulkSamplesRetrieved, counts );
				if( result == USBDeviceManager.SUCCESS ) {
					if( samplesAvailable <= numSamples ) {
						/*
						 * no more data remains to be retrieved; finish bulk read procedure
						 */
						result = readBulkFinish( getDeviceIndex() );
						readBulkSamplesRequested = readBulkSamplesRetrieved = 0;
						if( result != USBDeviceManager.SUCCESS )
							throw new OperationFailedException( result );
					} else
						readBulkSamplesRetrieved += numSamples;
				} else
					throw new OperationFailedException( result );
			}	// if( samplesAvailable ...
		}	// if( readBulkSamplesRequested ...
		return counts;
	}	// readBulkNext()

	/**
	 * Clears the streaming FIFO, using one of several different methods.
	 * @param method the method to use when clearing the FIFO. May be one of:<br>
	 * <i>{@link USBDevice#CLEAR_FIFO_METHOD_IMMEDIATE}<br>
	 * {@link USBDevice#CLEAR_FIFO_METHOD_AUTO}<br>
	 * {@link USBDevice#CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT}<br>
	 * {@link USBDevice#CLEAR_FIFO_METHOD_WAIT}</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	public AnalogInputSubsystem clearFIFO( int method ) {
		parent.clearFIFO( method );
		return this;
	}	// clearFIFO()

	/*
	 * utilities
	 */

	/**
	 * Converts a single A/D count value to volts, based on the current gain setting for the specified channel.
	 * Be careful to ensure that the count value was actually obtained from the specified channel and that the gain hasn't
	 * changed since the count value was obtained.
	 * @param channel the channel number to use for converting counts to volts.
	 * @param counts the count value to convert to volts.
	 * @return A voltage value calculated using the <i>current</i> A/D range of the channel <i>(see
	 * {@link #setRange( int channel, int range ) setRange()})</i>.
	 * @throws IllegalArgumentException
	 */

	public double countsToVolts( int channel, char counts ) {
		if(
			channel < 0
			|| channel >= numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		return inputRange[ channel / channelsPerGroup ].countsToVolts( counts );
	}	// countsToVolts()

	/**
	 * Converts an array of A/D count values to an array of voltage values, based on the current gain setting for each of
	 * the specified channels. This method is intended to convert an array of readings from sequential channels, such as
	 * might have been obtained from <i>{@link #readCounts( int startChannel, int numChannels ) readCounts()}</i>. Be careful to ensure
	 * that the count values were actually obtained from the specified channels and that the gains havn't changed since the
	 * count values were obtained.
	 * @param startChannel the first channel number to use for converting counts to volts.
	 * @param counts the count values to convert to volts.
	 * @return An array of voltage values calculated using the <i>current</i> A/D range of each of the channels <i>(see
	 * {@link #setRange( int channel, int range ) setRange()})</i>. The array returned has the same number of
	 * elements as <i>counts</i>.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public double[] countsToVolts( int startChannel, char[] counts ) {
		if(
			counts == null
			|| counts.length < 1
			|| startChannel < 0
			|| startChannel + counts.length > numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid counts array or start channel: " + startChannel );
		double[] volts = new double[ counts.length ];
		for( int index = 0; index < counts.length; index++ )
			volts[ index ] = inputRange[ ( startChannel + index ) / channelsPerGroup ].countsToVolts( counts[ index ] );
		return volts;
	}	// countsToVolts()

	/**
	 * Converts a single voltage value to A/D counts, based on the current gain setting for the specified channel.
	 * Be careful to ensure that the voltage value was actually obtained from the specified channel and that the gain hasn't
	 * changed since the voltage value was obtained.
	 * @param channel the channel number to use for converting volts to counts.
	 * @param volts the voltage value to convert to counts.
	 * @return A count value calculated using the <i>current</i> A/D range of the channel <i>(see
	 * {@link #setRange( int channel, int range ) setRange()})</i>.
	 * @throws IllegalArgumentException
	 */

	public char voltsToCounts( int channel, double volts ) {
		if(
			channel < 0
			|| channel >= numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid channel: " + channel );
		return ( char ) inputRange[ channel / channelsPerGroup ].voltsToCounts( volts );
	}	// voltsToCounts()

	/**
	 * Converts an array of voltage values to an array of A/D count values, based on the current gain setting for each of
	 * the specified channels. This method is intended to convert an array of readings from sequential channels, such as
	 * might have been obtained from <i>{@link #readVolts( int startChannel, int numChannels ) readVolts()}</i>. Be careful
	 * to ensure that the voltage values were actually obtained from the specified channels and that the gains havn't
	 * changed since the voltage values were obtained.
	 * @param startChannel the first channel number to use for converting volts to counts.
	 * @param volts the voltage values to convert to counts.
	 * @return An array of count values calculated using the <i>current</i> A/D range of each of the channels <i>(see
	 * {@link #setRange( int channel, int range ) setRange()})</i>. The array returned has the same number of
	 * elements as <i>volts</i>.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	public char[] voltsToCounts( int startChannel, double[] volts ) {
		if(
			volts == null
			|| volts.length < 1
			|| startChannel < 0
			|| startChannel + volts.length > numMUXChannels
		)
			throw new IllegalArgumentException( "Invalid volts array or start channel: " + startChannel );
		char[] counts = new char[ volts.length ];
		for( int index = 0; index < volts.length; index++ )
			counts[ index ] = ( char ) inputRange[ ( startChannel + index ) / channelsPerGroup ].voltsToCounts( volts[ index ] );
		return counts;
	}	// voltsToCounts()

	// }}}

}	// class AnalogInputSubsystem

/* end of file */
