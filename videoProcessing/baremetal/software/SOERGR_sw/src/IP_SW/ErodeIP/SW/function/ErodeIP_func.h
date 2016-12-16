// this file is going to be auto-generated after reading in the IP_SW and all of IP_Rule*Driver modules
// it will use the configuration flags provided in config file

// this file provides API functionality to application developer to perform HA-aware grayscale filtering on captured video


#ifndef __ERODEIP_FUNC_H__
#define __ERODEIP_FUNC_H__


#include "../driver/Rule1/ErodeIP_Rule1Driver.h"
// and so on if other grip rules are supported by this IP

#include "../config/ErodeIP_config.h"
#include "ErodeIP_SW.h"

#include "xscugic.h"

#define bool unsigned char


// API
void ErodeIP_func_init(XScuGic *InterruptController, unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);
void Erode_func(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short vertcialActiveTime);


// internal stuff

// driver instances for Grayscale_HA
#if ERODEIP_NUM_GRIPRULE1_INSTANCES != 0
    ErodeIPRule1DriverInstance ErodeIPRule1Driver[ERODEIP_NUM_GRIPRULE1_INSTANCES];
#endif

// and so on if other grip rules are supported by this IP

#endif



















