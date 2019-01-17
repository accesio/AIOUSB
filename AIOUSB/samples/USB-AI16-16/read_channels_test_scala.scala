import scala.collection.JavaConversions._
import AIOUSB._;
import java.util.ArrayList;
// import org.scijava.nativelib._

object read_channels_test_scala { 

  def main( args: Array[String] ) { 
    var options = AIOUSB.NewAIOCommandLineOptionsFromDefaultOptions( AIOUSB.AIO_CMDLINE_SCRIPTING_OPTIONS() )

    var tmpargs = new java.util.ArrayList( args.toList )
    var retval = AIOUSB.AIOProcessCommandLine( options, tmpargs )
    var indices = new java.util.ArrayList[Int]()

    println( s"After command line processing $tmpargs" );
    var retcode = AIOUSB.AIOUSB_FindDeviceIndicesByGroup( indices, AIOUSB.AIO_ANALOG_INPUT() );
    println(s"After indices:${indices} and size:${indices.size}" )
    println(s"Retcode was ${retcode}")


    // Fails at this line here

    retval = AIOUSB.AIOCommandLineOptionsListDevices( options, indices )

    var config = AIOUSB.NewADCConfigBlockFromJSON( AIOUSB.AIOCommandLineOptionsGetDefaultADCJSONConfig(options) )

    println(config)

    retval = AIOUSB.AIOCommandLineOptionsOverrideADCConfigBlock( config, options )


    println(s"Original device id is ${AIOUSB.AIOCommandLineOptionsGetDeviceIndex(options)}")

    var dev = AIOUSB.AIODeviceTableGetAIOUSBDeviceAtIndex( AIOUSB.AIOCommandLineOptionsGetDeviceIndex(options));
    // println(s"Got value $dev from the device index")
    
    retval = AIOUSB.ADC_SetCal(AIOUSB.AIOCommandLineOptionsGetDeviceIndex(options), ":AUTO:");
    
    var usb = AIOUSB.AIOUSBDeviceGetUSBHandle( dev );
    retval = AIOUSB.ADCConfigBlockCopy( AIOUSB.AIOUSBDeviceGetADCConfigBlock( dev ), config );
    retval = AIOUSB.USBDevicePutADCConfigBlock( usb, config );

    // var volts =  AIOUSB.new_doublearray(16)
    var volts = new doublearray(16)
    for ( i <- 1 to AIOUSB.AIOCommandLineOptionsGetScans(options).toInt ) { 
      retval = AIOUSB.ADC_GetScanV( AIOUSB.AIOCommandLineOptionsGetDeviceIndex(options), volts.cast() );
      println(s"${volts}")
    }

  }
}



