/**
 * @file   CounterSubsystem.cpp
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @brief  class CounterSubsystem implementation
 */

#include "CppCommon.h"
#include <assert.h>
#include <typeinfo>
#include <AIOUSB_Core.h>
#include "USBDeviceManager.hpp"
#include "CounterSubsystem.hpp"
#include "USB_CTR_15_Family.hpp"


using namespace std;

namespace AIOUSB {


const int CounterSubsystem::COUNTERS_PER_BLOCK;




CounterSubsystem::CounterSubsystem( USBDeviceBase &parent )
		: DeviceSubsystem( parent ) {
	numCounterBlocks = numCounters = 0;
	DeviceProperties properties;
	const int result = AIOUSB_GetDeviceProperties( getDeviceIndex(), &properties );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	numCounterBlocks = properties.Counters;
	assert( numCounterBlocks > 0 );
	if( numCounterBlocks > 20 )
		numCounterBlocks = 20;					// limit to arbitrary, reasonable number
	numCounters = numCounterBlocks * COUNTERS_PER_BLOCK;
	for( int index = 0; index < numCounters; index++ )
		counters.insert( counters.end(), new Counter( this, index ) );
	assert( ( int ) counters.size() == numCounters );
}	// CounterSubsystem::CounterSubsystem()

CounterSubsystem::~CounterSubsystem() {
	assert( ( int ) counters.size() == numCounters );
	for( int index = 0; index < numCounters; index++ ) {
		assert( counters.at( index ) != 0 );
		delete counters.at( index );
	}	// for( int index ...
	counters.clear();
}	// CounterSubsystem::~CounterSubsystem()





/*
 * properties
 */

/**
 * Prints the properties of this subsystem. Mainly useful for diagnostic purposes.
 * @param out the print stream where properties will be printed.
 * @return The print stream.
 */

ostream &CounterSubsystem::print( ostream &out ) {
	out
		<< "    Number of counter blocks: " << numCounterBlocks << endl
		<< "    Number of counters: " << numCounters << endl;
	return out;
}	// CounterSubsystem::print()

/*
 * operations
 */

/**
 * Gets a reference to an individual counter. You must obtain a reference to a counter
 * before you can perform counter operations.
 * @param counter the counter for which to obtain a reference (0 to n-1).
 * @return A reference to the specified counter.
 * @throws IllegalArgumentException
 */

Counter &CounterSubsystem::getCounter( int counter ) {
	if(
		counter < 0
		|| counter >= numCounters
	)
		throw IllegalArgumentException( "Invalid counter" );
	return *counters.at( counter );
}	// CounterSubsystem::getCounter()

/**
 * Reads the current count values of all the counters, optionally including an "old data" indication. If <i>oldData</i>
 * is <i>true</i>, then an extra word will be returned (one word for each counter, plus one extra word) that contains
 * an "old data" indication, which is useful for optimizing polling rates. If the value of the final word is zero, then
 * the data is "old data," meaning you are polling the counters faster than your gate signal is running. If <i>oldData</i>
 * is <i>false</i>, then only the count values are returned.
 * @param oldData <i>true</i> includes the "old data" indication in the returned data; <i>false</i> returns just the count values.
 * @return An array containing the current count values for all the counters plus an optional "old data" indication in the final word.
 * @throws OperationFailedException
 */

UShortArray CounterSubsystem::readCounts( bool oldData ) {
	if( typeid( parent ) != typeid( USB_CTR_15_Family ) )
		throw OperationFailedException( "Supported only in USB-CTR-15" );
	UShortArray counts( oldData ? numCounters : ( numCounters + 1 ) );
	const int result = oldData
		? CTR_8254ReadLatched( getDeviceIndex(), counts.data() )	// last word of 'counts' is non-zero if data is "old"
		: CTR_8254ReadAll( getDeviceIndex(), counts.data() );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return counts;
}	// CounterSubsystem::readCounts()

/**
 * Selects the counter to use as a gate in frequency measurement on other counters.
 * @param counter the counter to select as a gate (0 to n-1).
 * @return This subsystem, useful for chaining together multiple operations.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

CounterSubsystem &CounterSubsystem::selectGate( int counter ) {
	if( typeid( parent ) != typeid( USB_CTR_15_Family ) )
		throw OperationFailedException( "Supported only in USB-CTR-15" );
	if(
		counter < 0
		|| counter >= numCounters
	)
		throw IllegalArgumentException( "Invalid counter" );
	const int result = CTR_8254SelectGate( getDeviceIndex(), counter );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return *this;
}	// CounterSubsystem::selectGate()

/**
 * Selects an output frequency for a counter block and starts the counters.
 * <i>selectGate( int counter )</i> and <i>readCounts( bool oldData )</i>
 * are used in measuring frequency. To measure frequency one must count pulses for a known duration. In simplest terms,
 * the number of pulses that occur for 1 second translates directly to Hertz. In the USB-CTR-15 and other supported devices,
 * you can create a known duration by configuring one counter to act as a "gating" signal for any collection of other counters.
 * The other "measurement" counters will only count during the "high" side of the gate signal, which we can control.
 * So, to measure frequency you:
 * 1. Create a gate signal of known duration
 * 2. Connect this gating signal to the gate pins of all the "measurement" counters
 * 3. Call <i>selectGate()</i> to tell the board which counter is generating that gate
 * 4. Call <i>readCounts( true )</i> periodically to read the latched count values from
 * all the "measurement" counters.
 * In practice, it may not be possible to generate a gating signal of sufficient duration from a single counter. Simply concatenate
 * two or more counters into a series, or daisy-chain, and use the last counter's output as your gating signal. This last counter
 * in the chain should be selected as the "gate source" using <i>selectGate( int counter )</i>.
 * Once a value has been read from a counter using the <i>readCounts( true )</i> call, it
 * can be translated into actual Hz by dividing the count value returned by the high-side-duration of the gating signal, in seconds.
 * For example, if your gate is configured for 10Hz, the high-side lasts 0.05 seconds. If you read 1324 counts via the
 * <i>readCounts( true )</i> call, the frequency would be "1324 / 0.05", or 26.48 KHz.
 * @param counterBlock the counter block to use to generate the frequency.
 * @param clockHz the desired output frequency (in Hertz).
 * @return The actual frequency that will be generated, limited by the device's capabilities.
 * @throws IllegalArgumentException
 * @throws OperationFailedException
 */

double CounterSubsystem::startClock( int counterBlock, double clockHz ) {
	if(
		counterBlock < 0
		|| counterBlock >= numCounterBlocks
		|| clockHz < 0
	)
		throw IllegalArgumentException( "Invalid counter block or frequency" );
	const int result = CTR_StartOutputFreq( getDeviceIndex(), counterBlock, &clockHz );
	if( result != AIOUSB_SUCCESS )
		throw OperationFailedException( result );
	return clockHz;
}	// CounterSubsystem::startClock()



}	// namespace AIOUSB

/* end of file */
