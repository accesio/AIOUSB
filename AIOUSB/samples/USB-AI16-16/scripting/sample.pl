#!/usr/bin/perl

use AIOUSB;

{
    package Device;

    sub new(@) {
        my ($self,%args) = @_;
        bless \%args,Device;
    }

}


my $deviceIndex = 0;
my $MAX_NAME_SIZE = 20;

my $deviceIndex = 0;
my $deviceFound = $AIOUSB::AIOUSB_FALSE;

my $CAL_CHANNEL = 5;
my $MAX_CHANNELS = 128;
my $NUM_CHANNELS = 16;
my $NUM_OVERSAMPLES = 10;
my $NUM_SCANS = 100000;
my $BULK_BYTES = $NUM_SCANS * $NUM_CHANNELS * 2 * ($NUM_OVERSAMPLES+1);
my $CLOCK_SPEED = 500000 / ( $NUM_CHANNELS * ($NUM_OVERSAMPLES+1) );


AIOUSB::AIOUSB_Init();
AIOUSB::AIOUSB_Reset( $deviceIndex );
AIOUSB::AIOUSB_SetCommTimeout( $deviceIndex, 1000 );
AIOUSB::AIOUSB_SetDiscardFirstSample( $deviceIndex, $AIOUSB::AIOUSB_TRUE );

$deviceIndex = 0;
$result = AIOUSB::AIOUSB_Init();
if ( $result != AIOUSB::AIOUSB_SUCCESS ) {
    print STDERR "Error running AIOUSB_Init()...";
    exit(1);
}

$deviceMask = AIOUSB::GetDevices();
if( $deviceMask == 0 ) {
    print STDERR "No ACCES devices found on USB bus\n";
    exit(1);
}

$devices = [];
$number_devices = 1;
AIOUSB::AIOUSB_ListDevices();
$index = 0;

while( $deviceMask > 0 && $#{$devices} < $number_devices ) { 
    if( $deviceMask & 1 ) {
        my $obj = AIOUSB::AIODeviceInfoGet( $index );

        if ( ($obj->swig_PID_get() >= $AIOUSB::USB_AI16_16A && $obj->swig_PID_get() <= $AIOUSB::USB_AI12_128E) ||
             ($obj->swig_PID_get() >= $AIOUSB::USB_AIO16_16A &&  $obj->swig_PID_get() <= $AIOUSB::USB_AIO12_128E )) {
            push( @{$devices}, new Device( "index" => $index, "productID"=>$obj->swig_PID_get(), "numDIOBytes"=>$obj->swig_DIOBytes_get,"numCounters" => $obj->swig_Counters_get ));
            print "";
        }
    }
    $index += 1;
    $deviceMask >>= 1;
}

print "Size of devices is $#{$devices}\n";


$deviceIndex = $devices->[0]->{index};


AIOUSB::AIOUSB_Reset( $deviceIndex );
print "Setting timeout\n";
AIOUSB::AIOUSB_SetCommTimeout( $deviceIndex, 1000 );
AIOUSB::AIOUSB_SetDiscardFirstSample( $deviceIndex, $AIOUSB::AIOUSB_TRUE );

$result = 0;
($ndevice,$result) = AIOUSB::AIODeviceTableGetDeviceAtIndex( $deviceIndex );
$cb = AIOUSB::AIOUSBDeviceGetADCConfigBlock( $ndevice );
print "";


AIOUSB::AIOUSB_SetAllGainCodeAndDiffMode( $cb, $AIOUSB::AD_GAIN_CODE_10V, $AIOUSB::AIOUSB_FALSE );
AIOUSB::ADCConfigBlockSetCalMode( $cb, $AIOUSB::AD_CAL_MODE_NORMAL );
AIOUSB::ADCConfigBlockSetTriggerMode( $cb, 0 );
AIOUSB::ADCConfigBlockSetScanRange( $cb, 2, 13 );
AIOUSB::ADCConfigBlockSetOversample( $cb, 0 );



AIOUSB::AIOUSBDeviceWriteADCConfig( $ndevice, $cb );

print "A/D settings successfully configured\n";



$retval = AIOUSB::ADC_SetCal($deviceIndex, ":AUTO:");
if( $result != $AIOUSB::AIOUSB_SUCCESS ) {
    print "Error '%s' performing automatic A/D calibration" % ( AIOUSB_GetResultCodeAsString( result ) );
    sys.exit(0);
}

AIOUSB::ADC_SetOversample( $deviceIndex, 0 );
AIOUSB::ADC_SetScanLimits( $deviceIndex, $CAL_CHANNEL, $CAL_CHANNEL );
AIOUSB::ADC_ADMode( $deviceIndex, 0 , $AIOUSB::AD_CAL_MODE_GROUND );

$counts = AIOUSB::ushortarray->new( 16 );
$result = AIOUSB::ADC_GetScan( $deviceIndex, $counts->cast() );

if( $result < $AIOUSB::AIOUSB_SUCCESS ) {
    print sprintf "Error '%s' attempting to read ground counts\n" , AIOUSB::AIOUSB_GetResultCodeAsString( $result );
} else {
    print sprintf "Ground counts = %u (should be approx. 0)\n" , $counts->{$CAL_CHANNEL} ;
}


AIOUSB::ADC_ADMode( $deviceIndex, 0 , $AIOUSB::AD_CAL_MODE_REFERENCE ); # TriggerMode
$result = AIOUSB::ADC_GetScan( $deviceIndex, $counts->cast() );
if( result < AIOUSB_SUCCESS ) { 
    print sprintf "Error '%s' attempting to read reference counts\n" , AIOUSB_GetResultCodeAsString( result );
} else {
    print sprintf "Reference counts = %u (should be approx. 65130)\n", $counts->{$CAL_CHANNEL};
}

$gainCodes = [ map { 0 } 1..16 ];

#
# demonstrate scanning channels and measuring voltages
#

for($i = 0; $i < 16; $i ++ ) { 
    $gainCodes->[$i] = $AIOUSB::AD_GAIN_CODE_0_10V;
}

AIOUSB::ADC_RangeAll( $deviceIndex , $gainCodes, $AIOUSB::AIOUSB_TRUE );

AIOUSB::ADC_SetOversample( $deviceIndex, $NUM_OVERSAMPLES );
AIOUSB::ADC_SetScanLimits( $deviceIndex, 0, $NUM_CHANNELS - 1 );
AIOUSB::ADC_ADMode( $deviceIndex, 0 , $AD_CAL_MODE_NORMAL );



print "Volts read:\n";

if ( 1 ) { 
# $volts = [map { 0 } 1..16 ];
$volts = AIOUSB::doublearray->new(16);
for( $i = 0; $i < 1 ; $i ++ ) {
    $result = AIOUSB::ADC_GetScanV( $deviceIndex, $volts->cast() );
    for( $j = 0; $j < 16 ; $j ++ ) {
        print sprintf "  Channel %2d = %6.6f\n" , $j, $volts->{$j};
    }
}


}

# demonstrate reading a single channel in volts
$voltage = AIOUSB::new_udp();
$result = AIOUSB::ADC_GetChannelV( $deviceIndex, $CAL_CHANNEL, $voltage );

print sprintf("Result from A/D channel %d was %f\n" , $CAL_CHANNEL, AIOUSB::udp_value($voltage) );


$result = AIOUSB::ADC_GetChannelV( $deviceIndex, 1, $voltage );

print sprintf("Result from A/D channel %d was %f\n", 1, AIOUSB::udp_value($voltage) );


AIOUSB::AIOUSB_Reset( $deviceIndex );
AIOUSB::AIOUSB_Exit();



__END__

