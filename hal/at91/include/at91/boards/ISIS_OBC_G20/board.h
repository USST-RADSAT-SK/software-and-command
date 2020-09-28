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
/// \dir
/// !Purpose
///
/// Definition and functions for using AT91SAM9G20-related features, such
/// has PIO pins, memories, etc.
///
/// !Usage
/// -# The code for booting the board is provided by board_cstartup.S and
///    board_lowlevel.c.
/// -# For using board PIOs, board characteristics (clock, etc.) and external
///    components, see board.h.
/// -# For manipulating memories (remapping, SDRAM, etc.), see board_memories.h.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \unit
/// !Purpose
///
/// Definition of ISIS_OBC_G20 characteristics, AT91SAM9G20-dependant PIOs and
/// external components interfacing.
///
//------------------------------------------------------------------------------

#ifndef BOARD_H
#define BOARD_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#if defined(at91sam9g20)
    #include "at91/boards/ISIS_OBC_G20/at91sam9g20/AT91SAM9G20.h"
#else
    #error Board does not support the specified chip.
#endif

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "ISIS_OBC_G20 - Board Description"
/// This page lists several definition related to the board description.

/// Name of the board.
#define BOARD_NAME              "ISIS_OBC_G20"
/// Board definition.
#define ISIS_OBC_G20
/// CP15 available
#define CP15_PRESENT

#define USE_PDM_PINS 0
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "ISIS_OBC_G20 - Operating frequencies"
/// This page lists several definition related to the board operating frequency.
/// The PDIV, MDIV and PRES values are used upon startup by the bootloader.
/// Please do not modify these values, they will generally have no effect on the
/// system without recompiling the bootloader.
/// (when using the initialization done by board_lowlevel.c).

/// Frequency of the board main oscillator.
#define BOARD_MAINOSC           18432000

/// PDIV value used to configure PMC_MCKR. This affects the processor clock speed.
#define BOARD_PDIV				1

/// MDIV value used to configure PMC_MCKR. This affects the master clock speed.
#define BOARD_MDIV				3

/// PRES value used to configure PMC_MCKR. This affects both the master and processor clock speeds.
#define BOARD_PRES				0

/// Master clock frequency (when using board_lowlevel.c).
#if BOARD_MDIV==0
	#define BOARD_MCK			((18432000 * 43) / (1<<BOARD_PRES))
#else
	#define BOARD_MCK			((18432000 * 43) / ( BOARD_MDIV * 2 * (1<<BOARD_PRES) ))
#endif

/// Processor clock frequency (when using board_lowlevel.c).
#if BOARD_PDIV==0
	#define BOARD_PROCESSOR_CLOCK	((18432000 * 43) / (1<<BOARD_PRES))
#else
	#define BOARD_PROCESSOR_CLOCK	((18432000 * 43) / ( BOARD_PDIV * 2 * (1<<BOARD_PRES) ))
#endif

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ADC
//------------------------------------------------------------------------------
#define ADC_STARTUP_TIME_MAX       15
/// Track and hold Acquisition Time min (in ns)
#define ADC_TRACK_HOLD_TIME_MIN  1200


//------------------------------------------------------------------------------
/// \page "ISIS_OBC_G20 - USB device"
/// This page lists constants describing several characteristics (controller
/// type, D+ pull-up type, etc.) of the USB device controller of the chip/board.

/// Chip has a UDP controller.
#define BOARD_USB_UDP

/// Indicates the D+ pull-up is internal to the USB controller.
#define BOARD_USB_PULLUP_INTERNAL

/// Number of endpoints in the USB controller.
#define BOARD_USB_NUMENDPOINTS                  6

/// Returns the maximum packet size of the given endpoint.
#define BOARD_USB_ENDPOINTS_MAXPACKETSIZE(i)    ((i >= 4) ? 512 : 64)

/// Returns the number of FIFO banks for the given endpoint.
#define BOARD_USB_ENDPOINTS_BANKS(i)            (((i == 0) || (i == 3)) ? 1 : 2)

/// USB attributes configuration descriptor (bus or self powered, remote wakeup)
#define BOARD_USB_BMATTRIBUTES                  USBConfigurationDescriptor_SELFPOWERED_NORWAKEUP
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "ISIS_OBC_G20 - PIO definitions"
/// This pages lists all the pio definitions contained in board.h. The constants
/// are named using the following convention: PIN_* for a constant which defines
/// a single Pin instance (but may include several PIOs sharing the same
/// controller), and PINS_* for a list of Pin instances.


