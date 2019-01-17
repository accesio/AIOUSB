/*
 * sample program to write out the calibration table and then 
 * reload it again, verify that the data is in fact reversed
 */

package com.accesio;
import AIOUSB.*;
import java.util.ArrayList;
import java.util.Arrays;
import com.jdotsoft.jarloader.JarClassLoader;

public class read_channels_test_java {
    public static final String PROGNAME = new read_channels_test_java().getClass().getName();
    private final static String LIBRARY_NAME = "AIOUSB";
    private static boolean bLoaded = false;
    private final static String PREFIX = "AIOUSB: ";

    static public void loadLibrary() {
        String bitOS = System.getProperty("sun.arch.data.model", "");
        String lib = LIBRARY_NAME + bitOS;
        if (!bLoaded) {
            try {

                System.loadLibrary("aiousbdbg");
                System.loadLibrary("AIOUSB");
                // System.loadLibrary(lib);
                // #1 and #2 are identical: making a call to 
                //   ClassLoader.loadLibrary0(Class fromClass, final File file)            
                //System.out.println(PREFIX + "Library \"" + lib + "\" is loaded.");
                bLoaded = true;
                // return true;
            } catch (UnsatisfiedLinkError e) {
                //System.out.println(PREFIX + "Library \"" + lib + "\": " + e);
                // return false;
            }
        }
    }

    public static void main( String args[] ) {
        loadLibrary();
        AIOCommandLineOptions options = AIOUSB.NewAIOCommandLineOptionsFromDefaultOptions( AIOUSB.AIO_CMDLINE_SCRIPTING_OPTIONS() );
        long retval;
        
        // ArrayList nargs = new java.util.ArrayList( args.toList() );
        ArrayList<String> tmpargs = new ArrayList<String>(Arrays.asList(args));
        ArrayList<Integer> indices = new ArrayList<Integer>();

        
        retval = AIOUSB.AIOProcessCommandLine( options, tmpargs );

        retval = AIOUSB.AIOUSB_FindDeviceIndicesByGroup( indices, AIOUSB.AIO_ANALOG_INPUT() );

        retval = AIOUSB.AIOCommandLineOptionsListDevices( options, indices );
        
        ADCConfigBlock config = AIOUSB.NewADCConfigBlockFromJSON( AIOUSB.AIOCommandLineOptionsGetDefaultADCJSONConfig(options) )  ;
        
        System.out.println(config);

        retval = AIOUSB.AIOCommandLineOptionsOverrideADCConfigBlock( config, options );

        retval = AIOUSB.ADC_SetCal(AIOUSB.AIOCommandLineOptionsGetDeviceIndex(options), ":AUTO:");
        
        // SWIGTYPE_p_AIOUSBDevice dev = AIOUSB.AIODeviceTableGetAIOUSBDeviceAtIndex( AIOUSB.AIOCommandLineOptionsGetDeviceIndex(options));
        // SWIGTYPE_p_USBDevice usb = AIOUSB.AIOUSBDeviceGetUSBHandle( dev );
        // retval = AIOUSB.ADCConfigBlockCopy( AIOUSB.AIOUSBDeviceGetADCConfigBlock( dev ), config );
        // retval = AIOUSB.USBDevicePutADCConfigBlock( usb, config );
        // 
        // Or in just one line

        retval = AIOUSB.USBDevicePutADCConfigBlock( AIOUSB.AIOUSBDeviceGetUSBHandle( AIOUSB.AIODeviceTableGetAIOUSBDeviceAtIndex( AIOUSB.AIOCommandLineOptionsGetDeviceIndex(options))), config );

        doublearray volts = new doublearray(16);

        for ( int i = 1; i <= (int)(AIOUSB.AIOCommandLineOptionsGetScans(options)); i ++ ) { 
            retval = AIOUSB.ADC_GetScanV( AIOUSB.AIOCommandLineOptionsGetDeviceIndex(options), volts.cast() );
            System.out.println( volts );
        }


    }
};

