#!/bin/bash


./ctr15 -C 0=5  -C 1=60 -C 2=50.0 -C 3=500   -C 4=400000 

#and 

PYTHONPATH=${AIO_LIB_DIR}/wrappers/python/build/lib.linux-x86_64-2.7  python2.7 ctr15.py  --channel 0=1000 --channel 1=232323

