/*
 * $RCSfile: Sample.java,v $
 * $Date: 2010/01/29 18:34:04 $
 * $Revision: 1.3 $
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
	public static final String VERSION = "1.3, 29 January 2010";
	private static double CLOCK_SPEED = 1000000;	// Hz
	private static int NUM_POINTS = 1024000;		// must be multiple of 256

	public static void main( String args[] ) {
		new Sample();
	}	// main()

	public Sample() {
		USBDeviceManager deviceManager = null;
		try {
/*API*/		deviceManager = new USBDeviceManager();
/*API*/		deviceManager.open();
			System.out.println(
				  "USB-DIO-16A sample program version: " + VERSION + "\n"
/*API*/			+ "  AIOUSB Java library version: " + deviceManager.VERSION_NUMBER + ", " + deviceManager.VERSION_DATE + "\n"
/*API*/			+ "  AIOUSB library version: " + deviceManager.getAIOUSBVersion() + ", " + deviceManager.getAIOUSBVersionDate() + "\n"
				+ "  JRE version: " + System.getProperty( "java.version" ) + "\n"
				+ "  OS version: " + System.getProperty( "os.name" )
					+ " " + System.getProperty( "os.arch" )
					+ " " + System.getProperty( "os.version" ) + "\n"
				+ "  This program demonstrates high speed streaming between 2 USB-DIO-16A\n"
				+ "  devices on the same USB bus. For simplicity, it uses the first 2 such\n"
				+ "  devices found on the bus."
			);
/*API*/		deviceManager.printDevices();
/*API*/		USBDevice[] devices = deviceManager.getDeviceByProductID( USBDeviceManager.USB_DIO_16A );
			if( devices.length >= 2 ) {
				/*
				 * use first two devices found
				 */
				USB_DIO_16_Family sendDevice = ( USB_DIO_16_Family ) devices[ 0 ];
				USB_DIO_16_Family receiveDevice = ( USB_DIO_16_Family ) devices[ 1 ];
/*API*/			DIOStreamSubsystem sendStream = sendDevice.diostream();
/*API*/			DIOStreamSubsystem receiveStream = receiveDevice.diostream();

				/*
				 * set up sending device
				 */
/*API*/			sendDevice.setCommTimeout( 1000 );
/*API*/			sendStream.setStreamingBlockSize( 256 );
/*API*/			sendStream.setClock( false, CLOCK_SPEED );
/*API*/			sendStream.open( false );
/*API*/			boolean[] tristates = new boolean[ sendDevice.dio().getNumTristateGroups() ];
				Arrays.fill( tristates, false );		// all ports are not in tristate mode
/*API*/			boolean[] outputs = new boolean[ sendDevice.dio().getNumPorts() ];
				Arrays.fill( outputs, true );			// all ports except last one are output
				outputs[ outputs.length - 1 ] = false;	// last port is input
/*API*/			boolean[] values = new boolean[ sendDevice.dio().getNumChannels() ];
				Arrays.fill( values, true );
/*API*/			sendDevice.dio().configure( tristates, outputs, values );

				/*
				 * set up receiving device
				 */
/*API*/			receiveDevice.setCommTimeout( 1000 );
/*API*/			receiveStream.setStreamingBlockSize( 256 );
/*API*/			receiveStream.stopClock();
/*API*/			receiveStream.open( true );
				Arrays.fill( outputs, false );			// all ports except last one are input
				outputs[ outputs.length - 1 ] = true;	// last port is output
/*API*/			receiveDevice.dio().configure( tristates, outputs, values );
				ReceiveThread receiveThread = new ReceiveThread( receiveStream, NUM_POINTS );
				receiveThread.start();

				/*
				 * start sending
				 */
				char[] sendData = new char[ NUM_POINTS ];
				for( int point = 0; point < NUM_POINTS; point++ )
					sendData[ point ] = ( char ) ( point & 0xffff );
				final int samplesSent = sendStream.write( sendData );
				if( samplesSent == sendData.length )
					System.out.println( "Successfully sent " + sendData.length + " samples" );
				else
					System.out.println( "Error: sent " + sendData.length + " samples, but actual was " + samplesSent + " samples" );

				/*
				 * give receiver thread a little time to finish receiving the data and then check data;
				 * with the default settings it should take about one second to transmit the data; using
				 * a fixed delay here is obviously crude, but it's merely for demonstration purposes
				 */
				System.out.println( "Waiting for data to be received ..." );
				Thread.sleep( 3000 );
				char[] receiveData = receiveThread.getData();
				if(
					receiveData != null
					&& Arrays.equals( receiveData, sendData )
				)
					System.out.println( "Successfully received " + receiveData.length + " samples" );
				else
					System.out.println( "Error: data received did not match data sent" );

				/*
				 * tidy up
				 */
				sendStream.stopClock();
				sendStream.close();
				receiveStream.close();
			} else
				System.out.println( "Failed to find 2 USB-DIO-16A devices" );
/*API*/		deviceManager.close();
		} catch( Exception ex ) {
			System.out.println( "Error \'" + ex.toString() + "\' occurred while manipulating device" );
			if(
				deviceManager != null
/*API*/			&& deviceManager.isOpen()
			)
/*API*/			deviceManager.close();
		}	// catch( ...
	}	// Sample()

	private class ReceiveThread extends Thread {
		DIOStreamSubsystem stream;
		int numPoints;
		char[] data = null;

		public ReceiveThread( DIOStreamSubsystem stream, int numPoints ) {
			this.stream = stream;
			this.numPoints = numPoints;
		}	// ReceiveThread()

		public void run() {
			try {
				data = stream.read( numPoints );
			} catch( Exception ex ) {
				System.out.println( "Error \'" + ex.toString() + "\' occurred while receiving from DIO stream" );
			}	// catch( ...
		}	// run()

		public char[] getData() {
			return data;
		}	// getData()

	}	// class ReceiveThread

}	// class Sample

/* end of file */
