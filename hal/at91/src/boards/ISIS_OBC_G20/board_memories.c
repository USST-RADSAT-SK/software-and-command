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

/*
    Title: Memories implementation
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "at91/boards/ISIS_OBC_G20/board.h"
#include "at91/peripherals/pio/pio.h"

/*
    Macros:
        READ - Reads a register value. Useful to add trace information to read
               accesses.
        WRITE - Writes data in a register. Useful to add trace information to
                write accesses.
*/
#define READ(peripheral, register)          (peripheral->register)
#define WRITE(peripheral, register, value)  (peripheral->register = value)

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Changes the mapping of the chip so that the remap area mirrors the
///   internal ROM or the EBI CS0.
//------------------------------------------------------------------------------
void BOARD_RemapRom()
{
    WRITE(AT91C_BASE_MATRIX, MATRIX_MRCR, 0);
}

//------------------------------------------------------------------------------
/// Changes the mapping of the chip so that the remap area mirrors the
///   internal RAM.
//------------------------------------------------------------------------------
void BOARD_RemapRam()
{
    WRITE(AT91C_BASE_MATRIX,
          MATRIX_MRCR,
          (AT91C_MATRIX_RCA926I | AT91C_MATRIX_RCA926D));
}

//------------------------------------------------------------------------------
/// Initialize and configure the SDRAM
//------------------------------------------------------------------------------
void BOARD_ConfigureSdram(unsigned char busWidth)
{
    volatile unsigned int i;
    static const Pin pinsSdram = PINS_SDRAM;
    volatile unsigned int *pSdram = (unsigned int *) AT91C_EBI_SDRAM;
    unsigned short sdrc_dbw = 0;
    int rowAddress;

// We only enable the extra the extra address line if expressly told to do so
#if BOARD_SDRAM_SIZE == 64
    rowAddress = AT91C_SDRAMC_NR_13;
#else
    rowAddress = AT91C_SDRAMC_NR_12;
#endif

    switch (busWidth) {
        case 16:
            sdrc_dbw = AT91C_SDRAMC_DBW_16_BITS;
            break;

        case 32:
        default:
            sdrc_dbw = AT91C_SDRAMC_DBW_32_BITS;
            break;

    }

    // Enable corresponding PIOs
    PIO_Configure(&pinsSdram, 1);
    
    // Enable EBI chip select for the SDRAM
    WRITE(AT91C_BASE_MATRIX, MATRIX_EBI, AT91C_MATRIX_CS1A_SDRAMC);
    

    // CFG Control Register
    // All these values are very important to be set according to the datasheet of the SDRAM.
    WRITE(AT91C_BASE_SDRAMC, SDRAMC_CR, AT91C_SDRAMC_NC_9
                                        | rowAddress
                                        | AT91C_SDRAMC_CAS_3
                                        | AT91C_SDRAMC_NB_4_BANKS
                                        | sdrc_dbw
                                        | AT91C_SDRAMC_TWR_3
                                        | AT91C_SDRAMC_TRC_9
                                        | AT91C_SDRAMC_TRP_3
                                        | AT91C_SDRAMC_TRCD_3
                                        | AT91C_SDRAMC_TRAS_7
                                        | AT91C_SDRAMC_TXSR_15);

    // Delays to ensure the parameters are configured correctly
    for (i = 0; i < 1000; i++);

    // The SDRAM controller requires the SDRAM to be accessed to actually send the command set in the MR.
    // A different value is written to a different part of the SDRAM.
    // This is just a way to make sure that a memory access does occur.
    // The actual array indices and their assigned values don't matter as long as they are unique.
    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_NOP_CMD);    // Perform NOP
    pSdram[0] = 0x00000000;

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_PRCGALL_CMD);    // Set PRCHG AL
    pSdram[0] = 0x00000000;                        // Perform PRCHG

    // Delays to ensure the parameters are configured correctly
    for (i = 0; i < 10000; i++);

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 1st CBR
    pSdram[1] = 0x00000001;                        // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 2 CBR
    pSdram[2] = 0x00000002;                        // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 3 CBR
    pSdram[3] = 0x00000003;                       // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 4 CBR
    pSdram[4] = 0x00000004;                      // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 5 CBR
    pSdram[5] = 0x00000005;                      // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 6 CBR
    pSdram[6] = 0x00000006;                    // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 7 CBR
    pSdram[7] = 0x00000007;                    // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_RFSH_CMD);    // Set 8 CBR
    pSdram[8] = 0x00000008;                    // Perform CBR

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_LMR_CMD);        // Set LMR operation
    pSdram[9] = 0xcafedede;                    // Perform LMR burst=1, lat=2

    // Refresh period of a typical SDRAM is 64ms. This needs to be divided by the number of rows
    // Current SDRAM has 8192 rows in 16-bit bus width, and 4096 rows in 32-bit bus width(ours).
    // The datasheet recommends a refresh every 7.8uS. This value corresponds to 8192.
    // Therefore, the refresh mechanism still corresponds to the smaller rows.
    WRITE(AT91C_BASE_SDRAMC, SDRAMC_TR, (BOARD_MCK * 7) / 1000000);        // Set Refresh Timer

    WRITE(AT91C_BASE_SDRAMC, SDRAMC_MR, AT91C_SDRAMC_MODE_NORMAL_CMD);    // Set Normal mode
    pSdram[0] = 0x00000000;                        // Perform Normal mode
}

//------------------------------------------------------------------------------
/// Configures the EBI for NorFlash access
/// \Param busWidth Bus width 
//------------------------------------------------------------------------------
void BOARD_ConfigureNorFlash(unsigned char busWidth)
{
    // Configure SMC

    AT91C_BASE_SMC->SMC_SETUP0 = 0x00000002;
    AT91C_BASE_SMC->SMC_PULSE0 = 0x0A0A0A06;
    AT91C_BASE_SMC->SMC_CYCLE0 = 0x000A000A;
    AT91C_BASE_SMC->SMC_CTRL0  = (AT91C_SMC_READMODE
                                  | AT91C_SMC_WRITEMODE
                                  | AT91C_SMC_NWAITM_NWAIT_DISABLE
                                  | ((0x1 << 16) & AT91C_SMC_TDF));

    if (busWidth == 8) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_EIGTH_BITS;
    }
    else if (busWidth == 16) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
    }
    else if (busWidth == 32) {
        AT91C_BASE_SMC->SMC_CTRL0 |= AT91C_SMC_DBW_WIDTH_THIRTY_TWO_BITS;
    }
}

