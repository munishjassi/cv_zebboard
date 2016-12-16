//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization

// this file provides API functionality to application developer to perform HA-aware sobelfilter filtering on captured video
//

#include "sobelfilterIP_driver.h"

// updates the sobelfilter IP peripheral's memory mapped registers with content provided in mode 
static void SetHAMode(sobelfilterIP_RegMap *mode, unsigned int baseaddr) {
	// pointing to 1st register in passed regmap
	RegType *iter_ptr = (RegType *) mode;
	// getting the number of the registers in the passed reg-map
	int reg_in_regmap = sizeof(sobelfilterIP_RegMap)/sizeof(RegType);
	// iterating over all registers in the passed regmap
	int i=0;
	for (i=0; i<reg_in_regmap; i++) {
		sobelfilterIP_RegWrite(baseaddr + iter_ptr->offset, iter_ptr->mask, iter_ptr->value);
		iter_ptr++;
	}
}



//TODO: should this ISR function be here or inside Rule0?????????
static void sobelfilterIP_Driver_ISR(void *baseaddr_p) {
	sobelfilterIP_DriverInstance *InstancePtr = (sobelfilterIP_DriverInstance *) baseaddr_p;
	InstancePtr->busy = 0;
}



void sobelfilterIP_Driver_intrInitialize(sobelfilterIP_DriverInstance *InstancePtr, IntCntrl_t *InterruptController) {

	// Interrupt: registering this IP's ISR with the Interrupt Controller passed on by the application developer
	int Status = XScuGic_Connect(InterruptController, InstancePtr->intr_id, (Xil_ExceptionHandler) sobelfilterIP_Driver_ISR, (void *) InstancePtr);

	XScuGic_Enable(InterruptController, InstancePtr->intr_id);

	if (Status != XST_SUCCESS) {
		Xil_AssertVoid(0);
	}
}


void sobelfilterIP_Driver_initialize(sobelfilterIP_DriverInstance *InstancePtr, sobelfilterIP_RegMap InitMode) {
	// Registers Initialization
	SetHAMode(&InitMode, InstancePtr->baseaddr);
	InstancePtr->busy = 0;
}



void sobelfilterIP_Driver_start(sobelfilterIP_DriverInstance *InstancePtr, sobelfilterIP_RegMap StartMode) {
    SetHAMode(&StartMode, InstancePtr->baseaddr);
    InstancePtr->busy = 1;
}



void sobelfilterIP_Driver_stop(sobelfilterIP_DriverInstance *InstancePtr, sobelfilterIP_RegMap StopMode) {
	SetHAMode(&StopMode, InstancePtr->baseaddr);
}



bool sobelfilterIP_Driver_isBusy(sobelfilterIP_DriverInstance *InstancePtr) {
	return InstancePtr->busy;
}