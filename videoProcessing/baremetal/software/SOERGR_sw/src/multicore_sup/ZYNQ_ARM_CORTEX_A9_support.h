

// this module contains code that is used to initialize and activate ARM CortexA9 cores on Zynq chip

// the master cpu writes (SoCProc * i.e descriptors to each processor to be used in the design from _config module) for each of the individual core to a seperate address in shared memory starting from 0xffff0000
// each individual core would then be linked to its descriptor and will use it while processing

#ifndef ZYNQ_ARM_CORTEX_A9_SUPPORT_H_
#define ZYNQ_ARM_CORTEX_A9_SUPPORT_H_

#include "SoCProc_support_config.h"             // to read in all the Proc descriptors..if a descriptor is found to be of this core's type then link it to a particular core using shared memory location


// to share info between master cpu and these cores
#define SHARED_MEM_BASEADDR 0xffff0000                // for 1st instance of this Proc type
// and so on if platform has other processors of this type like
// #define SHARED_MEM_BASEADDR 0xffff0004 etc



// API functions that will be run by MasterCPU
void ZYNQ_ARM_CORTEX_A9_initialize();           // the master cpu will go over all the descriptors in _config module looking for processor of this type...if it finds one then it will associate that descriptor to a specific processor and boot it up
unsigned char ZYNQ_ARM_CORTEX_A9_process();     // find out a free processing core of this type and execute the RTC dataflow function provided by application developer..if no free processor found then return 0


// internal functions that will be run by core of this type
void Powerup();
void mainProcess();




//NOTE: get the function pointer prototype from application developer
extern void processFrame(unsigned int dataMemBaseAddr);




#endif /* ZYNQ_ARM_CORTEX_A9_SUPPORT_H_ */

