// runme.java

import AIOUSB.*;
import com.jdotsoft.jarloader.JarClassLoader;


public class test {

    public static void main(String argv[]) {
        System.out.println("Loading AIOUSB64");

        System.out.println( AIOUSB.AIOUSB_Init() );

        AIOUSB.AIOUSB_ListDevices();
        System.out.println("Result code is " + ResultCode.AIOUSB_ERROR_FILE_NOT_FOUND );
        
        System.out.println( 4 == ResultCode.AIOUSB_ERROR_FILE_NOT_FOUND.swigValue() );
        System.out.println( 0 == ResultCode.AIOUSB_SUCCESS.swigValue() );
        
    }
}
