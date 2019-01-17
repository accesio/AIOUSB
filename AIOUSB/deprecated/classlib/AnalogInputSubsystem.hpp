/*
 * $RCSfile: AnalogInputSubsystem.hpp,v $
 * $Revision: 1.26 $
 * $Date: 2010/01/19 17:14:47 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AnalogInputSubsystem declarations
 */

#if ! defined( AnalogInputSubsystem_hpp )
#define AnalogInputSubsystem_hpp


#include <AI16_InputRange.hpp>
#include <AI16_DataSet.hpp>
#include <DeviceSubsystem.hpp>


namespace AIOUSB {

class AI16_DataSet;

/**
 * Class AnalogInputSubsystem represents the analog input subsystem of a device. One accesses
 * this analog input subsystem through its parent object, typically through a method such as
 * <i>adc() (see USB_AI16_Family::adc())</i>.
 */

class AnalogInputSubsystem : public DeviceSubsystem {
	friend class USB_AI16_Family;


public:
	/*
	 * A/D calibration modes; if ground or reference mode is selected, only one A/D
	 * sample may be taken at a time; that means, one channel and no oversampling;
	 * attempting to read more than one channel or use an oversample setting of more
	 * than zero will result in a timeout error
	 */
	/** Selects normal measurement mode <i>(see setCalMode( int calMode ))</i>. */
	static const int CAL_MODE_NORMAL				= 0;		// normal measurement
	/** Selects ground calibration mode <i>(see setCalMode( int calMode ))</i>. */
	static const int CAL_MODE_GROUND				= 1;		// measure ground
	/** Selects reference (full scale) calibration mode <i>(see setCalMode( int calMode ))</i>. */
	static const int CAL_MODE_REFERENCE				= 3;		// measure reference

	/*
	 * A/D trigger and counter bits
	 */
	/** If set, counter 0 is externally triggered <i>(see setTriggerMode( int triggerMode ))</i>. */
	static const int TRIG_MODE_CTR0_EXT				= 0x10;		// 1 == counter 0 externally triggered
	/** If set, the A/D is triggered by the falling edge of its trigger source, otherwise it's triggered by the rising edge <i>(see setTriggerMode( int triggerMode ))</i>. */
	static const int TRIG_MODE_FALLING_EDGE			= 0x08;		// 1 == triggered by falling edge
	/** If set, each trigger will cause the A/D to scan all the channels, otherwise the A/D will read a single channel with each trigger <i>(see setTriggerMode( int triggerMode ))</i>. */
	static const int TRIG_MODE_SCAN					= 0x04;		// 1 == scan all channels, 0==single channel
	/** If set, the A/D is triggered by an external pin on the board <i>(see setTriggerMode( int triggerMode ))</i>. */
	static const int TRIG_MODE_EXTERNAL				= 0x02;		// 1 == triggered by external pin on board
	/** If set, the A/D is triggered by counter 2 <i>(see setTriggerMode( int triggerMode ))</i>. */
	static const int TRIG_MODE_TIMER				= 0x01;		// 1 == triggered by counter 2

	/*
	 * range codes passed to setRange(); these values are defined by the device and must not be changed
	 */
	/** Unipolar, 0-10 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_0_10V					= 0;		// 0-10V
	/** Bipolar, -10 to +10 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_10V						= 1;		// +/-10V
	/** Unipolar, 0-5 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_0_5V						= 2;		// 0-5V
	/** Bipolar, -5 to +5 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_5V						= 3;		// +/-5V
	/** Unipolar, 0-2 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_0_2V						= 4;		// 0-2V
	/** Bipolar, -2 to +2 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_2V						= 5;		// +/-2V
	/** Unipolar, 0-1 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_0_1V						= 6;		// 0-1V
	/** Bipolar, -1 to +1 volt range <i>(see setRange( int channel, int range ))</i>. */
	static const int RANGE_1V						= 7;		// +/-1V

	/** Minimum number of counts A/D can read. */
	static const int MIN_COUNTS 					= 0;
	/** Maximum number of counts A/D can read. */
	static const int MAX_COUNTS						= 0xffff;

