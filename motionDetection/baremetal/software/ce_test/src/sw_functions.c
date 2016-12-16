
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "xil_io.h"
#include "xparameters.h"
#include "cf_hdmi.h"
#include "sw_functions.h"
#include "xil_cache.h"
#include "xdmaps.h"
#include "profile_cnt.h"

/******************* Macros and Constants Definitions ************************/

#define ABS(x)					(x < 0 ? -x : x)
//                                    red      orange    light orange   yellow   light  green dark green  light blue dark blue
const unsigned int colors[8]={0x00FF0000, 0x00FF4000, 0x00FFC000, 0x00FFFF00, 0x0080FF00, 0x0000C000, 0x0000FFFF, 0x000000FF};
/* Takes the Raw image from DDR image space
 * converts it to Gray scale and writes back to DDR
 * at a different location
 */
void ConvToGray(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,
		unsigned short horizontalActiveTime)
{
	u32 row,col;
	u32 V_offset, VH_offset;
	u32 pixel_color,pixel_temp,pixel_gray;

	  for (row = 0; row < 480 ;row++) {
		  V_offset = row * horizontalActiveTime;

	  	  for (col = 0; col < 640 ;col++) {
	  		{
					pixel_color	= Xil_In32(ImgIn_BaseAddr + (V_offset + col) * 4); //Get the colored image
					pixel_temp 	= (int)(((pixel_color & 0xff) + ((pixel_color & 0xff00)>>8) + ((pixel_color & 0xff0000)>>16))/3); // convert to gray
					pixel_gray 	= (pixel_temp & 0xff) | ((pixel_temp & 0xff)<<8) | ((pixel_temp & 0xff)<<16) ;
					Xil_Out32( ImgOut_BaseAddr + ((V_offset+col) * 4) , pixel_gray ); // write to another location
	  		}
	  	  }
	  	}
}
/* *** Imititating the Vivado_HLS implementation of grayscale conversion
 * Converts the Raw image to Gray scale and writes back to DDR
 * at a different location 
 */
void ConvToGrayHLS(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short horizontalActiveTime)
{
	int i,j,V_offset;
	//kernel_CvtColor<CONVERSION>  kernel_opr;
	int    	cols=640;
	int   	rows=480;
	float 	par[3] = {0.114,0.587,0.299};

	int _s;	// src pixel
	int _d;	// dst pixel
	for(i= 0; i < rows; i++) {
		V_offset = i * horizontalActiveTime;
		for (j= 0; j < cols; j++) {
			_s	= Xil_In32(ImgIn_BaseAddr + (V_offset + j) * 4); //Get the colored image pixel
			kernel_apply(&_s,&_d, par);
			Xil_Out32( ImgOut_BaseAddr + ((V_offset+j) * 4) , _d ); // write to another locationgray pixel
		}
	}
}

void kernel_apply(int* _src, int* _dst,float _par[3])
{
	int b,g,r;
	r=_par[0]*( *_src & 0xff		);				//_src.val[2];
	b=_par[1]*((*_src & 0xff00	) >> 8);	//_src.val[1];
	g=_par[2]*((*_src & 0xff0000) >> 16);	//_src.val[0];
	int c;
	c=r+g+b;
	if(c>255)
	{
		c=255;
	}
	*_dst = (c & 0xff) | ((c & 0xff)<<8) | ((c & 0xff)<<16) ;

}

/* Get an Image frame from DDR memory and perform Edge Detection on this
 * Store the processed Image back to DDR at a different location
 * ImgIn_BaseAddr	: VIDEO_BASEADDR
 * ImgOut_BaseAddr 	: PROC_VIDEO_BASEADDR
 */
