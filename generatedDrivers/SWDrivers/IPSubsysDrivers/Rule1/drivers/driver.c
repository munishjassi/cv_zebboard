//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware GRIP Rule1 filtering on captured video
//


#include "driver.h"

#ifndef __CONFIG_H__
#include "config.h"
#endif

void Subsystem_Rule1_intrInitialize(Subsystem_Rule1_DriverInstance *InstancePtr, IntCntrl_t *InterruptController){

  	// Interrupt initializing the sobelfilterIP
	sobelfilterIP_Driver_intrInitialize(&InstancePtr->_sobelfilterIP_DriverInstance, InterruptController);
  	// Interrupt initializing the vdmaIP
	vdmaIP_Driver_intrInitialize(&InstancePtr->_vdmaIP_DriverInstance, InterruptController);

}



void Subsystem_Rule1_initialize(Subsystem_Rule1_DriverInstance *InstancePtr, Subsystem_Rule1_RegMap Subsystem_InitMode){

	//
	// Custom Code
	//

    	// initializing the sobelfilterIP
    	sobelfilterIP_Driver_initialize(&InstancePtr->_sobelfilterIP_DriverInstance, Subsystem_InitMode._sobelfilterIP_RegMap);
    	// initializing the vdmaIP
    	vdmaIP_Driver_initialize(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_InitMode._vdmaIP_RegMap);
  
  	//
	// Custom Code
	//
}



void Subsystem_Rule1_start(Subsystem_Rule1_DriverInstance *InstancePtr, Subsystem_Rule1_RegMap Subsystem_Rule1_StartMode) {

	//
	// Custom Code
	//
	
	// starting the sobelfilterIP
	sobelfilterIP_Driver_start(&InstancePtr->_sobelfilterIP_DriverInstance, Subsystem_Rule1_StartMode._sobelfilterIP_RegMap);
	// starting the vdmaIP
	vdmaIP_Driver_start(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_Rule1_StartMode._vdmaIP_RegMap);
	

	//
	// Custom code
	//
}



void Subsystem_Rule1_stop(Subsystem_Rule1_DriverInstance *InstancePtr, Subsystem_Rule1_RegMap Subsystem_Rule1_StopMode) {

	//
	// Custom Code
	//
	
	// stopping sobelfilterIP
	sobelfilterIP_Driver_stop(&InstancePtr->_sobelfilterIP_DriverInstance, Subsystem_Rule1_StopMode._sobelfilterIP_RegMap);
	// stopping vdmaIP
	vdmaIP_Driver_stop(&InstancePtr->_vdmaIP_DriverInstance, Subsystem_Rule1_StopMode._vdmaIP_RegMap);
  
  	//
	// Custom Code
	//
}



bool Subsystem_Rule1_isBusy(Subsystem_Rule1_DriverInstance *InstancePtr) {
	//
	// Custom code
	//
	return VDMAIP_Driver_isBusy(&InstancePtr->_vdmaIP_DriverInstance);
}