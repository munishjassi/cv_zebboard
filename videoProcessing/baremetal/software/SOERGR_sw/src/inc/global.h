
// header containing the declarations for all the global stuff that is to be shared among several source files!!

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

// global macros
#define SHARED_OCM_MEMORY_BASE 0xFFFF0000
#define FRAME_SIZE 0x1000000

//#define USE_MULTICORE
#define DO_PERF_EVAL

#ifdef DO_PERF_EVAL
// for performance evaluation
#define TOTAL_FRAMES_FOR_STATS 1000
#define CPU_FREQ 666666666				// for normal mode, this is 667 MHz
#define CAMERA_FPS	15
#define INTER_FRAME_TIME  CPU_FREQ/CAMERA_FPS
#define CPU_LOAD CPU_FREQ
// for performance evaluation
#endif

#include "profile_cnt.h"
#include "../multicore_sup/SoCProc_support.h"

// global variables
short int FRAME_INTR;

#ifdef DO_PERF_EVAL
// for performance evaluation
unsigned int received_frame_count;
unsigned int processed_frame_count;
double cpu_cycles_for_frameProc;
// for performance evaluation
#endif

int h_ActiveTime;
int v_ActiveTime;

#endif

