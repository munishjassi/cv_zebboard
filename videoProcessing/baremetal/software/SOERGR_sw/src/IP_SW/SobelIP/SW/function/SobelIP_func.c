// this file is going to be auto-generated after reading in the IP_SW and all of IP_Rule*Driver modules
// it will use the configuration flags provided in config file

// this file provides API functionality to application developer to perform HA-aware sobel filtering on captured video



#include "SobelIP_func.h"
#include <stdio.h>


void SobelIP_func_init(XScuGic *InterruptController, unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {

#if SOBELIP_NUM_INSTANCES != 0
    int ip_instance_idx = 0;
    int rule1_driver_idx = 0;

	for (ip_instance_idx=0; ip_instance_idx<SOBELIP_NUM_INSTANCES; ip_instance_idx++) {
		if (SOBELIP_INFO[ip_instance_idx].grip_rule == 1) {
			SobelIPRule1Driver[rule1_driver_idx].baseaddr = SOBELIP_INFO[ip_instance_idx].baseaddr;
			SobelIPRule1Driver[rule1_driver_idx].vdmaDriver.baseaddr = SOBELIP_INFO[ip_instance_idx].vdma_baseaddr;
			SobelIPRule1Driver[rule1_driver_idx].vdmaDriver.intr_id = SOBELIP_INFO[ip_instance_idx].intr_id;
			SobelIPRule1Driver[rule1_driver_idx].vdmaDriver.busy = 0;

			SobelIP_Rule1Driver_initialize(&SobelIPRule1Driver[rule1_driver_idx], InterruptController, ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
			rule1_driver_idx++;
		} // else if ... and so on depending on total grip rules supported by IP supplier
	}
    ip_instance_idx = 0;
    rule1_driver_idx = 0;
	// and so on depending on total grip rules supported by IP supplier

#endif
}

void EdgeDetection_func(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {

#if SOBELIP_NUM_INSTANCES == 0                      // no IP module in design so using SW implementation
    EdgeDetection(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
#else
    int ip_instance_idx = 0;
    int rule1_driver_idx = 0;

    for (ip_instance_idx=0; ip_instance_idx<SOBELIP_NUM_INSTANCES; ip_instance_idx++) {
    	if (SOBELIP_INFO[ip_instance_idx].grip_rule == 1) {
    		if (SobelIP_Rule1Driver_isBusy(&SobelIPRule1Driver[rule1_driver_idx]) == 0) {      // a free IP instance found
				SobelIP_Rule1Driver_start(&SobelIPRule1Driver[rule1_driver_idx], ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
				while(SobelIP_Rule1Driver_isBusy(&SobelIPRule1Driver[rule1_driver_idx]) == 1);
				return;
			}
    		rule1_driver_idx++;
    	}
    }    
    printf("S:SW\r\n");
    EdgeDetection(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime); 
#endif
}




