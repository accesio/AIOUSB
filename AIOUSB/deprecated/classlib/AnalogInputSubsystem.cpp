/**
 * @file   AnalogInputSubsystem.cpp
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @brief class AnalogInputSubsystem implementation
 */

#include "CppCommon.h"
#include <assert.h>
#include <string.h>
#include <AIOUSB_Core.h>
#include "AIOUSB_ADC.h"
#include "AIODeviceTable.h"
#include "AIOUSBDevice.h"
#include "aiousb.h"
#include "USBDeviceManager.hpp"
#include "AnalogInputSubsystem.hpp"


using namespace std;

namespace AIOUSB {


const int AnalogInputSubsystem::CAL_MODE_NORMAL;
const int AnalogInputSubsystem::CAL_MODE_GROUND;
const int AnalogInputSubsystem::CAL_MODE_REFERENCE;
const int AnalogInputSubsystem::TRIG_MODE_CTR0_EXT;
const int AnalogInputSubsystem::TRIG_MODE_FALLING_EDGE;
const int AnalogInputSubsystem::TRIG_MODE_SCAN;
const int AnalogInputSubsystem::TRIG_MODE_EXTERNAL;
const int AnalogInputSubsystem::TRIG_MODE_TIMER;
const int AnalogInputSubsystem::RANGE_0_10V;
const int AnalogInputSubsystem::RANGE_10V;
const int AnalogInputSubsystem::RANGE_0_5V;
const int AnalogInputSubsystem::RANGE_5V;
const int AnalogInputSubsystem::RANGE_0_2V;
const int AnalogInputSubsystem::RANGE_2V;
const int AnalogInputSubsystem::RANGE_0_1V;
const int AnalogInputSubsystem::RANGE_1V;
const int AnalogInputSubsystem::MIN_COUNTS;
const int AnalogInputSubsystem::MAX_COUNTS;
const int AnalogInputSubsystem::CAL_TABLE_WORDS;
const int AnalogInputSubsystem::NUM_CONFIG_REGISTERS;
const int AnalogInputSubsystem::NUM_MUX_CONFIG_REGISTERS;
const int AnalogInputSubsystem::NUM_GAIN_CODE_REGISTERS;
const int AnalogInputSubsystem::REG_GAIN_CODE;
const int AnalogInputSubsystem::REG_CAL_MODE;
const int AnalogInputSubsystem::REG_TRIG_MODE;
const int AnalogInputSubsystem::REG_START_END;
const int AnalogInputSubsystem::REG_OVERSAMPLE;
const int AnalogInputSubsystem::REG_MUX_START_END;
const int AnalogInputSubsystem::DIFFERENTIAL_MODE;
const int AnalogInputSubsystem::MAX_OVERSAMPLE;
const int AnalogInputSubsystem::TRIG_MODE_VALID_MASK;
const int AnalogInputSubsystem::AUTO_CAL_UNKNOWN;
const int AnalogInputSubsystem::AUTO_CAL_NOT_PRESENT;
const int AnalogInputSubsystem::AUTO_CAL_PRESENT;
const int AnalogInputSubsystem::MAX_CHANNELS;
const char AnalogInputSubsystem::RANGE_TEXT[][ 10 ] = {
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
};	// AnalogInputSubsystem::RANGE_TEXT[][]




AnalogInputSubsystem &AnalogInputSubsystem::setScanRange( int startChannel, int numChannels ) {
	const int endChannel = startChannel + numChannels - 1;
	if(
		numChannels < 1
		|| startChannel < 0
		|| ( configBlockSize == NUM_MUX_CONFIG_REGISTERS && endChannel >= numMUXChannels )
		|| ( configBlockSize != NUM_MUX_CONFIG_REGISTERS && endChannel >= this->numChannels )
	)
		throw IllegalArgumentException( "Invalid start channel or number of channels" );
	bool configChanged = false;
	if( this->startChannel != startChannel ) {
		this->startChannel = startChannel;
		configChanged = true;
	}	// if( this->startChannel ...
	if( this->endChannel != endChannel ) {
		this->endChannel = endChannel;
		configChanged = true;
	}	// if( this->endChannel ...
	if(
		configChanged
		&& autoConfig
	)
		writeConfig();
	return *this;
}	// AnalogInputSubsystem::setScanRange()

AnalogInputSubsystem::AnalogInputSubsystem( USBDeviceBase &parent )
		: DeviceSubsystem( parent ) {
	numChannels = numMUXChannels = channelsPerGroup = configBlockSize
		= calMode = triggerMode = startChannel = endChannel
		= overSample = readBulkSamplesRequested = readBulkSamplesRetrieved = 0;
	autoConfig = true;
	autoCalFeature = AUTO_CAL_UNKNOWN;
	inputRange = 0;
	differentialMode = 0;
	readBulkBuffer = 0;
	DeviceProperties properties;
	int result = AIOUSB_GetDeviceProperties( getDeviceIndex(), &properties );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	numMUXChannels = properties.ADCMUXChannels;
	assert( numMUXChannels >= 1
		&& numMUXChannels <= MAX_CHANNELS );
	numChannels = properties.ADCChannels;
	assert( numChannels >= 1
		&& numChannels <= numMUXChannels );
	channelsPerGroup = properties.ADCChannelsPerGroup;
	assert( channelsPerGroup == 1
		|| channelsPerGroup == 4
		|| channelsPerGroup == 8 );
	ADCConfigBlock temp;
	AIOUSB_InitConfigBlock( &temp, getDeviceIndex(), AIOUSB_FALSE );
	configBlockSize = temp.size;
	assert( configBlockSize == NUM_CONFIG_REGISTERS
		|| configBlockSize == NUM_MUX_CONFIG_REGISTERS );
	inputRange = new AI16_InputRange[ NUM_GAIN_CODE_REGISTERS ];
	assert( inputRange != 0 );
	for( int group = 0; group < NUM_GAIN_CODE_REGISTERS; group++ )
		inputRange[ group ].setCountRange( MIN_COUNTS, MAX_COUNTS );
	differentialMode = new bool[ NUM_GAIN_CODE_REGISTERS ];
	assert( differentialMode != 0 );
	readConfig();								// initializes remaining class members
}	// AnalogInputSubsystem::AnalogInputSubsystem()

AnalogInputSubsystem::~AnalogInputSubsystem() {
	if( inputRange != 0 )
		delete[] inputRange;
	if( differentialMode != 0 )
		delete[] differentialMode;
	if( readBulkBuffer != 0 )
		delete[] readBulkBuffer;				// just in case ...
}	// AnalogInputSubsystem::~AnalogInputSubsystem()





/*
 * properties
 */

/**
 * Prints the properties of this subsystem. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &AnalogInputSubsystem::print( ostream &out ) {
	out
		<< "    Number of A/D channels: " << dec << numChannels << endl
		<< "    Number of MUXed A/D channels: " << numMUXChannels << endl;
	return out;
}	// AnalogInputSubsystem::print()

/**
 * Tells if automatic calibration is possible with this device.
 * @param force <i>True</i> forces this class to interrogate the device anew; <i>false</i> returns the previous
 * result if available, or interrogates the device if a previous result is not available.
 * @return <i>True</i> indicates that automatic calibration is available.
 * @see calibrate( bool autoCal, bool returnCalTable, const std::string &saveFileName )
 * @throws OperationFailedException
 */

bool AnalogInputSubsystem::isAutoCalPresent( bool force ) {
	if(
		force
		|| autoCalFeature == AUTO_CAL_UNKNOWN
	) {
		const int result = ADC_QueryCal( getDeviceIndex() );
		if( result == AIOUSB_SUCCESS )
			autoCalFeature = AUTO_CAL_PRESENT;
		else if( result == AIOUSB_ERROR_NOT_SUPPORTED )
			autoCalFeature = AUTO_CAL_NOT_PRESENT;
		else {
			/*
			 * an error occurred; leave auto. cal. feature unchanged
			 */
			throw OperationFailedException( result );
		}	// else if( result ...
	}	// if( force ...
	return autoCalFeature == AUTO_CAL_PRESENT;
}	// AnalogInputSubsystem::isAutoCalPresent()

/**
 * Gets the textual string for the specified range.
 * @param range the range for which to obtain the textual string.
 * @return The textual string for the specified range.
 * @see setRange( int channel, int range )
 * @throws IllegalArgumentException
 */

std::string AnalogInputSubsystem::getRangeText( int range ) {
	if(
		range < RANGE_0_10V
		|| range > RANGE_1V
	)
		throw IllegalArgumentException( "Invalid range" );
	return RANGE_TEXT[ range ];
}	// AnalogInputSubsystem::getRangeText()

/*
 * configuration
 */

/**
 * Reads the A/D configuration from the device. This is done automatically when the class is
 * instantiated, so it generally does not need to be done again. However, if the A/D configuration
 * in the device has been changed without using this class (e.g. another program changed it),
 * <i>readConfig()</i> can be used to copy the device's configuration into this class.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

AnalogInputSubsystem &AnalogInputSubsystem::readConfig() {
	unsigned char configBlock[ configBlockSize ];
	unsigned long size = configBlockSize;
	const int result = ADC_GetConfig( getDeviceIndex(), configBlock, &size );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	const int RANGE_VALID_MASK = 7;
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
			| ( ( configBlock[ REG_START_END ] & 0xff ) >> 4 );
	} else {
		startChannel = configBlock[ REG_START_END ] & 0xf;
		endChannel = ( configBlock[ REG_START_END ] & 0xff ) >> 4;
	}	// if( configBlockSize ...
	overSample = configBlock[ REG_OVERSAMPLE ] & 0xff;
	return *this;
}	// AnalogInputSubsystem::readConfig()

/**
 * Writes the A/D configuration to the device. This is done automatically whenever the pertinent
 * settings within this class are changed. However, if the A/D configuration in the device has been
 * changed without using this class (e.g. another program changed it), or if automatic sending of
 * the configuration has been disabled <i>(see setAutoConfig( bool autoConfig ))</i>,
 * then <i>writeConfig()</i> can be used to copy this class' configuration settings into the device. 
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

AnalogInputSubsystem &AnalogInputSubsystem::writeConfig() {
	unsigned char configBlock[ configBlockSize ];
	for( int group = 0; group < NUM_GAIN_CODE_REGISTERS; group++ ) {
		int range = inputRange[ group ].getRange();
		if( differentialMode[ group ] )
			range |= DIFFERENTIAL_MODE;
		configBlock[ REG_GAIN_CODE + group ] = ( unsigned char ) range;
	}	// for( int group ...
	configBlock[ REG_CAL_MODE ] = ( unsigned char ) calMode;
	configBlock[ REG_TRIG_MODE ] = ( unsigned char ) triggerMode;
	configBlock[ REG_START_END ] = ( unsigned char ) ( ( endChannel << 4 ) | ( startChannel & 0x0f ) );
	if( configBlockSize == NUM_MUX_CONFIG_REGISTERS )
		configBlock[ REG_MUX_START_END ] = ( unsigned char ) ( ( endChannel & 0xf0 ) | ( ( startChannel >> 4 ) & 0x0f ) );
	configBlock[ REG_OVERSAMPLE ] = ( unsigned char ) overSample;
	unsigned long size = configBlockSize;
	const int result = ADC_SetConfig( getDeviceIndex(), configBlock, &size );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// AnalogInputSubsystem::writeConfig()

/**
 * Tells if the <i>read()</i>, <i>readCounts()</i> and <i>readVolts()</i> functions will discard the first A/D sample taken.
 * @return <i>False</i> indicates that no samples will be discarded; <i>true</i> indicates that the first sample will be discarded.
 */

bool AnalogInputSubsystem::isDiscardFirstSample() const {
	// return AIOUSB_IsDiscardFirstSample( getDeviceIndex() );
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *dev = AIODeviceTableGetDeviceAtIndex( getDeviceIndex() , &result );
    if ( result != AIOUSB_SUCCESS )
        throw OperationFailedException( result );
    return ( AIOUSBDeviceGetDiscardFirstSample( dev ) > 0 ? true : false );
}	// AnalogInputSubsystem::isDiscardFirstSample()


/**
 * Specifies whether the <i>read()</i>, <i>readCounts()</i> and <i>readVolts()</i> functions will discard the first A/D sample
 * taken. This setting does <b>not</b> pertain to the <i>readBulkNext()</i> function which returns all of the raw data captured.
 * Discarding the first sample may be useful in cases in which voltage "residue" from reading a different channel affects the
 * channel currently being read.
 * @param discard <i>false</i> indicates that no samples will be discarded; <i>true</i> indicates that the first sample will be discarded.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */
 // const int result = AIOUSB_SetDiscardFirstSample( getDeviceIndex(), discard );
AnalogInputSubsystem &AnalogInputSubsystem::setDiscardFirstSample( bool discard ) 
{
    AIORESULT result = AIOUSB_SUCCESS;
    AIOUSBDevice *dev = AIODeviceTableGetDeviceAtIndex( getDeviceIndex() , &result );
    if( result != AIOUSB_SUCCESS )
        throw OperationFailedException( result );
    
    result = AIOUSBDeviceSetDiscardFirstSample( dev , discard );
    if ( result != AIOUSB_SUCCESS )
        throw OperationFailedException( result );

    return *this;
}	// AnalogInputSubsystem::setDiscardFirstSample()

/**
 * Sets the A/D calibration mode. If ground or reference mode is selected, only one A/D sample may be taken at a time.
 * That means, one channel and no oversampling. Attempting to read more than one channel or use an oversample setting
 * of more than zero will result in a timeout error because the device will not send more than one sample. In order to
 * protect users from accidentally falling into this trap, the <i>read*()</i> functions automatically and temporarily
 * correct the scan parameters, restoring them when they complete.
 * @param calMode the calibration mode. May be one of:
 * <i>AnalogInputSubsystem::CAL_MODE_NORMAL
 * AnalogInputSubsystem::CAL_MODE_GROUND
 * AnalogInputSubsystem::CAL_MODE_REFERENCE</i>
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setCalMode( int calMode ) {
	if(
		calMode != CAL_MODE_NORMAL
		&& calMode != CAL_MODE_GROUND
		&& calMode != CAL_MODE_REFERENCE
	)
		throw IllegalArgumentException( "Invalid cal. mode" );
	if( this->calMode != calMode ) {
		this->calMode = calMode;
		if( autoConfig )
			writeConfig();
	}	// if( this->calMode ...
	return *this;
}	// AnalogInputSubsystem::setCalMode()

/**
 * Sets the trigger mode.
 * @param triggerMode a bitwise OR of these flags:
 * <i>AnalogInputSubsystem::TRIG_MODE_CTR0_EXT
 * AnalogInputSubsystem::TRIG_MODE_FALLING_EDGE
 * AnalogInputSubsystem::TRIG_MODE_SCAN
 * AnalogInputSubsystem::TRIG_MODE_EXTERNAL
 * AnalogInputSubsystem::TRIG_MODE_TIMER</i>
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setTriggerMode( int triggerMode ) {
	if( ( triggerMode & ~TRIG_MODE_VALID_MASK ) != 0 )
		throw IllegalArgumentException( "Invalid trigger mode" );
	if( this->triggerMode != triggerMode ) {
		this->triggerMode = triggerMode;
		if( autoConfig )
			writeConfig();
	}	// if( this->triggerMode ...
	return *this;
}	// AnalogInputSubsystem::setTriggerMode()

/**
 * Gets the current range for <i>channel</i>.
 * @param channel the channel for which to obtain the current range.
 * @return Current range for <i>channel</i>.
 * @see setRange( int channel, int range )
 * @throws IllegalArgumentException
 */

int AnalogInputSubsystem::getRange( int channel ) const {
	if(
		channel < 0
		|| channel >= numMUXChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	return inputRange[ channel / channelsPerGroup ].getRange();
}	// AnalogInputSubsystem::getRange()

/**
 * Gets the current range for multiple A/D channels.
 * @param startChannel the first channel for which to obtain the current range.
 * @param numChannels the number of channels for which to obtain the current range.
 * @return Array containing the current ranges for the specified channels.
 * @see setRange( int startChannel, const IntArray &range )
 * @throws IllegalArgumentException
 */

IntArray AnalogInputSubsystem::getRange( int startChannel, int numChannels ) const {
	if(
		numChannels < 1
		|| startChannel < 0
		|| startChannel + numChannels > numMUXChannels
	)
		throw IllegalArgumentException( "Invalid start channel or number of channels" );
	IntArray range( numChannels );
	for( int index = 0; index < numChannels; index++ )
		range[ index ] = inputRange[ ( startChannel + index ) / channelsPerGroup ].getRange();
	return range;
}	// AnalogInputSubsystem::getRange()

/**
 * Sets the range for a single A/D channel.
 * @param channel the channel for which to set the range.
 * @param range the range (voltage range) for the channel. May be one of:
 * <i>AnalogInputSubsystem::RANGE_0_1V
 * AnalogInputSubsystem::RANGE_1V
 * AnalogInputSubsystem::RANGE_0_2V
 * AnalogInputSubsystem::RANGE_2V
 * AnalogInputSubsystem::RANGE_0_5V
 * AnalogInputSubsystem::RANGE_5V
 * AnalogInputSubsystem::RANGE_0_10V
 * AnalogInputSubsystem::RANGE_10V</i>
 * @return This subsystem, useful for chaining together multiple operations.
 * @see setDifferentialMode( int channel, bool differentialMode )
 */

AnalogInputSubsystem &AnalogInputSubsystem::setRange( int channel, int range ) {
	if(
		channel < 0
		|| channel >= numMUXChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	const int group = channel / channelsPerGroup;
	if( inputRange[ group ].getRange() != range ) {
		inputRange[ group ].setRange( range );
		if( autoConfig )
			writeConfig();
	}	// if( inputRange[ ...
	return *this;
}	// AnalogInputSubsystem::setRange()

/**
 * Sets the range for multiple A/D channels.
 * @param startChannel the first channel for which to set the range.
 * @param range an array of ranges, one per channel <i>(see setRange( int channel, int range ))</i>.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setRange( int startChannel, const IntArray &range ) {
	if( range.size() < 1
            || startChannel < 0
            || startChannel + ( int ) range.size() > numMUXChannels
	)
		throw IllegalArgumentException( "Invalid gain code array or start channel" );
	bool configChanged = false;
	for( int index = 0; index < ( int ) range.size(); index++ ) {
		const int group = ( startChannel + index ) / channelsPerGroup;
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
	return *this;
}	// AnalogInputSubsystem::setRange()

/**
 * Tells if <i>channel</i> is configured for single-ended or differential mode.
 * @param channel the channel for which to obtain the current differential mode.
 * @return Current range for <i>channel</i>.
 * @return <i>False</i> indicates single-ended mode; <i>true</i> indicates differential mode.
 * @see setDifferentialMode( int channel, boolean differentialMode )
 * @throws IllegalArgumentException
 */

bool AnalogInputSubsystem::isDifferentialMode( int channel ) const {
	if(
		channel < 0
		|| channel >= numMUXChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	return differentialMode[ channel / channelsPerGroup ];
}	// AnalogInputSubsystem::isDifferentialMode()

/**
 * Tells if multiple A/D channels are configured for single-ended or differential mode.
 * @param startChannel the first channel for which to obtain the current differential mode.
 * @param numChannels the number of channels for which to obtain the current differential mode.
 * @return Array containing the current differential modes for the specified channels. <i>False</i> indicates
 * channel is configured for single-ended mode and <i>true</i> indicates channel is configured for differential mode.
 * @see setDifferentialMode( int startChannel, const BoolArray &differentialMode )
 * @throws IllegalArgumentException
 */

BoolArray AnalogInputSubsystem::isDifferentialMode( int startChannel, int numChannels ) const {
	if(
		numChannels < 1
		|| startChannel < 0
		|| startChannel + numChannels > numMUXChannels
	)
		throw IllegalArgumentException( "Invalid start channel or number of channels" );
	BoolArray differentialMode( numChannels );
	for( int index = 0; index < numChannels; index++ )
		differentialMode[ index ] = this->differentialMode[ ( startChannel + index ) / channelsPerGroup ];
	return differentialMode;
}	// AnalogInputSubsystem::isDifferentialMode()

/**
 * Sets a single A/D channel to differential or single-ended mode.
 * When using differential mode, one should have a good understanding of how the hardware implements it.
 * Considering the simple case of a device with only sixteen input channels, when differential mode is enabled
 * for a channel, that channel is paired with another channel, eight higher than the one for which differential
 * mode is enabled. For instance, if differential mode is enabled for channel 1, then it is paired with channel 9,
 * meaning that channel 1 will return the voltage difference between channels 1 and 9, and channel 9 will no longer
 * return a meaningful reading.
 * This scheme also means that enabling differential mode for channels 8-15 has no effect. In fact, if
 * one attempts to enable differential mode for channels 8-15, nothing happens and if the differential mode setting
 * is read back from the device for those channels, it will likely no longer be enabled! Further confusing matters is
 * that some newer firmware does not clear the differential mode setting for channels 8-15, meaning that it will be
 * returned from the device exactly as set even though it has no effect. So ... one should not rely on the
 * differential mode setting for channels 8-15 to behave in a consistent or predictable manner.
 * For consistency and simplicity, one may read counts or volts from channels 8-15 even while differential
 * mode is enabled, but the readings will not be meaningful. In differential mode, only the base channel (0-7) of the
 * pair that's enabled for differential mode will return a meaningful reading. Channels 8-15 which are not enabled
 * for differential mode will continue to return meaningful readings. For example, if differential mode is enabled
 * for channel 1, then channel 1 will return a meaningful reading, channel 9 will not, and channels 8 and
 * 10-15 will.
 * Considering the more complex case of a device such as the USB-AI16-64MA, which has an additional MUX
 * affording 32 differential, or 64 single-ended inputs, things are a bit more complex. In this case, channels
 * 0-3 share the same differential mode (and range) setting; channels 4-7 share the same setting; and so on. For the
 * sake of simplicity and to support future designs which may have distinct settings for all channels, this software
 * permits the differential mode (and range) to be specified for <i>any</i> MUXed channel, even though ultimately
 * multiple channels may share the same setting. For example, on such a device as this, setting the differential
 * mode (or range) of channel 1 also sets the differential mode (or range) of channels 0, 2 and 3.
 * There is yet another case to consider, that of devices such as the USB-AI16-128A. This device may have
 * up to 128 channels, which share settings in groups of eight rather than four on the USB-AI16-64MA. Method
 * <i>getChannelsPerGroup()</i> tells how many channels are grouped together on each device, and this topic is discussed
 * more thoroughly in <a href="http://accesio.com/MANUALS/USB-AI%20FAMILY.PDF">http://accesio.com/MANUALS/USB-AI FAMILY.PDF</a>.
 * The foregoing description also applies to the range setting, so one should refer to <i>setRange( int channel, int range )</i>
 * as well.
 * @param channel the channel for which to set differential or single-ended mode.
 * @param differentialMode <i>false</i> selects single-ended mode; <i>true</i> selects differential mode.
 * @return This subsystem, useful for chaining together multiple operations.
 */

AnalogInputSubsystem &AnalogInputSubsystem::setDifferentialMode( int channel, bool differentialMode ) {
	if(
		channel < 0
		|| channel >= numMUXChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	const int group = channel / channelsPerGroup;
	if( this->differentialMode[ group ] != differentialMode ) {
		this->differentialMode[ group ] = differentialMode;
		if( autoConfig )
			writeConfig();
	}	// if( this->differentialMode[ ...
	return *this;
}	// AnalogInputSubsystem::setDifferentialMode()

/**
 * Sets multiple A/D channels to differential or single-ended mode.
 * @param startChannel the first channel for which to set differential or single-ended mode.
 * @param differentialMode an array of mode selectors, one per channel. For each element in the array,
 * <i>false</i> selects single-ended mode for that channel and <i>true</i> selects differential mode.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setDifferentialMode( int startChannel, const BoolArray &differentialMode ) {
	if( differentialMode.size() < 1
		|| startChannel < 0
		|| startChannel + ( int ) differentialMode.size() > numMUXChannels
	)
		throw IllegalArgumentException( "Invalid differential mode array or start channel" );
	bool configChanged = false;
	for( int index = 0; index < ( int ) differentialMode.size(); index++ ) {
		const int group = ( startChannel + index ) / channelsPerGroup;
		if( this->differentialMode[ group ] != differentialMode[ index ] ) {
			this->differentialMode[ group ] = differentialMode[ index ];
			configChanged = true;
		}	// if( this->differentialMode[ ...
	}	// for( int index ...
	if(
		configChanged
		&& autoConfig
	)
		writeConfig();
	return *this;
}	// AnalogInputSubsystem::setDifferentialMode()

/**
 * Sets the range and differential mode for a single A/D channel.
 * @param channel the channel for which to set the range.
 * @param range the range (voltage range) for the channel <i>(see setRange( int channel, int range ))</i>.
 * @param differentialMode <i>false</i> selects single-ended mode; <i>true</i> selects differential mode.
 * @return This subsystem, useful for chaining together multiple operations.
 */

AnalogInputSubsystem &AnalogInputSubsystem::setRangeAndDiffMode( int channel, int range, bool differentialMode ) {
	if(
		channel < 0
		|| channel >= numMUXChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	bool configChanged = false;
	const int group = channel / channelsPerGroup;
	if( inputRange[ group ].getRange() != range ) {
		inputRange[ group ].setRange( range );
		configChanged = true;
	}	// if( inputRange[ ...
	if( this->differentialMode[ group ] != differentialMode ) {
		this->differentialMode[ group ] = differentialMode;
		configChanged = true;
	}	// if( this->differentialMode[ ...
	if(
		configChanged
		&& autoConfig
	)
		writeConfig();
	return *this;
}	// AnalogInputSubsystem::setRangeAndDiffMode()

/**
 * Sets the range and differential mode for multiple A/D channels.
 * @param startChannel the first channel for which to set the range and differential mode.
 * @param range an array of ranges, one per channel <i>(see setRange( int channel, int range ))</i>.
 * @param differentialMode an array of mode selectors, one per channel. For each element in the array,
 * <i>false</i> selects single-ended mode for that channel and <i>true</i> selects differential mode.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setRangeAndDiffMode( int startChannel, const IntArray &range, const BoolArray &differentialMode ) {
	if(range.size() < 1
           || differentialMode.size() < 1
           || startChannel < 0
           || startChannel + ( int ) range.size() > numMUXChannels
           || startChannel + ( int ) differentialMode.size() > numMUXChannels
	)
		throw IllegalArgumentException( "Invalid gain code array, differential mode array or start channel" );
	bool configChanged = false;
	for( int index = 0; index < ( int ) range.size(); index++ ) {
		const int group = ( startChannel + index ) / channelsPerGroup;
		if( inputRange[ group ].getRange() != range[ index ] ) {
			inputRange[ group ].setRange( range[ index ] );
			configChanged = true;
		}	// if( inputRange[ ...
	}	// for( int index ...
	for( int index = 0; index < ( int ) differentialMode.size(); index++ ) {
		const int group = ( startChannel + index ) / channelsPerGroup;
		if( this->differentialMode[ group ] != differentialMode[ index ] ) {
			this->differentialMode[ group ] = differentialMode[ index ];
			configChanged = true;
		}	// if( this->differentialMode[ ...
	}	// for( int index ...
	if(
		configChanged
		&& autoConfig
	)
		writeConfig();
	return *this;
}	// AnalogInputSubsystem::setRangeAndDiffMode()

/**
 * Sets all the A/D channels to the same range and differential mode.
 * @param range the range (voltage range) for the channels <i>(see setRange( int channel, int range ))</i>.
 * @param differentialMode <i>false</i> selects single-ended mode; <i>true</i> selects differential mode.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setRangeAndDiffMode( int range, bool differentialMode ) {
	bool configChanged = false;
	for( int group = 0; group < NUM_GAIN_CODE_REGISTERS; group++ ) {
		if( inputRange[ group ].getRange() != range ) {
			inputRange[ group ].setRange( range );
			configChanged = true;
		}	// if( inputRange[ ...
		if( this->differentialMode[ group ] != differentialMode ) {
			this->differentialMode[ group ] = differentialMode;
			configChanged = true;
		}	// if( this->differentialMode[ ...
	}	// for( int group ...
	if(
		configChanged
		&& autoConfig
	)
		writeConfig();
	return *this;
}	// AnalogInputSubsystem::setRangeAndDiffMode()

/**
 * Sets the number of over-samples for all A/D channels.
 * @param overSample number of over-samples (0-255).
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setOverSample( int overSample ) {
	if(
		overSample < 0
		|| overSample > MAX_OVERSAMPLE
	)
		throw IllegalArgumentException( "Invalid over-sample" );
	if( this->overSample != overSample ) {
		this->overSample = overSample;
		if( autoConfig )
			writeConfig();
	}	// if( this->overSample ...
	return *this;
}	// AnalogInputSubsystem::setOverSample()

/**
 * Loads a calibration table from a file into the A/D.
 * @param fileName the name of a file containing the calibration table. A calibration table must consist
 * of exactly 65,536 16-bit unsigned integers
 * <i>(see calibrate( bool autoCal, bool returnCalTable, const std::string &saveFileName ))</i>.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setCalibrationTable( const std::string &fileName ) {
	if(fileName.empty())
		throw IllegalArgumentException( "Invalid file name" );
	const int result = AIOUSB_ADC_LoadCalTable( getDeviceIndex(), fileName.c_str() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// AnalogInputSubsystem::setCalibrationTable()

/**
 * Sets the calibration table in the A/D to the contents of <i>calTable</i>.
 * @param calTable the calibration table to load. A calibration table must consist
 * of exactly 65,536 16-bit unsigned integers
 * <i>(see calibrate( bool autoCal, bool returnCalTable, const std::string &saveFileName ))</i>.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

AnalogInputSubsystem &AnalogInputSubsystem::setCalibrationTable( const UShortArray &calTable ) {
	if(( int ) calTable.size() != CAL_TABLE_WORDS	)
		throw IllegalArgumentException( "Invalid cal. table" );
	const int result = AIOUSB_ADC_SetCalTable( getDeviceIndex(), calTable.data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// AnalogInputSubsystem::setCalibrationTable()

/*
 * operations
 */

/**
 * Calibrates the A/D, generating either a default table or using the internal voltage references
 * to generate a calibration table.
 * @param autoCal <i>true</i> uses the internal voltage references to automatically calibrate the A/D; <i>false</i>
 * generates a default (uncalibrated) table.
 * @param returnCalTable <i>true</i> causes <i>calibrate()</i> to return the generated calibration table; <i>false</i>
 * returns an empty table.
 * @param saveFileName the name of the file in which to save the generated calibration table. If  empty, the generated
 * calibration table is not saved to a file.
 * @return If <i>returnCalTable</i> is <i>true</i>, an array of 65,536 16-bit unsigned integers representing the generated
 * calibration table is returned; otherwise, an empty table (containing zero elements) is returned.
 * @throws OperationFailedException
 */

UShortArray AnalogInputSubsystem::calibrate( bool autoCal, bool returnCalTable, const std::string &saveFileName ) {
	UShortArray calTable( returnCalTable ? CAL_TABLE_WORDS : 0 );
	const int result = AIOUSB_ADC_InternalCal( getDeviceIndex(), autoCal
		, returnCalTable ? calTable.data() : 0
		, saveFileName.length() > 0 ? saveFileName.c_str() : 0 );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return calTable;
}	// AnalogInputSubsystem::calibrate()

/**
 * Permits the A/D to be calibrated using an external voltage source. The proper way to use this function is to configure
 * the A/D with a default calibration table (such as by calling
 * <i>calibrate( bool autoCal, bool returnCalTable, const std::string &saveFileName )</i>). Then
 * inject a series of voltages into one of the A/D input channels, recording the count values reported by the A/D (by calling
 * <i>readCounts( int channel )</i>). It's also a good idea to enable oversampling while recording these
 * values in order to obtain the most stable readings. Alternatively, since <i>points</i> is an array of <i>double</i> values,
 * you can obtain individual A/D count measurements and average them yourself, producing a <i>double</i> average, and put that
 * value into the <i>points</i> array.
 * The <i>points</i> array consists of voltage-count pairs; <i>points[0]</i> is the first input voltage; <i>points[1]</i> is
 * the corresponding count value measured by the A/D; <i>points[2]</i> and <i>points[3]</i> contain the second pair of
 * voltage-count values; and so on. You can provide any number of pairs, although more than a few dozen is probably overkill,
 * not to mention would take a lot of effort to acquire.
 * This calibration procedure uses the current gain A/D setting for channel 0, so it must be the same as that used to collect
 * the measured A/D counts. It's recommended that all the channels be set to the same gain, the one that will be used during
 * normal operation. The calibration is gain dependent, so switching the gain after calibrating may introduce slight offset or
 * gain changes. So for best results, the A/D should be calibrated on the same gain setting that will be used during normal
 * operation. You can create any number of calibration tables. If your application needs to switch between ranges, you may wish
 * to create a separate calibration table for each range your application will use. Then when switching to a different range,
 * the application can load the appropriate calibration table.
 * Although calibrating in this manner does take some effort, it produces the best results, eliminating all sources of error
 * from the input pins onward. Furthermore, the calibration table can be saved to a file and reloaded into the A/D, ensuring consistency.
 * @param points array of voltage-count pairs to calibrate the A/D with.
 * @param returnCalTable <i>true</i> causes <i>calibrate()</i> to return the generated calibration table; <i>false</i>
 * returns an empty table.
 * @param saveFileName the name of the file in which to save the generated calibration table. If  empty, the generated
 * calibration table is not saved to a file.
 * @return If <i>returnCalTable</i> is <i>true</i>, an array of 65,536 16-bit unsigned integers representing the generated
 * calibration table is returned; otherwise, an empty table (containing zero elements) is returned.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

UShortArray AnalogInputSubsystem::calibrate( const DoubleArray &points, bool returnCalTable, const std::string &saveFileName ) {
	if(points.size() < 4					// at least 2 voltage-count pairs
           || ( points.size() & 1 ) != 0
	)
		throw IllegalArgumentException( "Invalid points array" );
	UShortArray calTable( returnCalTable ? CAL_TABLE_WORDS : 0 );
	const int result = AIOUSB_ADC_ExternalCal( getDeviceIndex(), points.data(), points.size() / 2 /* voltage-count pairs */
		, returnCalTable ? calTable.data() : 0
		, saveFileName.length() > 0 ? saveFileName.c_str() : 0 );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return calTable;
}	// AnalogInputSubsystem::calibrate()

/**
 * Reads from multiple A/D channels and returns a data set containing both the data captured and the parameters
 * in effect at the time the data was captured. Whereas readCounts( int startChannel, int numChannels )
 * and readVolts( int startChannel, int numChannels ) also read data from multiple channels,
 * they return only the raw data. <i>read()</i> returns a richer snapshot of the data.
 * @param startChannel the first channel to read.
 * @param numChannels the number of channels to read.
 * @return A data set containing the samples and the sampling parameters.
 * @throws OperationFailedException
 */

AI16_DataSet *AnalogInputSubsystem::read( int startChannel, int numChannels ) {
	assert( sizeof( long ) == sizeof( time_t ) );
	const long timeStamp = ( long ) time( 0 );	// record time when data capture starts
	UShortArray counts = readCounts( startChannel, numChannels );
	AI16_DataSet *const dataSet = new AI16_DataSet( *this, numChannels, timeStamp
		, calMode, triggerMode, overSample, isDiscardFirstSample() );
	for( int index = 0; index < numChannels; index++ ) {
		const int channel = startChannel + index;
		AI16_DataPoint &point = dataSet->points.at( index );
		point.counts = counts.at( index );
		point.channel = channel;
		point.range = getRange( channel );
		point.differentialMode = isDifferentialMode( channel );
	}	// for( int index ...
	return dataSet;
}	// AnalogInputSubsystem::read()

/**
 * Reads the A/D count value from a single channel.
 * @param channel the channel to read.
 * @return A/D counts (0-65,535). The meaning of these counts depends on the current A/D range of the channel
 * <i>(see setRange( int channel, int range ))</i>. The count value may be converted to
 * a voltage value using <i>countsToVolts( int channel, unsigned short counts ) const</i>.
 */

unsigned short AnalogInputSubsystem::readCounts( int channel ) {
	return readCounts( channel, 1 ).at( 0 );
}	// AnalogInputSubsystem::readCounts()

/**
 * Reads the A/D count values from multiple channels.
 * @param startChannel the first channel to read.
 * @param numChannels the number of channels to read.
 * @return An array of A/D counts (0-65,535), one per channel read. The meaning of these counts depends on the
 * current A/D range of each channel <i>(see setRange( int channel, int range ))</i>.
 * The array of count values may be converted to an array of voltage values using
 * <i>countsToVolts( int startChannel, const UShortArray &counts ) const</i>.
 * @throws OperationFailedException
 */

UShortArray AnalogInputSubsystem::readCounts( int startChannel, int numChannels ) {
	/*
	 * save current configuration and restore it when done; we don't need to restore
	 * the scan range because it is explicitly set during every read operation
	 */
	const bool origDiscardFirstSample = isDiscardFirstSample();
	const int origOverSample = overSample;

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
	UShortArray counts( numChannels );
	const int result = (int)AIOUSB_GetScan( getDeviceIndex(), counts.data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );

	/*
	 * restore original configuration
	 */
	setDiscardFirstSample( origDiscardFirstSample );
	setOverSample( origOverSample );

	return counts;
}	// AnalogInputSubsystem::Read()

/**
 * Reads the voltage from a single channel.
 * @param channel the channel to read.
 * @return A voltage value, limited to the current A/D range of the channel <i>(see
 * setRange( int channel, int range ))</i>. The voltage value may be converted to a
 * count value using <i>voltsToCounts( int channel, double volts ) const</i>.
 */

double AnalogInputSubsystem::readVolts( int channel ) {
	return readVolts( channel, 1 ).at( 0 );
}	// AnalogInputSubsystem::readVolts()

/**
 * Reads the voltage from multiple channels.
 * @param startChannel the first channel to read.
 * @param numChannels the number of channels to read.
 * @return An array of voltage values, one per channel read, each limited to the current A/D range of each channel
 * <i>(see setRange( int channel, int range ))</i>. The array of voltage values may be
 * converted to an array of count values using <i>voltsToCounts( int startChannel, const DoubleArray &volts ) const</i>.
 */

DoubleArray AnalogInputSubsystem::readVolts( int startChannel, int numChannels ) {
	return countsToVolts( startChannel, readCounts( startChannel, numChannels ) );
}	// AnalogInputSubsystem::readVolts()

/**
 * Starts a large A/D acquisition process in a background thread and returns immediately. The status of the
 * acquisition process can be monitored using <i>readBulkSamplesAvailable()</i>, which
 * returns the number of samples available to be retrieved by <i>readBulkNext( int numSamples )</i>.
 * When the last of the data has been retrieved using <i>readBulkNext()</i>, the bulk acquisition process is
 * automatically terminated and becomes ready to be used again.
 * <i>While a bulk acquisition process is in progress, no functions of the device other than readBulkSamplesAvailable()
 * or readBulkNext() should be used.</i>
 * This example shows the proper way to configure the device for a large A/D acquisition process using the internal timer.
 * <pre>device.adc()
 *   .setStreamingBlockSize( 100000 )
 *   .setCalMode( AnalogInputSubsystem::CAL_MODE_NORMAL )
 *   .setTriggerMode( AnalogInputSubsystem::TRIG_MODE_SCAN | AnalogInputSubsystem::TRIG_MODE_TIMER )
 *   .setClock( 100000 )
 *   .readBulkStart( 1, 1, numSamples );
 * do {
 *   UShortArray data = device.adc().readBulkNext( 20000 );
 *   ... do something with data ...
 * } while( ...more data is available... );</pre>
 * @param startChannel the first channel to read.
 * @param numChannels the number of channels to read.
 * @param numSamples the total number of <i>samples</i> to read.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

AnalogInputSubsystem &AnalogInputSubsystem::readBulkStart( int startChannel, int numChannels, int numSamples ) {
	if( numSamples < 1 )
		throw IllegalArgumentException( "Invalid number of samples" );
	readBulkBuffer = new unsigned short[ numSamples ];
	if( readBulkBuffer == 0 )
		throw OperationFailedException( AIOUSB_ERROR_NOT_ENOUGH_MEMORY );
	memset( readBulkBuffer, 0, numSamples * sizeof( unsigned short ) );
	setScanRange( startChannel, numChannels );
	const int result = ADC_BulkAcquire( getDeviceIndex(), numSamples * sizeof( unsigned short ), readBulkBuffer );
	if( result == AIOUSB_SUCCESS ) {
		readBulkSamplesRequested = numSamples;
		readBulkSamplesRetrieved = 0;
	} else {
		delete[] readBulkBuffer;
		readBulkBuffer = 0;
		readBulkSamplesRequested = readBulkSamplesRetrieved = 0;
		throw OperationFailedException( result );
	}	// if( result ...
	return *this;
}	// AnalogInputSubsystem::readBulkStart()

/**
 * Gets the number of samples available to be retrieved during a bulk acquisition process initiated by
 * <i>readBulkStart( int startChannel, int numChannels, int numSamples )</i>.
 * @return The number of samples available.
 * @throws OperationFailedException
 */

int AnalogInputSubsystem::readBulkSamplesAvailable() {
	int samplesAvailable = 0;
	if( readBulkBuffer != 0 ) {
		unsigned long bytesRemaining;
		const int result = ADC_BulkPoll( getDeviceIndex(), &bytesRemaining );
		if( result != AIOUSB_SUCCESS )
			throw OperationFailedException( result );
		const int samplesRemaining = bytesRemaining / sizeof( unsigned short );
		samplesAvailable = readBulkSamplesRequested - readBulkSamplesRetrieved - samplesRemaining;
		if( samplesAvailable < 0 )
			samplesAvailable = 0;
	}	// if( readBulkBuffer ...
	return samplesAvailable;
}	// AnalogInputSubsystem::readBulkSamplesAvailable()

/**
 * Retrieves the next set of samples acquired during a bulk acquisition process initiated by
 * <i>readBulkStart( int startChannel, int numChannels, int numSamples )</i>.
 * @param numSamples the number of samples to retrieve.
 * @return An array containing the number of samples requested, or all that are available. If fewer samples
 * are available than are requested, only the samples available are returned. If zero samples are available,
 * a zero-length array is returned.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

UShortArray AnalogInputSubsystem::readBulkNext( int numSamples ) {
	if( numSamples < 1 )
		throw IllegalArgumentException( "Invalid number of samples" );
	UShortArray counts;
	if( readBulkBuffer != 0 ) {
		const int samplesAvailable = readBulkSamplesAvailable();
		if( samplesAvailable > 0 ) {
			if( numSamples > samplesAvailable )
				numSamples = samplesAvailable;
			counts.resize( numSamples );
			memcpy( counts.data(), readBulkBuffer + readBulkSamplesRetrieved, numSamples * sizeof( unsigned short ) );
			if( samplesAvailable <= numSamples ) {
				/*
				 * no more data remains to be retrieved; finish bulk read procedure
				 */
				delete[] readBulkBuffer;
				readBulkBuffer = 0;
				readBulkSamplesRequested = readBulkSamplesRetrieved = 0;
			} else
				readBulkSamplesRetrieved += numSamples;
		}	// if( samplesAvailable ...
	}	// if( readBulkBuffer ...
	return counts;
}	// AnalogInputSubsystem::readBulkNext()

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
 * setRange( int channel, int range ))</i>.
 * @throws IllegalArgumentException
 */

