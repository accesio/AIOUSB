/*
 * $RCSfile: Sample.java,v $
 * $Date: 2010/01/29 23:00:04 $
 * $Revision: 1.1 $
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
import java.util.*;
// }}}

class Sample {
	public static final String VERSION = "1.1, 29 January 2010";

	public static void main( String args[] ) {
		USBDeviceManager deviceManager = null;
		try {
/*API*/		deviceManager = new USBDeviceManager();
/*API*/		deviceManager.open();
			System.out.println(
				  "USB-DA12-8A sample program version: " + VERSION + "\n"
/*API*/			+ "  AIOUSB Java library version: " + deviceManager.VERSION_NUMBER + ", " + deviceManager.VERSION_DATE + "\n"
/*API*/			+ "  AIOUSB library version: " + deviceManager.getAIOUSBVersion() + ", " + deviceManager.getAIOUSBVersionDate() + "\n"
				+ "  JRE version: " + System.getProperty( "java.version" ) + "\n"
				+ "  OS version: " + System.getProperty( "os.name" )
					+ " " + System.getProperty( "os.arch" )
					+ " " + System.getProperty( "os.version" ) + "\n"
				+ "  This program demonstrates controlling a USB-DA12-8A device on\n"
				+ "  the USB bus. For simplicity, it uses the first such device found\n"
				+ "  on the bus."
			);
/*API*/		deviceManager.printDevices();
/*API*/		USBDevice[] devices = deviceManager.getDeviceByProductID( USBDeviceManager.USB_DA12_8A_REV_A, USBDeviceManager.USB_DA12_8A );
			if( devices.length > 0 ) {
				USB_DA12_8A_Family device = ( USB_DA12_8A_Family ) devices[ 0 ];	// get first device found
/*API*/			device.setCommTimeout( 1000 );

				/*
				 * demonstrate accessing device properties
				 */
/*API*/			System.out.println( "Found device \'" + device.getName() + "\' with serial number " + Long.toHexString( device.getSerialNumber() ) );

				/*
				 * demonstrate writing D/A counts to one D/A channel
				 */
/*API*/			final int numChannels = device.dac().getNumChannels();
				final int TEST_CHANNEL = 0;		// channels are numbered 0 to numChannels - 1
				final char counts = DA12_AnalogOutputSubsystem.MAX_COUNTS / 2;	// half of full scale
/*API*/			device.dac().writeCounts( TEST_CHANNEL, counts );
				System.out.println( ( int ) counts + " D/A counts successfully output to channel " + TEST_CHANNEL );

				/*
				 * demonstrate writing D/A counts to multiple D/A channels
				 */
				char[] countPoints = new char[ numChannels * 2 ];	// channel/count pairs
				for( int channel = 0; channel < numChannels; channel++ ) {
					countPoints[ channel * 2 ] = ( char ) channel;
					countPoints[ channel * 2 + 1 ] = ( char ) ( ( channel + 1 ) * DA12_AnalogOutputSubsystem.MAX_COUNTS / numChannels );
				}	// for( int channel ...
/*API*/			device.dac().writeCounts( countPoints );
				System.out.println( "Multiple D/A counts successfully output to " + numChannels + " channels" );

				/*
				 * demonstrate writing volts to one D/A channel; note that the hardware range is selected
				 * by means of jumpers; this software setting is merely so that the correct D/A count values
				 * are calculated for the current hardware range setting; this demonstration assumes the
				 * hardware jumpers have been set to the +/-10V range
				 */
				final int HARDWARE_RANGE = DA12_AnalogOutputSubsystem.RANGE_10V;
				final double volts = 5;
/*API*/			device.dac().setRange( HARDWARE_RANGE );
/*API*/			device.dac().writeVolts( TEST_CHANNEL, volts );
				System.out.println( volts + " volts (" + ( int ) device.dac().voltsToCounts( TEST_CHANNEL, volts ) + " D/A counts)"
					+ " successfully output to channel " + TEST_CHANNEL );

				/*
				 * demonstrate writing volts to multiple D/A channels
				 */
/*API*/			OutputVoltagePoint[] voltPoints = new OutputVoltagePoint[ numChannels ];
				for( int channel = 0; channel < numChannels; channel++ )
/*API*/				voltPoints[ channel ] = new OutputVoltagePoint( channel, ( channel + 1 ) * 10.0 / numChannels );
/*API*/			device.dac().writeVolts( voltPoints );
				System.out.println( "Multiple volts successfully output to " + numChannels + " channels" );
			} else
				System.out.println( "No USB-DA12-8A devices found on USB bus" );
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
