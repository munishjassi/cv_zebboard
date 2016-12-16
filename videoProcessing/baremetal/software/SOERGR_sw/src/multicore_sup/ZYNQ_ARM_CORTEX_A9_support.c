
// this file contains code that is used to setup Zynq ARM Cortex A9 core and to enable it to execute any dataprocessing task
// The Master CPU does:
// * triggers this core to execute the initial bootup code to power up
// * triggers this core to execute a particular data processing method
#include "ZYNQ_ARM_CORTEX_A9_support.h"
#include "xil_io.h"
#include "xil_mmu.h"


#define sev() __asm__("sev")

static int num_proc_initialized = 0;

void ZYNQ_ARM_CORTEX_A9_initialize() {        
    int i, delay;
    for (i=0; i<NUM_PROCS; i++) {
        if (SoCProc_elements[i].type == ZYNQ_ARM_CORTEX_A9) {
            *((SoCProc volatile ** )(SHARED_MEM_BASEADDR + num_proc_initialized*4)) = &SoCProc_elements[i];
            if (num_proc_initialized == 0) {
                Xil_Out32(0xfffffff0, (u32) Powerup);           // booting up 1st cpu of this kind
                dmb();
                sev();
                
                for (delay=0; delay<10000; delay++);    // to ensure that the core is properly initialized before moving onto initializing other core
                num_proc_initialized++;
            } // else if (num_proc_initialized == 1) {
            // ......do something to boot 2nd cpu of this kind
            // and so on till all cpus are catered for
            
            // right now we only need to deal with only one cpu of this kind!!
        }
    }
}



unsigned char ZYNQ_ARM_CORTEX_A9_process() {
	int i;
	for (i=0; i<num_proc_initialized; i++) {
	    SoCProc *elem = (SoCProc *) *((SoCProc volatile ** )(SHARED_MEM_BASEADDR + i*4));
	    // TODO: should also check if this processor is not busy AND not halted (due to power management etc), only then execute function on this processor!!
	    if (elem->halted == 0 && elem->busy == 0) {      // a free CPU instance found
	        if (i==0) {             // 1st cpu of this kind is free    
	            Xil_Out32(0xfffffff0, (u32) mainProcess);           // triggering the 1st cpu of this kind to start executing the dataflow RTC function passed by application developer
	            dmb();
	            sev();
	            
	            return 1;
            } //else if (i== 1) {       // 2nd cpu of this kind is free
            // ......do something to boot 2nd cpu of this kind
            // and so on till all cpus are catered for
            
            // right now we only need to deal with only one cpu of this kind!!
	    }
	}
	
	// no processor instance found free so return 0
	return 0;
}



