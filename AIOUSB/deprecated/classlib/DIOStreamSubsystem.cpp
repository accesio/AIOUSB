/**
 * @file   DIOStreamSubsystem.cpp
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @brief  class DIOStreamSubsystem implementation
 *
 */



#include "CppCommon.h"
#include <assert.h>
#include <aiousb.h>
#include "USBDeviceManager.hpp"
#include "DIOStreamSubsystem.hpp"


using namespace std;

namespace AIOUSB {



DIOStreamSubsystem::DIOStreamSubsystem( USBDeviceBase &parent )
		: DeviceSubsystem( parent ) {
	clockHz = 0;
}	// DIOStreamSubsystem::DIOStreamSubsystem()

DIOStreamSubsystem::~DIOStreamSubsystem() {
	// nothing to do
}	// DIOStreamSubsystem::~DIOStreamSubsystem()





/*
 * properties
 */

/**
 * Prints the properties of this subsystem. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &DIOStreamSubsystem::print( ostream &out ) {
	out << "    Digital I/O streaming capability installed" << endl;
	return out;
}	// DIOStreamSubsystem::print()

/*
 * configuration
 */

/**
 * Sets the internal read/write clock speed of a digital I/O stream
 * <i>(see getClock())</i>.  Only one clock - the read or write clock
 * - may be active at a time, so this method <i><b>automatically turns
 * off</b></i> the clock not being set by this method. Therefore, do
 * not call this method to explicitly turn off one of the clocks
 * because it will turn off both of them. Also, when streaming between
 * two devices, only one should have an active internal clock; the
 * other should have its clocks turned off <i>(see stopClock())</i>.
 * @param directionRead <i>true</i> sets read clock; <i>false</i> sets
 * write clock.  @param clockHz the frequency at which to stream the
 * samples (in Hertz).  @return The actual frequency that will be
 * generated, limited by the device's capabilities.  @throws
 * OperationFailedException
 */

double DIOStreamSubsystem::setClock( bool directionRead, double clockHz ) {
	double unusedClockHz = 0;
	int result;
	if( directionRead )
		result = DIO_StreamSetClocks( getDeviceIndex(), &clockHz, &unusedClockHz );
	else
		result = DIO_StreamSetClocks( getDeviceIndex(), &unusedClockHz, &clockHz );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return ( this->clockHz = clockHz );
}	// DIOStreamSubsystem::setClock()

/*
 * operations
 */

/**
 * Opens a digital I/O stream. When you are done using the stream, you must close it by calling <i>close()</i>.
 * @param directionRead <i>true</i> open the stream for reading; <i>false</i> open the stream for writing.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

DIOStreamSubsystem &DIOStreamSubsystem::open( bool directionRead ) {
	const int result = DIO_StreamOpen( getDeviceIndex(), directionRead );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// DIOStreamSubsystem::open()

/**
 * Closes a digital I/O stream opened by a call to <i>open( bool directionRead )</i>.
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

DIOStreamSubsystem &DIOStreamSubsystem::close() {
	const int result = DIO_StreamClose( getDeviceIndex() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// DIOStreamSubsystem::close()

/**
 * Reads a frame from a digital I/O stream opened by a call to <i>open( true )</i>.
 * <i>You cannot read from, and write to a stream. A stream may be read-only or write-only.</i>
 * @param numSamples the number of samples to read.
 * @return An array containing the samples read. The array may be smaller than the number of samples requested
 * if fewer samples were received than were requested.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

UShortArray DIOStreamSubsystem::read( int numSamples ) {
	if( numSamples < 1 )
		throw IllegalArgumentException( "Invalid number of samples" );
	UShortArray values( numSamples );
	unsigned long bytesTransferred;
	const int result = DIO_StreamFrame( getDeviceIndex(), values.size(), values.data(), &bytesTransferred );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	const int samplesTransferred = bytesTransferred / sizeof( unsigned short );
	if( samplesTransferred != numSamples )
		values.resize( samplesTransferred );
	return values;
}	// DIOStreamSubsystem::read()

/**
 * Writes a frame to a digital I/O stream opened by a call to <i>open( false )</i>.
 * <i>You cannot read from, and write to a stream. A stream may be read-only or write-only.</i>
 * @param values an array containing the samples to write.
 * @return The number of samples actually written.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

int DIOStreamSubsystem::write( const UShortArray &values ) {
        if(values.size() < 1)
		throw IllegalArgumentException( "Invalid values" );
	unsigned long bytesTransferred;
	int result = (int)DIO_StreamFrame( getDeviceIndex(), values.size(), ( unsigned short * ) values.data(), &bytesTransferred );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return bytesTransferred / sizeof( unsigned short );
}	// DIOStreamSubsystem::write()



}	// namespace AIOUSB

/* end of file */
