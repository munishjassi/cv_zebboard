// config for Subsystem_Rule2 i.e corresponding to only SobelfilterIP+VDMAIP to do sobelfilter filtering
// GRIP: Generated file
//


#ifndef __CONFIG_H__
#include "../../.././IPSubsysDrivers/Rule2/./drivers/driver.h"



#define Subsystem_Rule2_NUM_INSTANCES 0
#define Subsystem_Rule2_ERODEFILTERIP_BASEADDR 0xFFFFFFFF 	//XPAR_ERODEFILTER_S_AXI_CONTROL_BUS_BASEADDR
#define Subsystem_Rule2_ERODEFILTERIP_INTRID 0 	//XPAR_FABRIC_ERODEFILTER_0_INTERRUPT_INTR
#define Subsystem_Rule2_VDMAIP_BASEADDR 0xFFFFFFFF 	//XPAR_VDMA_S_AXI_CONTROL_BUS_BASEADDR
#define Subsystem_Rule2_VDMAIP_INTRID 0 	//XPAR_FABRIC_VDMA_0_INTERRUPT_INTR



Subsystem_Rule2_RegMap Subsystem_Rule2_InitMode = {
				._erodefilterIP_RegMap = {
								.AP_CTRL = { .offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
								.GIE = { .offset = 0x04, .mask = 0x00000000, .value = 0xffffffff},
								.IER = { .offset = 0x08, .mask = 0x00000000, .value = 0xffffffff},
								.ISR = { .offset = 0x0c, .mask = 0x00000000, .value = 0xffffffff},
								.ROWS_DATA = { .offset = 0x14, .mask = 0x00000000, .value = 0xffffffff},
								.COLS_DATA = { .offset = 0x1c, .mask = 0x00000000, .value = 0xffffffff}
							},

				._vdmaIP_RegMap = {
								.MM2S_DMACR = { .offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_DMACR = { .offset = 0x30, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_DMASR = { .offset = 0x34, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_VSIZE = { .offset = 0x50, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_HSIZE = { .offset = 0x54, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_FRMDLY_STRIDE = { .offset = 0x58, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS1 = { .offset = 0x5c, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS2 = { .offset = 0x60, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS3 = { .offset = 0x64, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_VSIZE = { .offset = 0xa0, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_HSIZE = { .offset = 0xa4, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_FRMDLY_STRIDE = { .offset = 0xa8, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS1 = { .offset = 0xac, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS2 = { .offset = 0xb0, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS3 = { .offset = 0xb4, .mask = 0x00000000, .value = 0xffffffff}
							}

				};



Subsystem_Rule2_RegMap Subsystem_Rule2_StartMode = {
				._erodefilterIP_RegMap = {
								.AP_CTRL = { .offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
								.GIE = { .offset = 0x04, .mask = 0x00000000, .value = 0xffffffff},
								.IER = { .offset = 0x08, .mask = 0x00000000, .value = 0xffffffff},
								.ISR = { .offset = 0x0c, .mask = 0x00000000, .value = 0xffffffff},
								.ROWS_DATA = { .offset = 0x14, .mask = 0x00000000, .value = 0xffffffff},
								.COLS_DATA = { .offset = 0x1c, .mask = 0x00000000, .value = 0xffffffff}
							},

				._vdmaIP_RegMap = {
								.MM2S_DMACR = { .offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_DMACR = { .offset = 0x30, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_DMASR = { .offset = 0x34, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_VSIZE = { .offset = 0x50, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_HSIZE = { .offset = 0x54, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_FRMDLY_STRIDE = { .offset = 0x58, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS1 = { .offset = 0x5c, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS2 = { .offset = 0x60, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS3 = { .offset = 0x64, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_VSIZE = { .offset = 0xa0, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_HSIZE = { .offset = 0xa4, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_FRMDLY_STRIDE = { .offset = 0xa8, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS1 = { .offset = 0xac, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS2 = { .offset = 0xb0, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS3 = { .offset = 0xb4, .mask = 0x00000000, .value = 0xffffffff}
							}

				};



Subsystem_Rule2_RegMap Subsystem_Rule2_StopMode = {
				._erodefilterIP_RegMap = {
								.AP_CTRL = { .offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
								.GIE = { .offset = 0x04, .mask = 0x00000000, .value = 0xffffffff},
								.IER = { .offset = 0x08, .mask = 0x00000000, .value = 0xffffffff},
								.ISR = { .offset = 0x0c, .mask = 0x00000000, .value = 0xffffffff},
								.ROWS_DATA = { .offset = 0x14, .mask = 0x00000000, .value = 0xffffffff},
								.COLS_DATA = { .offset = 0x1c, .mask = 0x00000000, .value = 0xffffffff}
							},

				._vdmaIP_RegMap = {
								.MM2S_DMACR = { .offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_DMACR = { .offset = 0x30, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_DMASR = { .offset = 0x34, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_VSIZE = { .offset = 0x50, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_HSIZE = { .offset = 0x54, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_FRMDLY_STRIDE = { .offset = 0x58, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS1 = { .offset = 0x5c, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS2 = { .offset = 0x60, .mask = 0x00000000, .value = 0xffffffff},
								.MM2S_START_ADDRESS3 = { .offset = 0x64, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_VSIZE = { .offset = 0xa0, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_HSIZE = { .offset = 0xa4, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_FRMDLY_STRIDE = { .offset = 0xa8, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS1 = { .offset = 0xac, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS2 = { .offset = 0xb0, .mask = 0x00000000, .value = 0xffffffff},
								.S2MM_START_ADDRESS3 = { .offset = 0xb4, .mask = 0x00000000, .value = 0xffffffff}
							}

				};



#endif