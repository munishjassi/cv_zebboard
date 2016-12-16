// TODO: Implement this driver!!


#ifndef __GRAYSCALEIP_RULE2DRIVER_H__
#define __GRAYSCALEIP_RULE2DRIVER_H__

#include "xscugic.h"


#define bool unsigned char

// all the hardware related info needed by the driver functions should be provided in this struct
typedef struct {
    unsigned int baseaddr;
    bool busy;
    unsigned int intr_id;
} GrayscaleIPRule2DriverInstance;


typedef struct {
    unsigned int offset;
    unsigned int value;
    unsigned int mask;
} GrayscaleIPRule2RegType;



// register map for Grayscale IP peripheral registers
typedef struct {
	GrayscaleIPRule2RegType AP_CTRL;
	GrayscaleIPRule2RegType GIE;
	GrayscaleIPRule2RegType IER;
	GrayscaleIPRule2RegType ISR;
	GrayscaleIPRule2RegType ROWS_DATA;
	GrayscaleIPRule2RegType COLS_DATA;
} GrayscaleIPRule2RegMap;





// the IP supplier should modify the contents of below register to be able to monitor the status of IP processing
#define GRAYSCALEIPRULE2_BUSY_STATUS_REG_offset 0xff
#define GRAYSCALEIPRULE2_BUSY_STATUS_REG_bit 31          // little endian convention [31:0]



// API for GrayscaleIP_Driver to use if this rule is applied by GRIP
void GrayscaleIP_Rule2Driver_initialize(GrayscaleIPRule2DriverInstance *InstancePtr, XScuGic *InterruptController, unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);
void GrayscaleIP_Rule2Driver_start(GrayscaleIPRule2DriverInstance *InstancePtr, unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);
void GrayscaleIP_Rule2Driver_stop(GrayscaleIPRule2DriverInstance *InstancePtr);
bool GrayscaleIP_Rule2Driver_isBusy(GrayscaleIPRule2DriverInstance *InstancePtr);






#endif




// NOTE: everything seems to be auto-generatable!!











