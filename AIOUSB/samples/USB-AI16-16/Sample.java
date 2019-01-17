/*
 * $RCSfile: Sample.java,v $
 * $Date: 2009/12/25 18:44:28 $
 * $Revision: 1.22 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

// {{{ build instructions
/*
 * to compile:
 *   javac -cp ../../java/aiousb.jar Sample.java
 * to run:
 *   java -cp ../../java/aiousb.jar:. Sample
 */
// }}}

// {{{ imports
import com.acces.aiousb.*;
import java.io.*;
import java.text.*;
import java.util.*;
// }}}

class Sample {
	public static final String VERSION = "1.22, 25 December 2009";
	protected static final boolean SAVE_DATA_TO_FILE = true;
	protected static final boolean CHECK_FOR_ZERO_DATA = true;

	public static void main( String args[] ) {
		USBDeviceManager deviceManager = null;
		try {
/*API*/		deviceManager = new USBDeviceManager();
/*API*/		deviceManager.open();
			System.out.println(
				  "USB-AI16-16A sample program version: " + VERSION + "\n"
/*API*/			+ "  AIOUSB Java library version: " + deviceManager.VERSION_NUMBER + ", " + deviceManager.VERSION_DATE + "\n"
/*API*/			+ "  AIOUSB library version: " + deviceManager.getAIOUSBVersion() + ", " + deviceManager.getAIOUSBVersionDate() + "\n"
				+ "  JRE version: " + System.getProperty( "java.version" ) + "\n"
				+ "  OS version: " + System.getProperty( "os.name" )
					+ " " + System.getProperty( "os.arch" )
					+ " " + System.getProperty( "os.version" ) + "\n"
				+ "  This program demonstrates controlling a USB-AI16-16A family device on\n"
				+ "  the USB bus. For simplicity, it uses the first such device found\n"
				+ "  on the bus and supports these product IDs:"
/*API*/			+ Arrays.toString( USB_AI16_Family.getSupportedProductNames() )
			);
/*API*/		deviceManager.printDevices();
/*API*/		USBDevice[] devices = deviceManager.getDeviceByProductID( USB_AI16_Family.getSupportedProductIDs() );
			if( devices.length > 0 ) {
				USB_AI16_Family device = ( USB_AI16_Family ) devices[ 0 ];	// get first device found
/*API*/			device.reset()
/*API*/				.setCommTimeout( 1000 );

				/*
				 * demonstrate reading EEPROM
				 */
				System.out.println( "EEPROM contents:\n"
/*API*/				+ Arrays.toString( device.customEEPROMRead( 0, device.CUSTOM_EEPROM_SIZE ) ) );

				/*
				 * demonstrate A/D configuration
				 */
/*API*/			device.adc()
/*API*/				.setRangeAndDiffMode( AnalogInputSubsystem.RANGE_5V, false )
/*API*/				.setCalMode( AnalogInputSubsystem.CAL_MODE_NORMAL )
/*API*/				.setTriggerMode( AnalogInputSubsystem.TRIG_MODE_SCAN )
/*API*/				.setOverSample( 50 )
/*API*/				.setDiscardFirstSample( true );
				System.out.println( "A/D successfully configured" );

				/*
				 * demonstrate A/D calibration
				 */
/*API*/			if( device.adc().isAutoCalPresent( false ) ) {
					System.out.print( "Calibrating A/D, may take a few seconds ... " );
/*API*/				device.adc().calibrate( true, false, null );
					System.out.println( "successful" );
				}	// if( device.adc() ...

				/*
				 * demonstrate reading A/D inputs
				 */
				int startChannel = 0
/*API*/				, numChannels = device.adc().getNumMUXChannels();
/*API*/			char[] counts = device.adc().readCounts( startChannel, numChannels );
				System.out.println( "Read from channel " + startChannel
					+ " to channel " + ( startChannel + numChannels - 1 )
/*API*/				+ " using " + device.adc().getOverSample() + " oversamples:" );
				DecimalFormat voltageFormat = new DecimalFormat( "0.0000000" );
				for( int index = 0; index < counts.length; index++ )
					System.out.println( "  Channel " + ( startChannel + index ) + " = " + ( int ) counts[ index ]
/*API*/					+ " (" + voltageFormat.format( device.adc().countsToVolts( startChannel + index, counts[ index ] ) ) + " volts)" );

				/*
				 * demonstrate reading A/D data set
				 */
/*API*/			AI16_DataSet dataSet = device.adc().read( startChannel, numChannels );
/*API*/			dataSet.print( System.out );

				/*
				 * demonstrate bulk A/D reads
				 */
				BufferedOutputStream output = null;
				if( SAVE_DATA_TO_FILE )
					output = new BufferedOutputStream( new FileOutputStream( new File( "bulkdata" ) ) );
				int numSamples = 1000000
					, samplesReceived = 0;
/*API*/			device.adc()
/*API*/				.setStreamingBlockSize( 100000 )
/*API*/				.setCalMode( AnalogInputSubsystem.CAL_MODE_NORMAL )
/*API*/				.setTriggerMode( AnalogInputSubsystem.TRIG_MODE_SCAN | AnalogInputSubsystem.TRIG_MODE_TIMER )
/*API*/				.setClock( 1000000 )
/*API*/				.readBulkStart( 1, 1, numSamples );
				do {
					Thread.sleep( 100 );
/*API*/				char[] data = device.adc().readBulkNext( 4000 );
					if( data.length > 0 ) {
						if( SAVE_DATA_TO_FILE ) {
							for( int index = 0; index < data.length; index++ ) {
								output.write( data[ index ] & 0xff );
								output.write( ( data[ index ] >> 8 ) & 0xff );
							}	// for( int index ...
						}	// if( SAVE_DATA_TO_FILE )

						if( CHECK_FOR_ZERO_DATA ) {
							/*
							 * on the bipolar ranges, with nothing connected to the inputs, the
							 * count values should be around half of full scale, far from zero; if
							 * any count values are zero, it likely indicates some sort of serious
							 * hardware, firmware or software problem; print one warning per block
							 * to avoid flooding the screen with messages
							 */
							for( int index = 0; index < data.length; index++ ) {
								if( data[ index ] == 0 ) {
									System.err.println( "Warning: zero data at index " + ( samplesReceived + index ) );
									break;		// from for(); only one warning per block
								}	// if( data[ ...
							}	// for( int index ...
						}	// if( CHECK_FOR_ZERO_DATA )

						samplesReceived += data.length;
						numSamples -= data.length;
						System.out.println( "Bulk read " + samplesReceived + " samples, " + numSamples + " remaining" );
						if( numSamples <= 0 )
							break;				// from do()
					}	// if( data.length ...
				} while( true );
/*API*/			device.adc().setTriggerMode( AnalogInputSubsystem.TRIG_MODE_SCAN );
				if( SAVE_DATA_TO_FILE )
					output.close();
			} else
				System.out.println( "No USB-AI16-16A devices found on USB bus" );
/*API*/		deviceManager.close();
		} catch( Exception ex ) {
			System.err.println( "Error \'" + ex.toString() + "\' occurred while manipulating device" );
			if(
				deviceManager != null
/*API*/			&& deviceManager.isOpen()
			)
/*API*/			deviceManager.close();
		}	// catch( ...
	}	// main()
}	// class Sample

/* end of file */