void EdgeDetection(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,
		unsigned short width, unsigned short height, unsigned short h_ActiveTime)
{
	short x_weight, y_weight, edge_weight;
	unsigned char edge_val;
	unsigned char in_R, in_G, in_B;
	int pix_i = 0, pix_j = 0;
	u32 pixel_temp;

	/* Clearing 0th row : Sobel filter is not applied on 0th ROW: to have consistent data flow */
	for(pix_j = 0; pix_j < width; pix_j++)
	{
		Xil_Out32( ImgOut_BaseAddr + (pix_j * 4) , 0x0 );
	}

	/* move initial(i-1 & i) 2-rows of RGB pixel data into line buffers after conv. into Y(luminance) */
	pix_i = 0;
	for(pix_j = 0; pix_j < width ; pix_j++)
	{
		pixel_temp = Xil_In32(ImgIn_BaseAddr + (pix_i * h_ActiveTime + pix_j) * 4); //Get the original image's pixel
		in_B = (pixel_temp & 0x000000FF);
		in_G = ((pixel_temp >>8 ) & 0x000000FF);
		in_R = ((pixel_temp >>16) & 0x000000FF);
		line_buffer[0][pix_j]  = rgb2y(in_R, in_G, in_B) ;
	}
	pix_i = 1;
	for(pix_j = 0; pix_j < width ; pix_j++)
	{
		pixel_temp = Xil_In32(ImgIn_BaseAddr + (pix_i * h_ActiveTime + pix_j) * 4); //Get the original image's pixel
		in_B = (pixel_temp & 0x000000FF);
		in_G = ((pixel_temp >>8 ) & 0x000000FF);
		in_R = ((pixel_temp >>16) & 0x000000FF);
		line_buffer[1][pix_j]  = rgb2y(in_R, in_G, in_B) ;
	}

	for(pix_i = 1; pix_i < height-1; pix_i++) /* -1 as we read from pix_i+1 th row*/
		{	/* copy (pix_i) row into line_buffer, i.e: line_buffer[2] <-- row[pix_i] */
			for(pix_j = 0; pix_j < width ; pix_j++)
			{
				pixel_temp = Xil_In32(ImgIn_BaseAddr + ((pix_i+1) * h_ActiveTime + pix_j) * 4); //Get the original image's pixel
				in_B = (pixel_temp & 0x000000FF);
				in_G = ((pixel_temp >>8 ) & 0x000000FF);
				in_R = ((pixel_temp >>16) & 0x000000FF);
				line_buffer[2][pix_j]  = rgb2y(in_R, in_G, in_B) ;
			}

			Xil_Out32( (ImgOut_BaseAddr + (pix_i * h_ActiveTime + 0) * 4) , 0x0 ); // making first pixel zero.

			/* compute Sobel filtering over (pix_i - 1) pixels, i.e: sobel_filtering(&line_buffer[1][0])*/
			for(pix_j = 1; pix_j < width-1  ; pix_j++)
			{
				x_weight =  line_buffer[0][pix_j-1]*(-1) + line_buffer[0][pix_j+1]	 + line_buffer[1][pix_j-1]*(-2) + line_buffer[1][pix_j+1]*(2) 	+ line_buffer[2][pix_j-1]*(-1) + line_buffer[2][pix_j+1] ;
				y_weight =  line_buffer[0][pix_j-1]	     + line_buffer[0][pix_j]*(2) + line_buffer[0][pix_j+1] 		+ line_buffer[2][pix_j-1]*(-1)  + line_buffer[2][pix_j]*(-2)   + line_buffer[2][pix_j+1]*(-1) ;
				edge_weight = ABS(x_weight) + ABS(y_weight);
				//Added RGB fix-06-dec-12
				if(edge_weight < 255)
					edge_val = (255-(unsigned char)(edge_weight));
				else
					edge_val = 0;

				if(edge_val > 200)
					edge_val = 255;
				else if(edge_val < 100)
					edge_val = 0;
				pixel_temp = (edge_val) | (edge_val<<8) | (edge_val<<16) | 0xFF000000 ;
				// write the edge value into RGB
				Xil_Out32( (ImgOut_BaseAddr + (pix_i * h_ActiveTime + pix_j) * 4) , pixel_temp );

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


unsigned char rgb2y(unsigned char R, unsigned char G, unsigned char B)
{
	unsigned char y;
	y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
	return y;
}

/*
 * For opticalflow
 */

void ConvGray24ToGray8(int width,int height,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr)
{
	u8 src;
	u8 dest;
	int rows = height;
	int cols = width;
	int i, j, k, V_offset, V_offset_2;

	V_offset_2 = 0;
	V_offset = width * (height-1);

	for(i = 0; i < rows; i++)
	{
		k = 0;
		for(j = 0; j < cols; j++)
		{
			src = Xil_In8(ImgIn_BaseAddr + V_offset_2*3 + k );
			k = k + 3;

			dest = src;

			Xil_Out8(ImgOut_BaseAddr + V_offset + j, dest );
		}
		V_offset_2 = V_offset_2 + cols;
		V_offset = V_offset - cols;
	}
}

void ConvGray8ToGray32(int width,int height,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr)
{
	int 	i,j,V_offset;
	int    	cols = width;
	int   	rows = height;

	u8 _s;	// src pixel
	u32 _d;	// dst pixel


	V_offset = 0;
	for(i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			_s = Xil_In8(ImgIn_BaseAddr + V_offset + j ); //Get the gray image pixel
			_d = 0x00<<24|_s << 16 | _s<< 8 | _s;
			Xil_Out32(ImgOut_BaseAddr + ((V_offset + j) * 4 ), _d ); //write to another location gray pixel

		}
		V_offset = V_offset + cols;
	}
}

void ImageSmoothGray8(int width,int height,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr){
	int 	i,j,V_offset;
	int    	cols = width;
	int   	rows = height;

	u8 _s,_sul,_sum,_sur,_sml,_smr,_sbl,_sbm,_sbr;	// src pixel
	u8 _d;	// dst pixel


	V_offset = 0;
	for(i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			if (i >0 && i<rows-1 && j>0 && j<cols-1){
				_s = Xil_In8(ImgIn_BaseAddr + V_offset + j );
				_sul = Xil_In8(ImgIn_BaseAddr + V_offset + j-1-cols);
				_sum = Xil_In8(ImgIn_BaseAddr + V_offset + j-cols);
				_sur = Xil_In8(ImgIn_BaseAddr + V_offset + j+1-cols);
				_sml = Xil_In8(ImgIn_BaseAddr + V_offset -1);
				_smr = Xil_In8(ImgIn_BaseAddr + V_offset +1);
				_sbl = Xil_In8(ImgIn_BaseAddr + V_offset + j-1+cols);
				_sbm = Xil_In8(ImgIn_BaseAddr + V_offset + j+cols);
				_sbr = Xil_In8(ImgIn_BaseAddr + V_offset + j+1+cols);
				_d = (_sul+(_sum<<1)+_sur+(_sml<<1)+(_s<<2)+(_smr<<1)+_sbl+(_sbm<<1)+_sbr)>>4;
				Xil_Out8(ImgOut_BaseAddr + V_offset + j, _d ); //write to another location gray pixel
			}
			else
			{
				Xil_Out8(ImgOut_BaseAddr + V_offset + j, 0x00);
			}

		}
		V_offset = V_offset + cols;
	}
}

void ImageOutput8(int width,int height,unsigned long ImgIn_BaseAddr){
	int 	i,j,V_offset;
	int    	cols = width;
	int   	rows = height;

	u8 _s;


	V_offset = 0;
	for(i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			_s = Xil_In8(ImgIn_BaseAddr + V_offset + j ); //Get the gray image pixel
			xil_printf("%X",_s);
			if (j != cols-1)
				xil_printf(" ",_s);
		}
		V_offset = V_offset + cols;
		xil_printf("\n");
	}
}

