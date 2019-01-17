#!/usr/bin/python
# @file   sample.py
# @author $Format: %an <%ae>$
# @date   $Format: %ad$
# @release $Format: %t$
# @ingroup samples
# @brief Python version of DIO96 sample
#
#/

import sys
import time
from AIOUSB import *

MAX_DIO_BYTES = 32

devices = []                    # Array of our Devices
number_devices = 1

print """
USB-DIO-96 sample program version %s, %s 
This program demonstrates communicating using the USB-DIO-96 product.
AIOUSB library version %s, %s the same USB bus. It uses the 
first device found found on the bus
""" % ( "$Format: %t$", "$Format: %ad$", AIOUSB_GetVersion(), AIOUSB_GetVersionDate() )
         

def find_dio96(obj):
    if obj.PID == USB_DIO_96 :
        return True

indices = AIOUSB_FindDevices( find_dio96 )

if not indices:
    print """No USB-DIO-96 devices were found. Please make sure you have at least one 
ACCES I/O Products USB device plugged into your computer"""
    sys.exit(1)

device = indices[0]


# Ports 3 and 1 , all writable
mask = NewAIOChannelMaskFromStr("111000111000")

buf = DIOBuf(
"11111111" + 
"11111111" + 
"11111111" + 

"11011011" + # Group 1 Port A   00100100  corresponds to   11011011 Volts
"01010101" + #         Port B   10101010        "          01010101 Volts
"00001111" + #         Port C   11110000        "          00001111 Volts

"11111111" + 
"11111111" + 
"11111111" +

"11110000" + # Group 3 Port A   00001111        "          11110000 Volts
"11000011" + #         Port B   00111100        "          11000011 Volts
"00111100"   #         Port C   11000011        "          00111100 Volts
)

print "Buf is %s" % ( buf  )

DIO_ConfigureWithDIOBuf(device , AIOUSB_FALSE, mask, buf )

retval = DIO_ReadAllToDIOBuf(device, buf)
print "Retval was %d" % ( retval )

print "Read8: %s" % ( buf )

sys.exit(0)


for i in range(0,12):
    if i % 3 == 0:
        port = "A"
    elif i % 3 == 1:
        port = "B"
    else: 
        port = "C"

    print "Group %d Port %c:  Value: %s" % ( i / 3, port, "FOO" )

sys.exit(0)


