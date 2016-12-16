//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware GRIP Subsystem_Rule1 filtering on captured video
//


#include "function.h"
//#include "profile_cnt.h"


// driver instances for Subsystem_Rule1

Subsystem_Rule1_DriverInstance Subsystem_Rule1_Driver[Subsystem_Rule1_NUM_INSTANCES];

void Subsystems_Rule1_driverInstanceInit() {
    int i;
	for (i = 0; i<Subsystem_Rule1_NUM_INSTANCES; i++) {
		Subsystem_Rule1_Driver[i]._sobelfilterIP_DriverInstance.baseaddr = Subsystem_Rule1_SOBELFILTERIP_BASEADDR;
		Subsystem_Rule1_Driver[i]._sobelfilterIP_DriverInstance.intr_id = Subsystem_Rule1_SOBELFILTERIP_INTRID;
		Subsystem_Rule1_Driver[i]._sobelfilterIP_DriverInstance.busy = 0;
		Subsystem_Rule1_Driver[i]._vdmaIP_DriverInstance.baseaddr = Subsystem_Rule1_VDMAIP_BASEADDR;
		Subsystem_Rule1_Driver[i]._vdmaIP_DriverInstance.intr_id = Subsystem_Rule1_VDMAIP_INTRID;
		Subsystem_Rule1_Driver[i]._vdmaIP_DriverInstance.busy = 0;
    	}
}



void Subsystems_Rule1_intrInitialize(IntCntrl_t *InterruptController) {
    int i;
	for (i = 0; i<Subsystem_Rule1_NUM_INSTANCES; i++) {
		Subsystem_Rule1_intrInitialize(&Subsystem_Rule1_Driver[i], InterruptController);
	}
}



void Subsystems_Rule1_initialize() {
	int i;
	for (i = 0; i<Subsystem_Rule1_NUM_INSTANCES; i++) {
		Subsystem_Rule1_initialize(&Subsystem_Rule1_Driver[i], Subsystem_Rule1_InitMode);
	}
}



void Subsystems_Rule1_Init(IntCntrl_t *InterruptController) {
    Subsystems_Rule1_driverInstanceInit();
    Subsystems_Rule1_intrInitialize(InterruptController);
    Subsystems_Rule1_initialize();
}


int Subsystem_Rule1_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {

	#if Subsystem_Rule1_NUM_INSTANCES == 0
    	return 0;
	#else

    int idx;    
    // seeing if Subsystem_Rule1 filtering for current captured frame can be done on this Rule system    
    for (idx = 0; idx<Subsystem_Rule1_NUM_INSTANCES; idx++) {
        if (Subsystem_Rule1_isBusy(&Subsystem_Rule1_Driver[idx]) == 0) {
        	Subsystem_Rule1_start(&Subsystem_Rule1_Driver[idx], Subsystem_Rule1_StartMode);
        	//double cpu_cycles = get_cyclecount();
        	//printf("CPU cycles for Sobel Start HW: %f \n", cpu_cycles);
        	// Munish Commented while(Subsystem_Rule1_isBusy(&Subsystem_Rule1_Driver[idx]) == 1);
            return 1;
        }
    }    
    
    return 0;
#endif
}
