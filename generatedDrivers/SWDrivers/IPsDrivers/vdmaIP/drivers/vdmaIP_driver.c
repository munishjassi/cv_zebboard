//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization

// this file provides API functionality to application developer to perform HA-aware vdma filtering on captured video
//

#include "vdmaIP_driver.h"

// updates the vdma IP peripheral's memory mapped registers with content provided in mode 
static void SetHAMode(vdmaIP_RegMap *mode, unsigned int baseaddr) {
	// pointing to 1st register in passed regmap
	RegType *iter_ptr = (RegType *) mode;
	// getting the number of the registers in the passed reg-map
	int reg_in_regmap = sizeof(vdmaIP_RegMap)/sizeof(RegType);
	// iterating over all registers in the passed regmap
	int i=0;
	for (i=0; i<reg_in_regmap; i++) {
		vdmaIP_RegWrite(baseaddr + iter_ptr->offset, iter_ptr->mask, iter_ptr->value);
		iter_ptr++;
	}
}



//TODO: should this ISR function be here or inside Rule0?????????
static void vdmaIP_Driver_ISR(void *baseaddr_p) {
	vdmaIP_DriverInstance *InstancePtr = (vdmaIP_DriverInstance *) baseaddr_p;
	InstancePtr->busy = 0;
}



void vdmaIP_Driver_intrInitialize(vdmaIP_DriverInstance *InstancePtr, IntCntrl_t *InterruptController) {

	// Interrupt: registering this IP's ISR with the Interrupt Controller passed on by the application developer
	int Status = XScuGic_Connect(InterruptController, InstancePtr->intr_id, (Xil_ExceptionHandler) vdmaIP_Driver_ISR, (void *) InstancePtr);

	XScuGic_Enable(InterruptController, InstancePtr->intr_id);

	if (Status != XST_SUCCESS) {
		Xil_AssertVoid(0);
	}
}


void vdmaIP_Driver_initialize(vdmaIP_DriverInstance *InstancePtr, vdmaIP_RegMap InitMode) {
	// Registers Initialization
	SetHAMode(&InitMode, InstancePtr->baseaddr);
	InstancePtr->busy = 0;
}



void vdmaIP_Driver_start(vdmaIP_DriverInstance *InstancePtr, vdmaIP_RegMap StartMode) {
    SetHAMode(&StartMode, InstancePtr->baseaddr);
    InstancePtr->busy = 1;
}



void vdmaIP_Driver_stop(vdmaIP_DriverInstance *InstancePtr, vdmaIP_RegMap StopMode) {
	SetHAMode(&StopMode, InstancePtr->baseaddr);
}



bool vdmaIP_Driver_isBusy(vdmaIP_DriverInstance *InstancePtr) {
	return InstancePtr->busy;
}