void ImageOutput32(int width,int height,unsigned long ImgIn_BaseAddr){
	int 	i,j,V_offset;
	int    	cols = width;
	int   	rows = height;

	u32 _s;


	V_offset = 0;
	for(i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			_s = Xil_In32(ImgIn_BaseAddr + (V_offset + j)*4); //Get the gray image pixel
			if (j != cols-1)
				xil_printf("%X ",_s);
			else
				xil_printf("%X",_s);
		}
		xil_printf("\n");
	}
}

void CensusEngine8to32(int width,int height,unsigned char epsilon,unsigned char csd1,unsigned char csd2,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr){
		int min_w, max_w, min_h, max_h;
	    int z,o2l, o2m, o2r, m2l, m2r, u2l, u2m, u2r,
	        o1l, o1m, o1r, m1l, m1r, u1l, u1m, u1r;
	    int iiw,iih;

	    unsigned char cz,co2l, co2m, co2r, cm2l, cm2r, cu2l, cu2m, cu2r,
        co1l,co1m, co1r, cm1l, cm1r, cu1l, cu1m, cu1r;
	    unsigned char cl, ch;

	    u32 c;

	    min_w = csd2;
	    max_w = width-csd2-1;
	    min_h = csd2;
	    max_h = height-csd2-1;

	    z=-1;
		o2l= -csd2*width-csd2;  o2m= -csd2*width;  o2r= -csd2*width+csd2;
		m2l= -csd2;                  m2r=  csd2;
		u2l=  csd2*width-csd2;  u2m=  csd2*width;  u2r=  csd2*width+csd2;
		o1l= -csd1*width-csd1;  o1m= -csd1*width;  o1r= -csd1*width+csd1;
		m1l= -csd1;                  m1r=  csd1;
		u1l=  csd1*width-csd1;  u1m=  csd1*width;  u1r=  csd1*width+csd1;
	   for (iiw=0; iiw<width; iiw++) {
	           for (iih=0; iih<height; iih++) {
	               // Census Transformation:
	        	   if (iiw>=min_w && iiw<=max_w && iih >= min_h && iih <= max_h){
					   z = width*iih + iiw;
					   cz = Xil_In8(ImgIn_BaseAddr+z);
					   cl = cz - epsilon;
					   if (cl < 0)
						   cl = 0;
					   ch = cz + epsilon;
					   if (ch > 255)
						   ch =255;
					   co2l = Xil_In8(ImgIn_BaseAddr+z+o2l);
					   co2m = Xil_In8(ImgIn_BaseAddr+z+o2m);
					   co2r = Xil_In8(ImgIn_BaseAddr+z+o2r);
					   cm2l = Xil_In8(ImgIn_BaseAddr+z+m2l);
					   cm2r = Xil_In8(ImgIn_BaseAddr+z+m2r);
					   cu2l = Xil_In8(ImgIn_BaseAddr+z+u2l);
					   cu2m = Xil_In8(ImgIn_BaseAddr+z+u2m);
					   cu2r = Xil_In8(ImgIn_BaseAddr+z+u2r);
					   co1l = Xil_In8(ImgIn_BaseAddr+z+o1l);
					   co1m = Xil_In8(ImgIn_BaseAddr+z+o1m);
					   co1r = Xil_In8(ImgIn_BaseAddr+z+o1r);
					   cm1l = Xil_In8(ImgIn_BaseAddr+z+m1l);
					   cm1r = Xil_In8(ImgIn_BaseAddr+z+m1r);
					   cu1l = Xil_In8(ImgIn_BaseAddr+z+u1l);
					   cu1m = Xil_In8(ImgIn_BaseAddr+z+u1m);
					   cu1r = Xil_In8(ImgIn_BaseAddr+z+u1r);
					   c = 0x00000000;
					   c = ct32(co2l,c,cl,ch);c = ct32(co2m,c,cl,ch);c = ct32(co2r,c,cl,ch);
					   c = ct32(cm2l,c,cl,ch);c = ct32(cm2r,c,cl,ch);
					   c = ct32(cu2l,c,cl,ch);c = ct32(cu2m,c,cl,ch);c = ct32(cu2r,c,cl,ch);
					   c = ct32(co1l,c,cl,ch);c = ct32(co1m,c,cl,ch);c = ct32(co1r,c,cl,ch);
					   c = ct32(cm1l,c,cl,ch);c = ct32(cm1r,c,cl,ch);
					   c = ct32(cu1l,c,cl,ch);c = ct32(cu1m,c,cl,ch);c = ct32(cu1r,c,cl,ch);
					   Xil_Out32(ImgOut_BaseAddr+4*z,c);
	        	   }
	        	   else
	        	   {
	        		   Xil_Out32(ImgOut_BaseAddr+4*z,0x00000000);
	        	   }
	           }
	       }
}

inline u32 ct32(unsigned char data,u32 in,unsigned char central_low,unsigned char central_high) {
    in = in << 2;
    if ( data > central_high)
        in = in | 0x01;
    else if ( data < central_low)
        in = in | 0x02;
    return in;
}

