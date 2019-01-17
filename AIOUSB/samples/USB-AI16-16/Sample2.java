import java.io.*;
import AIOUSB.*;

public class Sample2 { 
    
    public static void main( String [] args ) {
        Boolean keepgoing = true;
        int read_count = 0;
        long retval;
        int numscans = 24000;
        int max_count = 4000000;
        int tobufsize = 32768;
        SWIGTYPE_p_unsigned_short tobuf = AIOUSB.new_ushortarray( tobufsize );
        
        try { 

            FileWriter outw = new FileWriter("output.txt");
            AIOUSB.AIOUSB_Init();
            AIOUSB.GetDevices();
            AIOUSB.AIOUSB_ListDevices();

            AIOContinuousBuf buf = new AIOContinuousBuf(0 , numscans+1, 16 );
            AIOUSB.AIOContinuousBufSetDeviceIndex( buf, 0 );
            AIOUSB.AIOContinuousBufInitConfiguration( buf );
            AIOUSB.AIOContinuousBufSetOverSample( buf, 0 );
            AIOUSB.AIOContinuousBufSetStartAndEndChannel( buf, 16, 31 );
            AIOUSB.AIOContinuousBufSetAllGainCodeAndDiffMode( buf , ADGainCode.AD_GAIN_CODE_0_2V, AIOUSB_BOOL_VAL.AIOUSB_TRUE );
            AIOUSB.AIOContinuousBufSaveConfig(buf);
            AIOUSB.AIOContinuousBufSetClock( buf, 10000 );
            AIOUSB.AIOContinuousBufCallbackStart( buf );
            // Thread.sleep(10000);
            while ( keepgoing ) {
                System.err.println( String.format("Waiting : total=%d, readpos=%d, writepos=%d, status=%s", 
                                                  read_count, 
                                                  AIOUSB.AIOContinuousBufGetReadPosition(buf), 
                                                  AIOUSB.AIOContinuousBufGetWritePosition(buf),
                                                  AIOUSB.AIOContinuousBufGetStatus( buf )
                                                  )
                                    );

                Thread.sleep(1000);
                if( read_count > max_count || AIOUSB.AIOContinuousBufGetStatus( buf ) != THREAD_STATUS.RUNNING.swigValue() ) {
                    System.err.println(String.format("Exit reason: read_count=%d, max=%d,but_status=%s",
                                                     read_count, 
                                                     max_count, 
                                                     AIOUSB.AIOContinuousBufGetStatus( buf ))
                                       );
                    keepgoing = false;
                    // System.out.println("Exiting from main");
                    AIOUSB.AIOContinuousBufEnd( buf );
                    retval = AIOUSB.AIOContinuousBufGetExitCode( buf );
                }
            }
            System.err.println( String.format("Predrain Waiting : total=%d, readpos=%d, writepos=%d", read_count, AIOUSB.AIOContinuousBufGetReadPosition(buf), AIOUSB.AIOContinuousBufGetWritePosition(buf)));
            System.err.println( String.format("Numscans=%d",(int)AIOUSB.AIOContinuousBufCountScansAvailable(buf)  ));


            while (  AIOUSB.AIOContinuousBufCountScansAvailable(buf) > 0 ) {
                System.err.println( String.format("Read=%d,Write=%d,size=%d,Avail=%d",
                                                  AIOUSB.AIOContinuousBufGetReadPosition(buf),
                                                  AIOUSB.AIOContinuousBufGetWritePosition(buf),
                                                  AIOUSB.AIOContinuousBufGetBufferSize( buf ),
                                                  (int)AIOUSB.AIOContinuousBufCountScansAvailable(buf))
                                    );
                retval = (int)AIOUSB.AIOContinuousBufReadIntegerScanCounts( buf, tobuf ,(long)tobufsize, (long)tobufsize );
                // System.err.println("Retval was " + retval );
                if ( retval < 0 ) {
                    System.err.println( String.format("not ok - ERROR reading from buffer at position: %d", AIOUSB.AIOContinuousBufGetReadPosition(buf)));
                    keepgoing = false;
                } else {
                    read_count += retval;
                    for( int i = 0, ch = 0 ; i < retval; i ++, ch = ((ch+1) % (int)AIOUSB.AIOContinuousBufNumberChannels(buf)) ) {
                        outw.write( String.format("%d,", AIOUSB.ushort_getitem( tobuf, i ) ));
                        if( ((i+1) % (int)AIOUSB.AIOContinuousBufNumberChannels(buf)) == 0 ) {
                            outw.write("\n");
                        }
                    }
                }
            }
            outw.close();
        } catch ( InterruptedException e ) {
            System.out.println("An error\n");
        } catch ( IOException ee )  {
            System.out.println("An error\n");
        }
    }
};
