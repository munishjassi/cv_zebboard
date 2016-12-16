/***************************************************************************//**
 *   @file   cf_hdmi.h
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

#ifndef CF_HDMI_H_
#define CF_HDMI_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xparameters.h"

/******************************************************************************/
/************************** Macros Definitions ********************************/
/******************************************************************************/
#define CF_CLKGEN_BASEADDR XPAR_AXI_CLKGEN_0_BASEADDR
#define CFV_BASEADDR       XPAR_AXI_HDMI_TX_16B_0_BASEADDR
#define DDR_BASEADDR       XPAR_DDR_MEM_BASEADDR
#define UART_BASEADDR      XPS_UART1_BASEADDR
#define VDMA_BASEADDR      XPAR_AXI_VDMA_0_BASEADDR
#define ADMA_DEVICE_ID		XPAR_XDMAPS_1_DEVICE_ID
#define IIC_BASEADDR       XPS_I2C0_BASEADDR
#define VIDEO_BASEADDR			DDR_BASEADDR + 0x2000000
#define PROC_VIDEO_BASEADDR		DDR_BASEADDR + 0x3000000
//for Test
#define GET_INPUT_ADDR(i)        	DDR_BASEADDR + 0x4000000+0x100000*(i)
#define CE_INPUT_1_BASEADDR			DDR_BASEADDR + 0x4000000
#define CE_INPUT_2_BASEADDR			DDR_BASEADDR + 0x5000000
#define CE_PROC_1_BASEADDR			DDR_BASEADDR + 0x6000000
#define CE_PROC_2_BASEADDR			DDR_BASEADDR + 0x7000000
//SW
#define CE_BYTE_1_BASEADDR			DDR_BASEADDR + 0x8000000
#define CE_BYTE_2_BASEADDR			DDR_BASEADDR + 0x9000000
#define CE_OUTPUT_SW_1_BASEADDR		DDR_BASEADDR + 0xa000000
#define CE_OUTPUT_SW_2_BASEADDR		DDR_BASEADDR + 0xb000000
#define FEATURE_SW_X1_BASEADDR			DDR_BASEADDR + 0xc000000
#define FEATURE_SW_Y1_BASEADDR			DDR_BASEADDR + 0xc100000
#define FEATURE_SW_X2_BASEADDR			DDR_BASEADDR + 0xc200000
#define FEATURE_SW_Y2_BASEADDR			DDR_BASEADDR + 0xc300000
#define OUTPUT_ME_BASEADDR				DDR_BASEADDR + 0xd000000
//HW
#define CE_OUTPUT_1_BASEADDR		DDR_BASEADDR + 0xe000000
#define CE_OUTPUT_2_BASEADDR		DDR_BASEADDR + 0xf000000
#define FEATURE_X1_BASEADDR			DDR_BASEADDR + 0x10000000
#define FEATURE_Y1_BASEADDR			DDR_BASEADDR + 0x10100000
#define FEATURE_X2_BASEADDR			DDR_BASEADDR + 0x10200000
#define FEATURE_Y2_BASEADDR			DDR_BASEADDR + 0x10300000
#define OUTPUT_BASEADDR				DDR_BASEADDR + 0x11000000
#define DIFFERENCE 					DDR_BASEADDR + 0x12000000
// for stefan CE
#define CE_OUTPUT_S_1_BASEADDR 		DDR_BASEADDR + 0x13000000
#define CE_OUTPUT_S_2_BASEADDR 		DDR_BASEADDR + 0x14000000
// for eee and ece
// SW
#define FEATURE_IMG					DDR_BASEADDR + 0x15000000
#define COMP_VECTOR 				DDR_BASEADDR + 0x16000000
#define EGO_VECTOR 					DDR_BASEADDR + 0x17000000
#define FULL_VECTOR					DDR_BASEADDR + 0x18000000
#define RES_VECTOR					DDR_BASEADDR + 0x19000000
//HW
#define PRE_COMP_VECTOR_HW			DDR_BASEADDR + 0x1a000000
#define COMP_VECTOR_HW 				DDR_BASEADDR + 0x1b000000
#define MODEL_ADDR					DDR_BASEADDR + 0x1c000000
#define RESULT_ADDR					DDR_BASEADDR + 0x1d000000
#define BUF_CAM 					DDR_BASEADDR + 0x1e000000
#define BEST_MODEL					DDR_BASEADDR + 0x1f000000
//END Test
#define A_SAMPLE_FREQ      48000
#define A_FREQ             1400
#define AUDIO_LENGTH		(A_SAMPLE_FREQ/A_FREQ)

