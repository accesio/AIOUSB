#
# A Simple package for calling something
#
#
package Filepp;

sub between
{
    my (%args) = @_;
    # print "args were %args\n";
    $cmd = "between.pl " . join(" ",map { "$_ $args{$_}" } keys %args);
    $,="\n";
    print "\/* #include \"$args{-f}\" */\n\n";
    # print "Keys: ",keys %args,"\n";
    # print "$cmd\n";
    system($cmd);
    print "\n";
}

sub verbatim(@)
{
    my (@args) = @_;
    # print "BLAH: @_\n";
    # my $arg1 = $args[0];
    # print "ARGS: $#args\nARGS: $arg1\n";

    foreach my $i ( @args ) {
        # print "$i\n";
        $i =~ s/\"//g;
        # print "$i\n";
        if ( -f "$i" ) {
            system("cat $i");
        }
    }
}


1;
