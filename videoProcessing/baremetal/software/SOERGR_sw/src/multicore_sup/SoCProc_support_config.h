// this file is to generated (preferably auto-generated) by the GRIP tool for the architecture produced

#ifndef __SOCPROC_SUPPORT_CONFIG_H__
#define __SOCPROC_SUPPORT_CONFIG_H__


#define NUM_PROCS 2
// NOTE: for now assuming that both cores (Microblaze + ARM 2nd core) are going to be used in the architecture!

typedef enum {ZYNQ_ARM_CORTEX_A9, XIL_MICROBLAZE_8_5} ProcType;

typedef struct {
	ProcType type;
	unsigned char id;
	unsigned char busy;
	unsigned char halted;
	unsigned int MEMSPACE_BASEADDR;
} SoCProc;

// #if NUM_PROCS > 0
extern SoCProc SoCProc_elements[NUM_PROCS];





#endif