#if USE_PDM_PINS
	#define PIN_LED_01	{1 << 8,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1,	PIO_DEFAULT}
	#define PIN_LED_02	{1 << 9,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1,	PIO_DEFAULT}
	#define PIN_LED_03	{1 << 10, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1,	PIO_DEFAULT}
	#define PIN_LED_04	{1 << 11, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1,	PIO_DEFAULT}
#else
	#define PIN_LED_01	{1 << 12,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
	#define PIN_LED_02	{1 << 13,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
	#define PIN_LED_03	{1 << 14,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
	#define PIN_LED_04	{1 << 15,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#endif
/// LED Pins
#define PINS_LED     {PIN_LED_01, PIN_LED_02, PIN_LED_03, PIN_LED_04}

/// List of all DBGU pin definitions.
#define PINS_DBGU  {(1<<14) | (1<<15), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}

/// Watchdog pin.
#if USE_PDM_PINS
	#define PIN_WATCHDOG	{(1<<26), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#else
	#define PIN_WATCHDOG	{(1<<30), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#endif

#define PIN_UART0_TXD  {1 << 4, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_UART0_RXD  {1 << 5, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP}
/// UART0 Pins
#define PINS_UART0	{PIN_UART0_TXD, PIN_UART0_RXD}

#define PIN_UART2_TXD			{1 << 8,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_UART2_RXD			{1 << 9,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_UART2_RTS			{1 << 4,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_UART2_CTS			{1 << 5,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_UART2_CFG_485_232	{1 << 8,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UART2_CFG_H_F		{1 << 10, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UART2_CFG_TE485		{1 << 25, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
/// UART2 Pins
#define PINS_UART2	{PIN_UART2_TXD, PIN_UART2_RXD, PIN_UART2_RTS, PIN_UART2_CTS, PIN_UART2_CFG_485_232, PIN_UART2_CFG_H_F, PIN_UART2_CFG_TE485}

#define PIN_SPI0_MISO		{1 << 0,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_SPI0_MOSI		{1 << 1,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_SPI0_SPCK		{1 << 2,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI0_NPCS0		{1 << 3,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP} // FRAM
#define PIN_SPI0_NPCS1		{1 << 11, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_PULLUP} // RTC
#define PIN_SPI0_NPCS2_SW	{1 << 17, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP} // Supervisor (PIC)
/// SPI0 Pins
#define PINS_SPI0 {PIN_SPI0_MISO, PIN_SPI0_MOSI, PIN_SPI0_SPCK, PIN_SPI0_NPCS0, PIN_SPI0_NPCS1, PIN_SPI0_NPCS2_SW}

#define PIN_SPI1_MISO		{1 << 0,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_SPI1_MOSI		{1 << 1,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_SPI1_SPCK		{1 << 2,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI1_NPCS0		{1 << 3,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_SPI1_NPCS1		{1 << 5,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_PULLUP}
#define PIN_SPI1_NPCS2		{1 << 4,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_PULLUP}
#define PIN_SPI1_NPCS3_SW	{1 << 10, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
#define PIN_SPI1_NPCS4_SW	{1 << 11, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
#define PIN_SPI1_NPCS5_SW	{1 << 12, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
#define PIN_SPI1_NPCS6_SW	{1 << 13, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
#define PIN_SPI1_NPCS7_SW	{1 << 20, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
/// SPI1 Pins
#define PINS_SPI1 {PIN_SPI1_MISO, PIN_SPI1_MOSI, PIN_SPI1_SPCK, PIN_SPI1_NPCS0, PIN_SPI1_NPCS1, PIN_SPI1_NPCS2}
/// SPI1 Software-Controlled Chip-Select lines
#define PINS_SPI1_SWCS {PIN_SPI1_NPCS3_SW, PIN_SPI1_NPCS4_SW, PIN_SPI1_NPCS5_SW, PIN_SPI1_NPCS6_SW, PIN_SPI1_NPCS7_SW}

