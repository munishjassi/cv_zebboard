//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware GRIP Subsystem_Rule3 filtering on captured video
//


#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "config.h"


void Subsystems_Rule3_driverInstanceInit();
void Subsystems_Rule3_intrInitialize(IntCntrl_t *InterruptController);
void Subsystems_Rule3_initialize();
void Subsystems_Rule3_Init(IntCntrl_t *InterruptController);
int Subsystem_Rule3_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);


#endif
