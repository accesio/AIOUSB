#!/usr/bin/python

from __future__ import print_function
from AIOUSB import *
import sys
import itertools


result = AIOUSB_SUCCESS;

def main():
    ReadClockHz = 40000
    WriteClockHz = 0;

    result = AIOUSB_Init();
    if ( result < AIOUSB_SUCCESS ):
        fprintf(stderr,"Error running AIOUSB_Init() ...exiting\n");
        exit(2)

    AIOUSB_ListDevices();
    if ( len(sys.argv) < 3  or len(sys.argv) > 4 ):
        print("""
USB-DIO-16A test program , AIOUSB library version %s, %s.
This program demonstrates a simple high speed receiver program
where the device in question performs digital acquisition using
the DIO_StreamFrame() API call. It uses the device specified by the
HEX_SERIAL_NUMBER that the user specifies.

Usage: %s HEX_SERIAL_NUMBER FRAME_POINTS [CLOCKSPEED]
        
Example: %s  0x40e4bc6f1fffffff 512  30000
""" % ( AIOUSB_GetVersion(), AIOUSB_GetVersionDate(),sys.argv[0], sys.argv[0]))
        exit(2)
    
    serialNumber = int(sys.argv[1],16)
    framePoints = int(sys.argv[2])
    if (framePoints / 512) * 512 != framePoints:
        oldfp = framePoints
        framePoints = (framePoints / 512)*512
        print("\nRounding framePoints(%ld) to %ld ( multiple of 512 )\n" % ( oldfp, framePoints) )
                   
    if ( len(sys.argv) == 4 ):
        ReadClockHz = int(sys.argv[3])

    print("Using serialNumber=%x, clock %d\n" % ( serialNumber, ReadClockHz ))
    deviceIndex = GetDeviceBySerialNumber( serialNumber );
    deviceIndex = 0

    result = AIOUSB_SetCommTimeout( deviceIndex, 5000 );

    AIOUSB_SetStreamingBlockSize( deviceIndex, 512 );

    readClock  = new_udp(); udp_assign(readClock,ReadClockHz );
    writeClock = new_udp(); udp_assign(writeClock,0);
    tmpclock  = new_udp(); udp_assign( tmpclock,0 );
    
    # Stop the clocks
    result = DIO_StreamSetClocks( deviceIndex, tmpclock, tmpclock );
    if result != AIOUSB_SUCCESS:
        print("Error '%s' setting stream clocks\n" % ( AIOUSB_GetResultCodeAsString( result ) ))
        sys.exit(1)

    # AIOUSB_ClearFIFO( deviceIndex, CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT )


    outputMask = new_ucp(); ucp_assign(outputMask, 0x08 );
    initialData = new_ulp(); ulp_assign( initialData, 0xffffffff );
    tristateMask = new_ucp(); ucp_assign( tristateMask, 0x00 );


    result = DIO_ConfigureEx( deviceIndex, outputMask, initialData, tristateMask );
    if result < AIOUSB_SUCCESS:
        print("Error '%s' configuring device at index %lu\n" % ( AIOUSB_GetResultCodeAsString( result ), deviceIndex ))
        sys.exit(1)

    # open stream for reading
    result = DIO_StreamOpen( deviceIndex, AIOUSB_TRUE );
    if result < AIOUSB_SUCCESS:
        print("Error '%s' opening read stream for device at index %lu\n" % ( AIOUSB_GetResultCodeAsString( result ), deviceIndex ))
        sys.exit(1)


    transferred = new_ulp()
    ulp_assign(transferred, 0)

    # receive frame
    frameData = ushortarray(framePoints)

    # Start clocks
    result = DIO_StreamSetClocks( deviceIndex, readClock, writeClock );
    if result < AIOUSB_SUCCESS:
        print("Error '%s' setting stream clock for device at index %lu\n" % ( AIOUSB_GetResultCodeAsString( result ), deviceIndex ))
        sys.exit(1)

    result = DIO_StreamFrameTmp( deviceIndex, framePoints, frameData, transferred );


    if result == AIOUSB_SUCCESS:
        if ulp_value(transferred) != framePoints * 2:
            print("Received bytes (%ld) was not equal to what was expected (%ld)\n" % (
                    transferred, framePoints * 2 ))
            sys.exit(1)

        print("%lu FramePoints successfully read from device at index %lu\n" % (framePoints, deviceIndex ))

        fp = file("output.dat","w")
        for x in range(0,framePoints):
            fp.write( chr(frameData[x] & 0xff ) )
            fp.write(chr((frameData[x] & 0xff00)>>8))
        fp.close()
    else:
        print("""
Error running DIO_StreamFrame, result=%ld
%lu\n attempted to read %lu bytes, but actually read %lu bytes""" % (
               result,
               deviceIndex, 
               framePoints, 
               ulp_value(transferred)
               ))


    result = DIO_StreamClose( deviceIndex )
    print("Result: %d\n" % result )

    udp_assign( tmpclock,0 );
    result = DIO_StreamSetClocks( deviceIndex, tmpclock, tmpclock );
    AIOUSB_Exit()



if __name__ == '__main__':
  main()




