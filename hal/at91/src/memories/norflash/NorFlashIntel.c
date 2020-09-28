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
#include "at91/memories/norflash/NorFlashIntel.h"

#include "at91/memories/norflash/NorFlashCFI.h"
#include "at91/memories/norflash/NorFlashCommon.h"
#include "at91/memories/norflash/NorFlashAmd.h"

#include "at91/utility/trace.h"

#include <string.h>

//------------------------------------------------------------------------------
//        Local defination
//------------------------------------------------------------------------------

/// Command for vendor command set CMD_SET_INTEL. Device commands are written
/// to the Command User Interface (CUI) to control all flash memory device operations.
#define INTEL_CMD_IDIN             0x0090
#define INTEL_CMD_BLOCK_ERASE_1    0x0020
#define INTEL_CMD_BLOCK_ERASE_2    0x00D0
#define INTEL_CMD_READ_STATUS      0x0070
#define INTEL_CMD_CLEAR_STATUS     0x0050
#define INTEL_CMD_BLOCK_LOCKSTART  0x0060
#define INTEL_CMD_BLOCK_LOCK       0x0001
#define INTEL_CMD_BLOCK_UNLOCK     0x00D0
#define INTEL_CMD_BLOCK_LOCKDOWN   0x002F
#define INTEL_CMD_PROGRAM_WORD     0x0010
#define INTEL_CMD_RESET            0x00FF


/// Intel norflash status resgister
#define INTEL_STATUS_DWS    0x80
#define INTEL_STATUS_ESS    0x40
#define INTEL_STATUS_ES     0x20
#define INTEL_STATUS_PS     0x10
#define INTEL_STATUS_VPPS   0x08
#define INTEL_STATUS_PSS    0x04
#define INTEL_STATUS_BLS    0x02
#define INTEL_STATUS_BWS    0x01

/// Intel norflash device Identifier infomation address offset.
#define INTEL_MANU_ID       0x00
#define INTEL_DEVIDE_ID     0x01
#define INTEL_LOCKSTATUS    0x02

/// Intel norflash device lock status.
#define INTEL_LOCKSTATUS_LOCKED         0x01
#define INTEL_LOCKSTATUS_LOCKDOWNED     0x02

const struct NorFlashOperations intelOperations = {
   INTEL_Reset,
   INTEL_Write_Data,
   INTEL_ReadManufactoryId,
   INTEL_ReadDeviceID,
   INTEL_EraseChip,
   INTEL_EraseSector

};

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// It implements a RESET command.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
//------------------------------------------------------------------------------
void intel_Reset(struct NorFlashInfo *pNorFlashInfo, unsigned int address)
{
    unsigned int busAddress;
    unsigned int busWidth;
    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);
    busAddress = NorFlash_GetAddressInChip(pNorFlashInfo, address);
    WriteCommand(busWidth, busAddress, INTEL_CMD_RESET);
}


//------------------------------------------------------------------------------
/// The Read Device Identifier command instructs the device to output manufacturer
/// code, device identifier code, block-lock status, protection register data,
/// or configuration register data by giving offset.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
/// \param offset 0: Identifier address offset.
//------------------------------------------------------------------------------
unsigned int intel_ReadIdentification(
    struct NorFlashInfo *pNorFlashInfo,
    unsigned int offset)
{
    unsigned int data;
    unsigned char busWidth;
    unsigned int address;

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);
    // Issue the Read Device Identifier command at specified address.
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, 0),
                 INTEL_CMD_IDIN);

    if(offset >= INTEL_LOCKSTATUS) {
        // Block base address.
        address = NorFlash_GetAddressInChip (pNorFlashInfo, offset);
    }
    else {
        address = NorFlash_GetByteAddressInChip (pNorFlashInfo, offset);
    }
    ReadRawData(busWidth, address, (unsigned char*)&data);
    return data;
}


//------------------------------------------------------------------------------
/// Return the status register value.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
//------------------------------------------------------------------------------
unsigned char intel_ReadStatus(struct NorFlashInfo *pNorFlashInfo, unsigned int address)
{
    unsigned int status;
    unsigned char busWidth;
    unsigned int budAddress;
    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);

    // Issue the Read Status Register command at any address.
    budAddress = NorFlash_GetAddressInChip(pNorFlashInfo, address),
    WriteCommand(busWidth, budAddress, INTEL_CMD_READ_STATUS);
    ReadRawData(busWidth, budAddress, (unsigned char*)&status);
    return status;
}

