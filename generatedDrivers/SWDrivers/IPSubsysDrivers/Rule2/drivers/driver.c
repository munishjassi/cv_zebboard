//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware GRIP Rule2 filtering on captured video
//


#include "driver.h"

#ifndef __CONFIG_H__
#include "config.h"
#endif

void Subsystem_Rule2_intrInitialize(Subsystem_Rule2_DriverInstance *InstancePtr, IntCntrl_t *InterruptController){

  	// Interrupt initializing the erodefilterIP
	erodefilterIP_Driver_intrInitialize(&InstancePtr->_erodefilterIP_DriverInstance, InterruptController);
  	// Interrupt initializing the vdmaIP
	vdmaIP_Driver_intrInitialize(&InstancePtr->_vdmaIP_DriverInstance, InterruptController);

}



void Subsystem_Rule2_initialize(Subsystem_Rule2_DriverInstance *InstancePtr, Subsystem_Rule2_RegMap Subsystem_InitMode){

	//
	// Custom Code
	//

    	// initializing the erodefilterIP
    	erodefilterIP_Driver_initialize(&InstancePtr->_erodefilterIP_DriverInstance, Subsystem_InitMode._erodefilterIP_RegMap);
    	// initializing the vdmaIP
    	vdmaIP_Driver_initialize(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_InitMode._vdmaIP_RegMap);
  
  	//
	// Custom Code
	//
}



void Subsystem_Rule2_start(Subsystem_Rule2_DriverInstance *InstancePtr, Subsystem_Rule2_RegMap Subsystem_Rule2_StartMode) {

	//
	// Custom Code
	//
	
	// starting the erodefilterIP
	erodefilterIP_Driver_start(&InstancePtr->_erodefilterIP_DriverInstance, Subsystem_Rule2_StartMode._erodefilterIP_RegMap);
	// starting the vdmaIP
	vdmaIP_Driver_start(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_Rule2_StartMode._vdmaIP_RegMap);
	

	//
	// Custom code
	//
}



void Subsystem_Rule2_stop(Subsystem_Rule2_DriverInstance *InstancePtr, Subsystem_Rule2_RegMap Subsystem_Rule2_StopMode) {

	//
	// Custom Code
	//
	
	// stopping erodefilterIP
	erodefilterIP_Driver_stop(&InstancePtr->_erodefilterIP_DriverInstance, Subsystem_Rule2_StopMode._erodefilterIP_RegMap);
	// stopping vdmaIP
	vdmaIP_Driver_stop(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_Rule2_StopMode._vdmaIP_RegMap);
  
  	//
	// Custom Code
	//
}



bool Subsystem_Rule2_isBusy(Subsystem_Rule2_DriverInstance *InstancePtr) {
	//
	// Custom code
	//
	return VDMAIP_Driver_isBusy(&InstancePtr->_vdmaIP_DriverInstance);
}