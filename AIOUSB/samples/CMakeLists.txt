#
# Build samples for USB-DA12-8A/USB-DA12-8E
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if ( NOT THIS_PROJECT ) 
   message(FATAL_ERROR "Expected THIS_PROJECT to be defined, not '${THIS_PROJECT}'")
endif( NOT THIS_PROJECT )


INCLUDE( build_samples )
include_testcase_lib( ${THIS_PROJECT} )
build_all_samples( ${THIS_PROJECT} )
