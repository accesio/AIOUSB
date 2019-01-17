#!/usr/bin/perl



sub myencode {
    my ($line) = @_;
    $line =~ s/\b\s*$//g;
    $line =~ s/\s/_/g;
    return $line;
}

if ( $ARGV[0] =~ /(README|Testing).md/ ) {

    while (<>) {
        if ( $. == 5 ) {
            print "[TOC]\n\n";
        } elsif ( /^\s*(#{1,8})\s*<a name="([^"]+)"><\/a>\s*(\S+.*)$/ ) {
            chomp;
            my ($hashes,$anchor,$rest) = ($1,$2,$3);
            $rest =~ s/<a href="[^"]+">(.*?)<\s*\/a\s*>/$1/g;
            print $hashes . " " . $rest . " {#" . $anchor . "}\n";
        } else {
            print;
        }
    }
} else {

    while (<>) {
        print;
    }
}


