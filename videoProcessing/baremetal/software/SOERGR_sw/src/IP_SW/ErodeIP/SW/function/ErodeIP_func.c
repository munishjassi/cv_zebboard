// this file is going to be auto-generated after reading in the IP_SW and all of IP_Rule*Driver modules
// it will use the configuration flags provided in config file

// this file provides API functionality to application developer to perform HA-aware erode filtering on captured video



#include "ErodeIP_func.h"
#include <stdio.h>


void ErodeIP_func_init(XScuGic *InterruptController, unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {

#if ERODEIP_NUM_INSTANCES != 0
	int ip_instance_idx = 0;
	int rule1_driver_idx = 0;

	for (ip_instance_idx=0; ip_instance_idx<ERODEIP_NUM_INSTANCES; ip_instance_idx++) {
		if (ERODEIP_INFO[ip_instance_idx].grip_rule == 1) {
			ErodeIPRule1Driver[rule1_driver_idx].baseaddr = ERODEIP_INFO[ip_instance_idx].baseaddr;
			ErodeIPRule1Driver[rule1_driver_idx].vdmaDriver.baseaddr = ERODEIP_INFO[ip_instance_idx].vdma_baseaddr;
			ErodeIPRule1Driver[rule1_driver_idx].vdmaDriver.intr_id = ERODEIP_INFO[ip_instance_idx].intr_id;
			ErodeIPRule1Driver[rule1_driver_idx].vdmaDriver.busy = 0;

			ErodeIP_Rule1Driver_initialize(&ErodeIPRule1Driver[rule1_driver_idx], InterruptController, ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
			rule1_driver_idx++;
		} // else if ... and so on depending on total grip rules supported by IP supplier
	}
	ip_instance_idx = 0;
	rule1_driver_idx = 0;
	// and so on depending on total grip rules supported by IP supplier

#endif
}

void Erode_func(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {

#if ERODEIP_NUM_INSTANCES == 0                      // no IP module in design so using SW implementation
    Erode(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
#else
    int ip_instance_idx = 0;
    int rule1_driver_idx = 0;

    for (ip_instance_idx=0; ip_instance_idx<ERODEIP_NUM_INSTANCES; ip_instance_idx++) {
    	if (ERODEIP_INFO[ip_instance_idx].grip_rule == 1) {
    		if (ErodeIP_Rule1Driver_isBusy(&ErodeIPRule1Driver[rule1_driver_idx]) == 0) {      // a free IP instance found
				ErodeIP_Rule1Driver_start(&ErodeIPRule1Driver[rule1_driver_idx], ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
				while(ErodeIP_Rule1Driver_isBusy(&ErodeIPRule1Driver[rule1_driver_idx]) == 1);
				return;
			}
    		rule1_driver_idx++;
    	}
    }    

    printf("E:SW\r\n");
    Erode(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime); 
#endif
}