	/** Number of 16-bit words in an A/D calibration table (65,536 16-bit words). */
	static const int CAL_TABLE_WORDS				= 64 * 1024;



protected:
	static const char RANGE_TEXT[][ 10 ];



protected:
	/*
	 * register indexes
	 */
	static const int NUM_CONFIG_REGISTERS			= 20;		// number of "registers" (bytes) in config. block
	static const int NUM_MUX_CONFIG_REGISTERS		= 21;		// number of "registers" (bytes) in config. block when MUX installed
	static const int NUM_GAIN_CODE_REGISTERS		= 16;		// number of gain code (range) registers in config. block
	static const int REG_GAIN_CODE					= 0;		// gain code (range) for A/D channel 0 (one of RANGE_* settings above)
																// gain codes for channels 1-15 occupy configuration block indexes 1-15
	static const int REG_CAL_MODE					= 16;		// calibration mode (one of CAL_MODE_* settings below)
	static const int REG_TRIG_MODE					= 17;		// trigger mode (one of TRIG_MODE_* settings below)
	static const int REG_START_END					= 18;		// start and end channels for scan (bits 7-4 contain end channel, bits 3-0 contain start channel)
	static const int REG_OVERSAMPLE					= 19;		// oversample setting (0-255 samples in addition to single sample)
	static const int REG_MUX_START_END				= 20;		// MUX start and end channels for scan (bits 7-4 contain end channel MS-nibble, bits 3-0 contain start channel MS-nibble)

	static const int DIFFERENTIAL_MODE				= 8;		// OR with range to enable differential mode for channels 0-7
	static const int MAX_OVERSAMPLE					= 0xff;
	static const int TRIG_MODE_VALID_MASK			= (
		TRIG_MODE_CTR0_EXT
		| TRIG_MODE_FALLING_EDGE
		| TRIG_MODE_SCAN
		| TRIG_MODE_EXTERNAL
		| TRIG_MODE_TIMER );

	static const int AUTO_CAL_UNKNOWN				= 0;
	static const int AUTO_CAL_NOT_PRESENT			= 1;
	static const int AUTO_CAL_PRESENT				= 2;

	static const int MAX_CHANNELS					= 128;		// maximum number of channels supported by this software

	int numChannels;											// number of A/D channels
	int numMUXChannels;											// number of MUXed A/D channels
	int channelsPerGroup;										// number of A/D channels in each config. group (1, 4 or 8 depending on model)
	int configBlockSize;										// size of configuration block (bytes)
	int autoCalFeature;											// cached A/D auto. cal. feature
	AI16_InputRange *inputRange;								// range for each channel group
	bool *differentialMode;										// differential mode for each channel group
	int calMode;												// calibration mode (CAL_MODE_*)
	int triggerMode;											// trigger mode (TRIG_MODE_*)
	int startChannel;											// starting channel for scans
	int endChannel;												// ending channel for scans
	int overSample;												// over-samples
	unsigned short *readBulkBuffer;								// buffer used during bulk read procedure
	int readBulkSamplesRequested;								// total number of samples requested during bulk read procedure
	int readBulkSamplesRetrieved;								// number of samples retrieved so far during  bulk read procedure
	bool autoConfig;											// true == automatically send modified configuration to device



public:
	AnalogInputSubsystem &setScanRange( int startChannel, int numChannels );
	AnalogInputSubsystem( USBDeviceBase &parent );
	virtual ~AnalogInputSubsystem();



public:

	/*
	 * properties
	 */

	virtual std::ostream &print( std::ostream &out );

	/**
	 * Gets the number of primary analog input channels.
	 * @return Number of channels, numbered 0 to n-1.
	 */

	int getNumChannels() const {
		return numChannels;
	}	// getNumChannels()

	/**
	 * Gets the number of analog input channels available through an optional multiplexer.
	 * @return Number of channels, numbered 0 to n-1.
	 */

