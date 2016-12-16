#ifndef __HWSW_FUNC_H__
#define __HWSW_FUNC_H__



#include "./IPSubsysDrivers/Rule1/drivers/driver.h"
#include "./IPSubsysDrivers/Rule2/drivers/driver.h"
#include "./IPSubsysDrivers/Rule3/drivers/driver.h"



void EdgeDetection_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);
void Erode_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);
void GrayScale_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);



#endif