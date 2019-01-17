#!/usr/bin/perl

use Test::More tests => 7;
use Cwd;

my $startdir= getcwd();
#
foreach my $language (qw(perl python ruby octave php java R) ) {

    $cmd = "make -f GNUMakefile inplace_${language} 2>&1 "  ;
    $got = `$cmd 2>&1`;
    like($got, qr{(No ACCES devices found|ACCES devices found.*\n\s*Device at index\s+\d+.*\n\s*Product ID:\s+\S+\n)}m,  "Testing language $language");

}


