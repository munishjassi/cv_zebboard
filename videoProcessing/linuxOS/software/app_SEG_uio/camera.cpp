	#include <stdio.h>
	#include <opencv2/opencv.hpp>
	#include <sys/mman.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include "config.h"
	#include "stdlib.h"


	using namespace cv;


	#define FRAME_WIDTH  320
	#define FRAME_HEIGHT 240

	#define DEBUG_MODE

	#define XFILTER_REG_AP_CTRL       0x00
	#define XFILTER_REG_GIE           0x04
	#define XFILTER_REG_IER           0x08
	#define XFILTER_REG_ISR           0x0c
	#define XFILTER_REG_ROWS	  0x14
	#define XFILTER_REG_COLS    	  0x1c

	#define SOBEL_VDMA_BASEADDR	  0x43040000
	#define SOBEL_VDMA_OUTADDR    	  0x1D100000
	#define SOBEL_BASEADDR        	  0x71800000

	#define ERODE_VDMA_BASEADDR       0x43020000
	#define ERODE_VDMA_OUTADDR        0x1E200000
	#define ERODE_BASEADDR            0x67400000

	#define GRAYSCALE_VDMA_BASEADDR   0x43000000
	#define GRAYSCALE_VDMA_OUTADDR    0x1F300000
	#define GRAYSCALE_BASEADDR        0x6e800000

	#define IMG_BASEADDR        	  0x1F000000

	#ifdef DEBUG_MODE
		#define DEBUG_Text(fmt) printf(fmt)
		#define DEBUG_Printf(fmt,...) printf(fmt,__VA_ARGS__)
	#else
		#define DEBUG_Text(fmt)
		#define DEBUG_Printf(fmt,...)
	#endif

	#define IN0()	DEBUG_Printf(">> IN >> %s",__func__)
	#define OUT0()	DEBUG_Printf("<< OUT << %s",__func__)


	#define REG_WRITE(addr, off, val) (*(volatile unsigned int*)(addr+off)=(val))
	#define REG_READ(addr,off) (*(volatile unsigned int*)(addr+off))



	//
	// Configures VDMA. Sets the VDMA configurations,
	//    	> Start addr
	//	> Dest addr
	//	> Hsize
	//	> Vsize
	// Starts the VDMA
	//
	void configureVDMA(unsigned long vdma_physical_base, unsigned VDMA_START_ADDR, unsigned VDMA_DEST_ADDR)
	{
		unsigned long int PhysicalAddress = vdma_physical_base ;
		int map_len = 0x20;
		int fd = open( "/dev/mem", O_RDWR);
		unsigned char* vdma_base_address;

		vdma_base_address = (unsigned char*)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)PhysicalAddress);
		
		//
		// It requires write permission on memory dev. Check if it worked
		//
		if(vdma_base_address == MAP_FAILED)
		{
			perror("VDMA Mapping memory for absolute memory access failed.\n");
			return;
		}
		printf("\n");
		DEBUG_Printf ("Configure VDMA: VDMA mapping 0x%x to 0x%x, size = %d\n", (int )PhysicalAddress, (int)vdma_base_address, map_len );

		REG_WRITE(vdma_base_address,0x0  ,0x00003);
		REG_WRITE(vdma_base_address,0x58 ,FRAME_WIDTH*4); /*VDMA_FRMDLY_STRIDE */
		REG_WRITE(vdma_base_address,0x5c ,VDMA_START_ADDR); /*VDMA_START_ADDRESS1*/
		REG_WRITE(vdma_base_address,0x60 ,VDMA_START_ADDR); /*VDMA_START_ADDRESS2*/
		REG_WRITE(vdma_base_address,0x64 ,VDMA_START_ADDR); /*VDMA_START_ADDRESS3*/
		REG_WRITE(vdma_base_address,0x54 ,FRAME_WIDTH*4); /*VDMA_HSIZE*/
		REG_WRITE(vdma_base_address,0x50 ,FRAME_HEIGHT); /*VDMA_VSIZE*/

		REG_WRITE(vdma_base_address,0x30,0x00003);
		REG_WRITE(vdma_base_address,0xa8,FRAME_WIDTH*4);
		REG_WRITE(vdma_base_address,0xac,VDMA_DEST_ADDR);
		REG_WRITE(vdma_base_address,0xb0,VDMA_DEST_ADDR);
		REG_WRITE(vdma_base_address,0xb4,VDMA_DEST_ADDR);
		REG_WRITE(vdma_base_address,0xa4,FRAME_WIDTH*4);
		REG_WRITE(vdma_base_address,0xa0,FRAME_HEIGHT);


		printf ("============================================\n");
		DEBUG_Printf (" CONFIGURING VDMA 0x%x \n", (int )PhysicalAddress);
		DEBUG_Printf (" VDMA 0x%x 0x0: 0x%x 0x00003\n", (int )PhysicalAddress, 0x00003);
		DEBUG_Printf (" VDMA 0x%x 0x50: %i FRAME_HEIGHT\n", (int )PhysicalAddress, FRAME_HEIGHT);
		DEBUG_Printf (" VDMA 0x%x 0x54: %i*4 FRAME_WIDTH*4\n", (int )PhysicalAddress, FRAME_WIDTH*4);
		DEBUG_Printf (" VDMA 0x%x 0x58: %i*4 FRAME_WIDTH*4\n", (int )PhysicalAddress, FRAME_WIDTH*4);
		DEBUG_Printf (" VDMA 0x%x 0x5c: 0x%x VDMA_START_ADDR\n", (int )PhysicalAddress, VDMA_START_ADDR);
		DEBUG_Printf (" VDMA 0x%x 0x60: 0x%x VDMA_START_ADDR\n", (int )PhysicalAddress, VDMA_START_ADDR);
		DEBUG_Printf (" VDMA 0x%x 0x64: 0x%x VDMA_START_ADDR\n", (int )PhysicalAddress, VDMA_START_ADDR);
		DEBUG_Printf (" VDMA 0x%x 0x30: 0x%x 0x00003\n", (int )PhysicalAddress, 0x00003);
		DEBUG_Printf (" VDMA 0x%x 0xa0: %i FRAME_HEIGHT\n", (int )PhysicalAddress, FRAME_HEIGHT);
		DEBUG_Printf (" VDMA 0x%x 0xa4: %i*4 FRAME_WIDTH*4\n", (int )PhysicalAddress, FRAME_WIDTH);
		DEBUG_Printf (" VDMA 0x%x 0xa8: %i*4 FRAME_WIDTH*4\n", (int )PhysicalAddress, FRAME_WIDTH);
		DEBUG_Printf (" VDMA 0x%x 0xac: 0x%x VDMA_DEST_ADDR\n", (int )PhysicalAddress, VDMA_DEST_ADDR);
		DEBUG_Printf (" VDMA 0x%x 0xb0: 0x%x VDMA_DEST_ADDR\n", (int )PhysicalAddress, VDMA_DEST_ADDR);
		DEBUG_Printf (" VDMA 0x%x 0xb4: 0x%x VDMA_DEST_ADDR\n", (int )PhysicalAddress, VDMA_DEST_ADDR);

		munmap((void *)vdma_base_address, map_len);
		close(fd);
	}


	//
	// Stops the VDMA located at given address
	//
		void stopVDMA(unsigned long vdma_physical_base)
		{
			unsigned long int PhysicalAddress = vdma_physical_base ;
			int map_len = 0x20;
			int fd = open( "/dev/mem", O_RDWR);
			unsigned char* vdma_base_address;

			vdma_base_address = (unsigned char*)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)PhysicalAddress);

			if(vdma_base_address == MAP_FAILED)
			{
				perror("VDMA Mapping memory for absolute memory access failed.\n");
				return;
			}

			REG_WRITE(vdma_base_address, 0x0 /* 0x30 + additional_offset*/, 0x00000);
			REG_WRITE(vdma_base_address, 0x30, 0x00000);

			munmap((void *)vdma_base_address, map_len);
			close(fd);
		}


	//
	// Initiallize Filter and set its configurations.
	//	> Frame_height
	//	> Frame_Width
	//
		void initilizeFilter(unsigned long filter_physical_base)
		{
			unsigned long int PhysicalAddress = filter_physical_base ;
			int map_len = 0x20;
			int fd = open( "/dev/mem", O_RDWR);
			unsigned char* filter_base_address;

			filter_base_address = (unsigned char*)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)PhysicalAddress);
			//check if it worked
			if(filter_base_address == MAP_FAILED)
			{
				perror("Filter Initialization Mapping memory for absolute memory access failed.\n");
				return;
			}

			printf("Filter Initiallization Mapping memory for absolute memory access DONE!\n");
			DEBUG_Printf ("Filter Initialize: Filter mapping 0x%x to 0x%x, size = %d\n", (int )PhysicalAddress, (int)filter_base_address, map_len );
			REG_WRITE(filter_base_address,XFILTER_REG_ROWS,FRAME_HEIGHT);
			REG_WRITE(filter_base_address,XFILTER_REG_COLS,FRAME_WIDTH);

			munmap((void *)filter_base_address, map_len);
			close(fd);
		}

	//
	// Starts Filter for image processing
	// 	> GIE-> 1
	//	> IER-> 1
	//	> CTRL-> 1
	// Waits till processing completes. Done bit (CTRL -> 0x2) to set.
	// 
		void startFilter(unsigned long filter_physical_base)
		{
			unsigned long int PhysicalAddress = filter_physical_base ;
			int map_len = 0x20;
			int fd = open( "/dev/mem", O_RDWR);
			unsigned char* filter_base_address;
			unsigned int done2;

			filter_base_address = (unsigned char*)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)PhysicalAddress);

			if(filter_base_address == MAP_FAILED)
			{
				printf("Filter Start Mapping memory for absolute memory access failed: 0x%x.\n", (int)PhysicalAddress);
				cvWaitKey(10);
				return;
			}
			//printf ("Sobel mapping 0x%x to 0x%x, size = %d\n", (int )PhysicalAddress, (int)filter_base_address, map_len );
			//cvWaitKey(10);

			REG_WRITE(filter_base_address,XFILTER_REG_GIE,1);
			REG_WRITE(filter_base_address,XFILTER_REG_IER,1);
			REG_WRITE(filter_base_address,XFILTER_REG_AP_CTRL,1);

			while (1)
			{
			  done2 = REG_READ(filter_base_address,XFILTER_REG_AP_CTRL);
			  //printf("done value = %d\n\r",done2);
			  if (done2 >> 1 & 0x1) {
				//printf("Filter processing finished Done value %x = %d\n\r",filter_physical_base, done2);
				break;
				}
			};

			REG_WRITE(filter_base_address,XFILTER_REG_GIE,0);
			REG_WRITE(filter_base_address,XFILTER_REG_AP_CTRL,0);
			munmap((void *)filter_base_address, map_len);
			close(fd);
		}

	//
	// Copies data from CPU program memory to HA memory, for processing
	//
		void copy_mem2dev(Mat img, unsigned long in_buffer)
		{
			int count=0;
			unsigned long int map_len = FRAME_WIDTH * FRAME_HEIGHT * 4 +4;
			int fd = open("/dev/mem", O_RDWR);
			unsigned char* virtual_addr_in;

			virtual_addr_in = (unsigned char*)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)in_buffer);
			if(virtual_addr_in == MAP_FAILED)
			{
				printf("copy_mem2dev virtual_addr_in mapping for absolute memory access failed.\n");
				//cvWaitKey(30);
				return;
			}

			//  printf("copy_mem2dev allocation\n");
			//cvWaitKey(10);
			for (int i=0; i<FRAME_HEIGHT;i++)
			{
				for (int j=0; j<FRAME_WIDTH;j++)
				{
					//Vec3b intensity = img.at<Vec3b>(i, j);
					//uchar red = intensity.val[0]; 		uchar green = intensity.val[1];         uchar blue = intensity.val[2];
					//unsigned int value = 0;
					//value = (red << 16) ||  (green << 8) || (blue);
					//value = value && 0xffffff;
					REG_WRITE(virtual_addr_in,count,(uchar)img.at<Vec3b>(i, j)[2]);
					REG_WRITE(virtual_addr_in,count+1,(uchar)img.at<Vec3b>(i, j)[1]);
					REG_WRITE(virtual_addr_in,count+2,(uchar)img.at<Vec3b>(i, j)[0]);
					//REG_WRITE(virtual_addr_in,count,img.at<uchar>(i, j));
					count+=4;
				}
			}
			munmap((void *)virtual_addr_in, map_len);
			close(fd);

		}


	//
	// Copies data from HA memory to CPU program memory.
	//
		void copy_dev2mem(Mat img, unsigned long in_buffer)
		{
			int count=0;
			unsigned long int map_len = FRAME_WIDTH * FRAME_HEIGHT * 4;
			int fd = open("/dev/mem", O_RDWR);

			unsigned char* virtual_addr_out;

			virtual_addr_out = (unsigned char*)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)in_buffer);
			if(virtual_addr_out == MAP_FAILED)
			{
				printf("copy_dev2mem virtual_addr_in mapping for absolute memory access failed.\n");
				cvWaitKey(10);
				return;
			}

			//printf("copy_dev2mem allocation\n");
			//cvWaitKey(10);
			for (int i=0; i<FRAME_HEIGHT;i++)
			{
				for (int j=0; j<FRAME_WIDTH;j++)
				{
					/*unsigned int value = 0;
					value = REG_READ(virtual_addr_out,count);
					img.at<Vec3b>(i, j)[0] = (uchar)((value >> 16 )&& 0xff);
					img.at<Vec3b>(i, j)[1] = (uchar)((value >> 8 )&& 0xff);
					img.at<Vec3b>(i, j)[2] = (uchar)( value && 0xff);*/
					img.at<Vec3b>(i, j)[2] = (uchar)REG_READ(virtual_addr_out,count);
					img.at<Vec3b>(i, j)[1] = (uchar)REG_READ(virtual_addr_out,count+1);
					img.at<Vec3b>(i, j)[0] = (uchar)REG_READ(virtual_addr_out,count+2);
					//img.at<uchar>(i, j) = REG_READ(virtual_addr_out,count);
					count+=4;
				}
			}
			munmap((void *)virtual_addr_out, map_len);
			close(fd);
		}



	//
	// Application code
	//