int MatchingEngine32(int width,int height,int w_search,int h_search,unsigned long ImgIn_BaseAddr1,unsigned long ImgIn_BaseAddr2,unsigned long list_x1,unsigned long list_y1,unsigned long list_x2,unsigned long list_y2){
	int min_w, max_w, min_h, max_h,z,zdiff,ct_corr;
	int short cpos_x1,cpos_y1, cpos_x2,cpos_y2;
	int total_matches=0;
	int iih,iiw,kkh,kkw;
	int ox1=0,oy1=0,ox2=0,oy2=0;
	u32 cens_1, cens_2;

	min_w = w_search;
	max_w = width - (w_search+1);
	min_h = h_search;
	max_h = height - (h_search+1);

	z=-1;
	for (iih=min_h; iih<=max_h; iih++) {
		for (iiw=min_w; iiw<=max_w; iiw++) {
			z = width*iih + iiw;
			cens_1 = Xil_In32(ImgIn_BaseAddr1+4*z);

			ct_corr=0;
			cpos_x1=0,cpos_y1=0, cpos_x2=0,cpos_y2=0;
			for (kkh= -h_search; kkh<=h_search; kkh++) {
				for (kkw= -w_search; kkw<=w_search; kkw++) {
					zdiff = width*kkh + kkw;
					cens_2 = Xil_In32(ImgIn_BaseAddr2+4*(z+zdiff));
					if(cens_1==cens_2) {
						ct_corr++;
						cpos_x1=iiw;
						cpos_y1=iih;
						cpos_x2=iiw+kkw;
						cpos_y2=iih+kkh;
					}
					if(ct_corr>1)
						goto afterloop;
				}
			}
			afterloop:
			if (1==ct_corr) {
				Xil_Out16(list_x1+ox1*2,cpos_x1);
				Xil_Out16(list_y1+oy1*2,cpos_y1);
				Xil_Out16(list_x2+ox2*2,cpos_x2);
				Xil_Out16(list_y2+oy2*2,cpos_y2);
				ox1++;
				oy1++;
				ox2++;
				oy2++;
				total_matches++;
			}
		}
	}
	return total_matches;
}

int MatchingEngine32HWO(int width,int height,int w_search,int h_search,unsigned long ImgIn_BaseAddr1,unsigned long ImgIn_BaseAddr2,unsigned long ImgOut_BaseAddr){
	int min_w, max_w, min_h, max_h,z,zdiff,ct_corr;
	int short cpos_x1,cpos_y1, cpos_x2,cpos_y2;
	int total_matches=0;
	int iih,iiw,kkh,kkw;
	u32 cens_1, cens_2;

	min_w = w_search;
	max_w = width - (w_search+1);
	min_h = h_search;
	max_h = height - (h_search+1);

	z=-1;
	for (iih=min_h; iih<=max_h; iih++) {
		for (iiw=min_w; iiw<=max_w; iiw++) {
			z = width*iih + iiw;
			cens_1 = Xil_In32(ImgIn_BaseAddr1+4*z);

			ct_corr=0;
			cpos_x1=0,cpos_y1=0, cpos_x2=0,cpos_y2=0;
			for (kkh= -h_search; kkh<=h_search; kkh++) {
				for (kkw= -w_search; kkw<=w_search; kkw++) {
					zdiff = width*kkh + kkw;
					cens_2 = Xil_In32(ImgIn_BaseAddr2+4*(z+zdiff));
					if(cens_1==cens_2) {
						ct_corr++;
						cpos_x1=iiw;
						cpos_y1=iih;
						cpos_x2=kkw;
						cpos_y2=kkh;
					}
					if(ct_corr>1)
						goto afterloop;
				}
			}
			afterloop:
			if (1==ct_corr) {
				Xil_Out32(ImgOut_BaseAddr+4*total_matches,(((u32)cpos_x1)<<22)+(((u32)cpos_y1)<<12)+((((int)cpos_x2)&0x3f)<<6)+(((int)cpos_y2)&0x3f));
				total_matches++;
			}
		}
	}
	return total_matches;
}

void DrawVector32(int width,int height,unsigned long list_x1,unsigned long list_y1,unsigned long list_x2,unsigned long list_y2,int matchcounts,unsigned long ImgOut_BaseAddr,int flag){
int i;
int short x1,y1,x2,y2;
for (i=0;i<matchcounts;i++){
		x1 = Xil_In16(list_x1+2*i);
		y1 = Xil_In16(list_y1+2*i);
		x2 = Xil_In16(list_x2+2*i);
		y2 = Xil_In16(list_y2+2*i);
		if (flag){
			if (!((ABS((x2-x1)) + ABS((y2-y1))) <=2 ))
				line(ImgOut_BaseAddr,width,height,x1,y1,x2,y2);
		}else{
			line(ImgOut_BaseAddr,width,height,x1,y1,x2,y2);
		}
	}
}

void DrawVector32eee(int width,int height,int matchcounts,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,int flag){
int i;
int short x1,y1,x2,y2;
for (i=0;i<matchcounts;i++){
		x1 = Xil_In16(ImgIn_BaseAddr+8*i);
		y1 = Xil_In16(ImgIn_BaseAddr+8*i+2);
		x2 = Xil_In16(ImgIn_BaseAddr+8*i+4);
		y2 = Xil_In16(ImgIn_BaseAddr+8*i+6);
		if (flag){
			if (!((ABS((x2-x1)) + ABS((y2-y1))) <=2 ))
				line(ImgOut_BaseAddr,width,height,x1,y1,x2,y2);
		}else{
			line(ImgOut_BaseAddr,width,height,x1,y1,x2,y2);
		}
	}
}

void DrawVector32hw(int width,int height,unsigned long list,int matchcounts,unsigned long ImgOut_BaseAddr,int flag){
int i;
u32 feature;
int short x1,y1,x2,y2;
for (i=0;i<matchcounts;i++){
		feature = Xil_In32(list+4*i);
		x1 = (feature & 0xffc00000)>>22;
		y1 = (feature & 0x003ff000)>>12;
		if ((feature & 0x00000800) == 0x00000800){
			x2 = x1-(((~feature & 0x000007c0)>>6)+1);
		}
		else
		{
			x2 = x1+((feature & 0x000007c0)>>6);
		}

		if ((feature & 0x00000020) == 0x00000020){
			y2 = y1-((~feature & 0x0000001f)+1);
		}
		else
		{
			y2 = y1+(feature & 0x0000001f);
		}

		if (flag){
			if (!((ABS((x2-x1)) + ABS((y2-y1))) <=2 ))
				line(ImgOut_BaseAddr,width,height,x1,y1,x2,y2);
		}else{
			line(ImgOut_BaseAddr,width,height,x1,y1,x2,y2);
		}
	}
}