	int getNumMUXChannels() const {
		return numMUXChannels;
	}	// getNumMUXChannels()

	/**
	 * Gets the number of analog input channels in each configuration group (1, 4 or 8 depending on the device model).
	 * @return The number of channels per group.
	 */

	int getChannelsPerGroup() {
		return channelsPerGroup;
	}	// getChannelsPerGroup()

	bool isAutoCalPresent( bool force );
	static std::string getRangeText( int range );

	/*
	 * configuration
	 */

	/**
	 * Tells whether the modified configuration will be automatically sent to the device.
	 * @return <i>True</i> indicates that the modified configuration will be automatically sent to the device,
	 * <i>false</i> indicates that you will have to explicitly call <i>writeConfig()</i>
	 * to send the configuration to the device.
	 * @see setAutoConfig( bool autoConfig )
	 */

	bool isAutoConfig() const {
		return autoConfig;
	}	// isAutoConfig()

	/**
	 * Enables or disables automatically sending the modified configuration to the device. Normally, it's desirable
	 * to send the modified configuration to the device immediately. However, in order to reduce the amount of
	 * communication with the device while setting multiple properties, this automatic sending mechanism can be
	 * disabled and the configuration can be sent by explicitly calling <i>writeConfig()</i>
	 * once all the properties have been set, like so:
	 * <pre>device.adc()
	 *   .setAutoConfig( false )
	 *   .setCalMode( AnalogInputSubsystem::CAL_MODE_NORMAL )
	 *   .setTriggerMode( AnalogInputSubsystem::TRIG_MODE_SCAN | AnalogInputSubsystem::TRIG_MODE_TIMER )
	 *   .setOverSample( 50 )
	 *   .writeConfig()
	 *   .setAutoConfig( true );</pre>
	 * <i>Remember to call setAutoConfig( true ) after configuring the properties, otherwise all subsequent
	 * configuration changes will have to be explicitly sent to the device by calling writeConfig().</i>
	 * @param autoConfig <i>True</i> enables automatically sending modified configuration, <i>false</i> disables it.
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	AnalogInputSubsystem &setAutoConfig( bool autoConfig ) {
		this->autoConfig = autoConfig;
		return *this;
	}	// isAutoConfig()

	AnalogInputSubsystem &readConfig();
	AnalogInputSubsystem &writeConfig();
	bool isDiscardFirstSample() const;
	AnalogInputSubsystem &setDiscardFirstSample( bool discard );

	/**
	 * Gets the current calibration mode.
	 * @return Current calibration mode (<i>AnalogInputSubsystem::CAL_MODE_NORMAL</i>,
	 * <i>AnalogInputSubsystem::CAL_MODE_GROUND</i> or <i>AnalogInputSubsystem::CAL_MODE_REFERENCE</i>).
	 * @see setCalMode( int calMode )
	 */

	int getCalMode() const {
		return calMode;
	}	// getCalMode()

	AnalogInputSubsystem &setCalMode( int calMode );

	/**
	 * Gets the current trigger mode.
	 * @return Current trigger mode (bitwise OR of <i>TRIG_MODE_CTR0_EXT</i>, <i>TRIG_MODE_FALLING_EDGE</i>,
	 * <i>TRIG_MODE_SCAN</i>, <i>TRIG_MODE_EXTERNAL</i> or <i>TRIG_MODE_TIMER</i>).
	 * @see setTriggerMode( int triggerMode )
	 */

	int getTriggerMode() const {
		return triggerMode;
	}	// getTriggerMode()

	AnalogInputSubsystem &setTriggerMode( int triggerMode );
	int getRange( int channel ) const;
	IntArray getRange( int startChannel, int numChannels ) const;
	AnalogInputSubsystem &setRange( int channel, int range );
	AnalogInputSubsystem &setRange( int startChannel, const IntArray &range );
	bool isDifferentialMode( int channel ) const;
	BoolArray isDifferentialMode( int startChannel, int numChannels ) const;
	AnalogInputSubsystem &setDifferentialMode( int channel, bool differentialMode );
	AnalogInputSubsystem &setDifferentialMode( int startChannel, const BoolArray &differentialMode );
	AnalogInputSubsystem &setRangeAndDiffMode( int channel, int range, bool differentialMode );
	AnalogInputSubsystem &setRangeAndDiffMode( int startChannel, const IntArray &range, const BoolArray &differentialMode );
	AnalogInputSubsystem &setRangeAndDiffMode( int range, bool differentialMode );

