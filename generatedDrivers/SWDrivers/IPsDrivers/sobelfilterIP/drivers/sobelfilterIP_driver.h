//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware sobelfilter filtering on captured video
//

#ifndef __SOBELFILTER_DRIVER_H__
#define __SOBELFILTER_DRIVER_H__


#include "ipconfig.h"

// all the hardware related info needed by the driver functions should be provided in this struct
typedef struct {
    unsigned int baseaddr;
    unsigned int intr_id;
    bool busy;
} sobelfilterIP_DriverInstance;


// API for SobelfilterIP_Driver to use if this rule is applied by GRIP
// NOTE: if any of the functions requires runtime info then their prototype should be matched with _SW() functions
void sobelfilterIP_Driver_intrInitialize(sobelfilterIP_DriverInstance *InstancePtr, IntCntrl_t *InterruptController);
void sobelfilterIP_Driver_initialize(sobelfilterIP_DriverInstance *InstancePtr, sobelfilterIP_RegMap InitMode);
void sobelfilterIP_Driver_start(sobelfilterIP_DriverInstance *InstancePtr, sobelfilterIP_RegMap StartMode);
void sobelfilterIP_Driver_stop(sobelfilterIP_DriverInstance *InstancePtr, sobelfilterIP_RegMap StopMode);
bool sobelfilterIP_Driver_isBusy(sobelfilterIP_DriverInstance *InstancePtr);


// NOTE: for now _stop(), _isBusy() are not expected to take in any realtime info from SW so they have no arguments but if they need it in future then they should have same arguments as that of _initialize(), _start() methods to make the job of code-generator easier!!


#endif
