#ifndef TEST_CASE_SETUP_H
#define TEST_CASE_SETUP_H

#include <aiousb.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include "AIOUSB_Core.h"

using namespace AIOUSB;

const int MAX_NAME_SIZE = 20;
const int DEF_MAX_CHANNELS = 128;
const int DEF_NUM_CHANNELS = 16;
const int DEF_CAL_CHANNEL  = 5;


#define ERROR_LEVEL 2<<1
#define FATAL_LEVEL 2<<1
#define ALERT_LEVEL 2<<2
#define WARN_LEVEL 2<<3
#define INFO_LEVEL 2<<4 
#define DEBUG_LEVEL 2<<5
#define TRACE_LEVEL 2<<6

extern int CURRENT_DEBUG_LEVEL;


#define LOG(X,...)   printf(X, ##__VA_ARGS__);
#define INFO(X,...)  if( CURRENT_DEBUG_LEVEL & INFO_LEVEL ) \
                         printf(X, ##__VA_ARGS__);

#define TRACE(X,...) if( CURRENT_DEBUG_LEVEL & TRACE_LEVEL ) \
                         printf(X, ##__VA_ARGS__ );

#define DEBUG(X,...) if( CURRENT_DEBUG_LEVEL & DEBUG_LEVEL ) \
                         printf(X, ##__VA_ARGS__ );

#undef ERROR
#define ERROR(X,...) if( CURRENT_DEBUG_LEVEL & ERROR_LEVEL ) \
                         printf(X, ##__VA_ARGS__ );

#define FATAL(X,...) if( CURRENT_DEBUG_LEVEL & ERROR_LEVEL ) \
                         printf(X, ##__VA_ARGS__ );


#define TERSE_LOGGING    ( WARN_LEVEL | ERROR_LEVEL | INFO_LEVEL )
#define VERBOSE_LOGGING  ( DEBUG_LEVEL | INFO_LEVEL | WARN_LEVEL | ERROR_LEVEL )

#define THROW_ERROR(x)  ThrowError( x , __LINE__ )
#define CHECK_RESULT(x) if( result != AIOUSB_SUCCESS ) ThrowError(result,__LINE__);



class Error : std::exception
{
public:
  explicit Error();
  Error(const char *entry) : message( entry ) {}
  //Error(const std::string entry) : message( entry ) {}
  virtual const char* what() const throw() { return message; }
  //virtual const char * what() const throw () { return message.c_str(); }
  
 private:
  const char *message;
  //std::string  message;
};


class TestCaseSetup {

 public:
  
  ~TestCaseSetup();
  TestCaseSetup();
  TestCaseSetup(int deviceIndex, int numChannels );
  void findDevice();
  void findDevice( AIOUSB_BOOL (*find)( AIOUSBDevice *dev ) );
  void doSomething();
  void setCurrentDeviceIndex( int DeviceIndex );
  void doBulkConfigBlock();
  void doPreSetup();
  void doSetAutoCalibration();
  void doVerifyGroundCalibration();
  void doVerifyReferenceCalibration();
  void doDemonstrateReadVoltages();
  void doScanSingleChannel();
  void doPreReadImmediateVoltages();
  void doCSVReadVoltages();
  void doCSVWithGetChannelV();
  void doCleanupAfterBulk();
  void doDACDirect(int channel, unsigned short voltage);
  void doDACDirectSetup();
  void writeBuffer( char *filename );
  void setMaxCount( int val );

  void ThrowError(unsigned long, int);

  void doFastITScanSetup();
  void doFastITScan( int numgets );


  unsigned short *doGetBuffer();

  static void THROW_IF_ERROR(int result, const char *format, ... );
  static int envGetInteger(const char *env);
  static double envGetDouble(const char *env);

  
  void doTestSetAutoCalibration();
  void doGenericVendorWrite(unsigned char Request, unsigned short Value, unsigned short Index, unsigned long *DataSize, void *pData);

  void doBulkAcquire();
  void doBulkAcquire(unsigned int block_size, unsigned int over_sample, unsigned int clock_speed );
  void doDisplayBulkResults();

  void resetCPU();

  double *getVolts();
  unsigned short *getCounts();
  unsigned char *getGainCodes();
  

  unsigned long productID, nameSize, numDIOBytes, numCounters;
  unsigned long DeviceIndex;
  bool deviceFound;
  const int CAL_CHANNEL  ;
  const int MAX_CHANNELS ;
  int NUM_CHANNELS ;
  // Dynamic parameters
  unsigned short *counts;
  double *volts;
  unsigned char *gainCodes;
  ADConfigBlock configBlock;

  unsigned int number_oversamples;
  unsigned int block_size;
  unsigned int clock_speed;
  int maxcounts;
  AIOUSB_BOOL calibration_enabled;


 private:
  void setupVoltageParameters(void);
  unsigned long TEST_ADC_BulkPoll( unsigned long DeviceIndex, unsigned long *BytesLeft );
  unsigned short  *dataBuf;


};


#endif
