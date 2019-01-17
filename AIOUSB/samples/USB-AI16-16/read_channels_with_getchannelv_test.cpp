/*
 * sample program to write out the calibration table and then 
 * reload it again, verify that the data is in fact reversed
 */

#include <aiousb.h>
#include <stdio.h>
#include <unistd.h>
#include <exception>
#include <iostream>       
#include <unistd.h>
#include "TestCaseSetup.h"
using namespace AIOUSB;

struct options {
  int maxcount;
  int use_maxcount;
};

struct options get_options(int argc, char **argv );


int main( int argc, char **argv ) {
  // printf("Sample test for Checking the Calibration on the board: %s, %s", AIOUSB_GetVersion(), AIOUSB_GetVersionDate());
  unsigned long result = AIOUSB_Init();

  struct options opts = { 0, 0 };
  opts = get_options(argc, argv);

  if( result == AIOUSB_SUCCESS ) {
          
    unsigned long deviceMask = GetDevices();
    if( deviceMask != 0 ) {
      // at least one ACCES device detected, but we want one of a specific type
      // AIOUSB_ListDevices();
      TestCaseSetup tcs;
      try { 
        tcs.findDevice();
        tcs.setCurrentDeviceIndex(0);
        tcs.doPreSetup();
        tcs.doBulkConfigBlock();
        tcs.doSetAutoCalibration();
        tcs.doVerifyGroundCalibration();
        tcs.doVerifyReferenceCalibration();
        tcs.doPreReadImmediateVoltages();

        if( opts.use_maxcount ) 
          tcs.setMaxCount( opts.maxcount );

        // tcs.doCSVReadVoltages();
        tcs.doCSVWithGetChannelV();
        // while(1) { 
        //   tcs.doCSVReadVoltages();
        //   // usleep(0.1);
        //   counter ++;
        //   if( opts.use_maxcount ) 
        //     if( counter > opts.maxcount ) 
        //       break;
        // }
      } catch ( Error &e  ) {
        std::cout << "Errors" << e.what() << std::endl;
      }
    }
  }
}

//
// Gets the options 
//
struct options get_options(int argc, char **argv )
{
  int opt;

  struct options retoptions;

  while ((opt = getopt(argc, argv, "c:t")) != -1) {
    switch (opt) {
    case 'c':
      retoptions.maxcount     = atoi(optarg);
      retoptions.use_maxcount = 1;
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-c maxcounts]\n",
              argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  return retoptions;
}


