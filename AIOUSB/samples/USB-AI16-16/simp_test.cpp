/*
 * sample program to write out the calibration table and then 
 * reload it again, verify that the data is in fact reversed
 * 
 *
 * 
 */

#include <aiousb.h>
#include <stdio.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "TestCaseSetup.h"
using namespace AIOUSB;

int main( int argc, char **argv ) {
  // printf("Sample test for Checking the Calibration on the board: %s, %s", AIOUSB_GetVersion(), AIOUSB_GetVersionDate());
  unsigned long result = AIOUSB_Init();

  if( result == AIOUSB_SUCCESS ) {
          
    unsigned long deviceMask = GetDevices();
    if( deviceMask != 0 ) {
      // at least one ACCES device detected, but we want one of a specific type
      AIOUSB_ListDevices();
      TestCaseSetup tcs;
      try { 
        tcs.findDevice();
        tcs.doPreSetup();
        tcs.doBulkConfigBlock();
        tcs.doSetAutoCalibration();
        tcs.doVerifyGroundCalibration();
        tcs.doVerifyReferenceCalibration();
        while(1) { 
          tcs.doCSVReadVoltages();
          usleep(10);
        }
      } catch ( Error &e  ) {
        std::cout << "Errors" << e.what() << std::endl;
      }
    }
  }
}

