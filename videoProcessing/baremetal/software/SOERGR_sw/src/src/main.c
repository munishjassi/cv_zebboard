/***************************************************************************//**
 *   @file   main.c

*******************************************************************************/

// include files
#include <stdio.h>
#include "xil_cache.h"
#include "xbasic_types.h"
#include "xil_io.h"
#include "cf_hdmi.h"
#include "atv_platform.h"
#include "transmitter.h"
#include "xil_exception.h"
#include "xuartps.h"
#include "axi_interrupt.h"
#include "xil_mmu.h"
#include "xpseudo_asm.h"
#include "xscugic.h"
#include "../IP_SW/funcs.h"
#include "global.h"





// macros
#define HDMI_CALL_INTERVAL_MS	10

#define VIDEO_BASEADDR 0x2000000


// global variables only to be used in main.c file
static unsigned char    MajorRev;      /* Major Release Number */
static unsigned char    MinorRev;      /* Usually used for code-drops */
static unsigned char    RcRev;         /* Release Candidate Number */
static unsigned char    DriverEnable;
static unsigned char    LastEnable;


static XScuGic_Config *GicConfig;/* The configuration parameters of the controller */
XScuGic InterruptController; /* Instance of the Interrupt Controller */


// functions declarations
int ScuGicInterrupt_Init();
void delay_us(u32 us_count);
void APP_EnableDriver (BOOL Enable);
static BOOL APP_DriverEnabled (void);
extern char inbyte(void);
int APP_ChangeResolution();
void InitSobelFilter();
void InitErodeFilter();
void InitGrayscaleFilter();
void processFrame(unsigned int dataMemBaseAddr);			// data processing chain (application's dataflow)

// for performance evaluation
void evaluate_frameThroughput();
// for performance evaluation


/***************************************************************************//**
 * @brief Main function.
 *
 * @return Returns 0.
*******************************************************************************/
int main()
{
	UINT32 StartCount;
	int xstatus;

	MajorRev     = 1;
	MinorRev     = 1;
	RcRev        = 1;
	DriverEnable = TRUE;
	LastEnable   = FALSE;

#ifdef DO_PERF_EVAL
	received_frame_count = 0;
	processed_frame_count = 0;
	cpu_cycles_for_frameProc = 0.0;
#endif


    /* Disable caching on shared OCM data by setting the appropriate TLB
     * attributes for the shared data space in OCM.
	 *
     * S=b1
     * TEX=b100
     * AP=b11
     * Domain=b1111
     * C=b0
     * B=b0
	 */
    Xil_SetTlbAttributes(SHARED_OCM_MEMORY_BASE, 0x14de2);
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	HAL_PlatformInit(XPAR_AXI_IIC_0_BASEADDR,	/* Perform any required platform init */
					 XPAR_SCUTIMER_DEVICE_ID,	/* including hardware reset to HDMI devices */
					 XPAR_SCUGIC_SINGLE_DEVICE_ID,
					 XPAR_SCUTIMER_INTR);

#ifdef USE_MULTICORE
	SoCProc_initialize();
#endif

	DBG_MSG("  To change the video resolution press:\r\n");
	DBG_MSG("  '0' - 640x480;  '1' - 800x600;  '2' - 1024x768; '3' - 1280x720 \r\n");
	DBG_MSG("  '4' - 1360x768; '5' - 1600x900; '6' - 1920x1080.\r\n");

	ADIAPI_TransmitterInit();   /* Initialize ADI repeater software and h/w */
	ADIAPI_TransmitterSetPowerMode(REP_POWER_UP);

	StartCount = HAL_GetCurrentMsCount();
	ADIAPI_TransmitterMain();

	/*Initialize the HDMI Core with default display settings*/
	// subsequently initialize hardware accelerators with current horizontal/vertical timing
	SetVideoResolution(RESOLUTION_640x480);
	h_ActiveTime = detailedTiming[currentResolution][H_ACTIVE_TIME];
	v_ActiveTime = detailedTiming[currentResolution][V_ACTIVE_TIME];

	/* Initialize the interrupt controller */
	xstatus = ScuGicInterrupt_Init();
	if (xstatus != XST_SUCCESS) {
		xil_printf("Unable to initialize Interrupts");
	  		//return XST_FAILURE;
	  	}

	GrayscaleIP_func_init(&InterruptController, VIDEO_BASEADDR, VIDEO_BASEADDR + FRAME_SIZE, 640, 480, h_ActiveTime, v_ActiveTime);
	ErodeIP_func_init(&InterruptController, VIDEO_BASEADDR, VIDEO_BASEADDR + FRAME_SIZE, 640, 480, h_ActiveTime, v_ActiveTime);
	SobelIP_func_init(&InterruptController, VIDEO_BASEADDR, VIDEO_BASEADDR + FRAME_SIZE, 640, 480, h_ActiveTime, v_ActiveTime);

	EnablePerfCounters();

	while (APP_ChangeResolution())
	{
		if (ATV_GetElapsedMs (StartCount, NULL) >= HDMI_CALL_INTERVAL_MS)
		{
			StartCount = HAL_GetCurrentMsCount();
			if (APP_DriverEnabled())
			{
				ADIAPI_TransmitterMain();
			}
		}

		FRAME_INTR = 0;
		while(FRAME_INTR == 0);

		// raw data passed through the data processing chain by Master CPU
		processFrame(VIDEO_BASEADDR);

	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return(0);
}



int ScuGicInterrupt_Init()
{
	int Status;
	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 * */
	Xil_ExceptionInit();

	GicConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
			GicConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	 // **** Setup the Interrupt System *****

//	 * Connect the interrupt controller interrupt handler to the hardware
//	 * interrupt handling logic in the ARM processor.
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			(void *) &InterruptController);

//	 * Connect a device driver handler that will be called when an
//	 * interrupt for the device occurs, the device driver handler performs
//	 * the specific interrupt processing for the device
	Status = XScuGic_Connect(&InterruptController,XPAR_FABRIC_CAM_INTERFACE_0_VSYNC_NEGEDGE_INTR,
			(Xil_ExceptionHandler)AXI_INTERRUPT_VsyncIntr_Handler,
			(void *) &InterruptController);
	XScuGic_Enable(&InterruptController, XPAR_FABRIC_CAM_INTERFACE_0_VSYNC_NEGEDGE_INTR);

// 	Enable interrupts in the ARM
	Xil_ExceptionEnable();

	//Only used for edge sensitive Interrupts
	XScuGic_SetPriorityTriggerType(&InterruptController, XPAR_FABRIC_CAM_INTERFACE_0_VSYNC_NEGEDGE_INTR, 0xa0, 3);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS ; 
}

