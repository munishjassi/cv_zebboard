//implementation for data structures in .config file for this IP

#include "GrayscaleIP_config.h"


GRAYSCALEIP_IPinfo GRAYSCALEIP_INFO[GRAYSCALEIP_NUM_INSTANCES] = {
														{.baseaddr = 0x6e800000, .grip_rule = 1, .vdma_baseaddr = 0x43040000, .intr_id = 84}/*,
														{.baseaddr = 0xffffffff, .grip_rule = 2, .vdma_baseaddr = 0xffffffff, .intr_id = 0}*/
													  };

