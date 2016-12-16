// GRIP: Generated file 
// Generated file contains the HW-SW interface functions to be used in an application 
// 


#include "hwswFunctions.h"


#include "..//home/eda-tum/Development/nGRIP_work/eclipse_workspace_mars_v2.1_100316.GUI.DATE16/SWdriversGeneration.Bare/./inputDir/Sobelfilter_SW.h"
#include "..//home/eda-tum/Development/nGRIP_work/eclipse_workspace_mars_v2.1_100316.GUI.DATE16/SWdriversGeneration.Bare/./inputDir/Erode_SW.h"
#include "..//home/eda-tum/Development/nGRIP_work/eclipse_workspace_mars_v2.1_100316.GUI.DATE16/SWdriversGeneration.Bare/./inputDir/Grayscale_SW.h"



//
// Functions from SW file: /home/eda-tum/Development/nGRIP_work/eclipse_workspace_mars_v2.1_100316.GUI.DATE16/SWdriversGeneration.Bare/./inputDir/Sobelfilter_SW.h
//
void EdgeDetection_funcInit(IntCntrl_t *InterruptController,unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {
				Subsystems_Rule1_driverInstanceInit();
				Subsystems_Rule1_intrInitialize(InterruptController);
				Subsystems_Rule1_initialize();
		}


void EdgeDetection_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {
				// no hw resources available for processing this current frame so using SW implementation
				if (Subsystem_Rule1_func(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime) == 0) {
				//double cpu_cycles = get_cyclecount();
				//printf("CPU cycles for Sobel Start SW: %f \n", cpu_cycles);
				EdgeDetection(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
			}
		}






//
// Functions from SW file: /home/eda-tum/Development/nGRIP_work/eclipse_workspace_mars_v2.1_100316.GUI.DATE16/SWdriversGeneration.Bare/./inputDir/Erode_SW.h
//
void Erode_funcInit(IntCntrl_t *InterruptController,unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {
				Subsystems_Rule2_driverInstanceInit();
				Subsystems_Rule2_intrInitialize(InterruptController);
				Subsystems_Rule2_initialize();
		}


void Erode_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {
				// no hw resources available for processing this current frame so using SW implementation
				if (Subsystem_Rule2_func(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime) == 0) {
				//double cpu_cycles = get_cyclecount();
				//printf("CPU cycles for Sobel Start SW: %f \n", cpu_cycles);
				Erode(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
			}
		}






//
// Functions from SW file: /home/eda-tum/Development/nGRIP_work/eclipse_workspace_mars_v2.1_100316.GUI.DATE16/SWdriversGeneration.Bare/./inputDir/Grayscale_SW.h
//
void GrayScale_funcInit(IntCntrl_t *InterruptController,unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {
				Subsystems_Rule3_driverInstanceInit();
				Subsystems_Rule3_intrInitialize(InterruptController);
				Subsystems_Rule3_initialize();
		}


void GrayScale_func(unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime) {
				// no hw resources available for processing this current frame so using SW implementation
				if (Subsystem_Rule3_func(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime) == 0) {
				//double cpu_cycles = get_cyclecount();
				//printf("CPU cycles for Sobel Start SW: %f \n", cpu_cycles);
				GrayScale(ImgIn_BaseAddr, ImgOut_BaseAddr, width, height, horizontalActiveTime, verticalActiveTime);
			}
		}



