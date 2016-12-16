//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization


// this is autogenerated by reading in the register information from the IPXact description of the system

#ifndef __VDMA_REGMAP_H__
#define __VDMA_REGMAP_H__

#include "../platform/common.h"



typedef struct {
	RegType MM2S_DMACR;
	RegType S2MM_DMACR;
	RegType S2MM_DMASR;
	RegType MM2S_VSIZE;
	RegType MM2S_HSIZE;
	RegType MM2S_FRMDLY_STRIDE;
	RegType MM2S_START_ADDRESS1;
	RegType MM2S_START_ADDRESS2;
	RegType MM2S_START_ADDRESS3;
	RegType S2MM_VSIZE;
	RegType S2MM_HSIZE;
	RegType S2MM_FRMDLY_STRIDE;
	RegType S2MM_START_ADDRESS1;
	RegType S2MM_START_ADDRESS2;
	RegType S2MM_START_ADDRESS3;
} vdmaIP_RegMap;



#endif
