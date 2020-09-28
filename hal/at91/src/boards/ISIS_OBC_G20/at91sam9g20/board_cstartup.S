/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <at91/boards/ISIS_OBC_G20/board.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
// If you change any of these from 0 to non-zero,
// make sure to add instructions to actually reserve this stack space
#define UND_STACK_SIZE	0x00000000		/* Stack size for "undefined instruction" interrupts		*/
#define ABT_STACK_SIZE	0x00028000		/* Stack size for "abort" interrupts						*/
#define FIQ_STACK_SIZE	0x00018000		/* Stack size for "FIQ" interrupts						*/
#define IRQ_STACK_SIZE	0x00018000		/* Stack size for "IRQ" normal interrupts				*/
#define SVC_STACK_SIZE	0x00018000		/* Stack size for "SVC" supervisor mode					*/

#define ARM_MODE_USR	0x10	/* Normal User Mode											*/
#define ARM_MODE_FIQ	0x11	/* FIQ Processing Fast Interrupts Mode						*/
#define ARM_MODE_IRQ	0x12	/* IRQ Processing Standard Interrupts Mode					*/
#define ARM_MODE_SVC	0x13	/* Supervisor Processing Software Interrupts Mode			*/
#define ARM_MODE_ABT	0x17	/* Abort Processing memory Faults Mode						*/
#define ARM_MODE_UND	0x1B	/* Undefined Processing Undefined Instructions Mode			*/
#define ARM_MODE_SYS	0x1F	/* System Running Priviledged Operating System Tasks Mode	*/


#define I_BIT            0x80
#define F_BIT            0x40

//------------------------------------------------------------------------------
//         Startup routine
//------------------------------------------------------------------------------

            .align      4
            .arm

/* Exception vectors
 *******************/
            .section    .vectors, "a", %progbits

resetVector:
        ldr     pc, =resetHandler				/* Reset */
undefVector:
        b       undefVector						/* Undefined instruction */
swiVector:
#ifdef no_os
		b       irqHandler						/* Interrupt */
#else
        ldr     pc, =vPortYieldProcessor		/* Software interrupt */
#endif
prefetchAbortVector:
        ldr     pc, =startOverPrefetchAbort		/* Prefetch abort */
dataAbortVector:
		ldr     pc, =startOverDataAbort			/* Data abort */
reservedVector:
        b       reservedVector					/* Reserved for future use */
irqVector:
        b       irqHandler						/* Interrupt */
fiqVector:
												/* Fast interrupt */
//------------------------------------------------------------------------------
/// Handles a fast interrupt request by branching to the address defined in the
/// AIC.
//------------------------------------------------------------------------------
fiqHandler:
        b       fiqHandler

//------------------------------------------------------------------------------
/// Handles incoming interrupt requests by branching to the corresponding
/// handler, as defined in the AIC. Supports interrupt nesting.
//------------------------------------------------------------------------------
irqHandler:

/* port save context
 *******************/

	// Push R0 as we are going to use the register.
	STMDB	SP!, {R0}

	// Set R0 to point to the task stack pointer.
	STMDB	SP, {SP}^
	NOP
	SUB		SP, SP, #4
	LDMIA	SP!, {R0}

	// Push the return address onto the stack.
	STMDB	R0!, {LR}

	// Now we have saved LR we can use it instead of R0.
	MOV		LR, R0

	// Pop R0 so we can save it onto the system mode stack.
	LDMIA	SP!, {R0}

	// Push all the system mode registers onto the task stack.
	STMDB	LR, {R0-LR}^
	NOP
	SUB		LR, LR, #60

	// Push the SPSR onto the task stack.
	MRS		R0, SPSR
	STMDB	LR!, {R0}

	LDR		R0, =ulCriticalNesting
	LDR		R0, [R0]
	STMDB	LR!, {R0}

	// Store the new top of stack for the task.
	LDR		R1, =pxCurrentTCB
	LDR		R0, [R1]
	STR		LR, [R0]