//------------------------------------------------------------------------------
/// Clear the status register.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
//------------------------------------------------------------------------------
void intel_ClearStatus(struct NorFlashInfo *pNorFlashInfo)
{
    unsigned char busWidth;
    unsigned int address;
    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);

    // Issue the Clear Status Register command at any address
    address = NorFlash_GetAddressInChip(pNorFlashInfo, 0),
    WriteCommand(busWidth, address, INTEL_CMD_CLEAR_STATUS);
}

//------------------------------------------------------------------------------
/// Unlocks the specified block of the device.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
/// \param address Address in sector.
//------------------------------------------------------------------------------
void intel_UnlockSector(struct NorFlashInfo *pNorFlashInfo, unsigned int address)
{
    unsigned int busAddress;
    unsigned char busWidth;
    // Clear the status register first.

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);
    busAddress = NorFlash_GetAddressInChip(pNorFlashInfo,address);

    WriteCommand(busWidth, busAddress, INTEL_CMD_BLOCK_LOCKSTART);
    WriteCommand(busWidth, busAddress, INTEL_CMD_BLOCK_UNLOCK);
    intel_Reset(pNorFlashInfo, 0);
}

//------------------------------------------------------------------------------
/// The Read Device Identifier command instructs the device to output block-lock
/// status.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
/// \param address 0: Address in sector/block.
//------------------------------------------------------------------------------
unsigned int intel_GetBlockLockStatus(struct NorFlashInfo *pNorFlashInfo, unsigned int address)
{
    return intel_ReadIdentification(pNorFlashInfo, (address + NorFlash_GetByteAddress(pNorFlashInfo ,INTEL_LOCKSTATUS)));
}

//------------------------------------------------------------------------------
/// It implement a program word command. Returns 0 if the operation was
/// successful; otherwise returns an error code.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
/// \param address Start address offset to be wrote.
/// \param data word to be written.
//------------------------------------------------------------------------------
unsigned char intel_Program(
    struct NorFlashInfo *pNorFlashInfo,
    unsigned int address,
    unsigned int data
    )
{
    unsigned int status;
    unsigned int datain;
    volatile unsigned int busAddress;
    unsigned char done = 0;
    unsigned char busWidth;

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);

    intel_Reset(pNorFlashInfo, address);
    busAddress = NorFlash_GetAddressInChip(pNorFlashInfo, address);

    // Check if the data already have been erased.
    ReadRawData(busWidth, busAddress, (unsigned char*)&datain);
    if((datain & data)!= data) {
        return NorCommon_ERROR_CANNOTWRITE;
    }

    // Word programming operations are initiated by writing the Word Program Setup command to the device.
    WriteCommand(busWidth, busAddress, INTEL_CMD_PROGRAM_WORD);
    // This is followed by a second write to the device with the address and data to be programmed.
    WriteRawData(busWidth, busAddress, (unsigned char*)&data);

    // Status register polling
    do {
        status = intel_ReadStatus(pNorFlashInfo,address);
        // Check if the device is ready.
        if ((status & INTEL_STATUS_DWS) == INTEL_STATUS_DWS ) {
            // check if VPP within acceptable limits during program or erase operation.
            if ((status & INTEL_STATUS_VPPS) == INTEL_STATUS_VPPS ) {
                return NorCommon_ERROR_CANNOTWRITE;
            }
            // Check if the erase block operation is completed.
            if ((status & INTEL_STATUS_PS) == INTEL_STATUS_PS ) {
                return NorCommon_ERROR_CANNOTWRITE;
            }
            // check if Block locked during program or erase, operation aborted.
                else if ((status & INTEL_STATUS_BLS) == INTEL_STATUS_BLS ) {
                    return NorCommon_ERROR_CANNOTWRITE;
            }
            else {
                done = 1;
            }
        }
    } while (!done);

    intel_ClearStatus(pNorFlashInfo);
    intel_Reset(pNorFlashInfo, address);
    return 0;
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// It implements a RESET command.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
//------------------------------------------------------------------------------
void INTEL_Reset(struct NorFlashInfo *pNorFlashInfo, unsigned int address)
{
    intel_Reset(pNorFlashInfo, address);
}

