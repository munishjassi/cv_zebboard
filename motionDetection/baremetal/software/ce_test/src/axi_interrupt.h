/*****************************************************************************
* Filename:          C:\Cases\subcases\nan\929077\sp605_axi_interrupt/drivers/axi_interrupt_v1_00_a/src/axi_interrupt.h
* Version:           1.00.a
* Description:       axi_interrupt Driver Header File
* Date:              Wed Jul 25 16:45:59 2012 (by Create and Import Peripheral Wizard)
*****************************************************************************/

#ifndef AXI_INTERRUPT_H
#define AXI_INTERRUPT_H

/***************************** Include Files *******************************/

#include "xbasic_types.h"
#include "xstatus.h"
#include "xil_io.h"

/************************** Constant Definitions ***************************/

/**
 * User Logic Slave Space Offsets
 * -- SLV_REG0 : user logic slave module register 0
 * -- SLV_REG1 : user logic slave module register 1
 * -- SLV_REG2 : user logic slave module register 2
 * -- SLV_REG3 : user logic slave module register 3
 */
#define AXI_INTERRUPT_USER_SLV_SPACE_OFFSET (0x00000000)
#define AXI_INTERRUPT_SLV_REG0_OFFSET (AXI_INTERRUPT_USER_SLV_SPACE_OFFSET + 0x00000000)
#define AXI_INTERRUPT_SLV_REG1_OFFSET (AXI_INTERRUPT_USER_SLV_SPACE_OFFSET + 0x00000004)
#define AXI_INTERRUPT_SLV_REG2_OFFSET (AXI_INTERRUPT_USER_SLV_SPACE_OFFSET + 0x00000008)
#define AXI_INTERRUPT_SLV_REG3_OFFSET (AXI_INTERRUPT_USER_SLV_SPACE_OFFSET + 0x0000000C)

/**
 * Software Reset Space Register Offsets
 * -- RST : software reset register
 */
#define AXI_INTERRUPT_SOFT_RST_SPACE_OFFSET (0x00000100)
#define AXI_INTERRUPT_RST_REG_OFFSET (AXI_INTERRUPT_SOFT_RST_SPACE_OFFSET + 0x00000000)

/**
 * Software Reset Masks
 * -- SOFT_RESET : software reset
 */
#define SOFT_RESET (0x0000000A)

/**
 * Interrupt Controller Space Offsets
 * -- INTR_DGIER : device (peripheral) global interrupt enable register
 * -- INTR_ISR   : ip (user logic) interrupt status register
 * -- INTR_IER   : ip (user logic) interrupt enable register
 */
#define AXI_INTERRUPT_INTR_CNTRL_SPACE_OFFSET (0x00000180)
#define AXI_INTERRUPT_INTR_DGIER_OFFSET (AXI_INTERRUPT_INTR_CNTRL_SPACE_OFFSET + 0x0000001C)
#define AXI_INTERRUPT_INTR_IPISR_OFFSET (AXI_INTERRUPT_INTR_CNTRL_SPACE_OFFSET + 0x00000020)
#define AXI_INTERRUPT_INTR_IPIER_OFFSET (AXI_INTERRUPT_INTR_CNTRL_SPACE_OFFSET + 0x00000028)

/**
 * Interrupt Controller Masks
 * -- INTR_TERR_MASK : transaction error
 * -- INTR_DPTO_MASK : data phase time-out
 * -- INTR_IPIR_MASK : ip interrupt requeset
 * -- INTR_RFDL_MASK : read packet fifo deadlock interrupt request
 * -- INTR_WFDL_MASK : write packet fifo deadlock interrupt request
 * -- INTR_IID_MASK  : interrupt id
 * -- INTR_GIE_MASK  : global interrupt enable
 * -- INTR_NOPEND    : the DIPR has no pending interrupts
 */
