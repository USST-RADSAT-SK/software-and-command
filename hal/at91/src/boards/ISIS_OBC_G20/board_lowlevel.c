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
/// \unit
///
/// !Purpose
///
/// Provides the low-level initialization function that gets called on chip
/// startup.
///
/// !Usage
///
/// LowLevelInit() is called in #board_cstartup.S#.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "at91/boards/ISIS_OBC_G20/board.h"
#include "at91/boards/ISIS_OBC_G20/board_memories.h"
#include "at91/utility/trace.h"
#include "at91/peripherals/pmc/pmc.h"

//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------
/*
    Constants: Clock and PLL settings

        BOARD_OSCOUNT - Startup time of main oscillator (in number of slow clock ticks).
        BOARD_USBDIV - USB PLL divisor value to obtain a 48MHz clock.
        BOARD_CKGR_PLL - PLL frequency range.
        BOARD_PLLCOUNT - PLL startup time (in number of slow clock ticks).
        BOARD_MUL - PLL MUL value.
        BOARD_DIV - PLL DIV value.
        BOARD_PRESCALER - Master clock prescaler value.
*/
#define BOARD_OSCOUNT           (AT91C_CKGR_OSCOUNT & (64 << 8))
#define BOARD_CKGR_PLLA         (AT91C_CKGR_SRCA | AT91C_CKGR_OUTA_0)
#define BOARD_PLLACOUNT         (0x3F << 8)
#define BOARD_MULA              (AT91C_CKGR_MULA & (0x2A << 16))
#define BOARD_DIVA              (AT91C_CKGR_DIVA & 1)
#define BOARD_PRESCALER         ((BOARD_PRES<<2) | (BOARD_MDIV<<8) | (BOARD_PDIV<<12))

#define BOARD_USBDIV            AT91C_CKGR_USBDIV_1
#define BOARD_CKGR_PLLB         AT91C_CKGR_OUTB_0
#define BOARD_PLLBCOUNT         BOARD_PLLACOUNT
#define BOARD_MULB              (25 << 16)
#define BOARD_DIVB              5

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Default spurious interrupt handler. Infinite loop.
//------------------------------------------------------------------------------
void defaultSpuriousHandler( void )
{
	TRACE_DEBUG("\n\r UNEXPECTED SPURIOUS INTERRUPT OCCURRED, HALTING! \n\r");
	while(1);
}

//------------------------------------------------------------------------------
/// Default handler for fast interrupt requests. Infinite loop.
//------------------------------------------------------------------------------
void defaultFiqHandler( void )
{
	TRACE_FATAL("\n\r UNEXPECTED FIQ INTERRUPT OCCURRED, HALTING! \n\r");
	while(1);
}

//------------------------------------------------------------------------------
/// Default handler for standard interrupt requests. Infinite loop.
//------------------------------------------------------------------------------
void defaultIrqHandler( void )
{
	TRACE_FATAL("\n\r UNEXPECTED INTERRUPT OCCURRED, HALTING! \n\r");
	while(1);
}

/*!
 * Akhil: Added a function that can be used to edit the AMBA Bus-Matrix priorities etc.
 * If very fast speed is needed using the PDC for buses like SPI, UART, USB or Ethernet, this optimization can be used.
 *
 * @note Here be Dragons.
 * Such optimizations should be used very carefully as speeding up access by one master may starve another.
 * For example, setting a high-priority for PDC may make the code run slower because the ARM-core now has low-priority.
 */