#define PIN_SPI1_MISO_POR		{1 << 0,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_MOSI_POR		{1 << 1,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_SPCK_POR		{1 << 2,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_NPCS0_POR		{1 << 3,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_NPCS1_POR		{1 << 5,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_NPCS2_POR		{1 << 4,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_NPCS3_SW_POR	{1 << 10, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_NPCS4_SW_POR	{1 << 11, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_NPCS5_SW_POR	{1 << 12, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_NPCS6_SW_POR	{1 << 13, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define PIN_SPI1_NPCS7_SW_POR	{1 << 20, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_PULLUP}
/// SPI1 Pins - POR
#define PINS_SPI1_POR {PIN_SPI1_MISO_POR, PIN_SPI1_MOSI_POR, PIN_SPI1_SPCK_POR, PIN_SPI1_NPCS0_POR, PIN_SPI1_NPCS1_POR, PIN_SPI1_NPCS2_POR}
/// SPI1 Software-Controlled Chip-Select lines - POR
#define PINS_SPI1_SWCS_POR {PIN_SPI1_NPCS3_SW_POR, PIN_SPI1_NPCS4_SW_POR, PIN_SPI1_NPCS5_SW_POR, PIN_SPI1_NPCS6_SW_POR, PIN_SPI1_NPCS7_SW_POR}

/// USB VBus monitoring pin definition.
#define PIN_USB_VBUS    {1 << 31, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}

