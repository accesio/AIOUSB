/*
 * $RCSfile: Counter.hpp,v $
 * $Revision: 1.4 $
 * $Date: 2010/01/29 23:49:53 $
 * jEdit:tabSize=4:collapseFolds=1:
 *
 * class Counter, CounterList declarations
 */

#if ! defined( Counter_hpp )
#define Counter_hpp


#include <vector>


namespace AIOUSB {

class CounterSubsystem;

/**
 * Class Counter represents a single counter/timer. One accesses a counter through its
 * CounterSubsystem parent object <i>(see CounterSubsystem::getCounter( int counter ))</i>.
 */

class Counter {
	friend class CounterSubsystem;


public:
	/*
	 * 8254 counter/timer modes
	 */
	/** Mode 0: interrupt on terminal count <i>(see setMode( int mode ))</i>. */
	static const int MODE_TERMINAL_COUNT			= 0;	// mode 0: interrupt on terminal count
	/** Mode 1: hardware retriggerable one-shot <i>(see setMode( int mode ))</i>. */
	static const int MODE_ONE_SHOT					= 1;	// mode 1: hardware retriggerable one-shot
	/** Mode 2: rate generator <i>(see setMode( int mode ))</i>. */
	static const int MODE_RATE_GENERATOR			= 2;	// mode 2: rate generator
	/** Mode 3: square wave mode <i>(see setMode( int mode ))</i>. */
	static const int MODE_SQUARE_WAVE				= 3;	// mode 3: square wave mode
	/** Mode 4: software triggered mode <i>(see setMode( int mode ))</i>. */
	static const int MODE_SW_TRIGGERED				= 4;	// mode 4: software triggered mode
	/** Mode 5: hardware triggered strobe (retriggerable) <i>(see setMode( int mode ))</i>. */
	static const int MODE_HW_TRIGGERED				= 5;	// mode 5: hardware triggered strobe (retriggerable)



protected:
	CounterSubsystem *parent;					// subsystem that this counter belongs to
	int counterIndex;							// counter index (using 0-based counter addressing)



	Counter( CounterSubsystem *parent, int counterIndex );



public:
	int getDeviceIndex() const;
	Counter &setMode( int mode );
	Counter &setCount( unsigned short count );
	Counter &setModeAndCount( int mode, unsigned short count );
	unsigned short readCount();
	UShortArray readCountAndStatus();
	unsigned short readCountAndSetModeAndCount( int mode, unsigned short count );


};	// class Counter



class CounterList : public std::vector<Counter*> {
};	// class CounterList



}	// namespace AIOUSB

#endif

/* end of file */
