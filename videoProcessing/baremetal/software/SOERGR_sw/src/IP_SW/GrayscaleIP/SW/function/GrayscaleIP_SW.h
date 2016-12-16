// this module is simulated to be as if being provided by the Grayscale HA IP supplier as pure SW implementation of its functionality!!


#ifndef __GRAYSCALE_SW_H__
#define __GRAYSCALE_SW_H__

// NOTE: this prototype should be generated having all the parameters that a driver might need at runtime as well even though it is needed for its definition (i.e in .c file)
void ConvToGray(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime);
		
		
#endif		
