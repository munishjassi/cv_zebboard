// ==============================================================
// File generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2013.3
// Copyright (C) 2013 Xilinx Inc. All rights reserved.
// 
// ==============================================================

#ifndef XMATCHINGENGINE32_H
#define XMATCHINGENGINE32_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xmatchingengine32_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Slv_para_BaseAddress;
} XMatchingengine32_Config;
#endif

typedef struct {
    u32 Slv_para_BaseAddress;
    u32 IsReady;
} XMatchingengine32;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XMatchingengine32_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XMatchingengine32_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XMatchingengine32_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XMatchingengine32_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XMatchingengine32_Initialize(XMatchingengine32 *InstancePtr, u16 DeviceId);
XMatchingengine32_Config* XMatchingengine32_LookupConfig(u16 DeviceId);
int XMatchingengine32_CfgInitialize(XMatchingengine32 *InstancePtr, XMatchingengine32_Config *ConfigPtr);
#else
int XMatchingengine32_Initialize(XMatchingengine32 *InstancePtr, const char* InstanceName);
int XMatchingengine32_Release(XMatchingengine32 *InstancePtr);
#endif

void XMatchingengine32_Start(XMatchingengine32 *InstancePtr);
u32 XMatchingengine32_IsDone(XMatchingengine32 *InstancePtr);
u32 XMatchingengine32_IsIdle(XMatchingengine32 *InstancePtr);
u32 XMatchingengine32_IsReady(XMatchingengine32 *InstancePtr);
void XMatchingengine32_EnableAutoRestart(XMatchingengine32 *InstancePtr);
void XMatchingengine32_DisableAutoRestart(XMatchingengine32 *InstancePtr);
u32 XMatchingengine32_GetReturn(XMatchingengine32 *InstancePtr);

void XMatchingengine32_SetHeight_v(XMatchingengine32 *InstancePtr, u32 Data);
u32 XMatchingengine32_GetHeight_v(XMatchingengine32 *InstancePtr);
void XMatchingengine32_SetWidth_v(XMatchingengine32 *InstancePtr, u32 Data);
u32 XMatchingengine32_GetWidth_v(XMatchingengine32 *InstancePtr);
void XMatchingengine32_SetReadaddress1_v(XMatchingengine32 *InstancePtr, u32 Data);
u32 XMatchingengine32_GetReadaddress1_v(XMatchingengine32 *InstancePtr);
void XMatchingengine32_SetReadaddress2_v(XMatchingengine32 *InstancePtr, u32 Data);
u32 XMatchingengine32_GetReadaddress2_v(XMatchingengine32 *InstancePtr);
void XMatchingengine32_SetFeatureout_v(XMatchingengine32 *InstancePtr, u32 Data);
u32 XMatchingengine32_GetFeatureout_v(XMatchingengine32 *InstancePtr);

void XMatchingengine32_InterruptGlobalEnable(XMatchingengine32 *InstancePtr);
void XMatchingengine32_InterruptGlobalDisable(XMatchingengine32 *InstancePtr);
void XMatchingengine32_InterruptEnable(XMatchingengine32 *InstancePtr, u32 Mask);
void XMatchingengine32_InterruptDisable(XMatchingengine32 *InstancePtr, u32 Mask);
void XMatchingengine32_InterruptClear(XMatchingengine32 *InstancePtr, u32 Mask);
u32 XMatchingengine32_InterruptGetEnabled(XMatchingengine32 *InstancePtr);
u32 XMatchingengine32_InterruptGetStatus(XMatchingengine32 *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