void line(unsigned long ImgIn_BaseAddr,int width,int height,int x,int y,int x2, int y2) {

    int w = x2 - x ;
    int h = y2 - y ;
    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0 ;
    if (w<0) dx1 = -1 ; else if (w>0) dx1 = 1 ;
    if (h<0) dy1 = -1 ; else if (h>0) dy1 = 1 ;
    if (w<0) dx2 = -1 ; else if (w>0) dx2 = 1 ;
    int longest = ABS(w) ;
    int shortest = ABS(h) ;
    if (!(longest>shortest)) {
        longest = ABS(h) ;
        shortest = ABS(w) ;
        if (h<0) dy2 = -1 ; else if (h>0) dy2 = 1 ;
        dx2 = 0 ;
    }
    int numerator = longest >> 1 ;
    int i;
    for (i=0;i<=longest;i++) {
        putpixel((unsigned int*)ImgIn_BaseAddr,width,height,x,y,Distance2Color(w,h,1)) ;
        numerator += shortest ;
        if (!(numerator<longest)) {
            numerator -= longest ;
            x += dx1 ;
            y += dy1 ;
        } else {
            x += dx2 ;
            y += dy2 ;
        }
    }

}

unsigned int Distance2Color(signed char DeltaX, signed char DeltaY, unsigned char N)
{
		if (0 == (ABS(DeltaX) + ABS(DeltaY)))
		{
		  return colors[(ABS(DeltaX) + ABS(DeltaY))];
		}
		else if (1 == (ABS(DeltaX) + ABS(DeltaY)) )
		{
		  return colors[(ABS(DeltaX) + ABS(DeltaY))];
		}
		else
		{
		   return colors[((ABS(DeltaX) + ABS(DeltaY)) >> N)];
		}
}

void putpixel(unsigned int* ImgIn_BaseAddr,int width,int height,int x,int y,unsigned int color){
	if(x > 0 && y > 0 && x < width && y < height)
	ImgIn_BaseAddr[y*width+x] = color;
}

void differ(int width,int height,unsigned long ImgIn_BaseAddr1,unsigned long ImgIn_BaseAddr2,unsigned long ImgOut_BaseAddr){
	int i,j;
	u32 s1,s2;
	for (i=0;i<height;i++){
		for (j=0;j<width;j++){
			s1=Xil_In32(ImgIn_BaseAddr1+(i*width+j)*4);
			s2=Xil_In32(ImgIn_BaseAddr2+(i*width+j)*4);
			if (i*width+j >= 5000 && i*width+j<=5500){
			}
			if (s1==s2)
				Xil_Out32(ImgOut_BaseAddr+(i*width+j)*4,0xffffffff);
			else
				Xil_Out32(ImgOut_BaseAddr+(i*width+j)*4,0x00000000);
		}
	}
}

void presample(unsigned long ImgIn_BaseAddr, unsigned long featureimg,int matchcount,int width, int height){
	int i,j;
	u32 feature;
	int short x1,y1,x2,y2;

	unsigned int short* p_featureimg = (unsigned int short*)featureimg;

	for (i=0;i<width;i++){
		for (j=0;j<height;j++){
			p_featureimg[(j*width+i)*4]=0xffff;
			p_featureimg[(j*width+i)*4+1]=0xffff;
			p_featureimg[(j*width+i)*4+2]=0xffff;
			p_featureimg[(j*width+i)*4+3]=0xffff;
		}
	}
	for (i=0;i<matchcount;i++){
		feature = Xil_In32(ImgIn_BaseAddr+4*i);
		x1 = (feature & 0xffc00000)>>22;
		y1 = (feature & 0x003ff000)>>12;
		if ((feature & 0x00000800) == 0x00000800){
			x2 = x1-(((~feature & 0x000007c0)>>6)+1);
		}
		else
		{
			x2 = x1+((feature & 0x000007c0)>>6);
		}

		if ((feature & 0x00000020) == 0x00000020){
			y2 = y1-((~feature & 0x0000001f)+1);
		}
		else
		{
			y2 = y1+(feature & 0x0000001f);
		}
		p_featureimg[(y2*width+x2)*4]=x1;
		p_featureimg[(y2*width+x2)*4+1]=y1;
		p_featureimg[(y2*width+x2)*4+2]=x2;
		p_featureimg[(y2*width+x2)*4+3]=y2;
	}
}