#define CAM_MEM_BUFF0_BASEADDR XPAR_CAM_INTERFACE_0_S_AXI_MEM0_BASEADDR
#define CAM_MEM_BUFF1_BASEADDR XPAR_CAM_INTERFACE_0_S_AXI_MEM1_BASEADDR

#define AXI_HDMI_REG_CTRL			0x04
#define AXI_HDMI_REG_HTIMING1		0x08
#define AXI_HDMI_REG_HTIMING2		0x0C
#define AXI_HDMI_REG_VTIMING1		0x10
#define AXI_HDMI_REG_VTIMING2		0x14
#define AXI_HDMI_REG_STATUS			0x18
#define AXI_HDMI_REG_COLOR_PATTERN	0x1c

#define AXI_VDMA_REG_DMA_CTRL		0x00
#define AXI_VDMA_REG_V_SIZE			0x50
#define AXI_VDMA_REG_H_SIZE			0x54
#define AXI_VDMA_REG_FRMDLY_STRIDE	0x58
#define AXI_VDMA_REG_START_1		0x5C
#define AXI_VDMA_REG_START_2		0x60
#define AXI_VDMA_REG_START_3		0x64

#define AXI_CLKGEN_REG_UPDATE_ENABLE	0x04
#define AXI_CLKGEN_REG_CLK_OUT1			0x08
#define AXI_CLKGEN_REG_CLK_OUT2			0x0c
#define AXI_CLKGEN_REG_CLK_DIV			0x10
#define AXI_CLKGEN_REG_CLK_FB1			0x14
#define AXI_CLKGEN_REG_CLK_FB2			0x18
#define AXI_CLKGEN_REG_LOCK1			0x1c
#define AXI_CLKGEN_REG_LOCK2			0x20
#define AXI_CLKGEN_REG_LOCK3			0x24
#define AXI_CLKGEN_REG_FILTER1			0x28
#define AXI_CLKGEN_REG_FILTER2			0x2c

unsigned char currentResolution;
unsigned long imageRow; 				// global var for DDR image row selection
unsigned short line_buffer[3][640]; 	// storing 3 image rows for edge detection

enum videoResolution
{
	RESOLUTION_640x480,
	RESOLUTION_800x600,
	RESOLUTION_1024x768,
	RESOLUTION_1280x720,
	RESOLUTION_1360x768,
	RESOLUTION_1600x900,
	RESOLUTION_1920x1080
};

enum detailedTimingElement
{
	PIXEL_CLOCK,
	H_ACTIVE_TIME,
	H_BLANKING_TIME,
	H_SYNC_OFFSET,
	H_SYNC_WIDTH_PULSE,
	V_ACTIVE_TIME,
	V_BLANKING_TIME,
	V_SYNC_OFFSET,
	V_SYNC_WIDTH_PULSE
};

enum imageToDisplay // TODO
{
	Original,
	Processed
};

static const unsigned long detailedTiming[7][9] =
{
	{25180000, 640, 144, 16, 96, 480, 29, 10, 2},
	{40000000, 800, 256, 40, 128, 600, 28, 1, 4},
	{65000000, 1024, 320, 136, 24, 768, 38, 3, 6},
	{74250000, 1280, 370, 110, 40, 720, 30, 5, 5},
	{84750000, 1360, 416, 136, 72, 768, 30, 3, 5},
	{108000000, 1600, 400, 32, 48, 900, 12, 3, 6},
	{148500000, 1920, 280, 44, 88, 1080, 45, 4, 5}
};


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
void InitHdmiVideoPcore(unsigned short horizontalActiveTime,
						unsigned short horizontalBlankingTime,
						unsigned short horizontalSyncOffset,
						unsigned short horizontalSyncPulseWidth,
						unsigned short verticalActiveTime,
						unsigned short verticalBlankingTime,
						unsigned short verticalSyncOffset,
						unsigned short verticalSyncPulseWidth);

void ConfigHdmiVDMA (unsigned short horizontalActiveTime,
		unsigned short verticalActiveTime,
		unsigned long vdmaVideoBaseAddress);

void SetVideoResolution(unsigned char resolution);

void InitHdmiAudioPcore(void);

int CLKGEN_SetRate(unsigned long rate, unsigned long parent_rate);

unsigned long CLKGEN_GetRate(unsigned long parent_rate);

void DDRVideoWr(unsigned short width, unsigned short height, unsigned short horizontalActiveTime,
				unsigned short verticalActiveTime);

void DDRLineWrite(unsigned short horizontalActiveTime, unsigned short verticalActiveTime);

void ResetImageSpace(unsigned short horizontalActiveTime, unsigned short verticalActiveTime);

#endif /* CF_HDMI_H_ */