void delay_us(u32 us_count) {
  u32 count;
  for (count = 0; count < ((us_count * 800) + 1); count++) {
    asm("nop");
  }
}

/***************************************************************************//**
 * @brief Enables the driver.
 *
 * @return Returns ATVERR_OK.
*******************************************************************************/
void APP_EnableDriver (BOOL Enable)
{
    DriverEnable = Enable;
}

/***************************************************************************//**
 * @brief Returns the driver enable status.
 *
 * @return Returns the driver enable status.
*******************************************************************************/
static BOOL APP_DriverEnabled (void)
{
    if ((DriverEnable && HAL_GetMBSwitchState()) != LastEnable)
    {
        LastEnable = DriverEnable && HAL_GetMBSwitchState();
        DBG_MSG ("APP: Driver %s\n\r", LastEnable? "Enabled": "Disabled");
    }
    return (LastEnable);
}



/***************************************************************************//**
 * @brief Changes the video resolution depending on User input.
 *
 * @return None.
*******************************************************************************/
int APP_ChangeResolution (void)
{
	char *resolutions[7] = {"640x480", "800x600", "1024x768", "1280x720", "1360x768", "1600x900", "1920x1080"};
	char receivedChar    = 0;

	if(XUartPs_IsReceiveData(UART_BASEADDR))
	{
		receivedChar = inbyte();
		if((receivedChar >= 0x30) && (receivedChar <= 0x36))
		{
			SetVideoResolution(receivedChar - 0x30);
			h_ActiveTime = detailedTiming[currentResolution][H_ACTIVE_TIME];
			v_ActiveTime = detailedTiming[currentResolution][V_ACTIVE_TIME];

			DBG_MSG("Resolution was changed to %s \r\n", resolutions[receivedChar - 0x30]);
		}
		else if (receivedChar == 'q') {
			DBG_MSG("Exiting Application. \n\r");
		    return(0);
		    }
		else
		{
			{
				SetVideoResolution(RESOLUTION_640x480);
				h_ActiveTime = detailedTiming[currentResolution][H_ACTIVE_TIME];
				v_ActiveTime = detailedTiming[currentResolution][V_ACTIVE_TIME];

				DBG_MSG("Resolution was changed to %s \r\n", resolutions[0]);
			}
		}
	}
	return 1;
}




void processFrame(unsigned int dataMemBaseAddr) {

#ifdef DO_PERF_EVAL
	init_perfcounters(1, 0);
#endif

	unsigned int dataMem_ptr = dataMemBaseAddr;

	// capturing the frame pixels from camera line buffers onto DDR memory
	DDRVideoWr(640, 480, h_ActiveTime, v_ActiveTime, dataMem_ptr);

	// sobel filtering the captured image
	EdgeDetection_func(dataMem_ptr, dataMem_ptr + FRAME_SIZE, 640, 480, h_ActiveTime, v_ActiveTime);
	dataMem_ptr += FRAME_SIZE;

	// erode filtering the captured image
	Erode_func(dataMem_ptr, dataMem_ptr + FRAME_SIZE, 640, 480, h_ActiveTime, v_ActiveTime);
	dataMem_ptr += FRAME_SIZE;

	// grayscale filtering the captured image
	ConvToGray_func(dataMem_ptr, dataMem_ptr + FRAME_SIZE, 640, 480, h_ActiveTime, v_ActiveTime);
	dataMem_ptr += FRAME_SIZE;

#ifdef DO_PERF_EVAL
	cpu_cycles_for_frameProc = (cpu_cycles_for_frameProc*(double)(processed_frame_count-1) + get_cyclecount()) / ((double) processed_frame_count);
#endif

	ConfigHdmiVDMA(h_ActiveTime, v_ActiveTime, dataMem_ptr);
}

#ifdef DO_PERF_EVAL
void evaluate_frameThroughput() {
	double frame_throughput;				// approx measure of no of frames processed in 1 sec
	frame_throughput = ( (double)((double)CPU_FREQ * (double)processed_frame_count) / (double)((double)INTER_FRAME_TIME * (double)TOTAL_FRAMES_FOR_STATS) );
	printf("\r\n\n\nconsidering first %d frames, frame_thorughput = %.3f\r\n", TOTAL_FRAMES_FOR_STATS, frame_throughput);

	printf("avg cpu cycles needed = %.2f\r\n", cpu_cycles_for_frameProc);
	printf("which amounts to relative cpu-load of %.2f%%\r\n", (cpu_cycles_for_frameProc * 100)/CPU_LOAD );
}
#endif


// TODO: build the codeGenerator which is responsible for generating:
// a) Driver templates
// b) IP_config modules from GRIP generated SoC architecture
// c) IP_func modules
// d) multicore optimization SoCProc_support modules







