#!/bin/bash

BUILD_LOG=$PWD/BUILD_LOG.txt
source scripts/build_utilities.sh

cd AIOUSB
source sourceme.sh

build_cd $AIO_LIB_DIR/wrappers
build_cmd prove tests/run_all_wrappers.t
build_cmd echo "Build Completed"
