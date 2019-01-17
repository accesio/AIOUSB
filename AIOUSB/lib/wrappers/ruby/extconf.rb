require 'mkmf'

$libs = append_library($libs, "usb-1.0")
$libs = append_library($libs, "pthread")
$libs = append_library($libs, "aiousb")

if ENV.has_key?("AIOUSB_ROOT") != true 
  STDERR.printf("Error, you must first source the sourceme.sh file: #{File.expand_path("../../sourceme.sh")}\n")
  exit(1)
end

find_library("usb-1.0","libusb_open",
             "/usr/local/lib/",
             "/usr/lib/#{`uname -p`.chomp!}-linux-gnu/",
             "/lib/#{`uname -p`.chomp!}-linux-gnu/"
             )


find_header("libusb.h",
            "/usr/include/libusb-1.0",
            "/usr/local/include/libusb-1.0"
            )


find_library("aiousb", "AIOUSB_Init",
             ENV["AIOUSB_ROOT"] + "/lib",
             ENV["AIOUSB_ROOT"] + "/classlib"
)


find_header("aiousb.h",
            ENV["AIO_LIB_DIR"],
            "/usr/include",
            "/usr/local/include"
            )

find_header("AIOUSB_Core.h",
            ENV["AIO_LIB_DIR"],
            "/usr/include",
            "/usr/local/include"
            )

find_header("ADCConfigBlock.h",
            ENV["AIO_LIB_DIR"],
            "/usr/include",
            "/usr/local/include"
            )

create_makefile('AIOUSB')

