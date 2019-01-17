/**
 * @file   DigitalIOSubsystem.cpp
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @brief class DigitalIOSubsystem implementation
 *
 */

#include "CppCommon.h"
#include <assert.h>
#include "AIOUSB_Core.h"
#include "AIOTypes.h"
#include "DIOBuf.h"
#include "USBDeviceManager.hpp"
#include "DigitalIOSubsystem.hpp"


using namespace std;

namespace AIOUSB {



UCharArray &DigitalIOSubsystem::bitsToBytes( UCharArray &dest, int bit, const BoolArray &src ) {
	assert( dest.size() > 0
		&& src.size() > 0
		&& bit >= 0
		&& ( bit + src.size() ) <= dest.size() * BITS_PER_BYTE );
	int destByte = bit / BITS_PER_BYTE;
	unsigned char destBit = ( unsigned char ) ( 1 << ( bit % BITS_PER_BYTE ) );
	const unsigned char maxDestBit = ( unsigned char ) ( 1 << ( BITS_PER_BYTE - 1 ) );
	for( int index = 0; index < ( int ) src.size(); index++ ) {
		if( src.at( index ) )
			dest.at( destByte ) |= destBit;
		else
			dest.at( destByte ) &= ~destBit;
		if( ( destBit & maxDestBit ) == 0 ) {
			destBit <<= 1;
		} else {
			// advance to next byte
			destBit = 1;
			destByte++;
		}	// if( ( destBit ...
	}	// for( int index ...
	return dest;
}	// DigitalIOSubsystem::bitsToBytes()

BoolArray &DigitalIOSubsystem::bytesToBits( BoolArray &dest, const UCharArray &src, int bit ) {
	assert( dest.size() > 0
		&& src.size() > 0
		&& bit >= 0
		&& ( bit + dest.size() ) <= src.size() * BITS_PER_BYTE );
	int srcByte = bit / BITS_PER_BYTE;
	unsigned char srcBit = ( unsigned char ) ( 1 << ( bit % BITS_PER_BYTE ) );
	const unsigned char maxSrcBit = ( unsigned char ) ( 1 << ( BITS_PER_BYTE - 1 ) );
	for( int index = 0; index < ( int ) dest.size(); index++ ) {
		dest.at( index ) = ( src.at( srcByte ) & srcBit ) != 0;
		if( ( srcBit & maxSrcBit ) == 0 ) {
			srcBit <<= 1;
		} else {
			// advance to next byte
			srcBit = 1;
			srcByte++;
		}	// if( ( srcBit ...
	}	// for( int index ...
	return dest;
}	// DigitalIOSubsystem::bytesToBits()

DigitalIOSubsystem::DigitalIOSubsystem( USBDeviceBase &parent )
		: DeviceSubsystem( parent ) {
	numPorts
		= numChannels
		= numTristateGroups
		= numTristateChannels
		= 0;
	DeviceProperties properties;
	const int result = AIOUSB_GetDeviceProperties( getDeviceIndex(), &properties );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	numPorts = properties.DIOPorts;
	numTristateGroups = properties.Tristates;
	assert( numPorts > 0 );
	numChannels = numPorts * BITS_PER_BYTE;
	numTristateChannels = numTristateGroups * BITS_PER_BYTE;
	writeValues.resize( numPorts );
}	// DigitalIOSubsystem::DigitalIOSubsystem()

DigitalIOSubsystem::~DigitalIOSubsystem() {
	// nothing to do
}	// DigitalIOSubsystem::~DigitalIOSubsystem()





/*
 * properties
 */

/**
 * Prints the properties of this subsystem. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &DigitalIOSubsystem::print( ostream &out ) {
	out
		<< "    Number of digital I/O ports: " << dec << numPorts << endl
		<< "    Number of digital I/O channels: " << numChannels << endl
		<< "    Number of tristate groups: " << numTristateGroups << endl
		<< "    Number of tristate channels: " << numTristateChannels << endl;
	return out;
}	// DigitalIOSubsystem::print()

/*
 * configuration
 */

/**
 * Configures the digital I/O ports.
 * @param tristate <i>true</i> causes all bits on the device to enter tristate (high-impedance) mode;
 * <i>false</i> removes tristate mode. All devices with this feature power up in tristate mode, and tristate
 * mode is changed after the remainder of the configuration has occurred.
 * @param outputs an array of boolean values, one per digital I/O <i>port</i>. Each <i>true</i> value in the array
 * configures the entire corresponding I/O port as an output port; each <i>false</i> value configures the entire
 * corresponding I/O port as an input port.
 * @param values an array of boolean values, one per digital I/O <i>bit</i>, starting with bit 0 of the device (that is,
 * the least significant bit on the lowest numbered port). Each <i>true</i> value in the array sets the corresponding
 * output bit to a "1"; each <i>false</i> value sets the corresponding output bit to a "0." The values are written to
 * the digital output ports before the ports are taken out of tristate mode.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

DigitalIOSubsystem &DigitalIOSubsystem::configure( bool tristate, const BoolArray &outputs, const BoolArray &values ) {
	if(outputs.size() < 1
           || ( int ) outputs.size() > numPorts
           || values.size() < 1
           || ( int ) values.size() > numChannels
	)
		throw IllegalArgumentException( "Invalid outputs or values" );
	UCharArray outputMask( ( numPorts + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE );
	const int result = DIO_Configure( getDeviceIndex(), ( unsigned char ) tristate , 
                                          bitsToBytes( outputMask, 0, outputs ).data() ,
                                          bitsToBytes( writeValues, 0, values ).data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// DigitalIOSubsystem::configure()

/**
 * Configures the digital I/O ports. <i>If the device does not support the per-port tristate feature, then
 * configure( bool tristate, const BoolArray &outputs, const BoolArray &values )
 * should be used instead, otherwise an exception will be thrown.</i>
 * @param tristates an array of boolean values, one per tristate group. Each <i>true</i> value in the array
 * puts the entire corresponding I/O port into tristate (high-impedance) mode; each <i>false</i> value takes
 * the entire corresponding I/O port out of tristate mode. All devices with this feature power up in tristate
 * mode, and tristate mode is changed after the remainder of the configuration has occurred.
 * @param outputs an array of boolean values, one per digital I/O <i>port</i>. Each <i>true</i> value in the array
 * configures the entire corresponding I/O port as an output port; each <i>false</i> value configures the entire
 * corresponding I/O port as an input port.
 * @param values an array of boolean values, one per digital I/O <i>bit</i>, starting with bit 0 of the device (that is,
 * the least significant bit on the lowest numbered port). Each <i>true</i> value in the array sets the corresponding
 * output bit to a "1"; each <i>false</i> value sets the corresponding output bit to a "0." The values are written to
 * the digital output ports before the ports are taken out of tristate mode.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

DigitalIOSubsystem &DigitalIOSubsystem::configure( const BoolArray &tristates, const BoolArray &outputs, const BoolArray &values ) {
	if( tristates.size() < 1
            || ( int ) tristates.size() > numTristateChannels
            || outputs.size() < 1
            || ( int ) outputs.size() > numPorts
            || values.size() < 1
            || ( int ) values.size() > numChannels
	)
		throw IllegalArgumentException( "Invalid tristates, outputs or values" );
	UCharArray outputMask( ( numPorts + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE );
	UCharArray tristateMask( ( numTristateGroups + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE );
	const int result = DIO_ConfigureEx( getDeviceIndex()
		, bitsToBytes( outputMask, 0, outputs ).data()
		, bitsToBytes( writeValues, 0, values ).data()
		, bitsToBytes( tristateMask, 0, tristates ).data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// DigitalIOSubsystem::configure()

/**
 * Gets the current configuration of the digital I/O ports. <i>If the device does not support the per-port tristate
 * feature, then this method should not be used, otherwise an exception will be thrown.</i>
 * @param tristates an array of boolean values, one per tristate group, which will receive the current tristate
 * mode of each tristate group. Each <i>true</i> value returned in the array indicates that the entire corresponding
 * I/O port is in tristate (high-impedance) mode; each <i>false</i> value indicates that the entire corresponding I/O
 * port is not in tristate mode. If this parameter is <i>null</i>, then the tristate configuration is not returned.
 * @param outputs an array of boolean values, one per digital I/O <i>port</i>, which will receive the current output
 * mode of each I/O port. Each <i>true</i> value returned in the array indicates that the entire corresponding I/O
 * port is configured as an output port; each <i>false</i> value indicates that the entire corresponding I/O port is
 * configured as an input port. If this parameter is <i>null</i>, then the output configuration is not returned.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

DigitalIOSubsystem &DigitalIOSubsystem::getConfiguration( BoolArray *tristates, BoolArray *outputs ) {
	if(
		(
			tristates != 0
			&& (
				tristates->size() < 1
				|| ( int ) tristates->size() > numTristateGroups
			)
		)
		|| (
			outputs != 0
			&& (
				outputs->size() < 1
				|| ( int ) outputs->size() > numPorts
			)
		)
	)
		throw IllegalArgumentException( "Invalid tristates or outputs" );
	UCharArray outputMask( ( numPorts + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE );
	UCharArray tristateMask( ( numTristateGroups + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE );
	const int result = DIO_ConfigurationQuery( getDeviceIndex(), outputMask.data(), tristateMask.data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	if( tristates != 0 )
		bytesToBits( *tristates, tristateMask, 0 );
	if( outputs != 0 )
		bytesToBits( *outputs, outputMask, 0 );
	return *this;
}	// DigitalIOSubsystem::getConfiguration()

/*
 * operations
 */

/**
 * Reads a single digital input channel.
 * @param channel the channel to read.
 * @return <i>True</i> indicates that the bit is set ("1"); <i>false</i> indicates that the bit is clear ("0").
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

bool DigitalIOSubsystem::read( int channel ) {
    if( channel < 0 || channel >= numChannels ) 
        throw IllegalArgumentException( "Invalid channel" );
    unsigned char byteValue = 0;
    int result = DIO_Read1( getDeviceIndex(), channel , &byteValue );
    if( result < AIOUSB_SUCCESS )
        throw OperationFailedException( result );
    return (byteValue != 0);
}

/**
 * Reads multiple digital input channels.
 * @param startChannel the first channel to read.
 * @param numChannels the number of channels to read.
 * @return An array containing the values read from the specified channels. For each channel, <i>true</i>
 * indicates that the bit is set ("1"); <i>false</i> indicates that the bit is clear ("0").
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

BoolArray DigitalIOSubsystem::read( int startChannel, int numChannels ) {
	if( numChannels < 1 || startChannel < 0 || 
            startChannel + numChannels > this->numChannels
	)
		throw IllegalArgumentException( "Invalid start channel or number of channels" );
	UCharArray ports( numPorts );
        DIOBuf *buf = NewDIOBuf( numPorts );
	const int result = DIO_ReadAll( getDeviceIndex(), buf );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	BoolArray values( numChannels );
	return bytesToBits( values, ports, startChannel );
}	// DigitalIOSubsystem::read()

/**
 * Writes a single digital output channel.
 * @param channel the channel to write.
 * @param value the value to write to the specified channel. <i>True</i> sets the output bit to a "1" and
 * <i>false</i> clears the output bit to a "0".
 * @return This subsystem, useful for chaining together multiple operations.
 */

DigitalIOSubsystem &DigitalIOSubsystem::write( int channel, bool value ) {
	BoolArray values( 1 );
	values.at( 0 ) = value;
	return write( channel, values );
}	// DigitalIOSubsystem::write()

/**
 * Writes multiple digital output channels.
 * @param startChannel the first channel to write.
 * @param values an array containing the values to write to the specified channels. For each channel,
 * <i>true</i> sets the output bit to a "1" and <i>false</i> clears the output bit to a "0".
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

DigitalIOSubsystem &DigitalIOSubsystem::write( int startChannel, const BoolArray &values ) {
	if(values.size() < 1
           || startChannel < 0
           || startChannel + ( int ) values.size() > numChannels)
		throw IllegalArgumentException( "Invalid values or start channel" );
	const int result = DIO_WriteAll( getDeviceIndex(), bitsToBytes( writeValues, startChannel, values ).data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// DigitalIOSubsystem::write()



}	// namespace AIOUSB

/* end of file */
