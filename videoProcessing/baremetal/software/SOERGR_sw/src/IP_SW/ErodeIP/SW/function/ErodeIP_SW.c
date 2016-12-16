#include "ErodeIP_SW.h"
#include "xil_types.h"
#include "xil_io.h"

void Erode(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr, unsigned short width, unsigned short height, unsigned short horizontalActiveTime, unsigned short verticalActiveTime)
 {
 	int pix_i = 0, pix_j = 0;
 	int x_win = 0, y_win = 0;
 	u32 pixel_temp, pixel_min;
 	unsigned short line_buffer[3][640]; 	// storing 3 image rows for edge detection

 	/* Clearing 0th row : Sobel filter is not applied on 0th ROW: to have consistent data flow */
 	for(pix_j = 0; pix_j < width; pix_j++)
 	{
 		Xil_Out32( ImgOut_BaseAddr + (pix_j * 4) , 0x0 );
 	}

 	/* move initial(i-1 & i) 2-rows of RGB pixel data into line buffers  */
 	pix_i = 0;
 	for(pix_j = 0; pix_j < width ; pix_j++)
 	{
 		line_buffer[0][pix_j] = Xil_In32(ImgIn_BaseAddr + (pix_i * horizontalActiveTime + pix_j) * 4); //Get the original image's pixel
 	}
 	pix_i = 1;
 	for(pix_j = 0; pix_j < width ; pix_j++)
 	{
 		line_buffer[1][pix_j] = Xil_In32(ImgIn_BaseAddr + (pix_i * horizontalActiveTime + pix_j) * 4); //Get the original image's pixel
 	}

 	for(pix_i = 1; pix_i < height-1; pix_i++) /* -1 as we read from pix_i+1 th row*/
 		{	/* copy (pix_i) row into line_buffer, i.e: line_buffer[2] <-- row[pix_i] */
 			for(pix_j = 0; pix_j < width ; pix_j++)
 			{
 				line_buffer[2][pix_j] = Xil_In32(ImgIn_BaseAddr + ((pix_i+1) * horizontalActiveTime + pix_j) * 4) & 0x00ffffff; //Get the original image's pixel
 			}
 			Xil_Out32( (ImgOut_BaseAddr + (pix_i * horizontalActiveTime + 0) * 4) , 0x0 ); // making first pixel zero.

 			/* compute Erode over (pix_i - 1) pixels, */
 			for(pix_j = 1; pix_j < width-1  ; pix_j++)
 			{
 				pixel_min = line_buffer[1][pix_j];
 				for (x_win = 0; x_win<3 ; x_win++){
 					for (y_win = -1; y_win<2 ; y_win++){
 						pixel_temp= line_buffer[x_win][pix_j+y_win];
 						if ( pixel_temp < pixel_min)
 							pixel_min = pixel_temp;
 					}
 				}
 				// write the edge value into RGB
 				Xil_Out32( (ImgOut_BaseAddr + (pix_i * horizontalActiveTime + pix_j) * 4) , pixel_min );

 			}	/* --Sobel_filtering(row[pix_i - 1]) complete */

 			/* copy the history of pixel data */
 			/* row[pix_i - 2] <-- row[pix_i - 1], i.e: line_buf[0] <-- line_buf[1] */
 			/* row[pix_i - 1] <-- row[pix_i], i.e: line_buf[1] <-- line_buf[2] */
 			for(pix_j = 0; pix_j < (width /* PBJ no need of '+ 1' */); pix_j++)
 			{
 				line_buffer[0][pix_j] = line_buffer[1][pix_j] ;
 				line_buffer[1][pix_j] = line_buffer[2][pix_j] ;
 			}
 		}

 }

