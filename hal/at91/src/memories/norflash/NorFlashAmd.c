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
#include "at91/memories/norflash/NorFlashAmd.h"

#include "at91/memories/norflash/NorFlashCFI.h"
#include "at91/memories/norflash/NorFlashCommon.h"
#include "at91/memories/norflash/NorFlashApi.h"
#include "at91/utility/trace.h"

#include <string.h>

//------------------------------------------------------------------------------
//        Local defination
//------------------------------------------------------------------------------

/// Command for vendor command set CMD_SET_AMD.
#define AMD_CMD_IDOUT         0x00F0
#define AMD_CMD_CFI           0x0098
#define AMD_CMD_IDIN          0x0090
#define AMD_CMD_UNLOCK_1      0x00AA
#define AMD_CMD_UNLOCK_2      0x0055
#define AMD_CMD_ERASE_SETUP   0x0080
#define AMD_CMD_ERASE_RESUME  0x0030
#define AMD_CMD_ERASE_CHIP    0x0010
#define AMD_CMD_ERASE_SECTOR  0x0030
#define AMD_CMD_PROGRAM       0x00A0
#define AMD_CMD_UNLOCK_BYPASS 0x0020

// Command offset for vendor command set CMD_SET_AMD
#define AMD_OFFSET_UNLOCK_1   0x05555
#define AMD_OFFSET_UNLOCK_2   0x0AAAA
/// Query command address.
#define FLASH_ADDRESS_CFI     0x0055

/// AMD norflash device Identifier infomation address offset.
#define AMD_MANU_ID           0x00
#define AMD_DEVIDE_ID         0x01

// Data polling mask for vendor command set CMD_SET_AMD
#define AMD_POLLING_DQ7       0x80
#define AMD_POLLING_DQ6       0x60
#define AMD_POLLING_DQ5       0x20
#define AMD_POLLING_DQ3       0x08

const struct NorFlashOperations amdOperations = {
   AMD_Reset,
   AMD_Write_Data,
   AMD_ReadManufactoryId,
   AMD_ReadDeviceID,
   AMD_EraseChip,
   AMD_EraseSector
};

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// It implements a RESET command.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
/// \param address  Dummy data for AMD.
//------------------------------------------------------------------------------
void amd_Reset(struct NorFlashInfo *pNorFlashInfo, unsigned int address)
{
    unsigned char busWidth;
    (void)address;

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_UNLOCK_1);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_2),
                 AMD_CMD_UNLOCK_2);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_IDOUT);
}


//------------------------------------------------------------------------------
/// Read specified manufactory id or device id.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
/// \param index 0: manufactorid 1: device id.
//------------------------------------------------------------------------------
unsigned int amd_ReadIdentification(
    struct NorFlashInfo *pNorFlashInfo,
    unsigned char index)
{
    unsigned int id;
    unsigned char busWidth;
    unsigned int address;

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);

    // The amd_Read identification command sequence is initiated by first
    // writing two unlock cycles.
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_UNLOCK_1);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_2),
                 AMD_CMD_UNLOCK_2);

    // Followed by a third write cycle that contains the autoselect command.
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_IDIN);

    // The device then enters the autoselect mode. It may read at any address any
    // number of times without initiating another autoselect command sequence.
    address = NorFlash_GetByteAddressInChip(pNorFlashInfo, index);
    ReadRawData(busWidth, address, (unsigned char*)&id);

    // The system must write the exit command to return to the read mode
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_UNLOCK_1);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_2),
                 AMD_CMD_UNLOCK_2);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_IDOUT);
    return id;
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// It implement a program word command. Returns 0 if the operation was
/// successful; otherwise returns an error code.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
/// \param address Start address offset to be wrote.
/// \param data word to be written.
//------------------------------------------------------------------------------
unsigned char AMD_Program(
    struct NorFlashInfo *pNorFlashInfo,
    unsigned int address,
    unsigned int data)
{
    unsigned int pollingData;
    unsigned int busAddress;
    unsigned char done = 0;
    unsigned char busWidth;

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);
    // The program command sequence is initiated by writing two unlock write cycles.
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_UNLOCK_1);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_2),
                 AMD_CMD_UNLOCK_2);
    // Followed by the program set-up command.
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_PROGRAM);

    // The program address and data are written next,
    // which in turn initiate the Embedded Program algorithm.
    busAddress = NorFlash_GetAddressInChip(pNorFlashInfo, address);
    WriteRawData(busWidth, busAddress, (unsigned char*)&data);

    // Data polling
    do {
        ReadRawData(busWidth, busAddress, (unsigned char *)&pollingData);
        // Check if the chip program algorithm is completed.
        if ((pollingData & AMD_POLLING_DQ7) == (data & AMD_POLLING_DQ7)) {
            // Program operation successful. Device in read mode.
            done = 1;
        }
        else {
            // check if chip Program algorithm exceeded timing limits

            if (pollingData & AMD_POLLING_DQ5 ) {

                // I/O should be rechecked.
                ReadRawData(busWidth, busAddress, (unsigned char *)&pollingData);

                if ((pollingData & AMD_POLLING_DQ7) == (data & AMD_POLLING_DQ7)) {
                    // Program operation successful. Device in read mode.
                    done = 1;
                }
                else {
                    // Program operation not successful, write reset command.
                    amd_Reset(pNorFlashInfo, 0);
                    return NorCommon_ERROR_CANNOTWRITE;
                }
            }
        }
    } while (!done);
    return 0;
}

