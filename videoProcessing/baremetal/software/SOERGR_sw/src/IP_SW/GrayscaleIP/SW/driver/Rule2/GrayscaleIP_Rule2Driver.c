// TODO: Implement this driver!!



#include "GrayscaleIP_Rule2Driver.h"


GrayscaleIPRule2RegMap GrayscaleIPRule2InitMode = {
													.AP_CTRL = {.offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
													.GIE = {.offset = 0x04, .mask = 0x00000000, .value = 0xffffffff},
													.IER = {.offset = 0x08, .mask = 0x00000000, .value = 0xffffffff},
													.ISR = {.offset = 0x0c, .mask = 0x00000000, .value = 0xffffffff},
													.ROWS_DATA = {.offset = 0x14, .mask = 0x00000000, .value = 0xffffffff},
													.COLS_DATA = {.offset = 0x1c, .mask = 0x00000000, .value = 0xffffffff}
												  };


GrayscaleIPRule2RegMap GrayscaleIPRule2StartMode = {
													.AP_CTRL = {.offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
													.GIE = {.offset = 0x04, .mask = 0x00000000, .value = 0xffffffff},
													.IER = {.offset = 0x08, .mask = 0x00000000, .value = 0xffffffff},
													.ISR = {.offset = 0x0c, .mask = 0x00000000, .value = 0xffffffff},
													.ROWS_DATA = {.offset = 0x14, .mask = 0x00000000, .value = 0xffffffff},
													.COLS_DATA = {.offset = 0x1c, .mask = 0x00000000, .value = 0xffffffff}
												   };


GrayscaleIPRule2RegMap GrayscaleIPRule2StopMode = {
													.AP_CTRL = {.offset = 0x00, .mask = 0x00000000, .value = 0xffffffff},
													.GIE = {.offset = 0x04, .mask = 0x00000000, .value = 0xffffffff},
													.IER = {.offset = 0x08, .mask = 0x00000000, .value = 0xffffffff},
													.ISR = {.offset = 0x0c, .mask = 0x00000000, .value = 0xffffffff},
													.ROWS_DATA = {.offset = 0x14, .mask = 0x00000000, .value = 0xffffffff},
													.COLS_DATA = {.offset = 0x1c, .mask = 0x00000000, .value = 0xffffffff}
												  };



static void localWriteReg(unsigned int addr, unsigned int mask, unsigned int value) {
    // only write to reg if mask != 0
    if ((mask | 0x0) != 0x0) {
    *((volatile unsigned int *)(addr)) = (*(volatile unsigned int *)(addr) & ~(mask)) | value;
	}
}


static unsigned int localReadReg(unsigned int addr) {
    return (unsigned int) *((volatile unsigned int *)(addr));
}


// updates the Grayscale IP peripheral's memory mapped registers with content provided in mode 
static void SetHAMode(GrayscaleIPRule2RegMap mode, unsigned int baseaddr) {
	localWriteReg(baseaddr + mode.AP_CTRL.offset, mode.AP_CTRL.mask, mode.AP_CTRL.value);
	localWriteReg(baseaddr + mode.GIE.offset, mode.GIE.mask, mode.GIE.value);
	localWriteReg(baseaddr + mode.IER.offset, mode.IER.mask, mode.IER.value);
	localWriteReg(baseaddr + mode.ISR.offset, mode.ISR.mask, mode.ISR.value);
	localWriteReg(baseaddr + mode.ROWS_DATA.offset, mode.ROWS_DATA.mask, mode.ROWS_DATA.value);
	localWriteReg(baseaddr + mode.COLS_DATA.offset, mode.COLS_DATA.mask, mode.COLS_DATA.value);
}

// the IP supplier should make necessary modifications to initialize the Grayscale IP here
void GrayscaleIP_Rule2Driver_initialize(GrayscaleIPRule2DriverInstance *InstancePtr, XScuGic *InterruptController, unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {
    SetHAMode(GrayscaleIPRule2InitMode, InstancePtr->baseaddr);
}


// the IP supplier should make necessary modifications to trigger the Grayscale IP to start processing here
void GrayscaleIP_Rule2Driver_start(GrayscaleIPRule2DriverInstance *InstancePtr, unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {
    SetHAMode(GrayscaleIPRule2StartMode, InstancePtr->baseaddr);
}


// the IP supplier should make necessary modifications to stop the Grayscale IP processing here
void GrayscaleIP_Rule2Driver_stop(GrayscaleIPRule2DriverInstance *InstancePtr) {
    SetHAMode(GrayscaleIPRule2StopMode, InstancePtr->baseaddr);
}

// the IP supplier should make necessary modifications to inform about the Busy status here
bool GrayscaleIP_Rule2Driver_isBusy(GrayscaleIPRule2DriverInstance *InstancePtr) {
    //return (bool) ((localReadReg(InstancePtr->baseaddr + GRAYSCALEIPRULE2_BUSY_STATUS_REG_offset) >> GRAYSCALEIPRULE2_BUSY_STATUS_REG_bit) & 1);

	// right now simulating it to be always busy
	return 1;
}









