#!/usr/bin/python

from __future__ import print_function
from AIOUSB import *
from string import split

import math
import sys

usage = """
Usage: 
       ctr15.py [ --rapiddisplay | -D ] ( --channel=<path>)... 

Options:
       -h, --help              Show the help screen
       --rapiddisplay      Update the counters

Try: 
        ctr15.py -D
        ctr15.py --channel ./here --channel ./there
        ctr15.py (-h | --help | --version)

"""

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def print_usage(args):
    print("%s :  -c CHANNEL_NUMBER=CLOCK_HZ [ -c CHANNEL_NUMBER=CLOCK_HZ ...]\n", args[0] );


def frequency_to_counter( frequency ):
    return int(math.sqrt( 10000000 / frequency ))

def find_idio(obj):
    if obj.PID == USB_CTR_15:
        return True



def main(args):
    channel_frequencies = [0.0,0.0,0.0,0.0,0.0]

    if len(sys.argv) == 1:
        eprint("You must specify an option")
        print(usage)

    AIOUSB_ListDevices();
    indices = AIOUSB_FindDevices( find_idio )
    num_devices = 1;
    if len(indices) <= 0:
        eprint("Unable to find a USB-CTR-16 device..exiting\n")
        sys.exit(1)
    else:
        print("Using device at index %d\n" % ( indices[0] ))

        
    # Adjust the parmeters
    for i in args["--channel"]:
        [channel,hz] = split(i,"=")
        channel = int(channel)
        hz = float(hz)

        if channel < 0 or channel > 5:
           eprint("Error: only Channels between 0 and 4" )
           sys.exit(1)
        channel_frequencies[channel] = float(hz)



    for i,val in enumerate(channel_frequencies):
        print("Channel %i is set to %.2f" % (i, channel_frequencies[i] ))

        
    for i in range(0,5):
        if channel_frequencies[i] == 0:
            counter_value = 62071
        else:
            counter_value = frequency_to_counter( channel_frequencies[i] )

        print("Setting channel %d counter to %d" % (i , counter_value ))
        CTR_8254ModeLoad( indices[0], i, 1, 2, counter_value )
        CTR_8254ModeLoad( indices[0], i, 2, 3, counter_value )

    if args["--rapiddisplay"] or args["-D"]:
        counts = ushortarray(15)
        count = 0
        print("Printing out counters")
        while count < 100000:
            CTR_8254ReadAll( indices[0] , counts.cast() );
            print("\r%5.5hu,%5.5hu,%5.5hu   %5.5hu,%5.5hu,%5.5hu   %5.5hu,%5.5hu,%5.5hu   %5.5hu,%5.5hu,%5.5hu    %5.5hu,%5.5hu,%5.5hu" % (counts[0], counts[1], counts[2], counts[3], counts[4], counts[5], counts[6], counts[7], counts[8], counts[9], counts[10], counts[11], counts[12], counts[13], counts[14] ), end="")
            count += 1
        print("")
        
   

if __name__ == "__main__":
    

    from docopt import *
    arguments = docopt(usage )
    main(arguments)