int sample_flow_vectors(unsigned long featureimg,unsigned long compvector,int width, int height,int select, int blk_size_vect_avg)
{
	int i,j,k,l;
	unsigned int short veccount,tmp;
	int x1c,y1c;
	int compcount;
	unsigned int short* p_featureimg = (unsigned int short*)featureimg;
	unsigned int short* p_compvector = (unsigned int short*)compvector;

	compcount=0;

	if (select == 1){
		for(i=0; i<width; i+=blk_size_vect_avg) {
			for (j=0;j<height;j+=blk_size_vect_avg) {
				x1c = 0;
				y1c = 0;
				veccount = 0;
				for(k=i; k<(i+blk_size_vect_avg); k++) {
					for (l=j; l<(j+blk_size_vect_avg); l++) {
						tmp = p_featureimg[(k+l*width)*4];
						if(tmp != 0xffff) {
							x1c += p_featureimg[(k+l*width)*4];
							y1c += p_featureimg[(k+l*width)*4+1];
							veccount++;
						}
					}
				}
				if(veccount > 0) {
					p_compvector[compcount*4] = x1c/veccount;
					p_compvector[compcount*4+1] = y1c/veccount;
					p_compvector[compcount*4+2] = i;
					p_compvector[compcount*4+3] = j;
					compcount++;
				}
			}
		}
		return compcount;
	}else if (select == 2){
		for(i=0; i<width; i+=blk_size_vect_avg) {
			for (j=0;j<height;j+=blk_size_vect_avg) {
				for(k=i; k<(i+blk_size_vect_avg); k++) {
					for (l=j; l<(j+blk_size_vect_avg); l++) {
//				for(k=(i+blk_size_vect_avg)-1;k>=i;k--) {
//					for (l=(j+blk_size_vect_avg)-1;l>=j;l--) {
						tmp = p_featureimg[(k+l*width)*4];
						if(tmp != 0xffff) {
							break;
						}
					}
					if (tmp != 0xffff) break;
				}
				if (tmp != 0xffff){
					p_compvector[compcount*4] = p_featureimg[(k+l*width)*4];
					p_compvector[compcount*4+1] = p_featureimg[(k+l*width)*4+1];
					p_compvector[compcount*4+2] = i;
					p_compvector[compcount*4+3] = j;
					compcount++;
				}
			}
		}
		return compcount;
	}else if (select == 3){
		for (i=0;i<width;i++){
			for (j=0;j<height;j++){
				tmp = p_featureimg[(i+j*(width/8))*4];
				if (tmp != 0xffff){
					p_compvector[compcount*4] = p_featureimg[(i+j*(width/8))*4];
					p_compvector[compcount*4+1] = p_featureimg[(i+j*(width/8))*4+1];
					p_compvector[compcount*4+2] = p_featureimg[(i+j*(width/8))*4+2];
					p_compvector[compcount*4+3] = p_featureimg[(i+j*(width/8))*4+3];
//					printf("x1: %d, y1:%d, x2: %d, y2:%d\n",p_featureimg[(i+j*(width/8))*4],p_featureimg[(i+j*(width/8))*4+1],p_featureimg[(i+j*(width/8))*4+2],p_featureimg[(i+j*(width/8))*4+3]);
					compcount++;
				}
			}
		}
		return compcount;
	}else{
		for (i=0;i<width;i++){
			for (j=0;j<height;j++){
				tmp = p_featureimg[(i+(j*width))*4];
				if (tmp != 0xffff){
					p_compvector[compcount*4] = p_featureimg[(i+j*width)*4];
					p_compvector[compcount*4+1] = p_featureimg[(i+j*width)*4+1];
					p_compvector[compcount*4+2] = p_featureimg[(i+j*width)*4+2];
					p_compvector[compcount*4+3] = p_featureimg[(i+j*width)*4+3];
					compcount++;
				}
			}
		}
		return compcount;
	}
	return 0;
}


int gen_model_param(int x1, int y1, int vx1, int vy1, int x2, int y2, int vx2, int vy2, float *model_param)
{
	float temp;

	/* xc -> model_param[0], yc -> model_param[1], D -> model_param[2], R -> model_param[3] */
    temp = (float)((vx1 *(vx1 - (2*vx2))) + (vx2 * vx2) + (vy1 * vy1) - (vy2 * ((2*vy1) - vy2)));
	if(temp == 0.0) { /* Check to prevent division by zero */
		return(0);
	}
	model_param[0] = ((( vx1 * ((-vx2*x1) + (vx1*x2) - (vx2*x2) + (vy2*y1) - (vy2*y2)))  + (vy1 * ((-vy2*x1) + (vy1*x2) - (vy2*x2) - (vx2*y1) + (vx2*y2) )) + (x1 * ((vy2* vy2) + (vx2 * vx2)))) / temp);

	temp = (float)((vx1 *(vx1 - (2*vx2))) + (vx2 * vx2) + (vy1 * vy1) - (vy2 * ((2*vy1) - vy2)));
	if(temp == 0.0) { /* Check to prevent division by zero */
		return(0);
	}
	model_param[1] = (((vx2 * ((vy1*x1) - (vy1*x2) - (vx1*y1) + (vx2*y1) - (vx1*y2))) + (vy2 * ((-vx1*x1) + (vx1*x2) - (vy1*y1) + (vy2*y1) - (vy1*y2))) + (y2 * ((vx1 * vx1) + (vy1 * vy1)))) / temp);

    temp = (float)((x1 * (x1 - (2*x2))) + (x2 * x2) + (y1 * (y1 - (2*y2))) + (y2 * y2));
	if(temp == 0.0) { /* Check to prevent division by zero */
		return(0);
	}
	model_param[2] = ((((x1 - x2) * (vx1 - vx2)) + ((y1 - y2) * (vy1 - vy2))) / temp);

	temp = (float)((x1 * (x1 - (2*x2))) + (x2 * x2) + (y1 * (y1 - (2*y2))) + (y2 * y2));
	if(temp == 0.0) { /* Check to prevent division by zero */
		return(0);
	}
	model_param[3] = ((((x1 - x2) * (vy1 - vy2)) + ((y2 - y1) * (vx1 - vx2))) / temp);

    return(1);
}