/* load ISR
 **********/

    /* Write in the IVR to support Protect Mode */
    LDR     lr, =AT91C_BASE_AIC
    LDR     r0, [r14, #AIC_IVR]
    STR     lr, [r14, #AIC_IVR]

    /* Branch to C portion of the interrupt handler */
    MOV     lr, pc
    BX      r0

    /* Acknowledge interrupt */
    LDR     lr, =AT91C_BASE_AIC
    STR     lr, [r14, #AIC_EOICR]

/* port restore context
 **********************/

	// Set the LR to the task stack.
	LDR		R1, =pxCurrentTCB
	LDR		R0, [R1]
	LDR		LR, [R0]

	// The critical nesting depth is the first item on the stack.
	// Load it into the ulCriticalNesting variable.
	LDR		R0, =ulCriticalNesting
	LDMFD	LR!, {R1}
	STR		R1, [R0]

	// Get the SPSR from the stack.
	LDMFD	LR!, {R0}
	MSR		SPSR_cxsf, R0

	// Restore all system mode registers for the task.
	LDMFD	LR, {R0-R14}^
	NOP

	// Restore the return address.
	LDR		LR, [LR, #+60]

	// And return - correcting the offset in the LR to obtain the
	// correct address.
	SUBS	PC, LR, #4

//------------------------------------------------------------------------------
/// Initializes the chip and branches to the main() function.
//------------------------------------------------------------------------------
            .section    .startup
            .global     entry

entry:
resetHandler:

/* Useless instruction for referencing the .vectors section */
        ldr     r0, =resetVector

/* Set pc to actual code location (i.e. not in remap zone) */
	    ldr     pc, =1f

/* Initialize the prerelocate segment */
1:
        ldr     r0, =_eextab
        ldr     r1, =_sprerelocate
        ldr     r2, =_eprerelocate
1:
        cmp     r1, r2
        ldrcc   r3, [r0], #4
        strcc   r3, [r1], #4
        bcc     1b

/* Perform low-level initialization of the chip using LowLevelInit() */
#ifndef sdram
        ldr     sp, =_sstacksram1
#else
        ldr     sp, =_sstack
#endif
        stmfd   sp!, {r0}
	    ldr     r0, =LowLevelInit
        blx     r0

/* Initialize the postrelocate segment */

        ldmfd   sp!, {r0}
        ldr     r1, =_spostrelocate
        ldr     r2, =_epostrelocate
1:
        cmp     r1, r2
        ldrcc   r3, [r0], #4
        strcc   r3, [r1], #4
        bcc     1b

#ifdef sdram
/* Initialize the sram data segment */
/* Only execute this when the code is running from SDRAM */
/* When the code runs from NORFLASH this part of the postrelocate section which is handled above */
        ldr     r1, =_ssramdata
        ldr     r2, =_esramdata
1:
        cmp     r1, r2
        ldrcc   r3, [r0], #4
        strcc   r3, [r1], #4
        bcc     1b
#endif

/* Clear the zero segment */
	    ldr     r0, =_szero
        ldr     r1, =_ezero
        mov     r2, #0
1:
        cmp     r0, r1
        strcc   r2, [r0], #4
        bcc     1b

#ifdef sdram
/* Only execute this when the code is running from SDRAM */
/* When the code runs from NORFLASH this is the same as normal bss which is handled above */
/* Clear the SRAM zero segment */
        ldr     r0, =_ssrambss
        ldr     r1, =_esrambss
        mov     r2, #0
1:
        cmp     r0, r1
        strcc   r2, [r0], #4
        bcc     1b
#endif
/* Setup stacks
 **************/
        ldr     r0, =_sstack /* Save the start of the stack in r0 */
        ldr     sp, =_sstack
#ifdef memtest_enabled
/* Bootloader normally switches the stack to SDRAM unless there is a memtest to be done. */
        ldr     r0, =_sstacksram1 /* Save the start of the stack in r0 */
        ldr     sp, =_sstacksram1
#endif
/* IRQ mode */
        msr     CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
        mov     sp, r0
        sub     r0, r0, #IRQ_STACK_SIZE

/* ABT mode */
        msr     CPSR_c, #ARM_MODE_ABT | I_BIT | F_BIT
        mov     sp, r0
        sub     r0, r0, #ABT_STACK_SIZE

/* Supervisor mode (interrupts enabled) */
        msr     CPSR_c, #ARM_MODE_SVC |I_BIT | F_BIT
        mov     sp, r0
        sub     r0, r0, #SVC_STACK_SIZE

 /* FIQ mode */
        msr     CPSR_c, #ARM_MODE_FIQ |I_BIT | F_BIT
        mov     sp, r0
        sub     r0, r0, #FIQ_STACK_SIZE

/* System mode, the normal mode of operation */
        msr     CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT
        mov     sp, r0

/* Start in supervisor mode, Operation will switch to system mode when the first task starts */
        msr     CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT

/* Branch to main()
 ******************/
        ldr     r0, =main
        blx     r0

startOver:
		ldr     r0, =restart
		blx		r0

startOverPrefetchAbort:
		ldr     r0, =restartPrefetchAbort
		blx		r0

startOverDataAbort:
		ldr     r0, =restartDataAbort
		blx		r0

/* Loop indefinitely when program is finished */
1:
        b       1b
