// this file is going to be auto-generated after reading in the IP_SW and all of IP_Rule*Driver modules
// it will use the configuration flags provided in config file
// right now assuming that the code generator is given the info from Grayscale IP supplier of using 2 grip rules

// this file provides API functionality to application developer to perform HA-aware grayscale filtering on captured video


#ifndef __GRAYSCALEIP_FUNC_H__
#define __GRAYSCALEIP_FUNC_H__


#include "../driver/Rule1/GrayscaleIP_Rule1Driver.h"
#include "../driver/Rule2/GrayscaleIP_Rule2Driver.h"
// and so on if other grip rules are supported by this IP

#include "../config/GrayscaleIP_config.h"
#include "GrayscaleIP_SW.h"

#include "xscugic.h"

#define bool unsigned char


// API
void GrayscaleIP_func_init(XScuGic *InterruptController, unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);
void ConvToGray_func(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);


// internal stuff

// driver instances for Grayscale_HA
#if GRAYSCALEIP_NUM_GRIPRULE1_INSTANCES != 0
    GrayscaleIPRule1DriverInstance GrayscaleIPRule1Driver[GRAYSCALEIP_NUM_GRIPRULE1_INSTANCES];
#endif

#if GRAYSCALEIP_NUM_GRIPRULE2_INSTANCES != 0
    GrayscaleIPRule2DriverInstance GrayscaleIPRule2Driver[GRAYSCALEIP_NUM_GRIPRULE2_INSTANCES];
#endif

// and so on if other grip rules are supported by this IP

#endif



















