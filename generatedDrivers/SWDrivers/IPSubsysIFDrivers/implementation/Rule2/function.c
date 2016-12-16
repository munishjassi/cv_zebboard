//
// GRIP Generated file: 2016-11-16T22:14:06Z
// GRIP: this module is auto-generated using Java Freemarker tool
// GRIP: Auto-generated via template engine and edit only for customization
//
// this file provides API functionality to application developer to perform HA-aware GRIP Subsystem_Rule2 filtering on captured video
//


#include "function.h"
//#include "profile_cnt.h"


// driver instances for Subsystem_Rule2

Subsystem_Rule2_DriverInstance Subsystem_Rule2_Driver[Subsystem_Rule2_NUM_INSTANCES];

void Subsystems_Rule2_driverInstanceInit() {
    int i;
	for (i = 0; i<Subsystem_Rule2_NUM_INSTANCES; i++) {
		Subsystem_Rule2_Driver[i]._erodefilterIP_DriverInstance.baseaddr = Subsystem_Rule2_ERODEFILTERIP_BASEADDR;
		Subsystem_Rule2_Driver[i]._erodefilterIP_DriverInstance.intr_id = Subsystem_Rule2_ERODEFILTERIP_INTRID;
		Subsystem_Rule2_Driver[i]._erodefilterIP_DriverInstance.busy = 0;
		Subsystem_Rule2_Driver[i]._vdmaIP_DriverInstance.baseaddr = Subsystem_Rule2_VDMAIP_BASEADDR;
		Subsystem_Rule2_Driver[i]._vdmaIP_DriverInstance.intr_id = Subsystem_Rule2_VDMAIP_INTRID;
		Subsystem_Rule2_Driver[i]._vdmaIP_DriverInstance.busy = 0;
    	}
}



void Subsystems_Rule2_intrInitialize(IntCntrl_t *InterruptController) {
    int i;
	for (i = 0; i<Subsystem_Rule2_NUM_INSTANCES; i++) {
		Subsystem_Rule2_intrInitialize(&Subsystem_Rule2_Driver[i], InterruptController);
	}
}



void Subsystems_Rule2_initialize() {
	int i;
	for (i = 0; i<Subsystem_Rule2_NUM_INSTANCES; i++) {
		Subsystem_Rule2_initialize(&Subsystem_Rule2_Driver[i], Subsystem_Rule2_InitMode);
	}
}



void Subsystems_Rule2_Init(IntCntrl_t *InterruptController) {
    Subsystems_Rule2_driverInstanceInit();
    Subsystems_Rule2_intrInitialize(InterruptController);
    Subsystems_Rule2_initialize();
}


int Subsystem_Rule2_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {

	#if Subsystem_Rule2_NUM_INSTANCES == 0
    	return 0;
	#else

    int idx;    
    // seeing if Subsystem_Rule2 filtering for current captured frame can be done on this Rule system    
    for (idx = 0; idx<Subsystem_Rule2_NUM_INSTANCES; idx++) {
        if (Subsystem_Rule2_isBusy(&Subsystem_Rule2_Driver[idx]) == 0) {
        	Subsystem_Rule2_start(&Subsystem_Rule2_Driver[idx], Subsystem_Rule2_StartMode);
        	//double cpu_cycles = get_cyclecount();
        	//printf("CPU cycles for Sobel Start HW: %f \n", cpu_cycles);
        	// Munish Commented while(Subsystem_Rule2_isBusy(&Subsystem_Rule2_Driver[idx]) == 1);
            return 1;
        }
    }    
    
    return 0;
#endif
}
