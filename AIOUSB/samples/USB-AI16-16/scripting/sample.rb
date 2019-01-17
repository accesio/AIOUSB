#!/usr/bin/env ruby

require 'AIOUSB'


class Device
  attr_accessor :name, :serialNumber,:index,:numDIOBytes,:numCounters,:productID
  def initialize( tmpopts )
    opts = Hash[*tmpopts]
    @name = opts["name"]
    @serialNumber = opts["serialNumber"]
    @index = opts["index"]
    @numDIOBytes = opts["numDIOBytes"]
    @numCounters = opts["numCounters"]
    @productID = opts["productID"]
  end
end

MAX_NAME_SIZE = 20;

deviceIndex = 0;
deviceFound = AIOUSB::AIOUSB_FALSE

CAL_CHANNEL = 5
MAX_CHANNELS = 128
NUM_CHANNELS = 16
NUM_OVERSAMPLES = 10
NUM_SCANS = 100000
BULK_BYTES = NUM_SCANS * NUM_CHANNELS * 2 * (NUM_OVERSAMPLES+1);
CLOCK_SPEED = 500000 / ( NUM_CHANNELS * (NUM_OVERSAMPLES+1) );




deviceIndex = 0
result = AIOUSB.AIOUSB_Init()
if result != AIOUSB::AIOUSB_SUCCESS
  STDERR.puts "Error running AIOUSB_Init()..."
  exit(1)
end

deviceMask = AIOUSB.GetDevices()
if deviceMask == 0
    STDERR.puts "No ACCES devices found on USB bus\n"
    sys.exit(1)
end

devices = []
number_devices = 1
index = 0

while deviceMask > 0 and devices.length < number_devices
    if deviceMask & 1  != 0
      obj = AIOUSB.AIODeviceInfoGet( index )
      if obj.PID == AIOUSB::USB_AIO16_16A or obj.PID == AIOUSB::USB_DIO_16A
          devices.push( Device.new( "index"=>index, "productID"=>obj.PID, "numDIOBytes"=>obj.DIOBytes,"numCounters"=>obj.Counters ))
      end
    end
    index += 1
    deviceMask >>= 1
end

AIOUSB.AIOUSB_Reset( deviceIndex );
print "Setting timeout"
AIOUSB.AIOUSB_SetCommTimeout( deviceIndex, 1000 );
AIOUSB.AIOUSB_SetDiscardFirstSample( deviceIndex, AIOUSB::AIOUSB_TRUE );


serialNumber = 0

serialNumber = AIOUSB.new_ulp()
result = AIOUSB.GetDeviceSerialNumber( deviceIndex, serialNumber );
puts sprintf "Serial number of device at index %d: %x", deviceIndex, AIOUSB.ulp_value( serialNumber )


result = 0
ndevice,result = AIOUSB.AIODeviceTableGetDeviceAtIndex( deviceIndex , result )

cb = AIOUSB.AIOUSBDeviceGetADCConfigBlock( ndevice )

AIOUSB.AIOUSB_SetAllGainCodeAndDiffMode( cb, AIOUSB::AD_GAIN_CODE_10V, AIOUSB::AIOUSB_FALSE );
AIOUSB.AIOUSB_SetCalMode( cb, AIOUSB::AD_CAL_MODE_NORMAL );
AIOUSB.AIOUSB_SetTriggerMode( cb, 0 );
AIOUSB.AIOUSB_SetScanRange( cb, 2, 13 );
AIOUSB.AIOUSB_SetOversample( cb, 0 );

AIOUSB.ADC_WriteADConfigBlock( deviceIndex, cb )


puts "A/D settings successfully configured"


retval = AIOUSB.ADC_SetCal(deviceIndex, ":AUTO:")
if result != AIOUSB::AIOUSB_SUCCESS
    puts sprintf "Error '%s' performing automatic A/D calibration" , AIOUSB. AIOUSB_GetResultCodeAsString( result )
    sys.exit(0)
end

AIOUSB.ADC_SetOversample( deviceIndex, 0 );
AIOUSB.ADC_SetScanLimits( deviceIndex, CAL_CHANNEL, CAL_CHANNEL );
AIOUSB.ADC_ADMode( deviceIndex, 0 , AIOUSB::AD_CAL_MODE_GROUND );

counts = AIOUSB.new_ushortarray( 16 )
result = AIOUSB.ADC_GetScan( deviceIndex, counts );

if result != AIOUSB::AIOUSB_SUCCESS
    puts sprintf "Error '%s' attempting to read ground counts\n" , AIOUSB.AIOUSB_GetResultCodeAsString( result )
