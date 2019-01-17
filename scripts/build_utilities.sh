#!/bin/bash

BUILD_LOG_TIME_FORMAT='+<%m/%d/%Y %H:%M:%S>'

export CPATH=/usr/include/libusb-1.0:$CPATH

function LOG_TIME
{
    echo $(date "${BUILD_LOG_TIME_FORMAT}")
}

function build_cmd {
    echo "# $(LOG_TIME) $@" | tee -a ${BUILD_LOG};
    eval $@ 2>&1 | tee -a ${BUILD_LOG};
    if [ ${PIPESTATUS[0]} != 0  ] ; then
        echo "$(LOG_TIME) Error with '$@'"
        exit 1
    fi
}


function build_cd {
    echo "# $(LOG_TIME) cd $@" | tee -a ${BUILD_LOG};
    cd $@
}


function start_build {
    echo "# $(LOG_TIME)  Starting build" | tee -a ${BUILD_LOG}
}