#define INTR_TERR_MASK (0x00000001UL)
#define INTR_DPTO_MASK (0x00000002UL)
#define INTR_IPIR_MASK (0x00000004UL)
#define INTR_RFDL_MASK (0x00000020UL)
#define INTR_WFDL_MASK (0x00000040UL)
#define INTR_IID_MASK (0x000000FFUL)
#define INTR_GIE_MASK (0x80000000UL)
#define INTR_NOPEND (0x80)

/**************************** Type Definitions *****************************/


/***************** Macros (Inline Functions) Definitions *******************/

/**
 *
 * Write a value to a AXI_INTERRUPT register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the AXI_INTERRUPT device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void AXI_INTERRUPT_mWriteReg(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Data)
 *
 */
#define AXI_INTERRUPT_mWriteReg(BaseAddress, RegOffset, Data) \
 	Xil_Out32((BaseAddress) + (RegOffset), (Xuint32)(Data))

/**
 *
 * Read a value from a AXI_INTERRUPT register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the AXI_INTERRUPT device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	Xuint32 AXI_INTERRUPT_mReadReg(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define AXI_INTERRUPT_mReadReg(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (RegOffset))


/**
 *
 * Write/Read 32 bit value to/from AXI_INTERRUPT user logic slave registers.
 *
 * @param   BaseAddress is the base address of the AXI_INTERRUPT device.
 * @param   RegOffset is the offset from the slave register to write to or read from.
 * @param   Value is the data written to the register.
 *
 * @return  Data is the data from the user logic slave register.
 *
 * @note
 * C-style signature:
 * 	void AXI_INTERRUPT_mWriteSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Value)
 * 	Xuint32 AXI_INTERRUPT_mReadSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define AXI_INTERRUPT_mWriteSlaveReg0(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (AXI_INTERRUPT_SLV_REG0_OFFSET) + (RegOffset), (Xuint32)(Value))
#define AXI_INTERRUPT_mWriteSlaveReg1(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (AXI_INTERRUPT_SLV_REG1_OFFSET) + (RegOffset), (Xuint32)(Value))
#define AXI_INTERRUPT_mWriteSlaveReg2(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (AXI_INTERRUPT_SLV_REG2_OFFSET) + (RegOffset), (Xuint32)(Value))
#define AXI_INTERRUPT_mWriteSlaveReg3(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (AXI_INTERRUPT_SLV_REG3_OFFSET) + (RegOffset), (Xuint32)(Value))

#define AXI_INTERRUPT_mReadSlaveReg0(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (AXI_INTERRUPT_SLV_REG0_OFFSET) + (RegOffset))
#define AXI_INTERRUPT_mReadSlaveReg1(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (AXI_INTERRUPT_SLV_REG1_OFFSET) + (RegOffset))
#define AXI_INTERRUPT_mReadSlaveReg2(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (AXI_INTERRUPT_SLV_REG2_OFFSET) + (RegOffset))
#define AXI_INTERRUPT_mReadSlaveReg3(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (AXI_INTERRUPT_SLV_REG3_OFFSET) + (RegOffset))

/**
 *
 * Reset AXI_INTERRUPT via software.
 *
 * @param   BaseAddress is the base address of the AXI_INTERRUPT device.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void AXI_INTERRUPT_mReset(Xuint32 BaseAddress)
 *
 */
#define AXI_INTERRUPT_mReset(BaseAddress) \
 	Xil_Out32((BaseAddress)+(AXI_INTERRUPT_RST_REG_OFFSET), SOFT_RESET)

/************************** Function Prototypes ****************************/


/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the AXI_INTERRUPT instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus AXI_INTERRUPT_SelfTest(void * baseaddr_p);
/**
*  Defines the number of registers available for read and write*/
#define AXI_INTERRUPT_LITE_USER_NUM_REG 4

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
void AXI_INTERRUPT_EnableInterrupt(void * baseaddr_p);

/**
 *
 * Example interrupt controller handler.
 *
 * @param   baseaddr_p is the base address of the TEST_AXI device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void AXI_INTERRUPT_VsyncIntr_Handler(void * baseaddr_p);

void AXI_INTERRUPT_HrefIntr_Handler(void * baseaddr_p);



#endif /** AXI_INTERRUPT_H */
