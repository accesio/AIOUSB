require 'AIOUSB'
    
numscans = 24000
max_count = 4000000
tobufsize = 32768

AIOUSB.AIOUSB_Init()
AIOUSB.GetDevices()
AIOUSB.AIOUSB_ListDevices()
tobuf = AIOUSB.new_ushortarray( tobufsize )

buf = AIOUSB::AIOContinuousBuf.new(0 , numscans+1, 16 )
AIOUSB.AIOContinuousBuf_SetDeviceIndex( buf, 0 )
AIOUSB.AIOContinuousBuf_InitConfiguration( buf )
AIOUSB.AIOContinuousBuf_SetOverSample( buf, 0 )
AIOUSB.AIOContinuousBuf_SetStartAndEndChannel( buf, 16, 31 )
AIOUSB.AIOContinuousBuf_SetAllGainCodeAndDiffMode( buf , AIOUSB::AD_GAIN_CODE_0_2V, AIOUSB::AIOUSB_TRUE )
AIOUSB.AIOContinuousBuf_SaveConfig(buf)
AIOUSB.AIOContinuousBufSetClock( buf, 10000 )
AIOUSB.AIOContinuousBufCallbackStart( buf )
keepgoing = true
read_count = 0
sleep(1)
while keepgoing
    puts( sprintf("Waiting : total=%d, readpos=%d, writepos=%d, status=%s", 
                  read_count, 
                  AIOUSB.AIOContinuousBufGetReadPosition(buf), 
                  AIOUSB.AIOContinuousBufGetWritePosition(buf),
                  AIOUSB.AIOContinuousBufGetStatus( buf )
                  )
          )

  sleep(1)
  if ( read_count > max_count || AIOUSB.AIOContinuousBufGetStatus( buf ) !=  AIOUSB::RUNNING ) 
    puts(sprintf("Exit reason: read_count=%d, max=%d,but_status=%s",
                 read_count, 
                 max_count, 
                 AIOUSB.AIOContinuousBufGetStatus( buf ))
         )
    keepgoing = false

    AIOUSB.AIOContinuousBufEnd( buf )
    retval = AIOUSB.AIOContinuousBufGetExitCode( buf );
  end
end
puts( sprintf("Predrain Waiting : total=%d, readpos=%d, writepos=%d", read_count, AIOUSB.AIOContinuousBufGetReadPosition(buf), AIOUSB.AIOContinuousBufGetWritePosition(buf)))
puts( sprintf("Numscans=%d",AIOUSB.AIOContinuousBufCountScansAvailable(buf)  ))

outw = File.open("output.txt","w")
while  (AIOUSB.AIOContinuousBufCountScansAvailable(buf) > 0 )
  puts( sprintf("Read=%d,Write=%d,size=%d,Avail=%d",
                                    AIOUSB.AIOContinuousBufGetReadPosition(buf),
                                    AIOUSB.AIOContinuousBufGetWritePosition(buf),
                                    AIOUSB.AIOContinuousBufGetSize( buf ),
                                    AIOUSB.AIOContinuousBufCountScansAvailable(buf))
        )
  retval = AIOUSB.AIOContinuousBufReadIntegerScanCounts( buf, tobuf ,tobufsize, tobufsize )
  # puts("Retval was " + retval );
  if retval < 0
    puts( sprintf("not ok - ERROR reading from buffer at position: %d", AIOUSB.AIOContinuousBufGetReadPosition(buf)));
    keepgoing = false;
  else
    read_count += retval;
    # for( int i = 0, ch = 0 ; i < retval; i ++, ch = ((ch+1) % AIOUSB.AIOContinuousBuf_NumberChannels(buf)) ) {
    ch = 0
    (0..retval-1 ).each { |i| 
        outw.write( sprintf("%d,", AIOUSB.ushort_getitem( tobuf, i ) ))
        if ( ((i+1) % AIOUSB.AIOContinuousBuf_NumberChannels(buf)) == 0 )
          outw.write("\n");
        end
        
        ch = ((ch+1) % AIOUSB.AIOContinuousBuf_NumberChannels(buf))
  }
  end
end
