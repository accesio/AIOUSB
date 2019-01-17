/*
 * $RCSfile: Sample.java,v $
 * $Date: 2009/12/25 19:11:55 $
 * $Revision: 1.4 $
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
	public static final String VERSION = "1.4, 25 December 2009";
	/*
	 * if you want to test writing to the EEPROM, set DEMO_EEPROM_WRITE to 'true'; after writing
	 * to the EEPROM this sample program will attempt to restore the original EEPROM contents,
	 * but you never know ...
	 */
	protected static final boolean DEMO_EEPROM_WRITE = false;

	public static void main( String args[] ) {
		USBDeviceManager deviceManager = null;
		try {
/*API*/		deviceManager = new USBDeviceManager();
/*API*/		deviceManager.open();
			System.out.println(
				  "USB-IIRO-16 sample program version: " + VERSION + "\n"
/*API*/			+ "  AIOUSB Java library version: " + deviceManager.VERSION_NUMBER + ", " + deviceManager.VERSION_DATE + "\n"
/*API*/			+ "  AIOUSB library version: " + deviceManager.getAIOUSBVersion() + ", " + deviceManager.getAIOUSBVersionDate() + "\n"
				+ "  JRE version: " + System.getProperty( "java.version" ) + "\n"
				+ "  OS version: " + System.getProperty( "os.name" )
					+ " " + System.getProperty( "os.arch" )
					+ " " + System.getProperty( "os.version" ) + "\n"
				+ "  This program demonstrates controlling a USB-IIRO-16 device on\n"
				+ "  the USB bus. For simplicity, it uses the first such device found\n"
				+ "  on the bus."
			);
/*API*/		deviceManager.printDevices();
/*API*/		USBDevice[] devices = deviceManager.getDeviceByProductID( USBDeviceManager.USB_IIRO_16 );
			if( devices.length > 0 ) {
				USB_DIO_Family device = ( USB_DIO_Family ) devices[ 0 ];	// get first device found
/*API*/			device.reset()
/*API*/				.setCommTimeout( 1000 );

				/*
				 * demonstrate writing EEPROM
				 */
				if( DEMO_EEPROM_WRITE ) {
					System.out.print( "Writing to EEPROM ... " );
					final int offset = 0x100;
					String message = "USB-IIRO-16 sample program";
/*API*/				byte[] prevData = device.customEEPROMRead( offset, message.length() );	// preserve current EEPROM contents
/*API*/				device.setCommTimeout( 10000 )	// writing the entire EEPROM (which we're not doing here) can take a long time
/*API*/					.customEEPROMWrite( offset, message.getBytes() );
/*API*/				String readMessage = new String( device.customEEPROMRead( offset, message.length() ) );
/*API*/				device.customEEPROMWrite( offset, prevData )
/*API*/					.setCommTimeout( 1000 );
					System.out.println(
						readMessage.equals( message )
							? "successful"
							: "failed: data read back did not compare" );
				}	// if( DEMO_EEPROM_WRITE )

				/*
				 * demonstrate reading EEPROM
				 */
				System.out.println( "EEPROM contents:\n"
/*API*/				+ Arrays.toString( device.customEEPROMRead( 0, device.CUSTOM_EEPROM_SIZE ) ) );

				/*
				 * demonstrate DIO configuration
				 */
				System.out.print( "Configuring digital I/O ... " );
/*API*/			final int numPorts = device.dio().getNumPorts();
				final int numOutputPorts = numPorts / 2;
/*API*/			final int numChannels = device.dio().getNumChannels();
				final int numOutputChannels = numChannels / 2;
				final int numInputChannels = numChannels - numOutputChannels;
				boolean[] outputs = new boolean[ numPorts ];
				Arrays.fill( outputs, 0, numOutputPorts, true );			// relays are outputs
				Arrays.fill( outputs, numOutputPorts, numPorts, false );	// other ports are inputs
				boolean[] values = new boolean[ numOutputChannels ];
				Arrays.fill( values, true );								// open all relays
/*API*/			device.dio().configure( false, outputs, values );
				System.out.println( "successful" );

				/*
				 * demonstrate writing outputs individually
				 */
				System.out.println(
					  "In the following demonstrations, you should hear the relays click\n"
					+ "and see the LED on the device blink."
				);
				System.out.print( "  CLosing relays:" );
				for( int channel = 0; channel < numOutputChannels; channel++ ) {
					System.out.print( " " + channel );
/*API*/				device.dio().write( channel, false );	// close relay
					Thread.sleep( 100 );
				}	// for( int channel ...
				System.out.print( "\n  Opening relays:" );
				for( int channel = 0; channel < numOutputChannels; channel++ ) {
					System.out.print( " " + channel );
/*API*/				device.dio().write( channel, true );	// open relay
					Thread.sleep( 100 );
				}	// for( int channel ...
				System.out.println();

				/*
				 * demonstrate writing outputs as a group
				 */
				System.out.print( "Closing all relays ... " );
				Arrays.fill( values, false );	// close all relays
/*API*/			device.dio().write( 0, values );
				System.out.println( "successful" );
				Thread.sleep( 2000 );
				System.out.print( "Opening all relays ... " );
				Arrays.fill( values, true );	// open all relays
/*API*/			device.dio().write( 0, values );
				System.out.println( "successful" );

				/*
				 * demonstrate reading inputs individually
				 */
				Thread.sleep( 1000 );
				System.out.print( "CLosing every other relay:" );
				for( int channel = 0; channel < numOutputChannels; channel += 2 ) {
					System.out.print( " " + channel );
/*API*/				device.dio().write( channel, false );	// close relay
					Thread.sleep( 100 );
				}	// for( int channel ...
				System.out.println( "\nReading back relays:" );
				for( int channel = 0; channel < numOutputChannels; channel++ ) {
/*API*/				System.out.println( "  Channel " + channel + " = " + device.dio().read( channel ) );
				}	// for( int channel ...
				System.out.println( "Reading isolated inputs:" );
				for( int channel = 0; channel < numInputChannels; channel++ ) {
/*API*/				System.out.println( "  Channel " + ( numOutputChannels + channel ) + " = " + device.dio().read( numOutputChannels + channel ) );
				}	// for( int channel ...

				/*
				 * open all relays
				 */
				Arrays.fill( values, true );
/*API*/			device.dio().write( 0, values );
			} else
				System.out.println( "No USB-IIRO-16 devices found on USB bus" );
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