int gen_firstOrderFlow_model(int flow_vector_count, unsigned long ImgIn_BaseAddr, float *model_param)
{
	int   rand_num, upper_limit, lower_limit;
	int   x1, x2, y1, y2, vx1, vx2, vy1, vy2, count=0;
	int  ret;

	upper_limit = flow_vector_count - 1;
	lower_limit = 0;
	while(1) {
		/* Select two motion vectors at random */
		rand_num = lower_limit + rand() % (upper_limit - lower_limit); /* Gen random number with the array size */
		x1  = Xil_In16(ImgIn_BaseAddr+rand_num*8);
		y1  = Xil_In16(ImgIn_BaseAddr+rand_num*8+2);
		vx1 = Xil_In16(ImgIn_BaseAddr+rand_num*8+4) - x1;
		vy1 = Xil_In16(ImgIn_BaseAddr+rand_num*8+6) - y1;


		rand_num = lower_limit + rand() % (upper_limit - lower_limit); /* Gen random number with the array size */
		x2  = Xil_In16(ImgIn_BaseAddr+rand_num*8);
		y2  = Xil_In16(ImgIn_BaseAddr+rand_num*8+2);
		vx2 = Xil_In16(ImgIn_BaseAddr+rand_num*8+4) - x2;
		vy2 = Xil_In16(ImgIn_BaseAddr+rand_num*8+6) - y2;

		if((vx1==0) && (vy1==0) && (vx2==0) && (vy2==0)) {
			model_param[0] = 0;
			model_param[1] = 0;
			model_param[2] = 0;
			model_param[3] = 0;
			return(1);
		}

		/* Function to generate model parameters according to first order flow (xc, yc, D and R) */
		ret = gen_model_param(x1, y1, vx1, vy1, x2, y2, vx2, vy2, model_param);
		if(ret == 1) {
			return(1);
		}
		else if((ret == 0) && (count > 100)) {
			return(0);
		}
		count++;
	}
}

void gen_firstOrderFlow_vectors_4(float *model_param, int flow_vector_count, unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr)
{
	float temp_x, temp_y;
	int i;

	/* Compute new set of motion vectors for each point specified in the flow_vector_array */
    for(i = 0; i < flow_vector_count; i++) {
		temp_x  = ((float)Xil_In16(ImgIn_BaseAddr+i*8)) - model_param[0];
        temp_y  = ((float)Xil_In16(ImgIn_BaseAddr+i*8+2))-model_param[1];
		Xil_Out16(ImgOut_BaseAddr+i*8,Xil_In16(ImgIn_BaseAddr+i*8));
		Xil_Out16(ImgOut_BaseAddr+i*8+2,Xil_In16(ImgIn_BaseAddr+i*8+2));
		Xil_Out16(ImgOut_BaseAddr+i*8+4,Xil_In16(ImgIn_BaseAddr+i*8)+ ((temp_x * model_param[2]) - (temp_y * model_param[3])));
		Xil_Out16(ImgOut_BaseAddr+i*8+6,Xil_In16(ImgIn_BaseAddr+i*8+2)+ ((temp_y * model_param[2]) + (temp_x * model_param[3])));
   }

}


void choose_better_model (unsigned long flow_vector, unsigned long ego_vector, int flow_vector_count, float *model_param, float *model_param_betterModel, int *outlier_count_betterModel, int error_threshold)
{
	int outlier_count = 0,i=0;
	int vx1, vy1, vx_ego, vy_ego;

	for(i = 0; i < flow_vector_count; i++) {
		vx1      = Xil_In16(flow_vector+i*8+4) - Xil_In16(flow_vector+i*8);
		vy1      = Xil_In16(flow_vector+i*8+6) - Xil_In16(flow_vector+i*8+2);
		vx_ego   = Xil_In16(ego_vector+i*8+4) - Xil_In16(ego_vector+i*8);
		vy_ego   = Xil_In16(ego_vector+i*8+6) - Xil_In16(ego_vector+i*8+2);

		if(!((abs(vx1 - vx_ego) < error_threshold)) || !((abs(vy1 - vy_ego) < error_threshold))) {
			outlier_count++;
		}
	}

	if (outlier_count < (*outlier_count_betterModel)) {
		model_param_betterModel[0] = model_param[0];
		model_param_betterModel[1] = model_param[1];
		model_param_betterModel[2] = model_param[2];
		model_param_betterModel[3] = model_param[3];
		(*outlier_count_betterModel) = outlier_count;
	}
}

int estimate_ego_motion_first_order_flow( unsigned long flow_vector, int size_flow_vector_array,
										   float* model_param_est,int max_iter, int min_iter, int error_threshold, float converg_threshold)

{
	int outlier_count_betterModel, loop_count;
	float model_param_betterModel[4];
	int ret;

	if(size_flow_vector_array == 0) {
		return (0);
	}

	outlier_count_betterModel = size_flow_vector_array;

	for(loop_count = 0; loop_count < max_iter; loop_count++) {
		// Obtain model parameters for First Oder FLow
		ret = gen_firstOrderFlow_model(size_flow_vector_array,flow_vector, model_param_est);
		if(ret == 0) {
			break;
		}

		gen_firstOrderFlow_vectors_4(model_param_est, size_flow_vector_array,flow_vector, EGO_VECTOR);

		choose_better_model (flow_vector,EGO_VECTOR, size_flow_vector_array, model_param_est, model_param_betterModel, &outlier_count_betterModel, error_threshold);
		if( ( loop_count > min_iter ) &&
            ( ((float)outlier_count_betterModel/(float)size_flow_vector_array) <= converg_threshold) ){
			break;
		}
	}

	Xil_DCacheFlush();  // make sure that model has reached system memory

	if((((float)outlier_count_betterModel/(float)size_flow_vector_array)) > converg_threshold) {
		xil_printf(" Model did not converge (outlier count: %d) ! \n\r ", outlier_count_betterModel);
        printf("Outlier Ratio    : %f \n\r",  ((float)outlier_count_betterModel)/((float)size_flow_vector_array));
		ret = 0;
	}
	else {
		model_param_est[0] = model_param_betterModel[0];
		model_param_est[1] = model_param_betterModel[1];
		model_param_est[2] = model_param_betterModel[2];
		model_param_est[3] = model_param_betterModel[3];
        model_param_est[4] = outlier_count_betterModel;
        model_param_est[5] = size_flow_vector_array;
        model_param_est[6] = ((float)outlier_count_betterModel)/((float)size_flow_vector_array);
		printf(" XC: %f \n\r", model_param_betterModel[0]);
		printf(" YC: %f \n\r", model_param_betterModel[1]);
		printf(" D : %f \n\r", model_param_betterModel[2]);
		printf(" R : %f \n\r", model_param_betterModel[3]);
		printf(" Total iterations : %d \n\r", loop_count);
		printf(" Outlier count    : %d \n\r", outlier_count_betterModel);
        printf(" Total count      : %d \n\r", size_flow_vector_array);
        printf(" Outlier Ratio    : %f \n\r",  ((float)outlier_count_betterModel)/((float)size_flow_vector_array));
		ret = 1;
	}
	return ret;

}

