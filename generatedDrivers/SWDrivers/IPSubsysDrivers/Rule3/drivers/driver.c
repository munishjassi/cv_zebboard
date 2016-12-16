//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware GRIP Rule3 filtering on captured video
//


#include "driver.h"

#ifndef __CONFIG_H__
#include "config.h"
#endif

void Subsystem_Rule3_intrInitialize(Subsystem_Rule3_DriverInstance *InstancePtr, IntCntrl_t *InterruptController){

  	// Interrupt initializing the grayfilterIP
	grayfilterIP_Driver_intrInitialize(&InstancePtr->_grayfilterIP_DriverInstance, InterruptController);
  	// Interrupt initializing the vdmaIP
	vdmaIP_Driver_intrInitialize(&InstancePtr->_vdmaIP_DriverInstance, InterruptController);

}



void Subsystem_Rule3_initialize(Subsystem_Rule3_DriverInstance *InstancePtr, Subsystem_Rule3_RegMap Subsystem_InitMode){

	//
	// Custom Code
	//

    	// initializing the grayfilterIP
    	grayfilterIP_Driver_initialize(&InstancePtr->_grayfilterIP_DriverInstance, Subsystem_InitMode._grayfilterIP_RegMap);
    	// initializing the vdmaIP
    	vdmaIP_Driver_initialize(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_InitMode._vdmaIP_RegMap);
  
  	//
	// Custom Code
	//
}



void Subsystem_Rule3_start(Subsystem_Rule3_DriverInstance *InstancePtr, Subsystem_Rule3_RegMap Subsystem_Rule3_StartMode) {

	//
	// Custom Code
	//
	
	// starting the grayfilterIP
	grayfilterIP_Driver_start(&InstancePtr->_grayfilterIP_DriverInstance, Subsystem_Rule3_StartMode._grayfilterIP_RegMap);
	// starting the vdmaIP
	vdmaIP_Driver_start(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_Rule3_StartMode._vdmaIP_RegMap);
	

	//
	// Custom code
	//
}



void Subsystem_Rule3_stop(Subsystem_Rule3_DriverInstance *InstancePtr, Subsystem_Rule3_RegMap Subsystem_Rule3_StopMode) {

	//
	// Custom Code
	//
	
	// stopping grayfilterIP
	grayfilterIP_Driver_stop(&InstancePtr->_grayfilterIP_DriverInstance, Subsystem_Rule3_StopMode._grayfilterIP_RegMap);
	// stopping vdmaIP
	vdmaIP_Driver_stop(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_Rule3_StopMode._vdmaIP_RegMap);
  
  	//
	// Custom Code
	//
}



bool Subsystem_Rule3_isBusy(Subsystem_Rule3_DriverInstance *InstancePtr) {
	//
	// Custom code
	//
	return VDMAIP_Driver_isBusy(&InstancePtr->_vdmaIP_DriverInstance);
}