#include "GrayscaleIP_SW.h"
#include "xil_types.h"
#include "xil_io.h"

/* Takes the Raw image from DDR image space
 * converts it to Gray scale and writes back to DDR
 * at a different location
 */
void ConvToGray(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,
		unsigned short width, unsigned short height,
		unsigned short horizontalActiveTime, unsigned short verticalActiveTime)
{
	u32 row,col;
	u32 V_offset;
	u32 pixel_color,pixel_temp,pixel_gray;

	  for (row = 0; row < height ;row++) {
		  V_offset = row * horizontalActiveTime;

	  	  for (col = 0; col < width ;col++) {
	  		{
					pixel_color	= Xil_In32(ImgIn_BaseAddr + (V_offset + col) * 4); //Get the colored image
					pixel_temp 	= (int)(((pixel_color & 0xff) + ((pixel_color & 0xff00)>>8) + ((pixel_color & 0xff0000)>>16))/3); // convert to gray
					pixel_gray 	= (pixel_temp & 0xff) | ((pixel_temp & 0xff)<<8) | ((pixel_temp & 0xff)<<16) ;
					Xil_Out32( ImgOut_BaseAddr + ((V_offset+col) * 4) , pixel_gray ); // write to another location
	  		}
	  	  }
	  	}
}