//------------------------------------------------------------------------------
/// The Read Device Identifier command instructs the device to output manufacturer
/// code.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
//------------------------------------------------------------------------------
unsigned int INTEL_ReadManufactoryId(struct NorFlashInfo *pNorFlashInfo)
{
    return intel_ReadIdentification(pNorFlashInfo, INTEL_MANU_ID);
}

//------------------------------------------------------------------------------
/// The Read Device Identifier command instructs the device to output device id.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
//------------------------------------------------------------------------------
unsigned int INTEL_ReadDeviceID(struct NorFlashInfo *pNorFlashInfo)
{
    return intel_ReadIdentification(pNorFlashInfo, INTEL_DEVIDE_ID);
}

//------------------------------------------------------------------------------
/// Erases the specified block of the device. Returns 0 if the operation was
/// successful; otherwise returns an error code.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
/// \param address Address offset to be erase.
//------------------------------------------------------------------------------
unsigned char INTEL_EraseSector(
    struct NorFlashInfo *pNorFlashInfo,
    unsigned int address)
{
    unsigned int status;
    unsigned int busAddress;
    unsigned char busWidth;
    unsigned char done = 0;

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);
    // Check the lock status is locked.
    status = intel_GetBlockLockStatus(pNorFlashInfo, address);
    if(( status & INTEL_LOCKSTATUS_LOCKED ) == INTEL_LOCKSTATUS_LOCKED){
        intel_UnlockSector(pNorFlashInfo, address);
    }
    // Clear the status register first.
    intel_ClearStatus(pNorFlashInfo);
    busAddress = NorFlash_GetAddressInChip(pNorFlashInfo,address);
    // Block erase operations are initiated by writing the Block Erase Setup command to the address of the block to be erased.
    WriteCommand(busWidth, busAddress, INTEL_CMD_BLOCK_ERASE_1);
    // Next, the Block Erase Confirm command is written to the address of the block to be erased.
    WriteCommand(busWidth, busAddress, INTEL_CMD_BLOCK_ERASE_2);

    // Status register polling
    do {
        status = intel_ReadStatus(pNorFlashInfo,address);
        // Check if the device is ready.
        if ((status & INTEL_STATUS_DWS) == INTEL_STATUS_DWS ) {
            // Check if the erase block operation is completed.
            if ((status & INTEL_STATUS_ES) == 0 ) {
                done = 1;
            }
            else {
                // check if VPP within acceptable limits during program or erase operation.
                if ((status & INTEL_STATUS_VPPS) == INTEL_STATUS_VPPS ) {
                    return NorCommon_ERROR_CANNOTERASE;
                }
                // check if Block locked during program or erase, operation aborted.
                else if ((status & INTEL_STATUS_BLS) == INTEL_STATUS_BLS ) {
                    return NorCommon_ERROR_CANNOTERASE;
                }
            }
        }

        // Akhil: This will make sure other things can happen while we wait for the nor flash to finish the erase operation.
        if(done != 1)
        {
//        	vTaskDelay(1);

        	/*
        	 * To remove the FreeRTOS dependency, we could add a forward
        	 * declared hook here. The purpose of this hook will be documented
        	 * and expected to be implemented in a upper layer. It could be as
        	 * simple as an empty call:
        	 * void NorFlash_Hook(void)
        	 * {
        	 * }
        	 * If an OS exist,the OS specific delay function could be called. In
        	 * the case of FreeRTOS the hook could be implemented as forth:
        	 * void NorFlash_Hook(void)
        	 * {
        	 *     vTaskDelay(1);
        	 * }
        	 */

        	extern void NorFlash_Hook(void);

        	NorFlash_Hook();
        }

    } while (!done);
    intel_Reset(pNorFlashInfo, 0);
    return 0;
}

//------------------------------------------------------------------------------
/// Erases all the block of the device. Returns 0 if the operation was successful;
/// otherwise returns an error code.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
//------------------------------------------------------------------------------

