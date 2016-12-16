//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware erodefilter filtering on captured video
//

#ifndef __ERODEFILTER_DRIVER_H__
#define __ERODEFILTER_DRIVER_H__


#include "ipconfig.h"

// all the hardware related info needed by the driver functions should be provided in this struct
typedef struct {
    unsigned int baseaddr;
    unsigned int intr_id;
    bool busy;
} erodefilterIP_DriverInstance;


// API for SobelfilterIP_Driver to use if this rule is applied by GRIP
// NOTE: if any of the functions requires runtime info then their prototype should be matched with _SW() functions
void erodefilterIP_Driver_intrInitialize(erodefilterIP_DriverInstance *InstancePtr, IntCntrl_t *InterruptController);
void erodefilterIP_Driver_initialize(erodefilterIP_DriverInstance *InstancePtr, erodefilterIP_RegMap InitMode);
void erodefilterIP_Driver_start(erodefilterIP_DriverInstance *InstancePtr, erodefilterIP_RegMap StartMode);
void erodefilterIP_Driver_stop(erodefilterIP_DriverInstance *InstancePtr, erodefilterIP_RegMap StopMode);
bool erodefilterIP_Driver_isBusy(erodefilterIP_DriverInstance *InstancePtr);


// NOTE: for now _stop(), _isBusy() are not expected to take in any realtime info from SW so they have no arguments but if they need it in future then they should have same arguments as that of _initialize(), _start() methods to make the job of code-generator easier!!


#endif
