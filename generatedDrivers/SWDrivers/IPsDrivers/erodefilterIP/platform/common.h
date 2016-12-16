//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// contains common types/structures that are to be used by various IP drivers
//

#ifndef __IP_COMMON_H__
#define __IP_COMMON_H__

#include "platform_config.h"

#define bool unsigned char


#if PLATFORM == XILINX_FPGA
    #include "xil_io.h"
    #include "xscugic.h"
    typedef XScuGic IntCntrl_t;
// #elsif for other platforms
#endif


typedef struct {
#if PLATFORM_WORD_SIZE == 32
    unsigned int offset;
    unsigned int mask;
    unsigned int value;
// #elsif for other wordsizes    
#endif
} RegType;


void erodefilter_RegWrite(unsigned int addr, unsigned int mask, unsigned int value);
unsigned int erodefilterIP_RegRead(unsigned int addr);


#endif