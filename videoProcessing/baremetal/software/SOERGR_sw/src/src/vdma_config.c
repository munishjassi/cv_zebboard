/*
 * vdma_config.c
 *
 */
#include "vdma_config.h"


void config_filterVDMA(int VDMA_baseAddr, int vdma_direction, unsigned long base_address, unsigned short horizontalActiveTime, unsigned short verticalActiveTime)
{

	if(vdma_direction == DMA_DEV_TO_MEM)		// from device to memory
	{// rx : S2MM
		/* FOR HDMI RELEVANT SETTINGS of VDMA*/
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_RX_CTRL), 0x00011003); // enable circular mode
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_RX_START1), base_address); // start address
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_RX_START2), base_address); // start address
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_RX_START3), base_address); // start address
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_RX_FRMDLY_STRIDE), horizontalActiveTime*4); // h offset
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_RX_HSIZE), horizontalActiveTime*4); // h size
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_RX_VSIZE), verticalActiveTime); // v size

	}
	else if (vdma_direction ==DMA_MEM_TO_DEV)		// from memory to device
	{//tx : MM2S
		/* FOR HDMI RELEVANT SETTINGS of VDMA */
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_TX_CTRL), 0x00011003); // enable circular mode, genlock synEn
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_TX_START1), base_address); // start address
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_TX_START2), base_address); // start address
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_TX_START3), base_address); // start address
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_TX_FRMDLY_STRIDE), horizontalActiveTime*4); // h offset
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_TX_HSIZE), horizontalActiveTime*4); // h size
		Xil_Out32((VDMA_baseAddr + AXI_FILTER_TX_VSIZE), verticalActiveTime); // v size
	}

}

void resetVDMA(int VDMA_baseAddr){
	//RX: reset S2MM
	Xil_Out32((VDMA_baseAddr + AXI_FILTER_RX_CTRL), 0x00000004);
	//TX: reset MM2S
	Xil_Out32((VDMA_baseAddr + AXI_FILTER_TX_CTRL), 0x00000004);
}

