//implementation for data structures in .config file for this IP

#include "ErodeIP_config.h"


ERODEIP_IPinfo ERODEIP_INFO[ERODEIP_NUM_INSTANCES] = {
												        {.baseaddr = 0x7C800000, .grip_rule = 1, .vdma_baseaddr = 0x43000000, .intr_id = 61}
											         };
