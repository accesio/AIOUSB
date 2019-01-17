#!/bin/bash


export NUM_SCANS=50000
export RANGE_ZERO_TO_10VOLTS=0
export RANGE_PLUS_MINUS_2VOLTS=4
export NUM_OVERSAMPLES=1
export NUM_CHANNELS=16

export MAX_CLOCK_RATE=$(perl  -e 'print int (10_000_000 / ( ( $ENV{NUM_OVERSAMPLES}+1) * $ENV{NUM_CHANNELS})); ' )

./continuous_mode -V --num_scans $NUM_SCANS -O $NUM_OVERSAMPLES --range 0-11=$RANGE_ZERO_TO_10VOLTS  --range 12-15=$RANGE_PLUS_MINUS_2VOLTS  --clockrate $MAX_CLOCK_RATE
