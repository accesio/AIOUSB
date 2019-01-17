/*
 * sample program to write out the calibration table and then 
 * reload it again, verify that the data is in fact reversed
 * 
 */

#include <aiousb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "TestCaseSetup.h"


using namespace AIOUSB;

extern int CURRENT_DEBUG_LEVEL;


int main( int argc, char **argv ) 
{
  CURRENT_DEBUG_LEVEL = VERBOSE_LOGGING;
  unsigned long result = AIOUSB_Init();
  int block_size;
  int over_sample;
  int numsamples;
  double clock_speed;
  
  block_size  = TestCaseSetup::envGetInteger("BLOCK_SIZE");
  over_sample = TestCaseSetup::envGetInteger("OVER_SAMPLE");
  clock_speed = TestCaseSetup::envGetDouble("CLOCK_SPEED");
  numsamples     = TestCaseSetup::envGetInteger("NUM_SAMPLES");
  numsamples = ( numsamples <= 0 ? 200 : numsamples );

  if( result == AIOUSB_SUCCESS ) {
          
    unsigned long deviceMask = GetDevices();
    if( deviceMask != 0 ) {
      // at least one ACCES device detected, but we want one of a specific type
      TestCaseSetup tcs;
      try { 
        tcs.findDevice();
        tcs.setCurrentDeviceIndex(0);

        tcs.doFastITScanSetup();
        tcs.doFastITScan( numsamples );
        tcs.doFastITScan( numsamples );

        tcs.doGetBuffer();
        tcs.doCleanupAfterBulk();

        std::cout << block_size << "," << over_sample << "," << clock_speed << "," << "Passed" << std::endl;
      } catch ( Error &e  ) {
        std::cout << e.what() << std::endl;
        std::cout << block_size << "," << over_sample << "," << clock_speed << "," << "Failed" << std::endl;
      }
    }
  }
}