void assignBusMatrixPriorities() {
	// See AT91SAM9G20 datasheet Page-16 for the list of Bus Matrix Masters and Slaves and Page-125 onwards for a description of the bus-matrix
	// Masters: 0=ARM-Instruction, 1=ARM-Data, 2=PDC, 3=ISI, 4=Ethernet MAC, 5=USB Host DMA
	// Slaves:  0=InternalSRAM0, 1=InternalSRAM1, 2=InternalROM + USB host user-interface, 3=EBI, 4=Peripherals.

	// Bus-Matrix-Slave-3(EBI) configuration for: Default Mater type = fixed, default mater = PDC, arbitration type = fixed. EBI is used to access the SDRAM. Slot-cycle=16(default)
	AT91C_BASE_MATRIX->MATRIX_SCFG3 = AT91C_MATRIX_DEFMSTR_TYPE_FIXED_DEFMSTR | AT91C_MATRIX_FIXED_DEFMSTR3_HPDC3 | 1<<24 | 0x10;
	// Bus-Matrix-Slave-4(peripherals) configuration for: Default Mater type = fixed, default mater = PDC, arbitration type = fixed
	AT91C_BASE_MATRIX->MATRIX_SCFG3 = AT91C_MATRIX_DEFMSTR_TYPE_FIXED_DEFMSTR | AT91C_MATRIX_FIXED_DEFMSTR4_HPDC3 | 1<<24 | 0x10;

	// Bus-Matrix-Master-Priority for Slave-3(EBI): Highest to PDC, then ARM-core (data and instruction), then ISI and USB, then Ethernet. Slot-cycle=16(default).
	AT91C_BASE_MATRIX->MATRIX_PRAS3 = 1<<20 | 0<<16 | 1<<12 | 3<<8 | 2<<4 | 2<<0;
	// Bus-Matrix-Master-Priority for Slave-4(peripherals) Highest to PDC, then ARM-core
	AT91C_BASE_MATRIX->MATRIX_PRAS4 = 0<<20 | 0<<16 | 0<<12 | 3<<8 | 2<<4 | 2<<0;

	// Note if internal buffers are to be located in SRAM1, uncomment and edit the following lines to set the desired priorities
	// Bus-Matrix-Slave-1(EBI) configuration for: Default Mater type = fixed, default mater = PDC, arbitration type = fixed. Slot-cycle=16(default)
	//AT91C_BASE_MATRIX->MATRIX_SCFG1 = AT91C_MATRIX_DEFMSTR_TYPE_FIXED_DEFMSTR | AT91C_MATRIX_FIXED_DEFMSTR3_HPDC3 | 1<<24 | 0x10;
	// Bus-Matrix-Master-Priority for Slave-1(SRAM1): Highest to PDC, then ARM-core (data and instruction), then ISI and USB, then Ethernet. Slot-cycle=16(default).
	//AT91C_BASE_MATRIX->MATRIX_PRAS3 = 1<<20 | 0<<16 | 1<<12 | 3<<8 | 2<<4 | 2<<0;
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Performs the low-level initialization of the chip. Initialisation depends
/// on where the application is executed:
/// - in sdram: it means that sdram has previously been initialized. No further
///             initialization is required.
/// - in sram:  PLL shall be initialized in LowLevelInit. Other initializations
///             can be done later by the application.
/// - in norflash: LowLevelInit can't be executed in norflash because SMC
///             settings can't be changed while executing in external flash.
///             LowLevelInit shall be executed in internal sram. It initializes
///             PLL and SMC.
/// This function also reset the AIC and disable RTT and PIT interrupts
//------------------------------------------------------------------------------
void LowLevelInit(void)
{
    unsigned char i;

#ifndef sdram
    /* Initialize main oscillator
     ****************************/
    AT91C_BASE_PMC->PMC_MOR = BOARD_OSCOUNT | AT91C_CKGR_MOSCEN;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS));

    /* Initialize PLLA at 200MHz (198.656) */
    AT91C_BASE_PMC->PMC_PLLAR = BOARD_CKGR_PLLA
                                | BOARD_PLLACOUNT
                                | BOARD_MULA
                                | BOARD_DIVA;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA));

    // Initialize PLLB for USB usage (if not already locked)
    if (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB)) {
        AT91C_BASE_PMC->PMC_PLLBR = BOARD_USBDIV
                                    | BOARD_CKGR_PLLB
                                    | BOARD_PLLBCOUNT
                                    | BOARD_MULB
                                    | BOARD_DIVB;
        while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB));
    }

    /* Wait for the master clock if it was already initialized */
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    /* Switch to fast clock
     **********************/
    /* Switch to main oscillator + prescaler */
    AT91C_BASE_PMC->PMC_MCKR = BOARD_PRESCALER;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    /* Switch to PLL + prescaler */
    AT91C_BASE_PMC->PMC_MCKR |= AT91C_PMC_CSS_PLLA_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));
#endif

    /* Initialize AIC
     ****************/
    AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
    AT91C_BASE_AIC->AIC_SVR[0] = (unsigned int) defaultFiqHandler;
    for (i = 1; i < 31; i++) {
        AT91C_BASE_AIC->AIC_SVR[i] = (unsigned int) defaultIrqHandler;
    }
    AT91C_BASE_AIC->AIC_SPU = (unsigned int) defaultSpuriousHandler;

    // Unstack nested interrupts
    for (i = 0; i < 8 ; i++) {

        AT91C_BASE_AIC->AIC_EOICR = 0;
    }


    /* Watchdog initialization
     *************************/
    AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

    /* Remap
     *******/
    BOARD_RemapRam();

    // Disable RTT and PIT interrupts (potential problem when program A
    // configures RTT, then program B wants to use PIT only, interrupts
    // from the RTT will still occur since they both use AT91C_ID_SYS)
    AT91C_BASE_RTTC->RTTC_RTMR &= ~(AT91C_RTTC_ALMIEN | AT91C_RTTC_RTTINCIEN);
    AT91C_BASE_PITC->PITC_PIMR &= ~AT91C_PITC_PITIEN;

#ifdef norflash
    BOARD_ConfigureNorFlash(BOARD_NORFLASH_BUSWIDTH);
#endif

#ifndef sdram
    BOARD_ConfigureSdram(BOARD_SDRAM_BUSWIDTH);
#endif
}

