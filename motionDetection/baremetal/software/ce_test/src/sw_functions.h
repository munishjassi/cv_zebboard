/*
 * sw_functions.h
 *
 *  Created on: Feb 12, 2014
 *      Author: ga73koz
 */

#ifndef SW_FUNCTIONS_H_
#define SW_FUNCTIONS_H_

void ConvToGray(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,
		unsigned short horizontalActiveTime);

void EdgeDetection(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,
		unsigned short width, unsigned short height, unsigned short h_ActiveTime);

unsigned char rgb2y(unsigned char R, unsigned char G, unsigned char B);

void ConvToGrayHLS(unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,unsigned short horizontalActiveTime);

void kernel_apply(int* _src, int* _dst,float _par[3]);

//for opticalflow
void ConvGray24ToGray8(int width,int height,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr);
void ConvGray8ToGray32(int width,int height,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr);
void ImageSmoothGray8(int width,int height,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr);
void ImageOutput8(int width,int height,unsigned long ImgIn_BaseAddr);
void ImageOutput32(int width,int height,unsigned long ImgIn_BaseAddr);
void CensusEngine8to32(int width,int height,unsigned char epsilon,unsigned char csd1,unsigned char csd2,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr);
int MatchingEngine32(int width,int height,int w_search,int h_search,unsigned long ImgIn_BaseAddr1,unsigned long ImgIn_BaseAddr2,unsigned long list_x1,unsigned long list_y1,unsigned long list_x2,unsigned long list_y2);
inline u32 ct32(unsigned char data,u32 in,unsigned char central_low,unsigned char central_high);
void DrawVector32(int width,int height,unsigned long list_x1,unsigned long list_y1,unsigned long list_x2,unsigned long list_y2,int matchcounts,unsigned long ImgOut_BaseAddr,int flag);
void line(unsigned long ImgIn_BaseAddr,int width,int height,int x,int y,int x2, int y2);
unsigned int Distance2Color(signed char DeltaX, signed char DeltaY, unsigned char N);
void putpixel(unsigned int* ImgIn_BaseAddr,int width,int height,int x,int y,unsigned int color);
void DrawVector32hw(int width,int height,unsigned long list,int matchcounts,unsigned long ImgOut_BaseAddr,int flag);

int sample_flow_vectors(unsigned long featureimg,unsigned long compvector,int width, int height,int select, int blk_size_vect_avg);
void presample(unsigned long ImgIn_BaseAddr, unsigned long featureimg,int matchcount,int width, int height);

int estimate_ego_motion_first_order_flow( unsigned long flow_vector, int size_flow_vector_array,
										   float* model_param_est,int max_iter, int min_iter, int error_threshold, float converg_threshold);
int estimate_ego_motion_first_order_flow_HW( unsigned long flow_vector, int size_flow_vector_array, float* model_param_est,int* model_param_32, int max_iter, int min_iter);
void gen_firstOrderFlow_vectors_4(float *model_param, int flow_vector_count, unsigned long ImgIn_BaseAddr, unsigned long ImgOut_BaseAddr);
void DrawVector32eee(int width,int height,int matchcounts,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr,int flag);
void diff_motion_vectors(unsigned long int fullvector, unsigned long int resvector, int vector_count, unsigned long int resvectorout, int error_threshold);
void differ(int width,int height,unsigned long ImgIn_BaseAddr1,unsigned long ImgIn_BaseAddr2,unsigned long ImgOut_BaseAddr);
int compressvectorHW(unsigned long int vector,unsigned long int compvector,int width,int height);
int MatchingEngine32HWO(int width,int height,int w_search,int h_search,unsigned long ImgIn_BaseAddr1,unsigned long ImgIn_BaseAddr2,unsigned long ImgOut_BaseAddr);
void CameraCopy(unsigned short width, unsigned short height, unsigned short horizontalActiveTime,
		unsigned short verticalActiveTime,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr);
void Camera24ToGray8(unsigned short width, unsigned short height, unsigned short horizontalActiveTime,
		unsigned short verticalActiveTime,unsigned long ImgIn_BaseAddr,unsigned long ImgOut_BaseAddr);
#endif /* SW_FUNCTIONS_H_ */