/// List of MCI1 pins definitions. MCI0 is not used as it shares pins with SPI
#define PINS_MCI1  \
	{0xec0,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}, \
	{1 << 8, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/// List of MCI1 pin definitions with all pins in off state to ensure SD card's are not fed power by MCI lines.
#define PINS_MCI1_OFF \
	{0xec0,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}, \
	{1 << 8, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}

/// GPIO Pin used to select amongst SD cards.
#if USE_PDM_PINS
#define PIN_SDSEL {1 << 25, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#else
#define PIN_SDSEL 		{1 << 16, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_NPWR_SD0	{1 << 6,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_NPWR_SD1	{1 << 7,  AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#endif

#define PIN_ADC_ADC0	{1 << 0,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define PIN_ADC_ADC1	{1 << 1,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define PIN_ADC_ADC2	{1 << 2,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define PIN_ADC_ADC3	{1 << 3,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define PIN_ADC_TRGIN	{1 << 22, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT} // PIN_TC_TIOB5 is routed via 0Ohm to this pin, so TC5 can trigger this periodically.
#define PIN_ADC_SEL		{1 << 18, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT} // Used as a Timer pin to select ADC channels in the multiplexer. Same pin as PIN_TC_TIOB4, so TC4 can be used by the ADC driver to periodically select between the lower and upper 4 ADC channels.
#define PIN_ADC_TRGOUT	{1 << 19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT} // Timer pin routed to PIN_ADC_TRGIN. Same as PIN_TC_TIOB5, so TC5 can be used by the ADC driver to trigger the sampling at the ADC at the desired sampling rate.
/// Pins ADC
#define PINS_ADC {PIN_ADC_ADC0, PIN_ADC_ADC1, PIN_ADC_ADC2, PIN_ADC_ADC3, PIN_ADC_TRGIN, PIN_ADC_SEL, PIN_ADC_TRGOUT}

#define PIN_TC_TIOA0	{1 << 26, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT} ///< PWM channel 0
#define PIN_TC_TIOA1	{1 << 27, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT} ///< PWM channel 2
#define PIN_TC_TIOA2	{1 << 28, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT} ///< PWM channel 4
#define PIN_TC_TIOB0	{1 << 9,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT} ///< PWM channel 1
#define PIN_TC_TIOB1	{1 << 7,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT} ///< PWM channel 3
#define PIN_TC_TIOB2	{1 << 6,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT} ///< PWM channel 5
/// Pins TC
#define PINS_TC  {PIN_TC_TCLK0, PIN_TC_TIOA0, PIN_TC_TIOA1, PIN_TC_TIOA2, PIN_TC_TCLK1, PIN_TC_TCLK2, PIN_TC_TIOB0, PIN_TC_TIOB1, PIN_TC_TIOB2}
/// Pins PWM, used by the ISIS PWM driver. It uses 6 channels and internal clock.
#define PINS_PWM {PIN_TC_TIOA0, PIN_TC_TIOB0, PIN_TC_TIOA1, PIN_TC_TIOB1, PIN_TC_TIOA2, PIN_TC_TIOB2}

#define PIN_TC_TIOA0_PIO	{1 << 26, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT} ///< PWM channel 0
#define PIN_TC_TIOA1_PIO	{1 << 27, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT} ///< PWM channel 2
#define PIN_TC_TIOA2_PIO	{1 << 28, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT} ///< PWM channel 4
#define PIN_TC_TIOB0_PIO	{1 << 9,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT} ///< PWM channel 1
#define PIN_TC_TIOB1_PIO	{1 << 7,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT} ///< PWM channel 3
#define PIN_TC_TIOB2_PIO	{1 << 6,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT} ///< PWM channel 5

/// PWM pins assigned as PIO with output set to 0. This is used by the ISIS PWM driver to make sure the pins are set to 0 when the PWM is stopped.
#define PINS_PWM_PIO {PIN_TC_TIOA0_PIO, PIN_TC_TIOB0_PIO, PIN_TC_TIOA1_PIO, PIN_TC_TIOB1_PIO, PIN_TC_TIOA2_PIO, PIN_TC_TIOB2_PIO}

/// TWI pins definition.
#define PINS_TWI  {0x01800000, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/// Base address of the MCI peripheral connected to the SD card.
#define BOARD_SD_MCI_BASE           AT91C_BASE_MCI
/// Peripheral identifier of the MCI connected to the SD card.
#define BOARD_SD_MCI_ID             AT91C_ID_MCI
/// MCI pins that shall be configured to access the SD card.
#define BOARD_SD_PINS               PINS_MCI1 // MCI0 is not used to avoid pin conflict with SPI0
/// MCI slot to which the SD card is connected to.
#define BOARD_SD_SLOT               MCI_SD_SLOTA // MCI_SD_SLOTB pins conflict with SPI0

/// ISI
#define BOARD_ISI_PIO_CTRL1 {1 << 4,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#define BOARD_ISI_PIO_CTRL2 {1 << 19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define BOARD_ISI_TWCK      {1 << 24, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define BOARD_ISI_TWD       {1 << 23, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
// PCK1 use instead of ISI_MCK
#define BOARD_ISI_MCK       {1 << 31, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define BOARD_ISI_VSYNC     {1 << 29, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
#define BOARD_ISI_HSYNC     {1 << 30, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
#define BOARD_ISI_PCK       {1 << 28, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
#define BOARD_ISI_PINS_DATA {(1<<20)|(1<<21)|(1<<22)|(1<<23)|(1<<24)|(1<<25)|(1<<26)|(1<<27)|\
                             (1<<10)|(1<<11)|(1<<12)|(1<<13),\
                             AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/*!
 * GPIO Pin definitions as assigned on the daughterboard.
 * @note Please use with caution: These pins can be multiplexed with SPI1 and ISI interfaces.
 */
#define PIN_GPIO00	{1 << 10, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO01	{1 << 11, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO02	{1 << 12, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO03	{1 << 13, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO04	{1 << 20, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO05	{1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO06	{1 << 22, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO07	{1 << 23, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO08	{1 << 24, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO09	{1 << 25, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO10	{1 << 26, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO11	{1 << 27, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO12	{1 << 28, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO13	{1 << 29, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO14	{1 << 30, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO15	{1 << 31, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO16	{1 << 12, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO17	{1 << 13, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO18	{1 << 14, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO19	{1 << 15, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO20	{1 << 16, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO21	{1 << 17, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}

/*!
 * GPIO Pin definitions of pins placed on the CSKB connector.
 */
#define PIN_GPIO22	{1 << 18, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO23	{1 << 19, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO24	{1 << 20, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO25	{1 << 21, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_GPIO26	{1 << 29, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "ISIS_OBC_G20 - Memories"
/// This page lists definitions related to external on-board memories.

/// Base address of the SDRAM
#define BOARD_SDRAM_BASE_ADDRESS	0x20000000

/// Board SDRAM size is defined in the project options through the BOARD_SDRAM_SIZE compiler define
//#define BOARD_SDRAM_SIZE        	0x02000000  // 32 MB

/// List of all SDRAM pins definitions.
#define PINS_SDRAM              	{0xFFFF0000, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}
/// SDRAM bus width.
#define BOARD_SDRAM_BUSWIDTH    	32


/// Base address of the NOR Flash. Also the address for transferring command bytes to the NOR Flash.
#define BOARD_NORFLASH_BASE_ADDRESS		0x10000000
/// Default NOR bus width after power up reset. Also the final/actual width.
#define BOARD_NORFLASH_BUSWIDTH 		16
/// Board NOR Flash size.
#define BOARD_NORFLASH_SIZE				0x00100000  // 1MB


//------------------------------------------------------------------------------

#endif //#ifndef BOARD_H