// NOTE: the initial boot code (wait for SEV from cpu0 code) is copied to DDR memory as it was found that it got corrupted while using code from 0xffffff00 during runtime
void Powerup() {              
    // BSP startup code!!
	asm volatile (
						/* Write to ACTLR */
						"mrc	p15, 0, r0, c1, c0, 1		/* Read ACTLR*/\n"
						"orr	r0, r0, #(0x01 << 6)		/* set SMP bit */\n"
						"orr	r0, r0, #(0x01 )		/* */\n"
						"mcr	p15, 0, r0, c1, c0, 1		/* Write ACTLR*/\n"

						/* Invalidate caches and TLBs */
						"mov	r0,#0				/* r0 = 0  */\n"
						"mcr	p15, 0, r0, c8, c7, 0		/* invalidate TLBs */\n"
						"mcr	p15, 0, r0, c7, c5, 0		/* invalidate icache */\n"
						"mcr	p15, 0, r0, c7, c5, 6		/* Invalidate branch predictor array */\n"
						"bl	invalidate_dcache		/* invalidate dcache */\n"

						"invalidate_dcache:\n"
						"	mrc	p15, 1, r0, c0, c0, 1		/* read CLIDR */\n"
						"	ands	r3, r0, #0x7000000\n"
						"	mov	r3, r3, lsr #23			/* cache level value (naturally aligned) */\n"
						"	beq	finished\n"
						"	mov	r10, #0				/* start with level 0 */\n"
						"loop1:\n"
						"	add	r2, r10, r10, lsr #1		/* work out 3xcachelevel */\n"
						"	mov	r1, r0, lsr r2			/* bottom 3 bits are the Cache type for this level */\n"
						"	and	r1, r1, #7			/* get those 3 bits alone */\n"
						"	cmp	r1, #2\n"
						"	blt	skip				/* no cache or only instruction cache at this level */\n"
						"	mcr	p15, 2, r10, c0, c0, 0		/* write the Cache Size selection register */\n"
						"	isb					/* isb to sync the change to the CacheSizeID reg */\n"
						"	mrc	p15, 1, r1, c0, c0, 0		/* reads current Cache Size ID register */\n"
						"	and	r2, r1, #7			/* extract the line length field */\n"
						"	add	r2, r2, #4			/* add 4 for the line length offset (log2 16 bytes) */\n"
						"	ldr	r4, =0x3ff\n"
						"	ands	r4, r4, r1, lsr #3		/* r4 is the max number on the way size (right aligned) */\n"
						"	clz	r5, r4				/* r5 is the bit position of the way size increment */\n"
						"	ldr	r7, =0x7fff\n"
						"	ands	r7, r7, r1, lsr #13		/* r7 is the max number of the index size (right aligned) */\n"
						"loop2:\n"
						"	mov	r9, r4				/* r9 working copy of the max way size (right aligned) */\n"
						"loop3:\n"
						"	orr	r11, r10, r9, lsl r5		/* factor in the way number and cache number into r11 */\n"
						"	orr	r11, r11, r7, lsl r2		/* factor in the index number */\n"
						"	mcr	p15, 0, r11, c7, c14, 2		/* clean & invalidate by set/way */\n"
						"	subs	r9, r9, #1			/* decrement the way number */\n"
						"	bge	loop3\n"
						"	subs	r7, r7, #1			/* decrement the index */\n"
						"	bge	loop2\n"
						"skip:\n"
						"	add	r10, r10, #2			/* increment the cache number */\n"
						"	cmp	r3, r10\n"
						"	bgt	loop1\n"

						"finished:\n"
						"	mov	r10, #0				/* swith back to cache level 0 */\n"
						"	mcr	p15, 2, r10, c0, c0, 0		/* select current cache level in cssr */\n"
						"	dsb\n"
						"	isb\n"


						/* Disable MMU, if enabled */
						"mrc	p15, 0, r0, c1, c0, 0		/* read CP15 register 1 */\n"
						"bic	r0, r0, #0x1			/* clear bit 0 */\n"
						"mcr	p15, 0, r0, c1, c0, 0		/* write value back */\n"

						/* setup stack pointer for cpu1*/
						"mrs	r0, cpsr			/* get the current PSR */\n"
						"mvn	r1, #0x1f			/* set up the system stack pointer */\n"
						"and	r2, r1, r0\n"
						"orr	r2, r2, #0x1F			/* SYS mode */\n"
						"msr	cpsr, r2\n"
						"ldr	r13,=__stack1			/* SYS stack pointer */\n"

						/* enable MMU and cache */
						"ldr	r0,=MMUTable			/* Load MMU translation table base */\n"
						"orr	r0, r0, #0x5B			/* Outer-cacheable, WB */\n"
						"mcr	15, 0, r0, c2, c0, 0		/* TTB0 */\n"

						"mvn	r0,#0				/* Load MMU domains -- all ones=manager */\n"
						"mcr	p15,0,r0,c3,c0,0\n"

						/* Enable mmu, icahce and dcache */
						"ldr	r0,=0b01000000000101\n"
						"mcr	p15,0,r0,c1,c0,0		/* Enable cache and MMU */\n"
						"dsb					/* dsb	allow the MMU to start up */\n"
						"isb					/* isb	flush prefetch buffer */\n"

						/* enable FPU */
						"mov	r0, r0\n"
						"mrc	p15, 0, r1, c1, c0, 2		/* read cp access control register (CACR) into r1 */\n"
						"orr	r1, r1, #(0xf << 20)		/* enable full access for p10 & p11 */\n"
						"mcr	p15, 0, r1, c1, c0, 2		/* write back into CACR */\n"

						/* enable vfp*/
						"fmrx	r1, FPEXC			/* read the exception register */\n"
						"orr	r1,r1, #0x40000000		/* set VFP enable bit, leave the others in orig state */\n"
						"fmxr	FPEXC, r1			/* write back the exception register */\n"

						"mrc	p15,0,r0,c1,c0,0		/* flow prediction enable */\n"
						"orr	r0, r0, #(0x01 << 11)		/* #0x8000 */\n"
						"mcr	p15,0,r0,c1,c0,0\n"

						"mrc	p15,0,r0,c1,c0,1		/* read Auxiliary Control Register */\n"
						"orr	r0, r0, #(0x1 << 2)		/* enable Dside prefetch */\n"
						"orr	r0, r0, #(0x1 << 1)		/* enable L2 Prefetch hint */\n"
						"mcr	p15,0,r0,c1,c0,1		/* write Auxiliary Control Register */\n"

				);

    // read in the processor id register to know which processor instance is executing this code if there are > 1 processors of this type
    unsigned int proc_instance = 0;         //right now this is not the case so hardcoding the id  
    // associating a unique processor descriptor passed from master cpu to this core
    SoCProc *elem = (SoCProc *) *((SoCProc volatile ** )(SHARED_MEM_BASEADDR + proc_instance*4));

    elem->halted = 0;					// indicating that this core is now activated

	// duplicating the initial boot code for CPU1 into ddr
	Xil_Out32((u32) elem->MEMSPACE_BASEADDR, (u32) 0xe3e0000f);
	Xil_Out32((u32) elem->MEMSPACE_BASEADDR+4, (u32) 0xe3a01000);
	Xil_Out32((u32) elem->MEMSPACE_BASEADDR+8, (u32) 0xe5801000);
	Xil_Out32((u32) elem->MEMSPACE_BASEADDR+12, (u32) 0xe320f002);
	Xil_Out32((u32) elem->MEMSPACE_BASEADDR+16, (u32) 0xe5902000);
	Xil_Out32((u32) elem->MEMSPACE_BASEADDR+20, (u32) 0xe1520001);
	Xil_Out32((u32) elem->MEMSPACE_BASEADDR+24, (u32) 0x0afffffb);
	Xil_Out32((u32) elem->MEMSPACE_BASEADDR+28, (u32) 0xe1a0f002);

    /* Disable caching on shared OCM data by setting the appropriate TLB
       attributes for the shared data space in OCM.*/
	// making sure that this core has the same memory view as that of Master CPU!!
    Xil_SetTlbAttributes(SHARED_MEM_BASEADDR, 0x14de2);


	// resetting the stack and then branching to initial boot code (waiting for sev from Master CPU) 
	asm volatile (  
						"mrs	r0, cpsr			/* get the current PSR */\n"
						"mvn	r1, #0x1f			/* set up the system stack pointer */\n"
						"and	r2, r1, r0\n"
						"orr	r2, r2, #0x1F			/* SYS mode */\n"
						"msr	cpsr, r2\n"
						"ldr	r13,=__stack1			/* SYS stack pointer */\n"
				   );	
	Xil_Out32((u32) 0xfffffff0, (u32) 0x0);
	asm volatile("bx %0" : : "r" (elem->MEMSPACE_BASEADDR));
}