int main() {
	   int sw_flag = 0,hw_flag = 0, smart_flag = 0;
	   int scale = 1;
	   int delta = 0;
	   int ddepth = CV_16S;

	   clock_t sw_s1=0, sw_s2=0, sw_e1=0, sw_e2=0, sw_g1=0, sw_g2=0, sw_t1=0, sw_t2=0, tot=0; 
	   clock_t hw_s1=0, hw_s2=0, hw_e1=0, hw_e2=0, hw_g1=0, hw_g2=0, hw_t1=0, hw_t2=0; 
	   clock_t hw_S1=0, hw_S2=0, hw_E1=0, hw_E2=0, hw_G1=0, hw_G2=0, hw_T1=0, hw_T2=0; 
	   clock_t sum_sw_s=0, sum_sw_e=0, sum_sw_g=0, sum_sw_t=0, sum_tot=0;
	   clock_t sum_hw_s=0, sum_hw_e=0, sum_hw_g=0, sum_hw_t=0;
	   clock_t sum_hw_S=0, sum_hw_E=0, sum_hw_G=0, sum_hw_T=0;

	   int swCount = 0;
	   int hwCount = 0;
	   int sCount = 0;
	
	   bool exit_flag = false;
	   bool lim_flag = true;
	   static int LIMIT = 500;
		
	   resetDev(); if (HAexists(ARCH_DIR, SOBEL_FILTER, 0, "name") == 1) { SOBEL_HW = true; strcat(SOBEL_DEV, getDev(ARCH_DIR, SOBEL_FILTER, 0, "name")); printf("HA SOBEL FOUND: %s\n", SOBEL_DEV);} 
	   resetDev(); if (HAexists(ARCH_DIR, ERODE_FILTER, 0, "name") == 1) { ERODE_HW = true; strcat(ERODE_DEV, getDev(ARCH_DIR, ERODE_FILTER, 0, "name")); printf("HA ERODE FOUND: %s\n", ERODE_DEV);} 
	   resetDev(); if (HAexists(ARCH_DIR, GRAY_FILTER, 0, "name") == 1) { GRAY_HW = true; strcat(GRAY_DEV, getDev(ARCH_DIR, GRAY_FILTER, 0, "name")); printf("HA GRAY FOUND: %s\n", GRAY_DEV);} 

	   size_t ln = strlen(SOBEL_DEV) - 1; if (SOBEL_DEV[ln] == '\n') SOBEL_DEV[ln] = '\0';
	   ln = strlen(ERODE_DEV) - 1; if (ERODE_DEV[ln] == '\n') ERODE_DEV[ln] = '\0';
	   ln = strlen(GRAY_DEV) - 1; if (GRAY_DEV[ln] == '\n') GRAY_DEV[ln] = '\0';

	   if (SOBEL_HW) printf("HA Exists: %s \n", SOBEL_FILTER);
		else printf("HA NOT Exists: %s \n", SOBEL_FILTER);
	   if (ERODE_HW) printf("HA Exists: %s \n", ERODE_FILTER);
		else printf("HA NOT Exists: %s \n", ERODE_FILTER);
	   if (GRAY_HW) printf("HA Exists: %s \n", GRAY_FILTER);
		else printf("HA NOT Exists: %s \n", GRAY_FILTER);


	   //	
	   // Initializing Filters
	   //
	   if (SOBEL_HW) initilizeFilter(SOBEL_BASEADDR);
	   if (ERODE_HW) initilizeFilter(ERODE_BASEADDR);
	   if (GRAY_HW) initilizeFilter(GRAYSCALE_BASEADDR);

	   //
	   // Configuring VDMAs
	   //
	   if (SOBEL_HW) configureVDMA(SOBEL_VDMA_BASEADDR, IMG_BASEADDR, SOBEL_VDMA_OUTADDR); // Config VDMA_1 - Sobel
	   if (ERODE_HW) configureVDMA(ERODE_VDMA_BASEADDR, IMG_BASEADDR, ERODE_VDMA_OUTADDR); // Config VDMA_2 - Erode
	   if (GRAY_HW) configureVDMA(GRAYSCALE_VDMA_BASEADDR, IMG_BASEADDR, GRAYSCALE_VDMA_OUTADDR); // Config VDMA_3 - Grayscale


	   //printf("Initialize: All hardware VDMAs initialized. Starting capturing camera image ..\n");

	   VideoCapture cap(0); // open the default camera
	   if(!cap.isOpened())  // check if we succeeded
	   {
		printf("ERROR: Camera device is not opened.\n");
		return -1;
	   } else {
		printf("Initialize: Camera device is opened SUCCESSFULLY.\n");
	   }

	   cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	   cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	   cap.set(CV_CAP_PROP_FPS, 15);
	   Mat src_gray;

	   int erosion_type;
	   int erosion_elem = 0;
	   int erosion_size = 3;
	   Mat grad_x, grad_y, sobel_dst, erosion_dst, grayscale_dst;
	   Mat abs_grad_x, abs_grad_y;

	   while ( !exit_flag ) {
		Mat frame;
		Mat sobelFrame = Mat::zeros(frame.size(), CV_8UC3);
		Mat erodeFrame = Mat::zeros(frame.size(), CV_8UC3);
		Mat grayscaleFrame = Mat::zeros(frame.size(), CV_8UC3);

		cap >> frame;
		//frame = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
		if (sw_flag == 0 & hw_flag == 0 & smart_flag == 0)
		{
		 imshow("Main Window", frame);
		}



		//
		// Only Software execution
		//
		if(sw_flag == 1 & hw_flag == 0 & smart_flag == 0)
		{
			sw_t1 = sw_s1 = clock();

			// Sobel filter
			//cvtColor( frame, src_gray, CV_RGB2GRAY );
			src_gray = frame;
			Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
			convertScaleAbs( grad_x, abs_grad_x );
			Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
			convertScaleAbs( grad_y, abs_grad_y );
			addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, sobel_dst);
			sw_s2 = sw_e1 = clock();
			sw_s2 = sw_s2 - sw_s1;

			// Erode filter
			if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
			else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
			else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

			Mat element = getStructuringElement( erosion_type,
					       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
					       Point( erosion_size, erosion_size ) );

			erode( frame, erosion_dst, element );
			sw_e2 = sw_g1 = clock();
			sw_e2 = sw_e2 - sw_e1;

			// Grayscale filter
			cvtColor( frame, grayscale_dst, CV_BGR2GRAY );
			sw_g2 = sw_t2 = clock();
			sw_g2 = sw_g2 - sw_g1;
			sw_t2 = sw_t2 - sw_t1;

			
			// Output display after Sobel, Erode and Grayscale
			imshow("Main Window", frame);
			imshow("Sobel Image", sobel_dst);
			moveWindow("Sobel Image", 300,0);
			imshow("Erosion Image", erosion_dst);
			moveWindow("Erosion Image", 0, 300);
			imshow("Grayscale Image", grayscale_dst );
			moveWindow("Grayscale Image", 300,300);
			sum_sw_t += sw_t2;
			sum_sw_s += sw_s2;
			sum_sw_e += sw_e2;
			sum_sw_g += sw_g2;
			swCount++;

			if (lim_flag && swCount >= LIMIT) {
				if (hwCount >= LIMIT) exit_flag = true;
				else {
					hw_flag = 1;
					sw_flag = 0;
				}
			}

		 }

		//
		// Only Hardware execution
		//
		 if(sw_flag == 0 & hw_flag == 1 & smart_flag == 0)
		 {
			//printf("using hardware accelerator \n");
			//cvWaitKey(30);
			sobelFrame = Mat::zeros(frame.size(), CV_8UC3);
			erodeFrame = Mat::zeros(frame.size(), CV_8UC3);
			grayscaleFrame = Mat::zeros(frame.size(), CV_8UC3);

			//printf("Staring hw\n");
			//cvWaitKey(30);
			hw_T1 = clock();
			copy_mem2dev(frame, IMG_BASEADDR);
			//printf("copy_mem2dev done\n");
			//cvWaitKey(10);

			hw_s1 = hw_S1 = clock();
			//startFilter(SOBEL_BASEADDR); // Sobel Processing

			unsigned long int PhysicalAddress = SOBEL_BASEADDR;
			int map_len = 0x20;
			int fd = open( "/dev/uio2", O_RDWR);
			volatile unsigned *filter;
			unsigned int done2;
	
			filter = (volatile unsigned *)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			if(filter == MAP_FAILED)
			{
				printf("Filter Start Mapping memory for absolute memory access failed: 0x%x.\n", (int)PhysicalAddress);
				cvWaitKey(10);
				return;
			}
		
			REG_WRITE(filter,XFILTER_REG_GIE,1);
			REG_WRITE(filter,XFILTER_REG_IER,1);
			REG_WRITE(filter,XFILTER_REG_AP_CTRL,1);

			while (1)
			{
			  done2 = REG_READ(filter,XFILTER_REG_AP_CTRL);
			  if (done2 >> 1 & 0x1) {
				break;
				}
			};

			REG_WRITE(filter,XFILTER_REG_GIE,0);
			REG_WRITE(filter,XFILTER_REG_AP_CTRL,0);
			munmap((void *)filter, map_len);
			close(fd);


			hw_s2 = clock();
			copy_dev2mem(sobelFrame,SOBEL_VDMA_OUTADDR); // Data transfer after Sobel
			hw_e1 = hw_E1 = hw_S2 = clock();			

	//		startFilter(ERODE_BASEADDR); // Erode Processing
			fd = open( "/dev/uio4", O_RDWR);
			PhysicalAddress = ERODE_BASEADDR;
			filter = (volatile unsigned *)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			if(filter == MAP_FAILED)
			{
				printf("Filter Start Mapping memory for absolute memory access failed: 0x%x.\n", (int)PhysicalAddress);
				cvWaitKey(10);
				return;
			}
		
			REG_WRITE(filter,XFILTER_REG_GIE,1);
			REG_WRITE(filter,XFILTER_REG_IER,1);
			REG_WRITE(filter,XFILTER_REG_AP_CTRL,1);

			while (1)
			{
			  done2 = REG_READ(filter,XFILTER_REG_AP_CTRL);
			  if (done2 >> 1 & 0x1) {
				break;
				}
			};

			REG_WRITE(filter,XFILTER_REG_GIE,0);
			REG_WRITE(filter,XFILTER_REG_AP_CTRL,0);
			munmap((void *)filter, map_len);
			close(fd);


			hw_e2 = clock();
			copy_dev2mem(erodeFrame,ERODE_VDMA_OUTADDR); // Data transfer after Erode
			hw_g1 = hw_G1 = hw_E2 = clock();

//			startFilter(GRAYSCALE_BASEADDR); // Grayscale Processing
			fd = open( "/dev/uio6", O_RDWR);
			PhysicalAddress = GRAYSCALE_BASEADDR;
			filter = (volatile unsigned *)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			if(filter == MAP_FAILED)
			{
				printf("Filter Start Mapping memory for absolute memory access failed: 0x%x.\n", (int)PhysicalAddress);
				cvWaitKey(10);
				return;
			}
			
			REG_WRITE(filter,XFILTER_REG_GIE,1);
			REG_WRITE(filter,XFILTER_REG_IER,1);
			REG_WRITE(filter,XFILTER_REG_AP_CTRL,1);

			while (1)
				{
				  done2 = REG_READ(filter,XFILTER_REG_AP_CTRL);
				  if (done2 >> 1 & 0x1) {
					break;
				}
			};

			REG_WRITE(filter,XFILTER_REG_GIE,0);
			REG_WRITE(filter,XFILTER_REG_AP_CTRL,0);
			munmap((void *)filter, map_len);
			close(fd);

			hw_g2 = clock();
			copy_dev2mem(grayscaleFrame,GRAYSCALE_VDMA_OUTADDR); // Data transfer after Grayscale processing
			hw_G2 = hw_T2 = clock();

			hw_s2 = hw_s2 - hw_s1;
			hw_e2 = hw_e2 - hw_e1;
			hw_g2 = hw_g2 - hw_g1;
			hw_S2 = hw_S2 - hw_S1;
			hw_E2 = hw_E2 - hw_E1;
			hw_G2 = hw_G2 - hw_G1;
			hw_T2 = hw_T2 - hw_T1;
			hw_t2 = hw_s2 + hw_e2 + hw_g2;

			sum_hw_t += hw_t2;
			sum_hw_T += hw_T2;
			sum_hw_s += hw_s2;
			sum_hw_e += hw_e2;
			sum_hw_g += hw_g2;
			sum_hw_S += hw_S2;
			sum_hw_E += hw_E2;
			sum_hw_G += hw_G2;

			//printf("copy_dev2mem done\n");
			//cvWaitKey(10);
			imshow("Main Window", frame);
			imshow("Sobel Image", sobelFrame);
			moveWindow("Sobel Image", 300, 0);
			imshow("Erosion Image", erodeFrame);
			moveWindow("Erosion Image", 0, 300);
			imshow("Grayscale Image", grayscaleFrame);		
			moveWindow("Grayscale Image", 300, 300);
			//cvWaitKey(10);
			hwCount++;

			if (lim_flag && hwCount >= LIMIT) {
				if (swCount >= LIMIT) exit_flag = true;
				else {
					hw_flag = 0;
					sw_flag = 1;
				}
			}

		 }


		//
		// SoC dependent HW/SW execution
		//
		 if(sw_flag == 0 & hw_flag == 0 & smart_flag == 1)
		 {
			tot = clock();

			sobelFrame = Mat::zeros(frame.size(), CV_8UC3);
			erodeFrame = Mat::zeros(frame.size(), CV_8UC3);
			grayscaleFrame = Mat::zeros(frame.size(), CV_8UC3);
			
			int map_len = 0x20;
			volatile unsigned *filter;
			unsigned int done2;

			if (SOBEL_HW || ERODE_HW || GRAY_HW) copy_mem2dev(frame, IMG_BASEADDR);

			if (SOBEL_HW) {
				hw_s1 = hw_S1 = clock();
				unsigned long int PhysicalAddress = SOBEL_BASEADDR;
				int fd = open(SOBEL_DEV, O_RDWR);

				filter = (volatile unsigned *)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if(filter == MAP_FAILED)
				{
					printf("Filter Start Mapping memory for absolute memory access failed: 0x%x.\n", (int)PhysicalAddress);
					cvWaitKey(10);
					return;
				}
		
				REG_WRITE(filter,XFILTER_REG_GIE,1);
				REG_WRITE(filter,XFILTER_REG_IER,1);
				REG_WRITE(filter,XFILTER_REG_AP_CTRL,1);

/*
			PhysicalAddress = SOBEL_VDMA_BASEADDR;
			int fdi = open( "/dev/mem", O_RDWR);
			unsigned char* filter_base_address;
			
			filter_base_address = (unsigned char*)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fdi, (off_t)PhysicalAddress);
			REG_WRITE(filter_base_address,0x0  ,0x00011013);

				while (1)
				{
				  done2 = REG_READ(filter_base_address,0x4);
				  printf("Done2: %x :: %x.\n", done2, done2 >> 12);

				  if (done2 >> 12 & 0x1) {
					break;
					}
				};
*/

				while (1)
				{
				  done2 = REG_READ(filter,XFILTER_REG_AP_CTRL);
				  //printf("Done2: %x.\n", (int)done2);

				  if (done2 >> 1 & 0x1) {
					break;
					}
				};

				REG_WRITE(filter,XFILTER_REG_GIE,0);
				REG_WRITE(filter,XFILTER_REG_AP_CTRL,0);
				munmap((void *)filter, map_len);
				close(fd);


				hw_s2 = clock();
				copy_dev2mem(sobelFrame,SOBEL_VDMA_OUTADDR); // Data transfer after Sobel
				hw_S2 = clock();			

			} else {
				// Sobel filter
				sw_s1 =clock();
				src_gray = frame;
				Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
				convertScaleAbs( grad_x, abs_grad_x );
				Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
				convertScaleAbs( grad_y, abs_grad_y );
				addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, sobelFrame);
				sw_s2 = clock() - sw_s1;
			}


			if (ERODE_HW) {
				hw_e1 = hw_E1 = clock();
				unsigned long int PhysicalAddress = ERODE_BASEADDR;
				int fd = open( ERODE_DEV, O_RDWR);

				filter = (volatile unsigned *)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if(filter == MAP_FAILED)
				{
					printf("Filter Start Mapping memory for absolute memory access failed: 0x%x.\n", (int)PhysicalAddress);
					cvWaitKey(10);
					return;
				}
		
				REG_WRITE(filter,XFILTER_REG_GIE,1);
				REG_WRITE(filter,XFILTER_REG_IER,1);
				REG_WRITE(filter,XFILTER_REG_AP_CTRL,1);

				while (1)
				{
				  done2 = REG_READ(filter,XFILTER_REG_AP_CTRL);
				  if (done2 >> 1 & 0x1) {
					break;
					}
				};

				REG_WRITE(filter,XFILTER_REG_GIE,0);
				REG_WRITE(filter,XFILTER_REG_AP_CTRL,0);
				munmap((void *)filter, map_len);
				close(fd);

				hw_e2 = clock();
				copy_dev2mem(erodeFrame,ERODE_VDMA_OUTADDR); // Data transfer after Sobel
				hw_E2 = clock();			
			} else {
			// Erode filter
				sw_e1 = clock();
				if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
				else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
				else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

				Mat element = getStructuringElement( erosion_type,
						       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
						       Point( erosion_size, erosion_size ) );

				erode( frame, erodeFrame, element );
				sw_e2 = clock() - sw_e1;
			}
			
			if (GRAY_HW) {
				hw_g1 = hw_G1 = clock();
				unsigned long int PhysicalAddress = GRAYSCALE_BASEADDR;
				int fd = open( GRAY_DEV, O_RDWR);

				filter = (volatile unsigned *)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
				if(filter == MAP_FAILED)
				{
					printf("Filter Start Mapping memory for absolute memory access failed: 0x%x.\n", (int)PhysicalAddress);
					cvWaitKey(10);
					return;
				}
		
				REG_WRITE(filter,XFILTER_REG_GIE,1);
				REG_WRITE(filter,XFILTER_REG_IER,1);
				REG_WRITE(filter,XFILTER_REG_AP_CTRL,1);

				while (1)
				{
				  done2 = REG_READ(filter,XFILTER_REG_AP_CTRL);
				  if (done2 >> 1 & 0x1) {
					break;
					}
				};

				REG_WRITE(filter,XFILTER_REG_GIE,0);
				REG_WRITE(filter,XFILTER_REG_AP_CTRL,0);
				munmap((void *)filter, map_len);
				close(fd);

				hw_g2 = clock();
				copy_dev2mem(grayscaleFrame,GRAYSCALE_VDMA_OUTADDR); // Data transfer after Sobel
				hw_G2 = clock();	
			} else {
				// Grayscale filter
				sw_g1 = clock();
				cvtColor( frame, grayscaleFrame, CV_BGR2GRAY );
				sw_g2 = clock() - sw_g1;
			}

			tot = clock() - tot;
			tot += sum_sw_t;

			hw_s2 = hw_s2 - hw_s1;
			hw_e2 = hw_e2 - hw_e1;
			hw_g2 = hw_g2 - hw_g1;
			hw_S2 = hw_S2 - hw_S1;
			hw_E2 = hw_E2 - hw_E1;
			hw_G2 = hw_G2 - hw_G1;
			hw_T2 = hw_T2 - hw_T1;
			hw_t2 = hw_s2 + hw_e2 + hw_g2;

			sum_hw_t += hw_t2;
			sum_hw_T += hw_T2;
			sum_hw_s += hw_s2;
			sum_hw_e += hw_e2;
			sum_hw_g += hw_g2;
			sum_hw_S += hw_S2;
			sum_hw_E += hw_E2;
			sum_hw_G += hw_G2;

			sum_sw_s += sw_s2;
			sum_sw_e += sw_e2;
			sum_sw_g += sw_g2;
			sum_tot  += tot;
	
			++sCount;
			if (sCount >= LIMIT & lim_flag) exit_flag = true;

			imshow("Main Window", frame);
			imshow("Sobel Image", sobelFrame);
			moveWindow("Sobel Image", 300, 0);
			imshow("Erosion Image", erodeFrame);
			moveWindow("Erosion Image", 0, 300);
			imshow("Grayscale Image", grayscaleFrame);		
			moveWindow("Grayscale Image", 300, 300);
		}

	     char c = cvWaitKey(10);
	     if ( c == 27 ) break;
	     if ( c == 's' )
	     {
			 smart_flag = 0;
			 sw_flag = 1;
			 hw_flag = 0;
 			 printf("Switching to Software processing\n");
		 }
	     if ( c == 'h' )
	     { 
			 smart_flag = 0;
			 sw_flag = 0;
			 hw_flag = 1;
 			 printf("Switching to Hardware processing\n");
		 }
	     if ( c == 'o' )
	     {
			 destroyAllWindows();
			 smart_flag = 0;
			 sw_flag = 0;
			 hw_flag = 0;
 			 printf("Switching Off processing\n");
		 }
	     if ( c == 'e'|| c == 'q')
		{
			exit_flag = true;
 			printf("Exiting application.\n");
		}
	     if ( c =='c')
		{
			sw_flag = 0;
			hw_flag = 0;
			smart_flag= 1;
 			printf("Switching to smart HW or SW processing\n");
		}
	   }


	   if (SOBEL_HW) stopVDMA(SOBEL_VDMA_BASEADDR);
	   if (ERODE_HW) stopVDMA(ERODE_VDMA_BASEADDR);
	   if (GRAY_HW) stopVDMA(GRAYSCALE_VDMA_BASEADDR);

	   swCount += sCount;
     	   hwCount += sCount;
	   float avg_tot = (float)sum_tot * 1000 / swCount;
	   float avg_sw_t = (float)sum_sw_t * 1000 / (swCount - sCount);
	   float avg_sw_s = (float)sum_sw_s * 1000 / swCount;
	   float avg_sw_e = (float)sum_sw_e * 1000 / swCount;
	   float avg_sw_g = (float)sum_sw_g * 1000 / swCount;
	   float avg_hw_s = (float)sum_hw_s * 1000 / swCount;
	   float avg_hw_e = (float)sum_hw_e * 1000 / swCount;
	   float avg_hw_g = (float)sum_hw_g * 1000 / swCount;
	   float avg_hw_S = (float)sum_hw_S * 1000 / swCount;
	   float avg_hw_E = (float)sum_hw_E * 1000 / swCount;
	   float avg_hw_G = (float)sum_hw_G * 1000 / swCount;
	   float avg_hw_t = (float)sum_hw_t * 1000 / hwCount;
	   float avg_hw_T = (float)sum_hw_T * 1000 / hwCount;

	   printf("\n");
	   printf("===================================================================\n");
	   printf("FINAL LATENCY PROCESSING REPORT\n");
	   printf("Software Filtering CLOCKS_PER_SEC = %i \n", CLOCKS_PER_SEC);
	   printf("Total SW operation count= %i \n", swCount - sCount);
	   printf("Total HW operation count= %i \n", hwCount - sCount );
	   printf("Total Smart operation count= %i \n", sCount);
	   printf("Software Filtering latency with %d counts TOTAL (SW only)= %f ms\n", swCount, avg_sw_t/CLOCKS_PER_SEC);
	   printf("Software Filtering latency with %d counts TOTAL (SW only + smart)= %f ms\n", swCount, tot/CLOCKS_PER_SEC);
	   printf("Hardware Filtering latency (w/o memcopy) with %d counts TOTAL (HW only)= %f ms\n", hwCount, avg_hw_t/CLOCKS_PER_SEC);
	   printf("Hardware Filtering latency (w memcopy) with %d counts TOTAL (HW only)= %f ms\n", hwCount, avg_hw_T/CLOCKS_PER_SEC);
	   printf("\n");
	   printf("Software Filtering latency with %d counts SOBEL= %f ms\n", swCount, avg_sw_s/CLOCKS_PER_SEC);
	   printf("Software Filtering latency with %d counts ERODE= %f ms\n", swCount, avg_sw_e/CLOCKS_PER_SEC);
	   printf("Software Filtering latency with %d counts GRAY= %f ms\n", swCount, avg_sw_g/CLOCKS_PER_SEC);
	   printf("\n");
	   printf("Hardware Filtering latency (w/o memcopy) with %d counts SOBEL= %f ms\n", hwCount, avg_hw_s/CLOCKS_PER_SEC);
	   printf("Hardware Filtering latency (w/o memcopy) with %d counts ERODE= %f ms\n", hwCount, avg_hw_e/CLOCKS_PER_SEC);
	   printf("Hardware Filtering latency (w/o memcopy) with %d counts GRAY= %f ms\n", hwCount, avg_hw_g/CLOCKS_PER_SEC);
	   printf("\n");
	   printf("Hardware Filtering latency (w memcopy) with %d counts SOBEL= %f ms\n", hwCount, avg_hw_S/CLOCKS_PER_SEC);
	   printf("Hardware Filtering latency (w memcopy) with %d counts ERODE= %f ms\n", hwCount, avg_hw_E/CLOCKS_PER_SEC);
	   printf("Hardware Filtering latency (w memcopy) with %d counts GRAY= %f ms\n", hwCount, avg_hw_G/CLOCKS_PER_SEC);
	   return 0;
 	}
