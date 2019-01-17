#!/usr/bin/env python

import sys
import time
import math
import os
sys.path.append(os.environ["AIO_LIB_DIR"] + "/wrappers/python/build/lib.linux-x86_64-2.7")


import AIOUSB
from AIOUSB import *

class Device:
    name = ""
    serialNumber = 0
    index = 0
    numDIOBytes = 0
    numCounters = 0
    productID = 0
    def __init__(self, **kwds):
        self.__dict__.update(kwds)


MAX_NAME_SIZE = 20;

deviceIndex = 0;
deviceFound = AIOUSB_FALSE

CAL_CHANNEL = 5
MAX_CHANNELS = 128
NUM_CHANNELS = 16
NUM_OVERSAMPLES = 10
NUM_SCANS = 100000
BULK_BYTES = NUM_SCANS * NUM_CHANNELS * 2 * (NUM_OVERSAMPLES+1);
CLOCK_SPEED = 500000 / ( NUM_CHANNELS * (NUM_OVERSAMPLES+1) );

print """USB-AI16-16A sample program
AIOUSB library version %s, %s
This program demonstrates controlling a USB-AI16-16A device on
the USB bus. For simplicity, it uses the first such device found
on the bus.
""" % ( AIOUSB_GetVersion(), AIOUSB_GetVersionDate() )

def find_ai(obj):
    if (obj.PID >= USB_AI16_16A and obj.PID <= USB_AI12_128E) or \
       (obj.PID >=  USB_AIO16_16A and obj.PID <= USB_AIO12_128E ):
        return True

# Simplified API call that uses Python callback that takes 
# an AIOUSBDevice as it's argument .
# Typically, all you care about is the obj.PID matching 

options = NewAIOCommandLineOptionsFromDefaultOptions( AIO_SCRIPTING_OPTIONS() )


device_indices = []
#AIOUSB_FindDevicesByGroup( device_indices, AIOUSB.AIO_ANALOG_INPUT() )
AIOUSB_FindDeviceIndicesByGroup( device_indices, AIOUSB.AIO_ANALOG_INPUT() ) 

# or
# device_indices = AIOUSB_FindDevices( find_ai )

AIOProcessCommandLine( options, sys.argv )

if options.index == -1:
    deviceIndex = device_indices[0]
elif set(device_indices) & set([options.index]):
    deviceIndex = (set(device_indices) & set([options.index])).pop()
else:
    sys.stderr.write("Error: can't find a valid device (index=%d) != (found device indices=%s)\n" % ( options.index,device_indices ))
    sys.exit(1)
                    

AIOUSB_Reset( deviceIndex );
print "Setting timeout"
AIOUSB_SetCommTimeout( deviceIndex, 1000 );

AIOUSB_SetDiscardFirstSample( deviceIndex, AIOUSB_TRUE );

serialNumber = 0

[ndevice,result] = AIODeviceTableGetDeviceAtIndex( deviceIndex )

if result < AIOUSB_SUCCESS:
    print "Error: Can't get device\n"
    exit(1)


cb = AIOUSBDeviceGetADCConfigBlock( ndevice )

AIOUSB_SetAllGainCodeAndDiffMode( cb, AD_GAIN_CODE_10V, AIOUSB_FALSE );
ADCConfigBlockSetCalMode( cb, AD_CAL_MODE_NORMAL );
ADCConfigBlockSetTriggerMode( cb, 0 );
ADCConfigBlockSetScanRange( cb, 2, 13 );
ADCConfigBlockSetOversample( cb, 0 );


AIOUSBDeviceWriteADCConfig( ndevice, cb ); # Write the config block to the device

print "A/D settings successfully configured"

retval = 0
retval = ADC_SetCal(deviceIndex, ":AUTO:")

if retval < AIOUSB_SUCCESS:
    print "Error '%s' performing automatic A/D calibration" % ( AIOUSB_GetResultCodeAsString( retval ) )
    sys.exit(0)

ADCConfigBlockSetScanRange( cb, CAL_CHANNEL, CAL_CHANNEL )
ADCConfigBlockSetTriggerMode( cb , 0 )
ADCConfigBlockSetCalMode( cb , AD_CAL_MODE_GROUND )
AIOUSBDeviceWriteADCConfig( ndevice, cb )

# A better API is coming soon, so you won't have to do
# this to get Data
counts = new_ushortarray( 16 )
result = ADC_GetScan( deviceIndex, counts.cast() );

if retval < AIOUSB_SUCCESS:
    print "Error '%s' attempting to read ground counts\n" % ( AIOUSB_GetResultCodeAsString( result ) )
else:
    print "Ground counts = %u (should be approx. 0)" % ( counts[CAL_CHANNEL] )


ADC_ADMode( deviceIndex, 0 , AD_CAL_MODE_REFERENCE ) # TriggerMode
result = ADC_GetScan( deviceIndex, counts.cast() );
if result < AIOUSB_SUCCESS:
    print "Error '%s' attempting to read reference counts" % ( AIOUSB_GetResultCodeAsString( result ) )
else:
    print "Reference counts = %u (should be approx. 65130)" % ( counts[CAL_CHANNEL] )

gainCodes = [0 for x in range(0,16)]

# 
# demonstrate scanning channels and measuring voltages
# 
for channel in range(0,len(gainCodes)):
    gainCodes[channel] = AD_GAIN_CODE_0_10V


ADC_RangeAll( deviceIndex, gainCodes, AIOUSB_TRUE );


ADC_SetOversample( deviceIndex, NUM_OVERSAMPLES );
ADC_SetScanLimits( deviceIndex, 0, NUM_CHANNELS - 1 );
ADC_ADMode( deviceIndex, 0 , AD_CAL_MODE_NORMAL );

# volts = [0.0 for x in range(0,16)]
volts = doublearray(16)
for i in range(0,1):
    result = ADC_GetScanV( deviceIndex, volts.cast() );
    for j in range(0,16):
        print "  Channel %2d = %6.6f" % ( j, volts[j] )


voltage = new_udp()
result = ADC_GetChannelV( deviceIndex, CAL_CHANNEL, voltage );
print "Result from A/D channel %d was %s " % (CAL_CHANNEL, udp_value(voltage) )





