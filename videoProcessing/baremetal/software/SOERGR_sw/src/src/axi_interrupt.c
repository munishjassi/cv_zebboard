/*****************************************************************************
* Filename:          C:\Cases\subcases\nan\929077\sp605_axi_interrupt/drivers/axi_interrupt_v1_00_a/src/axi_interrupt.c
* Version:           1.00.a
* Description:       axi_interrupt Driver Source File
* Date:              Wed Jul 25 16:45:59 2012 (by Create and Import Peripheral Wizard)
*****************************************************************************/


/***************************** Include Files *******************************/

#include "axi_interrupt.h"
#include "cf_hdmi.h"
#include "transmitter.h"
#include <stdio.h>

#include "profile_cnt.h"
#include "global.h"


/************************** Function Definitions ***************************/

/**
 *
 * Enable all possible interrupts from TEST_AXI device.
 *
 * @param   baseaddr_p is the base address of the TEST_AXI device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void AXI_INTERRUPT_EnableInterrupt(void * baseaddr_p)
{
  Xuint32 baseaddr;
  baseaddr = (Xuint32) baseaddr_p;

  /*
   * Enable all interrupt source from user logic.
   */
  AXI_INTERRUPT_mWriteReg(baseaddr, AXI_INTERRUPT_INTR_IPIER_OFFSET, 0x00000003);

  /*
   * Set global interrupt enable.
   */
  AXI_INTERRUPT_mWriteReg(baseaddr, AXI_INTERRUPT_INTR_DGIER_OFFSET, INTR_GIE_MASK);
}

/**
 *
 * Example interrupt controller handler for TEST_AXI device.
 * This is to show example of how to toggle write back ISR to clear interrupts.
 *
 * @param   baseaddr_p is the base address of the TEST_AXI device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
unsigned int t=0;
void AXI_INTERRUPT_VsyncIntr_Handler(void * baseaddr_p)
{

#ifdef DO_PERF_EVAL
	received_frame_count++;
#endif

	if (FRAME_INTR == 0) {		// master cpu available for processing
		FRAME_INTR = 1;

#ifdef DO_PERF_EVAL
		processed_frame_count++;
#endif

	} else {					// master cpu busy processing previous frame
#ifdef USE_MULTICORE
		if (SoCProc_processDataflow() == 0) {
			//printf("missing frame:%d\r\n", received_frame_count);
		} else {

#ifdef DO_PERF_EVAL
			processed_frame_count++;
#endif

		}
#else
		//printf("missing frame:%d\r\n", received_frame_count);
#endif
	}


#ifdef DO_PERF_EVAL
		// if received frames are equal to the frames considered for stats, then evaluate performance metrics
		if (received_frame_count == TOTAL_FRAMES_FOR_STATS) {
			evaluate_frameThroughput();
		}
#endif
}



void AXI_INTERRUPT_HrefIntr_Handler(void * baseaddr_p)
{
  static int cnt = 0;
  cnt++;
  imageRow++;
}
