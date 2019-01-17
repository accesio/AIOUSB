/*
 * $RCSfile: CounterSubsystem.hpp,v $
 * $Revision: 1.8 $
 * $Date: 2010/01/29 23:49:53 $
 * jEdit:tabSize=4:collapseFolds=1:
 *
 * class CounterSubsystem declarations
 */


#if ! defined( CounterSubsystem_hpp )
#define CounterSubsystem_hpp


#include <DeviceSubsystem.hpp>
#include <Counter.hpp>


namespace AIOUSB {

/**
 * Class CounterSubsystem represents the counter/timer subsystem of a device. One accesses
 * this counter/timer subsystem through its parent object, typically through a method such as
 * <i>ctr() (see USB_AI16_Family::ctr())</i>.
 */

class CounterSubsystem : public DeviceSubsystem {
	friend class Counter;
	friend class USB_AI16_Family;
	friend class USB_CTR_15_Family;
	friend class USB_DIO_32_Family;


protected:
	static const int COUNTERS_PER_BLOCK = 3;

	int numCounterBlocks;						// number of counter blocks
	int numCounters;							// number of individual counters
	CounterList counters;						// list of counters


public:
	CounterSubsystem( USBDeviceBase &parent );
	virtual ~CounterSubsystem();

	/*
	 * properties
	 */

	virtual std::ostream &print( std::ostream &out );

	/**
	 * Gets the number of counter blocks. Typically there are three counters per counter block.
	 * @return The number of counter blocks.
	 */

	int getNumCounterBlocks() const {
		return numCounterBlocks;
	}	// getNumCounterBlocks()

	/**
	 * Gets the number of individual counters, indexed from 0 to n-1.
	 * @return The number of individual counters.
	 */

	int getNumCounters() const {
		return numCounters;
	}	// getNumCounters()

	/*
	 * operations
	 */

	Counter &getCounter( int counter );
	UShortArray readCounts( bool oldData );
	CounterSubsystem &selectGate( int counter );
	double startClock( int counterBlock, double clockHz );

	/**
	 * Halts the counter started by <i>startClock( int counterBlock, double clockHz )</i>
	 * @param counterBlock the counter block to halt generating a frequency.
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	CounterSubsystem &stopClock( int counterBlock ) {
		startClock( counterBlock, 0 );
		return *this;
	}	// stopClock()



};	// class CounterSubsystem

}	// namespace AIOUSB

#endif

/* end of file */
