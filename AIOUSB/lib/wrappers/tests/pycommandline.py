#!/usr/bin/python

import sys
from AIOUSB import *

sys.argv = ['/usr/bin/ipython','--foobar','3434','--blah','-N','344','-S','3434','--foobar']

opts = NewAIOCommandLineOptionsFromDefaultOptions( AIO_SCRIPTING_OPTIONS() )

print "before: %s " % ( sys.argv )

AIOProcessCommandLine( opts, sys.argv )

print "after: %s " % ( sys.argv )


AIOProcessCommandLine( opts, sys.argv )

print "after: %s " % ( sys.argv )
sys.argv.append("-N")
sys.argv.append("3434")
sys.argv.append("-S")
sys.argv.append("3434")


print "before again: %s " % ( sys.argv )
AIOProcessCommandLine( opts, sys.argv )

print "after again: %s " % ( sys.argv )




