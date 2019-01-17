#!/bin/bash

function dynmake() {
    make $1 CFLAGS=" -I${AIO_LIB_DIR} " LDFLAGS="-L${AIO_LIB_DIR}" "$@" DEBUG=1
}

function run_cmd () { 
    echo "Running '$1'"
    eval $1
    if [ "$?" != "0" ] ; then
        echo "Error running '${1}'"
        exit 1
    fi
}

dynmake burst_test
dynmake continuous_mode
dynmake read_channels_test 
dynmake bulk_acquire_sample
dynmake continuous_mode_callback                                                                            


run_cmd "./burst_test -V --num_scans 20000 --range 0-5=2 -N 20000 -D 31 --clockrate 4000"
result=$(r -e 'tmp<-read.csv("output.txt",header=F); cat(if(max(tmp$V2) > 56000) { "ok" } else { "not ok" } )')
if [[ $result == "not ok" ]] ; then
    echo "ERROR: max value not ok at 1"
    exit 1
fi

result=$(r -e 'tmp<-read.csv("output.txt",header=F); cat(if(min(tmp$V1) < 200) { "ok" } else { "not ok" } )')
if [[ $result == "not ok" ]] ; then
    echo "ERROR: min value not ok at 2"
    exit 1
fi

run_cmd "./continuous_mode  -V  --range 0-4=0 -N 20000 -D 31 --clockrate 4000"
result=$(r -e 'tmp<-read.csv("output.txt",header=F); cat(if(max(tmp$V1) > 4.4) { "ok" } else { "not ok" } )')
if [ "$result" == "not ok" ] ; then
    echo "ERROR: max value not ok"
    exit 1
fi
result=$(r -e 'tmp<-read.csv("output.txt",header=F); cat(if(min(tmp$V1) < 0.3) { "ok" } else { "not ok" } )')
if [ "${result}" == "not ok" ] ; then
    echo "ERROR: min value not ok"
    exit 1
fi

run_cmd "bulk_acquire_sample | tee output.txt"
if [ "$?" != "0" ] ; then
    echo "Bulk acquire error"
    exit 1
fi

run_cmd "./read_channels_test -N 1000 > output.txt"
perl -i -ne 'if ( s/^(\d+.*)$/$1/g ) { print; }' output.txt
result=$(r -e 'tmp<-read.csv("output.txt",header=F); cat(if(max(tmp$V2) > 4.4) { "ok" } else { "not ok" } )')
if [ "${result}" == "not ok" ] ; then
    echo "ERROR: max value not ok"
    exit 1
fi

result=$(r -e 'tmp<-read.csv("output.txt",header=F); cat(if(min(tmp$V2) < 0.3) { "ok" } else { "not ok" } )')
if [ "${result}" == "not ok" ] ; then
    echo "ERROR: min value not ok"
    exit 1
fi

run_cmd "./continuous_mode_callback -V -N 16301 --range 0-3=2 -D 31 -S 200000 --clockrate 4000"
result=$(r -e 'tmp<-read.csv("output.txt",header=F); cat(if(max(tmp$V2) > 4.4) { "ok" } else { "not ok" } )')
if [[ "${result}" == "not ok" ]] ; then
    echo "ERROR: max value not ok"
    exit 1
fi

result=$(r -e 'tmp<-read.csv("output.txt",header=F); cat(if(min(tmp$V2) < 0.3) { "ok" } else { "not ok" } )')
if [[ "${result}" == "not ok" ]] ; then
    echo "ERROR: min value not ok"
    exit 1
fi

