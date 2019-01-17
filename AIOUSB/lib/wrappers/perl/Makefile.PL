use ExtUtils::MakeMaker;
WriteMakefile(
    "NAME"       => "AIOUSB",    # Name of module
    "LIBS"       => ["-L$ENV{AIOUSB_ROOT}/lib  -lusb-1.0 -lpthread -laiousbdbg "],
    "CCFLAGS"      => "-L$ENV{AIOUSB_ROOT}/classlib",
    "INC"        => "-I$ENV{AIOUSB_ROOT}/lib -I$ENV{AIOUSB_ROOT}/classlib -I/usr/include/libusb-1.0/",
    "OBJECT" => "AIOUSB_wrap.o"
   );
