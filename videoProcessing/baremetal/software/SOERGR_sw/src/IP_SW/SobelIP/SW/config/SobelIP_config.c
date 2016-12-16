//implementation for data structures in .config file for this IP

#include "SobelIP_config.h"


SOBELIP_IPinfo SOBELIP_INFO[SOBELIP_NUM_INSTANCES] = {
												{.baseaddr = 0x71800000, .grip_rule = 1, .vdma_baseaddr = 0x43020000, .intr_id = 64}
											  };
