/***************************************************************************//**
 *   @file   main.c

 *******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include "xil_cache.h"
#include "xbasic_types.h"
#include "xil_io.h"
#include "cf_hdmi.h"
#include "atv_platform.h"
#include "transmitter.h"
#include "xil_exception.h"
#include "xuartps.h"
#include "profile_cnt.h"
// for interrupt handling
#include "axi_interrupt.h"
#include "xscugic.h"
// for Software and Hardware engines
#include "sw_functions.h"
#include "xce.h"
#include "xmatchingengine32.h"
#include "xeee.h"
#include "xece.h"

extern void delay_ms(u32 ms_count);
extern char inbyte(void);

/******************************************************************************/
/************************** Macros Definitions ********************************/
/******************************************************************************/
#define HDMI_CALL_INTERVAL_MS	10			/* Interval between two         */
/* iterations of the main loop  */
#define DBG_MSG                 xil_printf

#define CPUFREQ 				XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ // cycles in one ms
#define FNUM 10	//maximal still images
/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
static UCHAR MajorRev; /* Major Release Number */
static UCHAR MinorRev; /* Usually used for code-drops */
static UCHAR RcRev; /* Release Candidate Number */
static BOOL DriverEnable;
static BOOL LastEnable;

short int FRAME_INTR = 0;

XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of the controller */

XCe hlsCE;
XMatchingengine32 hlsME;
XEee hlsEEE;
XEce hlsECE;

volatile static int CEisdone = 0;
volatile static int MEisdone = 0;
volatile static int EEEisdone = 0;
volatile static int ECEisdone = 0;
volatile static int cameraframe = 0;

UINT32 StartCount;
int xstatus;

u32 width = 640;
u32 height = 480;
u32 csd1 = 3;
u32 csd2 = 7;
u32 epsilon = 20;

int ceflag = 1, meflag = 1, eeeflag = 1, eceflag = 1, Camflag = 0, Oflag = 1;
int camceflag = 0, cammeflag = 0;
int count = 0;
int ceswcount = 0, cehwcount = 0, meswcount = 0, mehwcount = 0, eeeswcount = 0,
		eeehwcount = 0, eceswcount = 0, ecehwcount = 0;
float ceswsum = 0.0, meswsum = 0.0, cehwsum = 0.0, mehwsum = 0.0,
		eeeswsum = 0.0, eceswsum = 0.0, eeehwsum = 0.0, ecehwsum = 0.0;
int fnum = 0;
int matchcount = 0, matchcount2 = 0, samplecount = 0;
int ret;

float model_param_est[8];
int model_param_32[1200];
int model_param_32_tmp[4];
int bmid;

