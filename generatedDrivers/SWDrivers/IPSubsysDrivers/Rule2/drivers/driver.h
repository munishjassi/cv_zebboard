//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware GRIP Rule2 filtering on captured video
//

#ifndef __RULE2_DRIVER_H__
#define __RULE2_DRIVER_H__

#include "../../.././IPsDrivers//erodefilterIP/drivers/erodefilterIP_driver.h"
#include "../../.././IPsDrivers//vdmaIP/drivers/vdmaIP_driver.h"

typedef struct {
    erodefilterIP_DriverInstance _erodefilterIP_DriverInstance;
    vdmaIP_DriverInstance _vdmaIP_DriverInstance;
} Subsystem_Rule2_DriverInstance;


typedef struct {
	erodefilterIP_RegMap _erodefilterIP_RegMap;
	vdmaIP_RegMap _vdmaIP_RegMap;
} Subsystem_Rule2_RegMap;



// API for SobelfilterIP_Driver to use if this rule is applied by GRIP
// NOTE: if any of the functions requires runtime info then their prototype should be matched with _SW() function..for multiple IPs having their own SW implementations then they all should be passed on in a specific order
// NOTE: template driver functions would have no SW argument...this is the responsibility of IP supplier to pass these arguments incase they are needed at runtime

void Subsystem_Rule2_intrInitialize(Subsystem_Rule2_DriverInstance *InstancePtr, IntCntrl_t *InterruptController);
void Subsystem_Rule2_initialize(Subsystem_Rule2_DriverInstance *InstancePtr, Subsystem_Rule2_RegMap Subsystem_InitMode);
void Subsystem_Rule2_start(Subsystem_Rule2_DriverInstance *InstancePtr, Subsystem_Rule2_RegMap Subsystem_Rule2_StartMode);
void Subsystem_Rule2_stop(Subsystem_Rule2_DriverInstance *InstancePtr, Subsystem_Rule2_RegMap Subsystem_Rule2_StopMode);
bool Subsystem_Rule2_isBusy(Subsystem_Rule2_DriverInstance *InstancePtr);

// NOTE: for now _stop(), _isBusy() are not expected to take in any realtime info from SW so they have no arguments but if they need it in future then they should have same arguments as that of _initialize(), _start() methods to make the job of code-generator easier!!


#endif