//------------------------------------------------------------------------------
/// It implements a RESET command.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
/// \param address  Dummy data for AMD.
//------------------------------------------------------------------------------
void AMD_Reset(struct NorFlashInfo *pNorFlashInfo, unsigned int address)
{
    amd_Reset(pNorFlashInfo, address);
}


//------------------------------------------------------------------------------
/// The Read Device Identifier command instructs the device to output manufacturer
/// code.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
//------------------------------------------------------------------------------
unsigned int AMD_ReadManufactoryId(struct NorFlashInfo *pNorFlashInfo)
{
    return amd_ReadIdentification(pNorFlashInfo, AMD_MANU_ID);
}

//------------------------------------------------------------------------------
/// The Read Device Identifier command instructs the device to output device id.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
//------------------------------------------------------------------------------
unsigned int AMD_ReadDeviceID(struct NorFlashInfo *pNorFlashInfo)
{
    return amd_ReadIdentification(pNorFlashInfo, AMD_DEVIDE_ID);
}

//------------------------------------------------------------------------------
/// Erases the specified block of the device. Returns 0 if the operation was
/// successful; otherwise returns an error code.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
/// \param address Address offset to be erase.
//------------------------------------------------------------------------------
unsigned char AMD_EraseSector(
    struct NorFlashInfo *pNorFlashInfo,
    unsigned int address)
{
    unsigned int pollingData;
    unsigned int busAddress;
    unsigned char busWidth;
    unsigned char done = 0;

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);

    //Programming is a six-bus-cycle operation.
    // The erase command sequence is initiated by writing two unlock write cycles.
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_UNLOCK_1);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_2),
                 AMD_CMD_UNLOCK_2);
    // Followed by the program set-up command.
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_ERASE_SETUP);
    // Two additional unlock cycles are written.
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_UNLOCK_1);
    WriteCommand(busWidth,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_2),
                 AMD_CMD_UNLOCK_2);

    // Followed by the address of the sector to be erased, and the sector erase command.
    busAddress = NorFlash_GetAddressInChip(pNorFlashInfo,address);
    WriteCommand(busWidth, busAddress, AMD_CMD_ERASE_SECTOR);

    // Data polling
    do {
        ReadRawData(busWidth, busAddress, (unsigned char *)&pollingData);
        // Check if the chip erase algorithm is completed.
        if ((pollingData & AMD_POLLING_DQ7) == AMD_POLLING_DQ7 ) {
            // Erase operation successful. Device in read mode.
            done = 1;
        }
        else {
            // check if sector erase algorithm exceeded timing limits
            if (pollingData & AMD_POLLING_DQ5 ) {

                // I/O should be rechecked.
                ReadRawData(busWidth, busAddress, (unsigned char *)&pollingData);
                if ((pollingData & AMD_POLLING_DQ7) == AMD_POLLING_DQ7 ){
                    // Erase operation successful. Device in read mode.
                    done = 1;
                }
                else {
                    // Erase operation not successful, write reset command.
                    amd_Reset(pNorFlashInfo, 0);
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
    return 0;
}

//------------------------------------------------------------------------------
/// Erases all the block of the device. Returns 0 if the operation was successful;
/// otherwise returns an error code.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
//------------------------------------------------------------------------------
unsigned char AMD_EraseChip(struct NorFlashInfo *pNorFlashInfo)
{
    unsigned int pollingData;
    unsigned char busWidth;
    unsigned int address;
    unsigned char done = 0;

    busWidth = NorFlash_GetDataBusWidth(pNorFlashInfo);

    //Programming is a six-bus-cycle operation.
    // The erase command sequence is initiated by writing two unlock write cycles.
    WriteCommand(busWidth ,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_UNLOCK_1);
    WriteCommand(busWidth ,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_2),
                 AMD_CMD_UNLOCK_2);
    // Followed by the program set-up command.
    WriteCommand(busWidth ,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_ERASE_SETUP);

    // Two additional unlock cycles are written.
    WriteCommand(busWidth ,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_UNLOCK_1);
    WriteCommand(busWidth ,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_2),
                 AMD_CMD_UNLOCK_2);

    // Then followed by the chip erase command.
    WriteCommand(busWidth ,
                 NorFlash_GetByteAddressInChip(pNorFlashInfo, AMD_OFFSET_UNLOCK_1),
                 AMD_CMD_ERASE_CHIP);

    address = NorFlash_GetByteAddressInChip(pNorFlashInfo, 0);
    // Data polling
    do {
        ReadRawData(busWidth , address, (unsigned char*)&pollingData);
        // Check if the chip erase algorithm is completed.
        if ((pollingData & AMD_POLLING_DQ7) == AMD_POLLING_DQ7 ) {
            // Erase operation successful. Device in read mode.
            done = 1;
        }
        else {

            // When the time-out period is complete, DQ3 switches from a ��0�� to a ��1.��
            if (pollingData & AMD_POLLING_DQ3 ) {
                return NorCommon_ERROR_CANNOTERASE;
            }
            // check if chip earse algrithm exceeded timing limits
            if (pollingData & AMD_POLLING_DQ5 ) {

                // I/O should be rechecked.
                ReadRawData(busWidth , address, (unsigned char*)&pollingData);
                if ((pollingData & AMD_POLLING_DQ7) == AMD_POLLING_DQ7 ){
                    // Erase operation successful. Device in read mode.
                    done = 1;
                }
                else {
                    // Erase operation not successful, write reset command.
                    amd_Reset(pNorFlashInfo, 0);
                    return NorCommon_ERROR_CANNOTERASE;
                }
            }
        }

        // Akhil: This will make sure other things can happen while we wait for the nor flash to finish the erase operation.
//        if(done != 1) {
//        	vTaskDelay(1);
//        }

    } while (!done);
    return 0;
}


//------------------------------------------------------------------------------
/// Sends data to the NorFlashInfo chip from the provided buffer.
/// \param pNorFlashInfo  Pointer to an NorFlashInfo instance.
/// \param address Start address offset to be wrote.
/// \param buffer Buffer where the data is stored.
/// \param size Number of bytes that will be written.
//------------------------------------------------------------------------------
//Akhil: Fixed data alignment issues.
//Akhil: Fixed an issue where this function wrote fewer bytes than asked when the size is not a multiple of bus-width.
//Akhil: Fixed an issue where wrong data is written if the address is not perfectly divisible by bus-width(in bytes).
unsigned char AMD_Write_Data(
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
            if(AMD_Program(pNorFlashInfo, address, val16b)) {
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

            if(AMD_Program(pNorFlashInfo, address, val16b)) {
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

        	if(AMD_Program(pNorFlashInfo, address, val32b)) {
                return NorCommon_ERROR_CANNOTWRITE;
            }
            address+= 4;
        }
    }
    return 0;
}