else
    puts sprintf  "Ground counts = %u (should be approx. 0)" , AIOUSB.ushort_getitem( counts, CAL_CHANNEL) 
end


AIOUSB.ADC_ADMode( deviceIndex, 0 , AIOUSB::AD_CAL_MODE_REFERENCE ) # TriggerMode
result = AIOUSB.ADC_GetScan( deviceIndex, counts );
if result != AIOUSB::AIOUSB_SUCCESS
    puts sprintf "Error '%s' attempting to read reference counts" , AIOUSB_GetResultCodeAsString( result ) 
else
    puts sprintf "Reference counts = %u (should be approx. 65130)", AIOUSB.ushort_getitem( counts, CAL_CHANNEL )
end
gainCodes = (1..16).to_a.collect { |i| 0 } 


# 
# demonstrate scanning channels and measuring voltages
#
for channel in 0..15
    gainCodes[channel] = AIOUSB::AD_GAIN_CODE_0_10V
end

#
# NEeds to be fixed here
#
AIOUSB.ADC_RangeAll( deviceIndex , gainCodes, AIOUSB::AIOUSB_TRUE )
AIOUSB.ADC_SetOversample( deviceIndex, NUM_OVERSAMPLES )
AIOUSB.ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 )
AIOUSB.ADC_ADMode( deviceIndex, 0 , AIOUSB::AD_CAL_MODE_NORMAL )

print "Volts read:"

volts = (1..16).to_a.map { |i| 0 }
for i in (1..1)
    result = AIOUSB.ADC_GetScanV( deviceIndex, volts );
    for j in (0..volts.length-1)
       puts sprintf "  Channel %2d = %6.6f" ,  j, result[j]
    end
end


# demonstrate reading a single channel in volts
result = AIOUSB.ADC_GetChannelV( deviceIndex, CAL_CHANNEL, volts[ CAL_CHANNEL ] );

puts sprintf "Result from A/D channel %d was %f " , CAL_CHANNEL, result[0]
result = AIOUSB.ADC_GetChannelV( deviceIndex, 0 , volts[ CAL_CHANNEL ] );
puts sprintf "Result from A/D channel %d was %f " ,  0 , result[0] 


# 
# demonstrate bulk acquire
# 

AIOUSB.AIOUSB_Reset( deviceIndex );
AIOUSB.ADC_SetOversample( deviceIndex, NUM_OVERSAMPLES );
AIOUSB.ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );

AIOUSB.AIOUSB_SetStreamingBlockSize( deviceIndex, 64*1024 );


puts sprintf("Allocating %d Bytes" , ( BULK_BYTES ))
databuf = AIOUSB.new_ushortarray( BULK_BYTES )


clockHz = 0;
AIOUSB.CTR_StartOutputFreq( deviceIndex, 0, clockHz );


# 
#  configure A/D for timer-triggered acquisition
#
AIOUSB.ADC_ADMode( deviceIndex, AIOUSB::AD_TRIGGER_SCAN | AIOUSB::AD_TRIGGER_TIMER, AIOUSB::AD_CAL_MODE_NORMAL );



# 
# start bulk acquire; ADC_BulkAcquire() will take care of starting
# and stopping the counter; but we do have to tell it what clock
# speed to use, which is why we call AIOUSB_SetMiscClock()
# 
puts sprintf("Using Clock speed %d to acquire data",( CLOCK_SPEED ))
AIOUSB.AIOUSB_SetMiscClock( deviceIndex, CLOCK_SPEED );

result = AIOUSB.ADC_BulkAcquire( deviceIndex, BULK_BYTES, databuf );

if result != AIOUSB::AIOUSB_SUCCESS
    puts sprintf( "Error '%s' attempting to start bulk acquire of %d bytes\n", AIOUSB.AIOUSB_GetResultCodeAsString( result ), BULK_BYTES )
    exit(1)
else
    puts sprintf( "Started bulk acquire of %d bytes" ,  BULK_BYTES )
end

bytesRemaining = AIOUSB.new_ulp()
AIOUSB.ulp_assign( bytesRemaining, BULK_BYTES )

for i in (0..60)
    sleep(1)
    result = AIOUSB.ADC_BulkPoll( deviceIndex, bytesRemaining )
    if result != AIOUSB::AIOUSB_SUCCESS
        puts sprintf("Error '%s' polling bulk acquire progress", AIOUSB_GetResultCodeAsString( result ) )
    else
        puts sprintf( "  %d bytes remaining", ( AIOUSB.ulp_value( bytesRemaining )))
        if AIOUSB.ulp_value(bytesRemaining) == 0
            break
        end
    end
end



__END__