	/**
	 * Gets the current number of over-samples.
	 * @return Current number of over-samples (0-255).
	 * @see setOverSample( int oversample )
	 */

	int getOverSample() const {
		return overSample;
	}	// getOverSample()

	AnalogInputSubsystem &setOverSample( int overSample );
	AnalogInputSubsystem &setCalibrationTable( const std::string &fileName );
	AnalogInputSubsystem &setCalibrationTable( const UShortArray &calTable );

	/**
	 * Gets the current streaming block size.
	 * @return The current streaming block size. The value returned may not be the same as the value passed to
	 * <i>setStreamingBlockSize( int blockSize )</i> because that value is rounded up to a whole multiple of 512.
	 * @throws OperationFailedException
	 */

	int getStreamingBlockSize() {
		return parent->getStreamingBlockSize();
	}	// getStreamingBlockSize()

	/**
	 * Sets the streaming block size.
	 * @param blockSize the streaming block size you wish to set. This will be rounded up to the
	 * next multiple of 512.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	AnalogInputSubsystem &setStreamingBlockSize( int blockSize ) {
		parent->setStreamingBlockSize( blockSize );
		return *this;
	}	// setStreamingBlockSize()

	/**
	 * Gets the current clock frequency for timer-driven bulk reads <i>(see setClock( double clockHz ))</i>.
	 * @return The current frequency at which to take the samples (in Hertz).
	 */

	double getClock() {
		return parent->getMiscClock();
	}	// getClock()

	/**
	 * Sets the clock frequency for timer-driven bulk reads <i>(see getClock() and
	 * readBulkStart( int startChannel, int numChannels, int numSamples ))</i>.
	 * @param clockHz the frequency at which to take the samples (in Hertz).
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	AnalogInputSubsystem &setClock( double clockHz ) {
		parent->setMiscClock( clockHz );
		return *this;
	}	// setClock()

	/*
	 * operations
	 */

	UShortArray calibrate( bool autoCal, bool returnCalTable, const std::string &saveFileName );
	UShortArray calibrate( const DoubleArray &points, bool returnCalTable, const std::string &saveFileName );
	AI16_DataSet *read( int startChannel, int numChannels );
	unsigned short readCounts( int channel );
	UShortArray readCounts( int startChannel, int numChannels );
	double readVolts( int channel );
	DoubleArray readVolts( int startChannel, int numChannels );
	AnalogInputSubsystem &readBulkStart( int startChannel, int numChannels, int numSamples );
	int readBulkSamplesAvailable();
	UShortArray readBulkNext( int numSamples );

	/**
	 * Clears the streaming FIFO, using one of several different methods.
	 * @param method the method to use when clearing the FIFO. May be one of:
	 * <i>USBDeviceBase::CLEAR_FIFO_METHOD_IMMEDIATE
	 * USBDeviceBase::CLEAR_FIFO_METHOD_AUTO
	 * USBDeviceBase::CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT
	 * USBDeviceBase::CLEAR_FIFO_METHOD_WAIT</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	AnalogInputSubsystem &clearFIFO( FIFO_Method method ) {
		parent->clearFIFO( method );
		return *this;
	}	// clearFIFO()

	/*
	 * utilities
	 */

	double countsToVolts( int channel, unsigned short counts ) const;
	DoubleArray countsToVolts( int startChannel, const UShortArray &counts ) const;
	unsigned short voltsToCounts( int channel, double volts ) const;
	UShortArray voltsToCounts( int startChannel, const DoubleArray &volts ) const;



};	// class AnalogInputSubsystem

}	// namespace AIOUSB

#endif

/* end of file */
