// this file is going to be auto-generated after reading in the IP_SW and all of IP_Rule*Driver modules
// it will use the configuration flags provided in config file

// this file provides API functionality to application developer to perform HA-aware grayscale filtering on captured video



#include "GrayscaleIP_func.h"
#include <stdio.h>

void GrayscaleIP_func_init(XScuGic *InterruptController, unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {

#if GRAYSCALEIP_NUM_INSTANCES != 0
	int ip_instance_idx = 0;
	int rule1_driver_idx = 0;
	int rule2_driver_idx = 0;
	// and so on depending on total grip rules supported by IP supplier

	for (ip_instance_idx=0; ip_instance_idx<GRAYSCALEIP_NUM_INSTANCES; ip_instance_idx++) {
		if (GRAYSCALEIP_INFO[ip_instance_idx].grip_rule == 1) {
			GrayscaleIPRule1Driver[rule1_driver_idx].baseaddr = GRAYSCALEIP_INFO[ip_instance_idx].baseaddr;
			GrayscaleIPRule1Driver[rule1_driver_idx].vdmaDriver.baseaddr = GRAYSCALEIP_INFO[ip_instance_idx].vdma_baseaddr;
			GrayscaleIPRule1Driver[rule1_driver_idx].vdmaDriver.intr_id = GRAYSCALEIP_INFO[ip_instance_idx].intr_id;
			GrayscaleIPRule1Driver[rule1_driver_idx].vdmaDriver.busy = 0;

			GrayscaleIP_Rule1Driver_initialize(&GrayscaleIPRule1Driver[rule1_driver_idx], InterruptController, ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
			rule1_driver_idx++;
		}
#if GRAYSCALEIP_NUM_GRIPRULE2_INSTANCES != 0
		else if (GRAYSCALEIP_INFO[ip_instance_idx].grip_rule == 2) {
			GrayscaleIPRule2Driver[rule2_driver_idx].baseaddr = GRAYSCALEIP_INFO[ip_instance_idx].baseaddr;
			GrayscaleIPRule2Driver[rule2_driver_idx].intr_id = GRAYSCALEIP_INFO[ip_instance_idx].intr_id;
			GrayscaleIPRule2Driver[rule2_driver_idx].busy = 0;

			GrayscaleIP_Rule2Driver_initialize(&GrayscaleIPRule2Driver[rule2_driver_idx], InterruptController, ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
			rule2_driver_idx++;
		}
#endif
		// else if ... and so on depending on total grip rules supported by IP supplier
	}
	ip_instance_idx = 0;
	rule1_driver_idx = 0;
	rule2_driver_idx = 0;
	// and so on depending on total grip rules supported by IP supplier

#endif

}



void ConvToGray_func(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {

#if GRAYSCALEIP_NUM_INSTANCES == 0                      // no IP module in design so using SW implementation
    ConvToGray(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
#else
    int ip_instance_idx = 0;
    int rule1_driver_idx = 0;
    int rule2_driver_idx = 0;
    // and so on depending on total grip rules supported by IP supplier

    for (ip_instance_idx=0; ip_instance_idx<GRAYSCALEIP_NUM_INSTANCES; ip_instance_idx++) {
    	if (GRAYSCALEIP_INFO[ip_instance_idx].grip_rule == 1) {
    		if (GrayscaleIP_Rule1Driver_isBusy(&GrayscaleIPRule1Driver[rule1_driver_idx]) == 0) {      // a free IP instance found
				GrayscaleIP_Rule1Driver_start(&GrayscaleIPRule1Driver[rule1_driver_idx], ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
				while(GrayscaleIP_Rule1Driver_isBusy(&GrayscaleIPRule1Driver[rule1_driver_idx]) == 1);
				return;
			}
    		rule1_driver_idx++;
    	}
#if GRAYSCALEIP_NUM_GRIPRULE2_INSTANCES != 0
    	else if (GRAYSCALEIP_INFO[ip_instance_idx].grip_rule == 2) {
    		if (GrayscaleIP_Rule2Driver_isBusy(&GrayscaleIPRule2Driver[rule2_driver_idx]) == 0) {
				GrayscaleIP_Rule2Driver_start(&GrayscaleIPRule2Driver[rule2_driver_idx], ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
				while(GrayscaleIP_Rule2Driver_isBusy(&GrayscaleIPRule2Driver[rule2_driver_idx]) == 1);
				return;
    		}
    		rule2_driver_idx++;
    	}
#endif
    	// else if ... so on depending on total grip rules supported by IP supplier
    }

    printf("G:SW\r\n");
    ConvToGray(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime); 
#endif
}




