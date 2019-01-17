#!/bin/bash


AIO_START_DIR=$PWD
BUILD_LOG=$PWD/BUILD_LOG.txt
source scripts/build_utilities.sh

cd AIOUSB
source sourceme.sh
export CPATH=/usr/include/libusb-1.0

build_cmd echo $AIO_LIB_DIR
build_cd $AIO_LIB_DIR
build_cmd echo $(pwd)
build_cmd make 
build_cd $AIO_CLASSLIB_DIR
build_cmd make 
build_cd $AIOUSB_ROOT
build_cmd doxygen 
build_cd $AIO_START_DIR