unsigned char INTEL_EraseChip(struct NorFlashInfo *pNorFlashInfo)
{
    // Interl flash have no independent Chip-erase command.
    unsigned int i;
    unsigned int sectors;
    sectors = NorFlash_GetDeviceNumOfBlocks(pNorFlashInfo);
    for (i = 0; i < sectors; i++) {
        if (INTEL_EraseSector(pNorFlashInfo, NorFlash_GetDeviceSectorAddress(pNorFlashInfo, i))) {
            return NorCommon_ERROR_CANNOTERASE;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/// Sends data to the struct NorFlashInfo chip from the provided buffer.
/// \param pNorFlashInfo  Pointer to an struct NorFlashInfo instance.
/// \param address Start address offset to be wrote.
/// \param buffer Buffer where the data is stored.
/// \param size Number of bytes that will be written.
//------------------------------------------------------------------------------
//Akhil: Fixed data alignment issues.
//Akhil: Fixed an issue where this function wrote fewer bytes than asked when the size is not a multiple of bus-width.
//Akhil: Fixed an issue where wrong data is written if the address is not perfectly divisible by bus-width(in bytes).
unsigned char INTEL_Write_Data(
	struct NorFlashInfo *pNorFlashInfo,
	unsigned int address,
	unsigned char *buffer,
	unsigned int size)
{
	unsigned int i, sizeReal;
	unsigned char busWidth;
	busWidth = pNorFlashInfo->deviceChipWidth;

	if(busWidth == FLASH_CHIP_WIDTH_8BITS) {
		for(i=0; i < size; i++) {
			if(AMD_Program(pNorFlashInfo, address, buffer[i])) {
				return NorCommon_ERROR_CANNOTWRITE;
			}
			address++;
		}
	}
	else if(busWidth == FLASH_CHIP_WIDTH_16BITS) {
		unsigned short val16b;

		// Akhil: If address is not an even number (not bus-width aligned), write the first byte separately.
		// Akhil: The first word(unsigned short) is created as a combination of data already existing on the flash and the first-byte to be written.
		if((address%2 != 0) && size!=0) {
			val16b = *(unsigned short *)(pNorFlashInfo->baseAddress + (address-1));
			val16b = (unsigned short)(buffer[0]<<8) + (val16b & 0x00FF);
			if(intel_Program(pNorFlashInfo, address, val16b)) {
				return NorCommon_ERROR_CANNOTWRITE;
			}

			address++;
			buffer++;
			size--;
		}

		// Akhil: Round-up instead of round-down when size is an odd number.
		sizeReal = (size + 1) >> 1;

		for(i=0; i<sizeReal; i++) {
			//Akhil: Create a 16-bit variable from the buffer instead of casting the buffer into an unsigned short pointer.
			val16b = (((unsigned short)buffer[i*2 + 1])<<8) + (unsigned short)buffer[i*2];

           	// Akhil: If the last piece of data to be written is a single byte,
        	// Akhil: then the last word is written as a combination of the byte to be written and the next-byte already existing on the flash.
			if((i==sizeReal-1) && (sizeReal != size/2)) {
        		val16b = *(unsigned short *)(pNorFlashInfo->baseAddress + address);
        		val16b = (unsigned short)buffer[i*2] +  (val16b & 0xFF00);
			}

			if(intel_Program(pNorFlashInfo, address, val16b)) {
				return NorCommon_ERROR_CANNOTWRITE;
			}
			address+= 2;
		}
	}
	else if(busWidth == FLASH_CHIP_WIDTH_32BITS) {
		unsigned int val32b;

		// Akhil: Not fixing address or size bugs from Atmel for 32-bit NOR-flash address bus because we only use 16-bit.
		// Akhil: Added warning for future cases.
		if(address%4!=0 || size%4!=0) {
			TRACE_WARNING("NORflash write address or size not bus-aligned!");
			return NorCommon_ERROR_CANNOTWRITE;
		}

		// Akhil: Round-up instead of round-down when size is not a multiple of 4.
		sizeReal = (size + 3) >> 2;

		for(i=0; i<sizeReal; i++) {
			//Akhil: Create a 32-bit variable from the buffer instead of casting the buffer into an unsigned int pointer.
			val32b = (((unsigned int)buffer[i*4 + 3])<<24) + (((unsigned int)buffer[i*4 + 2])<<16) + (((unsigned int)buffer[i*4 + 1])<<8) + (unsigned int)buffer[i*4];

			if(intel_Program(pNorFlashInfo, address, val32b)) {
				return NorCommon_ERROR_CANNOTWRITE;
			}
			address+= 4;
		}
	}
	return 0;
}
