/*
 * $RCSfile: DIOStreamSubsystem.hpp,v $
 * $Revision: 1.9 $
 * $Date: 2010/01/29 00:08:11 $
 * jEdit:tabSize=4:collapseFolds=1:
 *
 * class DIOStreamSubsystem declarations
 */

#if ! defined( DIOStreamSubsystem_hpp )
#define DIOStreamSubsystem_hpp


#include <DeviceSubsystem.hpp>


namespace AIOUSB {

/**
 * Class DIOStreamSubsystem represents the digital I/O streaming subsystem of a device. One accesses
 * this counter/timer subsystem through its parent object, typically through a method such as
 * <i>diostream() (see USB_DIO_16_Family::diostream())</i>.
 */

class DIOStreamSubsystem : public DeviceSubsystem {
	friend class USB_DIO_16_Family;


protected:
	double clockHz;								// last actual frequency set by setClock()



protected:
	DIOStreamSubsystem( USBDeviceBase &parent );
	virtual ~DIOStreamSubsystem();



public:

	/*
	 * properties
	 */

	virtual std::ostream &print( std::ostream &out );

	/*
	 * configuration
	 */

	/**
	 * Gets the current streaming block size.
	 * @return The current streaming block size. The value returned may not be the same as the value passed to
	 * <i>setStreamingBlockSize( int blockSize )</i> because that value is rounded up to a whole multiple of 256.
	 * @throws OperationFailedException
	 */

	int getStreamingBlockSize() const {
		return parent->getStreamingBlockSize();
	}	// getStreamingBlockSize()

	/**
	 * Sets the streaming block size.
	 * @param blockSize the streaming block size you wish to set. This will be rounded up to the
	 * next multiple of 256.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws IllegalArgumentException
	 * @throws OperationFailedException
	 */

	DIOStreamSubsystem &setStreamingBlockSize( int blockSize ) {
		parent->setStreamingBlockSize( blockSize );
		return *this;
	}	// setStreamingBlockSize()

	/**
	 * Gets the current internal read/write clock speed of a digital I/O stream.
	 * @return The actual frequency that will be generated, based on the last call to
	 * <i>setClock( bool directionRead, double clockHz )</i>.
	 */

	double getClock() const {
		return clockHz;
	}	// getClock()

	double setClock( bool directionRead, double clockHz );

	/**
	 * Stops the internal read/write clocks of a digital I/O stream.
	 * @return This subsystem, useful for chaining together multiple operations.
	 * @throws OperationFailedException
	 */

	DIOStreamSubsystem &stopClock() {
		setClock( false /* which clock doesn't matter */, 0 );
		return *this;
	}	// stopClock()

	/*
	 * operations
	 */

	DIOStreamSubsystem &open( bool directionRead );
	DIOStreamSubsystem &close();
	UShortArray read( int numSamples );
	int write( const UShortArray &values );

	/**
	 * Clears the streaming FIFO, using one of several different methods.
	 * @param method the method to use when clearing the FIFO. May be one of:
	 * <i>USBDeviceBase::CLEAR_FIFO_METHOD_IMMEDIATE
	 * USBDeviceBase::CLEAR_FIFO_METHOD_AUTO
	 * USBDeviceBase::CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT
	 * USBDeviceBase::CLEAR_FIFO_METHOD_WAIT</i>
	 * @return This subsystem, useful for chaining together multiple operations.
	 */

	DIOStreamSubsystem &clearFIFO( FIFO_Method method ) {
		parent->clearFIFO( method );
		return *this;
	}	// clearFIFO()



};	// class DIOStreamSubsystem

}	// namespace AIOUSB

#endif

/* end of file */
