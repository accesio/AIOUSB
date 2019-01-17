/**
 * @file   Counter.cpp
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @brief  class Counter implementation
 */

#include "CppCommon.h"
#include <assert.h>
#include <typeinfo>
#include <AIOUSB_Core.h>
#include "USBDeviceManager.hpp"
#include "Counter.hpp"
#include "CounterSubsystem.hpp"
#include "USB_CTR_15_Family.hpp"


using namespace std;

namespace AIOUSB {


const int Counter::MODE_TERMINAL_COUNT;
const int Counter::MODE_ONE_SHOT;
const int Counter::MODE_RATE_GENERATOR;
const int Counter::MODE_SQUARE_WAVE;
const int Counter::MODE_SW_TRIGGERED;
const int Counter::MODE_HW_TRIGGERED;




Counter::Counter( CounterSubsystem *parent, int counterIndex ) {
	assert( parent != 0
		&& counterIndex >= 0 );
	this->parent = parent;
	this->counterIndex = counterIndex;
}	// Counter::Counter()





/**
 * Gets the index of the parent device on the USB bus. Used internally in calls to the underlying API.
 * @return The index of the parent device on the USB bus.
 */

int Counter::getDeviceIndex() const {
	assert( parent != 0 );
	return parent->getDeviceIndex();
}	// Counter::getDeviceIndex()

/**
 * Sets the counter's mode.
 * @param mode the counter mode. May be one of:
 * <i>Counter::MODE_TERMINAL_COUNT
 * Counter::MODE_ONE_SHOT
 * Counter::MODE_RATE_GENERATOR
 * Counter::MODE_SQUARE_WAVE
 * Counter::MODE_SW_TRIGGERED
 * Counter::MODE_HW_TRIGGERED</i>
 * @return This counter, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

Counter &Counter::setMode( int mode ) {
	assert( counterIndex >= 0 );
	if(
		mode < MODE_TERMINAL_COUNT
		|| mode > MODE_HW_TRIGGERED
	)
		throw IllegalArgumentException( "Invalid mode" );
	const int result = CTR_8254Mode( getDeviceIndex(), 0 /* BlockIndex */, counterIndex, mode );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// Counter::setMode()

/**
 * Loads a count value into the counter.
 * @param count the count value (0-65,535) to load into the counter.
 * @return This counter, useful for chaining together multiple operations.
 * @throws OperationFailedException
 */

Counter &Counter::setCount( unsigned short count ) {
	if( typeid( parent ) != typeid( USB_CTR_15_Family ) )
		throw OperationFailedException( "Supported only in USB-CTR-15" );
	assert( counterIndex >= 0 );
	const int result = CTR_8254Load( getDeviceIndex(), 0 /* BlockIndex */, counterIndex, count );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// Counter::setCount()

/**
 * Sets a counter mode and loads a count value into the counter.
 * @param mode the counter mode <i>(see setMode( int mode ))</i>.
 * @param count the count value (0-65,535) to load into the counter.
 * @return This counter, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

Counter &Counter::setModeAndCount( int mode, unsigned short count ) {
	assert( counterIndex >= 0 );
	if(
		mode < MODE_TERMINAL_COUNT
		|| mode > MODE_HW_TRIGGERED
	)
		throw IllegalArgumentException( "Invalid mode" );
	const int result = CTR_8254ModeLoad( getDeviceIndex(), 0 /* BlockIndex */, counterIndex, mode, count );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// Counter::setModeAndCount()

/**
 * Reads a counter's current count value.
 * @return The current count value (0-65,535).
 * @throws OperationFailedException
 */

unsigned short Counter::readCount() {
	assert( counterIndex >= 0 );
	unsigned short count;
	const int result = CTR_8254Read( getDeviceIndex(), 0 /* BlockIndex */, counterIndex, &count );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return count;
}	// Counter::ReadCount()

/**
 * Reads a counter's current count value and status.
 * @return An array of 2 16-bit integers:
 * char[ 0 ] contains the current count value (0-65,535)
 * char[ 1 ] contains the current counter status (0-255)
 * @throws OperationFailedException
 */

UShortArray Counter::readCountAndStatus() {
	assert( counterIndex >= 0 );
	unsigned short count;
	unsigned char status;
	const int result = CTR_8254ReadStatus( getDeviceIndex(), 0 /* BlockIndex */, counterIndex, &count, &status );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	UShortArray values( 2 );
	values.at( 0 ) = count;
	values.at( 1 ) = status;
	return values;
}	// Counter::readCountAndStatus()

/**
 * Reads a counter's current count value, <i>then</i> sets a new mode and loads a new count value into the counter.
 * @param mode the counter mode <i>(see setMode( int mode ))</i>.
 * @param count the count value (0-65,535) to load into the counter.
 * @return The 16-bit count value (0-65,535) prior to setting the new mode and count.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

unsigned short Counter::readCountAndSetModeAndCount( int mode, unsigned short count ) {
	assert( counterIndex >= 0 );
	if(
		mode < MODE_TERMINAL_COUNT
		|| mode > MODE_HW_TRIGGERED
	)
		throw IllegalArgumentException( "Invalid mode" );
	unsigned short prevCount;
	const int result = CTR_8254ReadModeLoad( getDeviceIndex(), 0 /* BlockIndex */, counterIndex, mode, count, &prevCount );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return prevCount;
}	// Counter::readCountAndSetModeAndCount()



}	// namespace AIOUSB

/* end of file */
