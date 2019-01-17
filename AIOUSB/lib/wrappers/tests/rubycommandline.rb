#!/usr/bin/env ruby

require 'AIOUSB'

ARGV=%w{/usr/bin/ipython --foobar 3434 --blah -N 344 -S 3434 --foobar};

opts = AIOUSB.NewAIOCommandLineOptionsFromDefaultOptions( AIOUSB.AIO_SCRIPTING_OPTIONS() );

print "before: #{ARGV}\n";

AIOUSB::AIOProcessCommandLine( opts, ARGV );


print "after : #{ARGV}\n";


AIOUSB.AIOProcessCommandLine( opts, ARGV )

print "after : #{ARGV}\n";

ARGV.push("-N","3434","-S","3434")

print "before 1 : #{ARGV}\n";

AIOUSB.AIOProcessCommandLine( opts, ARGV )

print "after  1 : #{ARGV}\n";

ARGV.push("-N","3434","-S","3434")

print "before 2 : #{ARGV}\n";


opts.ProcessCommandLine( ARGV )

print "after  2 : #{ARGV}\n";