int estimate_ego_motion_first_order_flow_HW( unsigned long flow_vector, int size_flow_vector_array, float* model_param_est,int* model_param_32, int max_iter, int min_iter)
{
	int loop_count,offset=0;
	int ret;

	if(size_flow_vector_array == 0) {
		return (0);
	}

	for(loop_count = 0; loop_count < max_iter; loop_count++) {
		// Obtain model parameters for First Oder FLow
		ret = gen_firstOrderFlow_model(size_flow_vector_array,flow_vector, model_param_est);
		if(ret == 0) {
			break;
		}

		//---- store the model param for use by ego-motion esitmation engine -----

		model_param_32[offset+0] = (int)(model_param_est[0] * 8);
		model_param_32[offset+1] = (int)(model_param_est[1] * 8);
		model_param_32[offset+2] = (int)(model_param_est[2] * 2147483648);
		model_param_32[offset+3] = (int)(model_param_est[3] * 2147483648);
		offset+=4;

	}

	Xil_DCacheFlush();  // make sure that model has reached system memory

	return ret;
}

void diff_motion_vectors(unsigned long int fullvector, unsigned long int resvector, int vector_count, unsigned long int resvectorout, int error_threshold){

int vx_error, vy_error,i;

	for(i=0; i<vector_count; i++) {
		vx_error = (Xil_In16(resvector+8*i+4) - Xil_In16(fullvector+8*i+4));
		vy_error = (Xil_In16(resvector+8*i+6) - Xil_In16(fullvector+8*i+6));

		if((abs(vx_error) < error_threshold) && (abs(vy_error) < error_threshold)){
			Xil_Out16(resvectorout+8*i,0);
			Xil_Out16(resvectorout+8*i+2,0);
			Xil_Out16(resvectorout+8*i+4,0);
			Xil_Out16(resvectorout+8*i+6,0);
		}
		else {
			Xil_Out16(resvectorout+8*i+4,Xil_In16(resvector+8*i)+vx_error);
			Xil_Out16(resvectorout+8*i+6,Xil_In16(resvector+8*i+2)+vy_error);
		}
	}
}
int compressvectorHW(unsigned long int vector,unsigned long int compvector,int width,int height){
	int i,j;
	u32 feature;
	u16 x1,y1,x2,y2;
	int count=0;

	u32* p_vector = (u32*) vector;
	u16* p_compvector = (u16*) compvector;

	for (i=0;i<width;i++){
		for (j=0;j<height;j++){
			feature = p_vector[j*width+i];
			if (feature != 0){
				x1 = (feature >> 15)+(i<<3);
				y1 = ((feature >> 12)& 0x7)+ (j<<3);
				if ((feature & 0x00000800) == 0x00000800){
					x2 = x1-(((~feature & 0x000007c0)>>6)+1);
				}
				else
				{
					x2 = x1+((feature & 0x000007c0)>>6);
				}

				if ((feature & 0x00000020) == 0x00000020){
					y2 = y1-((~feature & 0x0000001f)+1);
				}
				else
				{
					y2 = y1+(feature & 0x0000001f);
				}
				p_compvector[4*count] = x1;
				p_compvector[4*count+1] = y1;
				p_compvector[4*count+2] = x2;
				p_compvector[4*count+3] = y2;
				count++;
			}
		}
	}

	return count;
}

void CameraCopy(unsigned short width, unsigned short height, unsigned short horizontalActiveTime,
		unsigned short verticalActiveTime,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr){
	u32 row,col;
	u32 V_offset;
	u32 pixel_val1;

		for (row = 0; row < height; row++) {
		  V_offset = row * horizontalActiveTime;
		  for (col = 0; col < width; col++) {
			  pixel_val1 = Xil_In32(ImgIn_BaseAddr+(V_offset+col)*4);
			  Xil_Out32(ImgOut_BaseAddr+(V_offset+col)*4, (u32)pixel_val1);
		  }
		}
}

void Camera24ToGray8(unsigned short width, unsigned short height, unsigned short horizontalActiveTime,
		unsigned short verticalActiveTime,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr){
	u32 row,col;
	u32 V_offset;
	u32 pixel_val1,pixel_val2;

	for (row = 0; row < height; row++) {
	  V_offset = row * horizontalActiveTime;
	  for (col = 0; col < width; col++) {
		  pixel_val1 = Xil_In32(ImgIn_BaseAddr+(V_offset+col)*4);
		  pixel_val2 = (int)(((pixel_val1 & 0xff) + ((pixel_val1 & 0xff00)>>8) + ((pixel_val1 & 0xff0000)>>16))/3);
		  Xil_Out8(ImgOut_BaseAddr + V_offset + col, (u8)pixel_val2);
	  }
	}
}