double AnalogInputSubsystem::countsToVolts( int channel, unsigned short counts ) const {
	if(
		channel < 0
		|| channel >= numMUXChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	return inputRange[ channel / channelsPerGroup ].countsToVolts( counts );
}	// AnalogInputSubsystem::countsToVolts()

/**
 * Converts an array of A/D count values to an array of voltage values, based on the current gain setting for each of
 * the specified channels. This method is intended to convert an array of readings from sequential channels, such as
 * might have been obtained from <i>readCounts( int startChannel, int numChannels )</i>. Be careful to ensure
 * that the count values were actually obtained from the specified channels and that the gains havn't changed since the
 * count values were obtained.
 * @param startChannel the first channel number to use for converting counts to volts.
 * @param counts the count values to convert to volts.
 * @return An array of voltage values calculated using the <i>current</i> A/D range of each of the channels <i>(see
 * setRange( int channel, int range ))</i>. The array returned has the same number of
 * elements as <i>counts</i>.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

DoubleArray AnalogInputSubsystem::countsToVolts( int startChannel, const UShortArray &counts ) const {
	if(counts.size() < 1
           || startChannel < 0
           || startChannel + ( int ) counts.size() > numMUXChannels
	)
		throw IllegalArgumentException( "Invalid counts array or start channel" );
	DoubleArray volts( counts.size() );
	for( int index = 0; index < ( int ) counts.size(); index++ )
		volts.at( index ) = inputRange[ ( startChannel + index ) / channelsPerGroup ].countsToVolts( counts.at( index ) );
	return volts;
}	// AnalogInputSubsystem::countsToVolts()

/**
 * Converts a single voltage value to A/D counts, based on the current gain setting for the specified channel.
 * Be careful to ensure that the voltage value was actually obtained from the specified channel and that the gain hasn't
 * changed since the voltage value was obtained.
 * @param channel the channel number to use for converting volts to counts.
 * @param volts the voltage value to convert to counts.
 * @return A count value calculated using the <i>current</i> A/D range of the channel <i>(see
 * setRange( int channel, int range ))</i>.
 * @throws IllegalArgumentException
 */

unsigned short AnalogInputSubsystem::voltsToCounts( int channel, double volts ) const {
	if(
		channel < 0
		|| channel >= numMUXChannels
	)
		throw IllegalArgumentException( "Invalid channel" );
	return ( unsigned short ) inputRange[ channel / channelsPerGroup ].voltsToCounts( volts );
}	// AnalogInputSubsystem::voltsToCounts()

/**
 * Converts an array of voltage values to an array of A/D count values, based on the current gain setting for each of
 * the specified channels. This method is intended to convert an array of readings from sequential channels, such as
 * might have been obtained from <i>readVolts( int startChannel, int numChannels )</i>. Be careful
 * to ensure that the voltage values were actually obtained from the specified channels and that the gains havn't
 * changed since the voltage values were obtained.
 * @param startChannel the first channel number to use for converting volts to counts.
 * @param volts the voltage values to convert to counts.
 * @return An array of count values calculated using the <i>current</i> A/D range of each of the channels <i>(see
 * setRange( int channel, int range ))</i>. The array returned has the same number of
 * elements as <i>volts</i>.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

UShortArray AnalogInputSubsystem::voltsToCounts( int startChannel, const DoubleArray &volts ) const {
	if(volts.size() < 1
           || startChannel < 0
           || startChannel + ( int ) volts.size() > numMUXChannels
           )
		throw IllegalArgumentException( "Invalid volts array or start channel" );
	UShortArray counts( volts.size() );
	for( int index = 0; index < ( int ) volts.size(); index++ )
		counts.at( index ) = ( unsigned short ) inputRange[ ( startChannel + index ) / channelsPerGroup ].voltsToCounts( volts.at( index ) );
	return counts;
}	// AnalogInputSubsystem::voltsToCounts()



}	// namespace AIOUSB


/* end of file */
