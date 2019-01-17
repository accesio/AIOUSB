#ifndef LIBAIOUSB_H
#define LIBAIOUSB_H


#include <stdint.h>
typedef unsigned long AIORESULT;
typedef int64_t AIORET_TYPE;

#define MATLAB

/* only needed because of mexFunction below and mexPrintf */
#include "AIOUSB_Core.h"
#include "AIOChannelMask.h"
#include "AIOContinuousBuffer.h"
#include "AIOTypes.h"
#include "DIOBuf.h"
#include "AIODeviceInfo.h"
#include "AIODeviceQuery.h"
#include "AIOCommandLine.h"
#include "AIOUSB_Properties.h"


/* #include <pthread.h> */

#include <mex.h>  

