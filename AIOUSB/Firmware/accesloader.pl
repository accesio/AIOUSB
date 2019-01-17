#!/usr/bin/perl
#
# $RCSfile: accesloader.pl,v $
# $Revision: 1.8 $
# $Date: 2009/10/13 20:09:55 $
# :tabSize=4:indentSize=4:collapseFolds=1:
#

use strict;
use warnings;
use feature "switch";

my $testString;
my @lines;
my @parameters;
my $currentLine;
my $vendID;
my $prodID;
my $hexFile;
my $command;
my $pauseForRenum;
my $userTest;
my $vendorID = "1605";
my $deviceDir = "/dev/bus/usb/";
my $firmwareDir = "/usr/share/usb/";

print "This script will upload the appropriate firmware to any ACCES USB devices that are found\n";
print "on the system. If firmware is uploaded to any devices, then the script will pause for five\n";
print "seconds before attempting to make all ACCES USB devices on the system usable by users other\n";
print "than root. This script must be run with root privileges.\n";

$pauseForRenum = 0;
$testString = "lsusb -d " . $vendorID . ":";
@lines = split /\n/, `$testString`;

foreach $currentLine ( @lines ) {
	@parameters = split ' ', $currentLine;
	chop( $parameters[ 3 ] );					# is in the form xxx: and we just want xxx
	( $vendID, $prodID ) = split ':', $parameters[ 5 ], 2;

	given( $prodID ) {
		when( "0001" ) { $hexFile = "USB-DIO-32.hex" }			# Model: USB-DIO-32
		when( "0002" ) { $hexFile = "USB-DIO-48.hex" }			# USB-DIO-48
		when( "0003" ) { $hexFile = "USB-DIO-96.hex" }			# USB-DIO-96
#		when( "0008" ) { $hexFile = "USB-DI16A Rev A1.hex" }	# USB-DI16A Rev A1
#		when( "0009" ) { $hexFile = "USB-DO16A Rev A1.hex" }	# USB-DO16A Rev A1
#		when( "000a" ) { $hexFile = "USB-DI16A Rev A2.hex" }	# USB-DI16A Rev A2
		when( "000c" ) { $hexFile = "USB-DIO-16A.hex" }			# USB-DIO-16H Rev B and newer
		when( "000d" ) { $hexFile = "USB-DIO-16A.hex" }			# USB-DI16A Rev B and newer
		when( "000e" ) { $hexFile = "USB-DIO-16A.hex" }			# USB-DO16A Rev B and newer
		when( "000f" ) { $hexFile = "USB-DIO-16A.hex" }			# USB-DIO-16A Rev B and newer
		when( "0010" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-IIRO-16
		when( "0011" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-II-16
		when( "0012" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-RO-16
		when( "0014" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-IIRO-8
		when( "0015" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-II-8
		when( "0016" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-IIRO-4
		when( "0018" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-IDIO-16
#		when( "0019" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-II-16
		when( "001a" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-IDO-16
		when( "001c" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-IDIO-8
#		when( "001d" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-II-8
		when( "001e" ) { $hexFile = "USB-IDIO-16.hex" }			# USB-IDIO-4
		when( "0020" ) { $hexFile = "USB-CTR-15.hex" }			# USB-CTR-15
		when( "0030" ) { $hexFile = "USB-IIRO4-2SM.hex" }		# USB-IIRO4-2SM
		when( "0032" ) { $hexFile = "USBP-DIO16RO8.hex" }		# USBP-DIO16RO8
		when( "0033" ) { $hexFile = "PICO-DIO16RO8.hex" }		# PICO-DIO16RO8
		when( "0040" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-16A
		when( "0041" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-16E
		when( "0042" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-16A
		when( "0043" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-16
		when( "0044" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-16E
		when( "0045" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-64MA
		when( "0046" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-64ME
		when( "0047" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-64MA
		when( "0048" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-64M
		when( "0049" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-64ME
		when( "004a" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-32A
		when( "004b" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-32E
		when( "004c" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-32A
		when( "004d" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-32
		when( "004e" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-32E
		when( "004f" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-64A
		when( "0050" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-64E
		when( "0051" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-64A
		when( "0052" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-64
		when( "0053" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-64E
		when( "0054" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-96A
		when( "0055" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-96E
		when( "0056" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-96A
		when( "0057" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-96
		when( "0058" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-96E
		when( "0059" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-128A
		when( "005a" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI16-128E
		when( "005b" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-128A
		when( "005c" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-128
		when( "005d" ) { $hexFile = "USB-AI16-16A.hex" }			# USB-AI12-128E
		when( "0140" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-16A
		when( "0141" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-16E
		when( "0142" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-16A
		when( "0143" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-16
		when( "0144" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-16E
		when( "0145" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-64MA
		when( "0146" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-64ME
		when( "0147" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-64MA
		when( "0148" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-64M
		when( "0149" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-64ME
		when( "014a" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-32A
		when( "014b" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-32E
		when( "014c" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-32A
		when( "014d" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-32
		when( "014e" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-32E
		when( "014f" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-64A
		when( "0150" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-64E
		when( "0151" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-64A
		when( "0152" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-64
		when( "0153" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-64E
		when( "0154" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-96A
		when( "0155" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-96E
		when( "0156" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-96A
		when( "0157" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-96
		when( "0158" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-96E
		when( "0159" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-128A
		when( "015a" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO16-128E
		when( "015b" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-128A
		when( "015c" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-128
		when( "015d" ) { $hexFile = "USB-AIO16-16A.hex" }			# USB-AIO12-128E
		when( "0070" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO16-16A
		when( "0071" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO16-16
		when( "0072" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO16-12A
		when( "0073" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO16-12
		when( "0074" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO16-8A
		when( "0075" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO16-8
		when( "0076" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO16-4A
		when( "0077" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO16-4
		when( "0078" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO12-16A
		when( "0079" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO12-16
		when( "007a" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO12-12A
		when( "007b" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO12-12
		when( "007c" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO12-8A
		when( "007d" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO12-8
		when( "007e" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO12-4A
		when( "007f" ) { $hexFile = "USB-AO16-16.hex" }			# USB-AO12-4
#		when( "4001" ) { $hexFile = "USB-DA128-ArevA.hex" }		# USB-DA12-8A (Rev A)
		when( "4002" ) { $hexFile = "USB-DA12-8A.hex" }			# USB-DA12-8A
		when( "4003" ) { $hexFile = "USB-DA12-8E.hex" }			# USB-DA12-8E
#		when( "8031" ) { $hexFile = "USB-IIRO4-COM.hex" }		# USB-IIRO4-COM (Xerox)
		default {
			next
		}
	}	# given( ...

	$pauseForRenum = 1;
	$command = "fxload -t fx2lp -D " . $deviceDir . $parameters[ 1 ] . "/" . $parameters[ 3 ] . " -I " . $firmwareDir . $hexFile;
	print $command . "\n";
	system( $command );
}	# foreach $currentLine( ...

#
# at this point we have programmed any raw devices that were attached to the system; now we want
# to make the files usable without being root; on the test system it actually took this long for
# renumeration to work
#

if( $pauseForRenum == 1 ) {
	sleep 5
}

@lines = split /\n/, `$testString`;

foreach $currentLine ( @lines ) {
	@parameters = split ' ', $currentLine;
	chop( $parameters[ 3 ] );					# is in the form xxx: and we just want xxx
	( $vendID, $prodID ) = split ':', $parameters[ 5 ], 2;
	$command = "chmod 0666 " . $deviceDir . $parameters[ 1 ] . "/" . $parameters[ 3 ];
	print $command . "\n";
	system( $command );
}	# foreach $currentLine( ...

# end of file