int CE_init() {
	XCe_Config *cfgPtr;
	int status;
	cfgPtr = XCe_LookupConfig(XPAR_CE_TOP_0_DEVICE_ID);
	if (!cfgPtr) {
		print("ERROR: Lookup of CE accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}
	status = XCe_CfgInitialize(&hlsCE, cfgPtr);
	if (status != XST_SUCCESS) {
		print("ERROR: Could not initialize CE accelerator.\n\r");
		return XST_FAILURE;
	}
	return status;
}

int ME_init() {
	XMatchingengine32_Config *cfgPtr;
	int status;
	cfgPtr = XMatchingengine32_LookupConfig(
			XPAR_MATCHINGENGINE32_TOP_0_DEVICE_ID);
	if (!cfgPtr) {
		print("ERROR: Lookup of ME accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}
	status = XMatchingengine32_CfgInitialize(&hlsME, cfgPtr);
	if (status != XST_SUCCESS) {
		print("ERROR: Could not initialize ME accelerator.\n\r");
		return XST_FAILURE;
	}
	return status;
}

int EEE_init() {
	XEee_Config *cfgPtr;
	int status;
	cfgPtr = XEee_LookupConfig(XPAR_EEE_TOP_0_DEVICE_ID);
	if (!cfgPtr) {
		print("ERROR: Lookup of EEE accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}
	status = XEee_CfgInitialize(&hlsEEE, cfgPtr);
	if (status != XST_SUCCESS) {
		print("ERROR: Could not initialize EEE accelerator.\n\r");
		return XST_FAILURE;
	}
	return status;
}

int ECE_init() {
	XEce_Config *cfgPtr;
	int status;
	cfgPtr = XEce_LookupConfig(XPAR_ECE_TOP_0_DEVICE_ID);
	if (!cfgPtr) {
		print("ERROR: Lookup of ECE accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}
	status = XEce_CfgInitialize(&hlsECE, cfgPtr);
	if (status != XST_SUCCESS) {
		print("ERROR: Could not initialize ECE accelerator.\n\r");
		return XST_FAILURE;
	}
	return status;
}

void CE_start() {
	XCe_InterruptEnable(&hlsCE, 1);
	XCe_InterruptGlobalEnable(&hlsCE);
	XCe_Start(&hlsCE);
}

void ME_start() {
	XMatchingengine32_InterruptEnable(&hlsME, 1);
	XMatchingengine32_InterruptGlobalEnable(&hlsME);
	XMatchingengine32_Start(&hlsME);
}

void EEE_start() {
	XEee_InterruptEnable(&hlsEEE, 1);
	XEee_InterruptGlobalEnable(&hlsEEE);
	XEee_Start(&hlsEEE);
}

void ECE_start() {
	XEce_InterruptEnable(&hlsECE, 1);
	XEce_InterruptGlobalEnable(&hlsECE);
	XEce_Start(&hlsECE);
}

void AXI_INTERRUPT_CEIntr_Handler() {
	//Disable the global and local interrupt
	XCe_InterruptGlobalDisable(&hlsCE);
	XCe_InterruptDisable(&hlsCE, 1);
	XCe_InterruptClear(&hlsCE, 1);
	//Set a result status flag
	CEisdone = 1;
}

void AXI_INTERRUPT_MEIntr_Handler() {
	//Disable the global and local interrupt
	XMatchingengine32_InterruptGlobalDisable(&hlsME);
	XMatchingengine32_InterruptDisable(&hlsME, 1);
	//Clear the ap_done interrupt
	XMatchingengine32_InterruptClear(&hlsME, 1);
	//Set a result status flag
	MEisdone = 1;
}

void AXI_INTERRUPT_EEEIntr_Handler() {
	//Disable the global and local interrupt
	XEee_InterruptGlobalDisable(&hlsEEE);
	XEee_InterruptDisable(&hlsEEE, 1);
	//Clear the ap_done interrupt
	XEee_InterruptClear(&hlsEEE, 1);
	//Set a result status flag
	EEEisdone = 1;
}

void AXI_INTERRUPT_ECEIntr_Handler() {
	//Disable the global and local interrupt
	XEce_InterruptGlobalDisable(&hlsECE);
	XEce_InterruptDisable(&hlsECE, 1);
	//Clear the ap_done interrupt
	XEce_InterruptClear(&hlsECE, 1);
	//Set a result status flag
	ECEisdone = 1;
}

int ScuGicInterrupt_Init() {
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
	Status = XScuGic_Connect(&InterruptController,
			XPAR_FABRIC_CAM_INTERFACE_0_VSYNC_NEGEDGE_INTR,
			(Xil_ExceptionHandler) AXI_INTERRUPT_VsyncIntr_Handler,
			(void *) &InterruptController);
	XScuGic_Enable(&InterruptController,
			XPAR_FABRIC_CAM_INTERFACE_0_VSYNC_NEGEDGE_INTR);

// 	Enable interrupts in the ARM
	Xil_ExceptionEnable();

	//Only used for edge sensitive Interrupts
	XScuGic_SetPriorityTriggerType(&InterruptController,
			XPAR_FABRIC_CAM_INTERFACE_0_VSYNC_NEGEDGE_INTR, 0xa0, 3);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// CE_TOP_Interrupt
	Status = XScuGic_Connect(&InterruptController,
			XPAR_FABRIC_CE_TOP_0_INTERRUPT_INTR,
			(Xil_InterruptHandler) AXI_INTERRUPT_CEIntr_Handler,
			(void *) &hlsCE);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	XScuGic_Enable(&InterruptController, XPAR_FABRIC_CE_TOP_0_INTERRUPT_INTR);

	XScuGic_SetPriorityTriggerType(&InterruptController,
			XPAR_FABRIC_CE_TOP_0_INTERRUPT_INTR, 0x90, 3);

	// ME_TOP_Interrupt
	Status = XScuGic_Connect(&InterruptController,
			XPAR_FABRIC_MATCHINGENGINE32_TOP_0_INTERRUPT_INTR,
			(Xil_InterruptHandler) AXI_INTERRUPT_MEIntr_Handler,
			(void *) &hlsME);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	XScuGic_Enable(&InterruptController,
			XPAR_FABRIC_MATCHINGENGINE32_TOP_0_INTERRUPT_INTR);

	XScuGic_SetPriorityTriggerType(&InterruptController,
			XPAR_FABRIC_MATCHINGENGINE32_TOP_0_INTERRUPT_INTR, 0x70, 3);

	// EEE_TOP_Interrupt
	Status = XScuGic_Connect(&InterruptController,
			XPAR_FABRIC_EEE_TOP_0_INTERRUPT_INTR,
			(Xil_InterruptHandler) AXI_INTERRUPT_EEEIntr_Handler,
			(void *) &hlsEEE);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	XScuGic_Enable(&InterruptController, XPAR_FABRIC_EEE_TOP_0_INTERRUPT_INTR);

	XScuGic_SetPriorityTriggerType(&InterruptController,
			XPAR_FABRIC_EEE_TOP_0_INTERRUPT_INTR, 0x60, 3);

	// ECE_TOP_Interrupt
	Status = XScuGic_Connect(&InterruptController,
			XPAR_FABRIC_ECE_TOP_0_INTERRUPT_INTR,
			(Xil_InterruptHandler) AXI_INTERRUPT_ECEIntr_Handler,
			(void *) &hlsECE);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	XScuGic_Enable(&InterruptController, XPAR_FABRIC_ECE_TOP_0_INTERRUPT_INTR);

	XScuGic_SetPriorityTriggerType(&InterruptController,
			XPAR_FABRIC_ECE_TOP_0_INTERRUPT_INTR, 0x50, 3);

	return XST_SUCCESS;
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
void APP_EnableDriver(BOOL Enable) {
	DriverEnable = Enable;
}

/***************************************************************************//**
 * @brief Returns the driver enable status.
 *
 * @return Returns the driver enable status.
 *******************************************************************************/
static BOOL APP_DriverEnabled(void) {
	if ((DriverEnable && HAL_GetMBSwitchState()) != LastEnable) {
		LastEnable = DriverEnable && HAL_GetMBSwitchState();
//        DBG_MSG ("APP: Driver %s\n\r", LastEnable? "Enabled": "Disabled");
	}
	return (LastEnable);
}
/***************************************************************************//**
 * @brief config function
 *
 * @return Returns 0 or 1
 *******************************************************************************/
int config() {
	char ctmp = 0;
	char count = 0;
	count = 0;
	if (!Camflag) {
		while (1) {
			while (!XUartPs_IsReceiveData(UART_BASEADDR))
				;
			ctmp = inbyte();
			if (count == 0) {
				if (ctmp == 27) {
					count++;
				}
				if (ctmp == 'q' || ctmp == 'Q') {
					printf("-----------Exiting Application-------\n");
					return 0;
				}
				if (ctmp == 'v' || ctmp == 'V') {
					printf("-----------Switch to Camera mode-------\n");
					Camflag = 1;
					return 1;
				}
				if (ctmp == 's' || ctmp == 'S') {
					ceflag = 0;
					meflag = 0;
					eeeflag = 0;
					eceflag = 0;
					printf("-----------Pure Software Mode-------\n");
				}
				if (ctmp == 'h' || ctmp == 'H') {
					ceflag = 1;
					meflag = 1;
					eeeflag = 1;
					eceflag = 1;
					printf("-----------Pure Hardware Mode-------\n");
				}
				if (ctmp == 'c' || ctmp == 'C') {
					if (ceflag == 0) {
						ceflag = 1;
						printf("-----------Hardware CE Mode-------\n");
					} else {
						ceflag = 0;
						printf("-----------Software CE Mode-------\n");
					}
				}
				if (ctmp == 'm' || ctmp == 'M') {
					if (meflag == 0) {
						meflag = 1;
						printf("-----------Hardware ME Mode-------\n");
					} else {
						meflag = 0;
						printf("-----------Software ME Mode-------\n");
					}
				}
				if (ctmp == 'e' || ctmp == 'E') {
					if (eeeflag == 0) {
						eeeflag = 1;
						printf("-----------Hardware EEE Mode-------\n");
					} else {
						eeeflag = 0;
						printf("-----------Software EEE Mode-------\n");
					}
				}
				if (ctmp == 'f' || ctmp == 'F') {
					if (eceflag == 0) {
						eceflag = 1;
						printf("-----------Hardware ECE Mode-------\n");
					} else {
						eceflag = 0;
						printf("-----------Software ECE Mode-------\n");
					}
				}
				if (ctmp == 'o' || ctmp == 'O') {
					if (Oflag == 0) {
						Oflag = 1;
						printf("-----------No Processing Mode-------\n");
					} else {
						Oflag = 0;
						printf("-----------With Processing Mode-------\n");
					}
				}
				if (ctmp == '1') {
					ConfigHdmiVDMA(
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							CE_OUTPUT_2_BASEADDR);
					if (ATV_GetElapsedMs(StartCount,
							NULL) >= HDMI_CALL_INTERVAL_MS) {
						StartCount = HAL_GetCurrentMsCount();
						if (APP_DriverEnabled()) {
							ADIAPI_TransmitterMain();
						}
					}
				}
				if (ctmp == '2') {
					ConfigHdmiVDMA(
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							OUTPUT_ME_BASEADDR);
					if (ATV_GetElapsedMs(StartCount,
							NULL) >= HDMI_CALL_INTERVAL_MS) {
						StartCount = HAL_GetCurrentMsCount();
						if (APP_DriverEnabled()) {
							ADIAPI_TransmitterMain();
						}
					}
				}
				if (ctmp == '3') {
					ConfigHdmiVDMA(
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							OUTPUT_BASEADDR);
					if (ATV_GetElapsedMs(StartCount,
							NULL) >= HDMI_CALL_INTERVAL_MS) {
						StartCount = HAL_GetCurrentMsCount();
						if (APP_DriverEnabled()) {
							ADIAPI_TransmitterMain();
						}
					}
				}
			} else if (count == 1) {
				if (ctmp == 91) {
					count++;
				} else {
					count--;
				}
			} else if (count == 2) {
				count = 0;
				if (ctmp == 68) {
					if (fnum > 0)
						fnum--;
					printf("-------------Play Backward-----------\n");
					break;
				} else if (ctmp == 67) {
					if (fnum < 8)
						fnum++;
					printf("-------------Play Forward------------\n");
					break;
				} else if (ctmp == 65) {
					fnum = 0;
					printf("-------------Start Frame-------------\n");
					break;
				} else if (ctmp == 66) {
					fnum = 8;
					printf("--------------End Frame--------------\n");
					break;
				}
			}
		}
	} else {
		char ctmp = 0;
		int i;
		for (i = 0; i < 15000; i++) {
			if (XUartPs_IsReceiveData(UART_BASEADDR)) {
				ctmp = inbyte();
				break;
			}
		}
		if (ctmp == 'q' || ctmp == 'Q') {
			printf("-----------Exiting Application-------\n");
			return 0;
		}
		if (ctmp == 'i' || ctmp == 'I') {
			printf("-----------Switch to Still Image mode-------\n");
			Camflag = 0;
		}
		if (ctmp == 's' || ctmp == 'S') {
			ceflag = 0;
			meflag = 0;
			eeeflag = 0;
			eceflag = 0;
			printf("-----------Pure Software Mode-------\n");
		}
		if (ctmp == 'h' || ctmp == 'H') {
			ceflag = 1;
			meflag = 1;
			eeeflag = 1;
			eceflag = 1;
			printf("-----------Pure Hardware Mode-------\n");
		}
		if (ctmp == 'c' || ctmp == 'C') {
			if (ceflag == 0) {
				ceflag = 1;
				printf("-----------Hardware CE Mode-------\n");
			} else {
				ceflag = 0;
				printf("-----------Software CE Mode-------\n");
			}
		}
		if (ctmp == 'm' || ctmp == 'M') {
			if (meflag == 0) {
				meflag = 1;
				printf("-----------Hardware ME Mode-------\n");
			} else {
				meflag = 0;
				printf("-----------Software ME Mode-------\n");
			}
		}
		if (ctmp == 'e' || ctmp == 'E') {
			if (eeeflag == 0) {
				eeeflag = 1;
				printf("-----------Hardware EEE Mode-------\n");
			} else {
				eeeflag = 0;
				printf("-----------Software EEE Mode-------\n");
			}
		}
		if (ctmp == 'f' || ctmp == 'F') {
			if (eceflag == 0) {
				eceflag = 1;
				printf("-----------Hardware ECE Mode-------\n");
			} else {
				eceflag = 0;
				printf("-----------Software ECE Mode-------\n");
			}
		}
		if (ctmp == 'o' || ctmp == 'O') {
			if (Oflag == 0) {
				Oflag = 1;
				printf("-----------No Processing Mode-------\n");
			} else {
				Oflag = 0;
				printf("-----------With Processing Mode-------\n");
			}
		}
		if (ctmp == '1') {
			camceflag = 1;
			cammeflag = 0;
		}
		if (ctmp == '2') {
			camceflag = 0;
			cammeflag = 1;
		}
		if (ctmp == '3') {
			camceflag = 0;
			cammeflag = 0;
		}
	}
	return 1;
}
/***************************************************************************//**
 * @brief Main function.
 *
 * @return Returns 0.
 *******************************************************************************/
int main() {

	MajorRev = 1;
	MinorRev = 1;
	RcRev = 1;
	DriverEnable = TRUE;
	LastEnable = FALSE;

	/*Enable cache*/
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	/* Perform any required platform init */
	/* including hardware reset to HDMI devices */
	HAL_PlatformInit(XPAR_AXI_IIC_0_BASEADDR, XPAR_SCUTIMER_DEVICE_ID,
			XPAR_SCUGIC_SINGLE_DEVICE_ID, XPAR_SCUTIMER_INTR);

	/* Initialize ADI repeater software and h/w */
	ADIAPI_TransmitterInit();
	ADIAPI_TransmitterSetPowerMode(REP_POWER_UP);

	StartCount = HAL_GetCurrentMsCount();
	ADIAPI_TransmitterMain();

	/*Initialize the HDMI Core with default display settings*/
	SetVideoResolution(RESOLUTION_640x480);

	/*
	 * Initialize CE engine
	 */
	xstatus = CE_init();
	if (xstatus != XST_SUCCESS) {
		xil_printf("Unable to initialize CE HA!\n");
		return XST_FAILURE;
	}

	/*
	 * Initialize ME engine
	 */
	xstatus = ME_init();
	if (xstatus != XST_SUCCESS) {
		xil_printf("Unable to initialize ME HA!\n");
		return XST_FAILURE;
	}

	/*
	 * Initialize EEE engine
	 */
	xstatus = EEE_init();
	if (xstatus != XST_SUCCESS) {
		xil_printf("Unable to initialize EEE HA!\n");
		return XST_FAILURE;
	}

	/*
	 * Initialize ECE engine
	 */
	xstatus = ECE_init();
	if (xstatus != XST_SUCCESS) {
		xil_printf("Unable to initialize ECE HA!\n");
		return XST_FAILURE;
	}

	/* Initialize the interrupt controller */
	xstatus = ScuGicInterrupt_Init();
	if (xstatus != XST_SUCCESS) {
		xil_printf("Unable to initialize Interrupts\n");
		return XST_FAILURE;
	}

	/*
	 * Initially configure CE
	 */
	XCe_SetWidth(&hlsCE, width);
	XCe_SetHeight(&hlsCE, height);
	XCe_SetCsd1(&hlsCE, csd1);
	XCe_SetCsd2(&hlsCE, csd2);
	XCe_SetEpsilon(&hlsCE, epsilon);

	/*
	 *  Initially configure ME
	 */

	XMatchingengine32_SetWidth_v(&hlsME, width);
	XMatchingengine32_SetHeight_v(&hlsME, height);
	XMatchingengine32_SetReadaddress1_v(&hlsME, CE_OUTPUT_1_BASEADDR);
	XMatchingengine32_SetReadaddress2_v(&hlsME, CE_OUTPUT_2_BASEADDR);
	XMatchingengine32_SetFeatureout_v(&hlsME, FEATURE_X1_BASEADDR);

	/*
	 *  Initially configure EEE
	 */

	XEee_SetFaddr_v(&hlsEEE, FEATURE_X1_BASEADDR);
	XEee_SetVecoutaddr_v(&hlsEEE, PRE_COMP_VECTOR_HW);
	XEee_SetModelcount_v(&hlsEEE, 300);
	XEee_SetErrthres_v(&hlsEEE, 2);
	XEee_SetResultaddr_v(&hlsEEE, RESULT_ADDR);
	XEee_SetModeladdr_v(&hlsEEE, (u32) model_param_32);

	/*
	 *  No initial configuration for EEE
	 */

	XEee_SetFaddr_v(&hlsEEE, FEATURE_X1_BASEADDR);
	XEee_SetVecoutaddr_v(&hlsEEE, PRE_COMP_VECTOR_HW);
	XEee_SetModelcount_v(&hlsEEE, 300);
	XEee_SetErrthres_v(&hlsEEE, 2);
	XEee_SetResultaddr_v(&hlsEEE, RESULT_ADDR);
	XEee_SetModeladdr_v(&hlsEEE, (u32) model_param_32);

	/*
	 * Initial Camera/Still Image mode choose
	 */
	printf("-------------------------------------\n");
	printf("----------------Menu-----------------\n");
	printf("-------------------------------------\n");
	printf("-----Camera mode: press 'v' or 'V'---\n");
	printf("-Sill images mode: press 'i' or 'I'--\n");
	printf("--Quit Processing: press 'q' or 'Q'--\n");
	printf("-------------------------------------\n");
	printf("-------------------------------------\n");

	char ctmp = 0;
	while (1) {
		while (!XUartPs_IsReceiveData(UART_BASEADDR))
			;
		ctmp = inbyte();
		if (ctmp == 'v' || ctmp == 'V') {
			Camflag = 1;
			printf("-------------------------------------\n");
			printf("--------------Camera mode------------\n");
			printf("-------------------------------------\n");
			printf("-------------------------------------\n");
			printf(
					"You can switch to Still Image mode using 'i' or 'I' later\n");
			printf("-------------------------------------\n");
			break;
		}
		if (ctmp == 'i' || ctmp == 'I') {
			Camflag = 0;
			printf("-------------------------------------\n");
			printf("------------Still image mode---------\n");
			printf("-------------------------------------\n");
			printf("-------------------------------------\n");
			printf("You can switch to Camera mode using 'v' or 'V' later\n");
			printf("-------------------------------------\n");
			break;
		}
		if (ctmp == 'q' || ctmp == 'Q') {
			printf("-----------Exiting Application-------\n");
			return 0;
		}
		printf("No such Option!!\n");
	}

	/*
	 * Instruction book
	 */
	printf("-------------------------------------\n");
	printf("-------------Instructions------------\n");
	printf("-------------------------------------\n");
	printf("With/W.o. processing: press 'o' or 'O'\n");
	printf("---Pure software: press 's' or 'S'---\n");
	printf("---Pure hardware: press 'h' or 'H'---\n");
	printf("Toggle SW/HW for CE: press 'c' or 'C'\n");
	printf("Toggle SW/HW for ME: press 'm' or 'M'\n");
	printf("Toggle SW/HW for EEE: press 'e' or 'E'\n");
	printf("Toggle SW/HW for ECE: press 'f' or 'F'\n");
	printf("--------Show CE output: press '1'--------\n");
	printf("--------Show ME output: press '2'--------\n");
	printf("------Show final output: press '3'-------\n");
	printf("--Quit Processing: press 'q' or 'Q'--\n");
	printf("-------------------------------------\n");
	printf("-------------------------------------\n");
	printf("-----------------------------------------\n");
	printf("Special instructions for Still Image mode\n");
	printf("-----------------------------------------\n");
	printf("Start from the first frame: press up arrow\n");
	printf("---Go to the last frame: press down arrow\n");
	printf("Processing the next frame: press right arrow\n");
	printf("Processing the previous frame: press left arrow\n");
	printf("-----------------------------------------\n");
	printf("-----------------------------------------\n");

	/*
	 * Main loop
	 */
	while (config()) {
		if (Oflag) {
			if (Camflag) {
				while (FRAME_INTR == 0)
					;
				FRAME_INTR = 0;
				CameraCopy(640, 480,
						detailedTiming[currentResolution][H_ACTIVE_TIME],
						detailedTiming[currentResolution][V_ACTIVE_TIME],
						VIDEO_BASEADDR, OUTPUT_BASEADDR);
			} else {
				ConvGray8ToGray32(640, 480, GET_INPUT_ADDR(fnum+1),
				OUTPUT_BASEADDR);
			}
		} else {
			if (!ceflag) {
				if (!Camflag) {
					printf("Still image mode, Software CE processing\n");
					init_perfcounters(1, 0);
					EnablePerfCounters();
					count = get_cyclecount();
					ImageSmoothGray8(640, 480, GET_INPUT_ADDR(fnum),
					CE_BYTE_1_BASEADDR);
					CensusEngine8to32(640, 480, 20, 3, 7, CE_BYTE_1_BASEADDR,
					CE_OUTPUT_1_BASEADDR);
					count = get_cyclecount() - count;
					printf("CE software processing 1 time:%f \n",
							((float) count) / CPUFREQ * 1000);
					ceswcount++;
					ceswsum += count;

					init_perfcounters(1, 0);
					EnablePerfCounters();
					count = get_cyclecount();
					ImageSmoothGray8(640, 480, GET_INPUT_ADDR(fnum+1),
					CE_BYTE_2_BASEADDR);
					CensusEngine8to32(640, 480, 20, 3, 7, CE_BYTE_2_BASEADDR,
					CE_OUTPUT_2_BASEADDR);
					count = get_cyclecount() - count;
					printf("CE software processing 2 time:%f \n",
							((float) count) / CPUFREQ * 1000);
					ceswcount++;
					ceswsum += count;
					Xil_DCacheFlush();
				} else {
					printf("Camera mode, Software CE processing\n");
					while (FRAME_INTR == 0)
						;
					FRAME_INTR = 0;
					Camera24ToGray8(640, 480,
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							VIDEO_BASEADDR, CE_PROC_1_BASEADDR);

					init_perfcounters(1, 0);
					EnablePerfCounters();
					count = get_cyclecount();
					ImageSmoothGray8(640, 480, CE_PROC_1_BASEADDR,
					CE_BYTE_1_BASEADDR);
					CensusEngine8to32(640, 480, 20, 3, 7, CE_BYTE_1_BASEADDR,
					CE_OUTPUT_1_BASEADDR);
					count = get_cyclecount() - count;
					printf("CE software processing 1 time:%f \n",
							((float) count) / CPUFREQ * 1000);
					ceswcount++;
					ceswsum += count;

					while (FRAME_INTR == 0)
						;
					FRAME_INTR = 0;
					CameraCopy(640, 480,
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							VIDEO_BASEADDR, BUF_CAM);
					Camera24ToGray8(640, 480,
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							VIDEO_BASEADDR, CE_PROC_2_BASEADDR);

					init_perfcounters(1, 0);
					EnablePerfCounters();
					count = get_cyclecount();
					ImageSmoothGray8(640, 480, CE_PROC_2_BASEADDR,
					CE_BYTE_2_BASEADDR);
					CensusEngine8to32(640, 480, 20, 3, 7, CE_BYTE_2_BASEADDR,
					CE_OUTPUT_2_BASEADDR);
					count = get_cyclecount() - count;
					printf("CE software processing 2 time:%f \n",
							((float) count) / CPUFREQ * 1000);
					ceswcount++;
					ceswsum += count;

					Xil_DCacheFlush();
				}

			} else {
				if (!XCe_IsReady(&hlsCE)) {
					DBG_MSG(
							"!!! HLS_CE peripheral is not ready! Exiting...\n\r");
					return XST_FAILURE;
				}
				if (!Camflag) {
					printf("Still image mode, Hardware CE processing\n");
					XCe_SetReadaddress(&hlsCE, GET_INPUT_ADDR(fnum));
				} else {
					printf("Camera mode, Hardware CE processing\n");
					while (FRAME_INTR == 0)
						;
					FRAME_INTR = 0;
					Camera24ToGray8(640, 480,
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							VIDEO_BASEADDR, CE_PROC_1_BASEADDR);
					Xil_DCacheFlush();
					XCe_SetReadaddress(&hlsCE, CE_PROC_1_BASEADDR);
				}

				XCe_SetWriteaddress(&hlsCE, CE_OUTPUT_1_BASEADDR);
				init_perfcounters(1, 0);
				EnablePerfCounters();
				count = get_cyclecount();
				CE_start();
				while (CEisdone == 0)
					;
				CEisdone = 0;
				count = get_cyclecount() - count;
				printf("CE hardware processing 1 time:%f \n",
						((float) count) / CPUFREQ * 1000);
				cehwcount++;
				cehwsum += count;

				if (!XCe_IsReady(&hlsCE)) {
					DBG_MSG(
							"!!! HLS_CE peripheral is not ready! Exiting...\n\r");
					return XST_FAILURE;
				}

				if (!Camflag) {
					XCe_SetReadaddress(&hlsCE, GET_INPUT_ADDR(fnum+1));
				} else {
					while (FRAME_INTR == 0)
						;
					FRAME_INTR = 0;
					CameraCopy(640, 480,
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							VIDEO_BASEADDR, BUF_CAM);
					Camera24ToGray8(640, 480,
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							VIDEO_BASEADDR, CE_PROC_2_BASEADDR);
					Xil_DCacheFlush();
					XCe_SetReadaddress(&hlsCE, CE_PROC_2_BASEADDR);
				}

				XCe_SetWriteaddress(&hlsCE, CE_OUTPUT_2_BASEADDR);
				init_perfcounters(1, 0);
				EnablePerfCounters();
				count = get_cyclecount();
				CE_start();
				while (CEisdone == 0)
					;
				CEisdone = 0;
				count = get_cyclecount() - count;
				printf("CE hardware processing 2 time:%f \n",
						((float) count) / CPUFREQ * 1000);
				cehwcount++;
				cehwsum += count;
			}
			if (!meflag) {
				if (!Camflag) {
					printf("Still image mode, Software ME processing\n");
				} else {
					printf("Camera mode, Software ME processing\n");
				}
				init_perfcounters(1, 0);
				EnablePerfCounters();
				count = get_cyclecount();
				matchcount = MatchingEngine32HWO(640, 480, 7, 7,
				CE_OUTPUT_1_BASEADDR, CE_OUTPUT_2_BASEADDR,
				FEATURE_X1_BASEADDR);
				count = get_cyclecount() - count;
				printf("ME software processing time:%f \n",
						((float) count) / CPUFREQ * 1000);
				printf("matchcount: %d\n", matchcount);
				meswcount++;
				meswsum += count;

				Xil_DCacheFlush();
			} else {
				if (!Camflag) {
					printf("Still image mode, Hardware ME processing\n");
				} else {
					printf("Camera mode, Hardware ME processing\n");
				}
				if (!XMatchingengine32_IsReady(&hlsME)) {
					DBG_MSG(
							"!!! HLS_ME peripheral is not ready! Exiting...\n\r");
					return XST_FAILURE;
				}

				init_perfcounters(1, 0);
				EnablePerfCounters();
				count = get_cyclecount();
				ME_start();
				while (MEisdone == 0)
					;
				MEisdone = 0;
				count = get_cyclecount() - count;
				printf("ME hardware processing time:%f \n",
						((float) count) / CPUFREQ * 1000);
				matchcount = XMatchingengine32_GetReturn(&hlsME);
				printf("matchcount: %d\n", matchcount);
				mehwcount++;
				mehwsum += count;
			}
			if (!Camflag) {
				ConvGray8ToGray32(640, 480, GET_INPUT_ADDR(fnum+1),
				OUTPUT_ME_BASEADDR);
				DrawVector32hw(640, 480, FEATURE_X1_BASEADDR, matchcount,
				OUTPUT_ME_BASEADDR, 0);
				Xil_DCacheFlush();
			}
			presample(FEATURE_X1_BASEADDR, FEATURE_IMG, matchcount, 640, 480);

			if (!eeeflag) {
				if (!Camflag) {
					printf("Still image mode, Software EEE processing\n");
				} else {
					printf("Camera mode, Software EEE processing\n");
				}
				init_perfcounters(1, 0);
				EnablePerfCounters();
				count = get_cyclecount();
				samplecount = sample_flow_vectors(FEATURE_IMG, COMP_VECTOR, 640,
						480, 2, 8);
				ret = estimate_ego_motion_first_order_flow(COMP_VECTOR,
						samplecount, model_param_est, 300, 300, 2, 0.75F);
				count = get_cyclecount() - count;
				printf("SW EEE consuming time: %f ms\n",
						((float) count) / CPUFREQ * 1000);
				printf("samplecount: %d\n", samplecount);

				if (ret) {
					printf("Software EEE succeeds!!\n");
					eeeswcount++;
					eeeswsum += count;
					Xil_DCacheFlush();
				} else {
					printf("Software EEE fails!!\n");
				}
			} else {
				if (!Camflag) {
					printf("Still image mode, Hardware EEE processing\n");
				} else {
					printf("Camera mode, Hardware EEE processing\n");
				}

				if (!XEee_IsReady(&hlsEEE)) {
					DBG_MSG(
							"!!! HLS_EEE peripheral is not ready! Exiting...\n\r");
					return XST_FAILURE;
				}

				XEee_SetMatchcount_v(&hlsEEE, matchcount);
				XEee_SetMode_v(&hlsEEE, 0);

				init_perfcounters(1, 0);
				EnablePerfCounters();
				count = get_cyclecount();

				EEE_start();
				while (EEEisdone == 0)
					;
				EEEisdone = 0;

				samplecount = compressvectorHW(PRE_COMP_VECTOR_HW,
				COMP_VECTOR_HW, 80, 60);

				ret = estimate_ego_motion_first_order_flow_HW( COMP_VECTOR_HW,
						samplecount, model_param_est, model_param_32, 300, 300);

				Xil_DCacheFlush();

				if (ret) {
					if (!XEee_IsReady(&hlsEEE)) {
						DBG_MSG(
								"!!! HLS_EEE peripheral is not ready! Exiting...\n\r");
						return XST_FAILURE;
					}

					XEee_SetMode_v(&hlsEEE, 1);
					XEee_SetModeladdr_v(&hlsEEE, (u32) model_param_32);

					EEE_start();
					while (EEEisdone == 0)
						;
					EEEisdone = 0;

					count = get_cyclecount() - count;
					printf("HW EEE consuming time: %f ms\n",
							((float) count) / CPUFREQ * 1000);
					printf("samplecount: %d\n", samplecount);

					eeehwcount++;
					eeehwsum += count;

					int validvectors = Xil_In32(RESULT_ADDR) >> 16;
					int betteroutlier = Xil_In32(RESULT_ADDR) & 0xffff;
					bmid = Xil_In32(RESULT_ADDR + 4);

					printf(" XC: %f \n\r",
							((float) model_param_32[4 * bmid]) / 8.0);
					printf(" YC: %f \n\r",
							((float) model_param_32[4 * bmid + 1]) / 8.0);
					printf(" D : %f \n\r",
							((float) model_param_32[4 * bmid + 2])
									/ 2147483648.0);
					printf(" R : %f \n\r",
							((float) model_param_32[4 * bmid + 3])
									/ 2147483648.0);
					printf(" Total iterations : %d \n\r", 300);
					printf(" Outlier count    : %d \n\r", betteroutlier);
					printf(" Total count      : %d \n\r", validvectors);
					printf(" Outlier Ratio    : %f \n\r",
							((float) betteroutlier) / ((float) validvectors));
					printf("HW EEE succeeds!!\n");
				} else {
					count = get_cyclecount() - count;
					printf("HW EEE consuming time: %f ms\n",
							((float) count) / CPUFREQ * 1000);
					printf("samplecount: %d\n", samplecount);
					printf("HW EEE fails!!\n");
				}
			}

			if (ret) {
				if (!eceflag) {
					if (eeeflag) {
						model_param_est[0] = ((float) model_param_32[4 * bmid])
								/ 8.0;
						model_param_est[1] = ((float) model_param_32[4 * bmid
								+ 1]) / 8.0;
						model_param_est[2] = ((float) model_param_32[4 * bmid
								+ 2]) / 2147483648.0;
						model_param_est[3] = ((float) model_param_32[4 * bmid
								+ 3]) / 2147483648.0;
					}
					init_perfcounters(1, 0);
					EnablePerfCounters();
					count = get_cyclecount();
					samplecount = sample_flow_vectors(FEATURE_IMG, FULL_VECTOR,
							640, 480, 0, 1);
					gen_firstOrderFlow_vectors_4(model_param_est, samplecount,
					FULL_VECTOR, RES_VECTOR);
					diff_motion_vectors(FULL_VECTOR, RES_VECTOR, samplecount,
					RES_VECTOR, 2);
					count = get_cyclecount() - count;
					printf("SW ECE consuming time: %f ms\n",
							((float) count) / CPUFREQ * 1000);
					if (!Camflag) {
						ConvGray8ToGray32(640, 480, GET_INPUT_ADDR(fnum+1),
						OUTPUT_BASEADDR);
					} else {
						CameraCopy(640, 480,
								detailedTiming[currentResolution][H_ACTIVE_TIME],
								detailedTiming[currentResolution][V_ACTIVE_TIME],
								BUF_CAM, OUTPUT_BASEADDR);
					}
					DrawVector32eee(640, 480, samplecount, RES_VECTOR,
					OUTPUT_BASEADDR, 0);
					Xil_DCacheFlush();

					eceswcount++;
					eceswsum += count;
				} else {
					if (!XEce_IsReady(&hlsECE)) {
						DBG_MSG(
								"!!! HLS_ECE peripheral is not ready! Exiting...\n\r");
						return XST_FAILURE;
					}

					XEce_SetFullvectoraddr_v(&hlsECE, FEATURE_X1_BASEADDR);
					XEce_SetVeccount_v(&hlsECE, matchcount);
					XEce_SetResultvectoraddr_v(&hlsECE, FEATURE_X2_BASEADDR);
					XEce_SetErrthres_v(&hlsECE, 2);

					if (!eeeflag) {
						model_param_32_tmp[0] = (int) (model_param_est[0] * 8);
						model_param_32_tmp[1] = (int) (model_param_est[1] * 8);
						model_param_32_tmp[2] = (int) (model_param_est[2]
								* 2147483648);
						model_param_32_tmp[3] = (int) (model_param_est[3]
								* 2147483648);
					} else {
						model_param_32_tmp[0] = model_param_32[4 * bmid];
						model_param_32_tmp[1] = model_param_32[4 * bmid + 1];
						model_param_32_tmp[2] = model_param_32[4 * bmid + 2];
						model_param_32_tmp[3] = model_param_32[4 * bmid + 3];
					}
					Xil_Out32(BEST_MODEL, model_param_32_tmp[0]);
					Xil_Out32(BEST_MODEL + 4, model_param_32_tmp[1]);
					Xil_Out32(BEST_MODEL + 8, model_param_32_tmp[2]);
					Xil_Out32(BEST_MODEL + 12, model_param_32_tmp[3]);
					Xil_DCacheFlush();

					XEce_SetBestmodeladdr_v(&hlsECE, BEST_MODEL);

					init_perfcounters(1, 0);
					EnablePerfCounters();
					count = get_cyclecount();
					ECE_start();
					while (ECEisdone == 0)
						;
					ECEisdone = 0;
					count = get_cyclecount() - count;
					matchcount2 = XEce_GetReturn(&hlsECE);

					printf("HW ECE consuming time: %f ms\n",
							((float) count) / CPUFREQ * 1000);
					printf("Rest vector numbers: %d \n", matchcount2);

					if (!Camflag) {
						ConvGray8ToGray32(640, 480, GET_INPUT_ADDR(fnum+1),
						OUTPUT_BASEADDR);
					} else {
						CameraCopy(640, 480,
								detailedTiming[currentResolution][H_ACTIVE_TIME],
								detailedTiming[currentResolution][V_ACTIVE_TIME],
								BUF_CAM, OUTPUT_BASEADDR);
					}
					DrawVector32hw(640, 480, FEATURE_X2_BASEADDR, matchcount2,
					OUTPUT_BASEADDR, 0);
					Xil_DCacheFlush();

					ecehwcount++;
					ecehwsum += count;
				}
			} else {
				printf("Because of EEE failure, no compensation done!!\n");
				if (!Camflag) {
					ConvGray8ToGray32(640, 480, GET_INPUT_ADDR(fnum+1),
					OUTPUT_BASEADDR);
				} else {
					CameraCopy(640, 480,
							detailedTiming[currentResolution][H_ACTIVE_TIME],
							detailedTiming[currentResolution][V_ACTIVE_TIME],
							BUF_CAM, OUTPUT_BASEADDR);
				}
				DrawVector32hw(640, 480, FEATURE_X1_BASEADDR, matchcount,
				OUTPUT_BASEADDR, 0);
				Xil_DCacheFlush();
			}
		}
		if (!Camflag) {
			ConfigHdmiVDMA(detailedTiming[currentResolution][H_ACTIVE_TIME],
					detailedTiming[currentResolution][V_ACTIVE_TIME],
					OUTPUT_BASEADDR);
			if (ATV_GetElapsedMs(StartCount, NULL) >= HDMI_CALL_INTERVAL_MS) {
				StartCount = HAL_GetCurrentMsCount();
				if (APP_DriverEnabled()) {
					ADIAPI_TransmitterMain();
				}
			}
		} else {
			if (camceflag) {
				ConfigHdmiVDMA(detailedTiming[currentResolution][H_ACTIVE_TIME],
						detailedTiming[currentResolution][V_ACTIVE_TIME],
						CE_OUTPUT_1_BASEADDR);
			} else if (cammeflag) {
				CameraCopy(640, 480,
						detailedTiming[currentResolution][H_ACTIVE_TIME],
						detailedTiming[currentResolution][V_ACTIVE_TIME],
						BUF_CAM, OUTPUT_ME_BASEADDR);
				DrawVector32hw(640, 480, FEATURE_X1_BASEADDR, matchcount,
				OUTPUT_ME_BASEADDR, 0);
				Xil_DCacheFlush();
				ConfigHdmiVDMA(detailedTiming[currentResolution][H_ACTIVE_TIME],
						detailedTiming[currentResolution][V_ACTIVE_TIME],
						OUTPUT_ME_BASEADDR);
			} else {
				ConfigHdmiVDMA(detailedTiming[currentResolution][H_ACTIVE_TIME],
						detailedTiming[currentResolution][V_ACTIVE_TIME],
						OUTPUT_BASEADDR);

			}
			if (ATV_GetElapsedMs(StartCount, NULL) >= HDMI_CALL_INTERVAL_MS) {
				StartCount = HAL_GetCurrentMsCount();
				if (APP_DriverEnabled()) {
					ADIAPI_TransmitterMain();
				}
			}
		}

	}

	printf("-------------------------------------\n");
	printf("-------Final Performance Report------\n");
	printf("-------------------------------------\n");
	printf("Totally SW CE processed frame amount %d frames\n", ceswcount);
	printf("Totally HW CE processed frame amount %d frames\n", cehwcount);
	printf("Totally SW ME processed frame amount %d frames\n", meswcount);
	printf("Totally HW ME processed frame amount %d frames\n", mehwcount);
	printf("Totally SW EEE processed frame amount %d frames\n", eeeswcount);
	printf("Totally HW EEE processed frame amount %d frames\n", eeehwcount);
	printf("Totally SW ECE processed frame amount %d frames\n", eceswcount);
	printf("Totally HW ECE processed frame amount %d frames\n", ecehwcount);
	ceswcount == 0 ?
			printf("No SW CE processing time!!\n") :
			printf("Average SW CE processing time: %f ms \n",
					ceswsum / ceswcount / CPUFREQ * 1000);
	cehwcount == 0 ?
			printf("No HW CE processing time!!\n") :
			printf("Average HW CE processing time: %f ms \n",
					cehwsum / cehwcount / CPUFREQ * 1000);
	meswcount == 0 ?
			printf("No SW ME processing time!!\n") :
			printf("Average SW ME processing time: %f ms \n",
					meswsum / meswcount / CPUFREQ * 1000);
	mehwcount == 0 ?
			printf("No HW ME processing time!!\n") :
			printf("Average HW ME processing time: %f ms \n",
					mehwsum / mehwcount / CPUFREQ * 1000);
	eeeswcount == 0 ?
			printf("No SW EEE processing time!!\n") :
			printf("Average SW EEE processing time: %f ms \n",
					eeeswsum / eeeswcount / CPUFREQ * 1000);
	eeehwcount == 0 ?
			printf("No HW EEE processing time!!\n") :
			printf("Average HW EEE processing time: %f ms \n",
					eeehwsum / eeehwcount / CPUFREQ * 1000);
	eceswcount == 0 ?
			printf("No SW ECE processing time!!\n") :
			printf("Average SW ECE processing time: %f ms \n",
					eceswsum / eceswcount / CPUFREQ * 1000);
	ecehwcount == 0 ?
			printf("No HW ECE processing time!!\n") :
			printf("Average HW ECE processing time: %f ms \n",
					ecehwsum / ecehwcount / CPUFREQ * 1000);
	printf("-------------------------------------\n");
	printf("-------------------------------------\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return (0);
}
