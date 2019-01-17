#!/usr/bin/perl

use URI::Encode qw(/\S+/);

open(FOO,">>foo");

sub myencode {
    my ($line) = @_;
    $line =~ s/\b\s*$//g;
    $line =~ s/\s/_/g;
    return $line;
}

if ( $ARGV[0] =~ /(README|Testing).md/ ) {
    print FOO "Ahh::: $ARGV[0]\n";
    while (<>) {
        if ( /^\s*(#{1,8})\s*<a name="([^"]+)"><\/a>\s*(\S+.*)$/ ) {
            chomp;
            #print "$1: $2 $3\n";
            #print FOO (("#" x $1 ) . " $3 " . "  { #$2 } \n" );
            # print (("#" x $1 ) . " $3 " . "  { #$2 } \n" );
            print $1 . " " . $3 . " {#" . $2 . "}\n";
        } else {
            print;
        }
    }
} else {
    print FOO "Ohhh::: $ARGV[0]\n";
    while (<>) {
        print;
    }
}

close(FOO);
