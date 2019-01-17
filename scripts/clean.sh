#!/bin/bash


BUILD_LOG=$PWD/BUILD_LOG.txt
source scripts/build_utilities.sh

cd AIOUSB
source sourceme.sh
export CPATH=/usr/include/libusb-1.0

build_cmd echo $AIO_LIB_DIR
build_cd $AIO_LIB_DIR
build_cmd echo $(pwd)
build_cmd make distclean
build_cd $AIO_CLASSLIB_DIR
build_cmd echo $(pwd)
build_cmd make distclean