void mainProcess() {
    // read in the processor id register to know which processor instance is executing this code if there are > 1 processors of this type
    unsigned int proc_instance = 0;         //right now this is not the case so hardcoding the id  
    // associating a unique processor descriptor passed from master cpu to this core
    SoCProc *elem = (SoCProc *) *((SoCProc volatile ** )(SHARED_MEM_BASEADDR + proc_instance*4));
    // marking the busy flag
    elem->busy = 1;    
    
    // executing the dataflow RTC function provided by the application developer
	processFrame(elem->MEMSPACE_BASEADDR + 0x2000000);

    // deasserting the busy flag to show this core is now free
    elem->busy = 0;

    // resetting the stack and then branching to initial boot code (waiting for sev from Master CPU) 
	asm volatile (
						"mrs	r0, cpsr			/* get the current PSR */\n"
						"mvn	r1, #0x1f			/* set up the system stack pointer */\n"
						"and	r2, r1, r0\n"
						"orr	r2, r2, #0x1F			/* SYS mode */\n"
						"msr	cpsr, r2\n"
						"ldr	r13,=__stack1			/* SYS stack pointer */\n"
				   );

	Xil_Out32((u32) 0xfffffff0, (u32) 0x0);
	asm volatile("bx %0" : : "r" (elem->MEMSPACE_BASEADDR));
}








