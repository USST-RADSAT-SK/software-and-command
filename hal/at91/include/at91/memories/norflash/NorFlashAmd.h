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
/// !!!Purpose
/// 
/// The AMD %norflash Low-level driver code implement procedures to program 
/// basic operations described AMD-specified command set flash devices. 
/// The various commands recognized by the devices are listed in the Commands 
/// Tables provided in the corresponding AMD command set compatible flash 
/// datasheets. All operation functions are blocked, they wait for the 
/// completion of an operation by polling the status register.
///
/// !!!Usage
/// -# Flash program using AMD_Write_Data().
///    - The Program command is used to modify the data stored at the 
///      specified device address. Programming can only change bits 
///      from ��1�� to ��0��. It may be necessary to erase the block before 
///      programming to addresses within it. Programming modifies a single 
///      Word at a time using static function amd_Program(). Programming 
///      larger amounts of data must be done in one Word at a time by 
///      giving a Program command, waiting for the command to complete, 
///      giving the next Program command and so on.
/// -# erase a block within the flash using AMD_EraseSector().
///    - Flash erase is performed on a block basis. An entire block is 
///      erased each time an erase command sequence is given. 
/// -# erase whole blocks within the flash using AMD_EraseChip().
/// -# AMD_Reset() function can be issued, between Bus Write cycles 
///    before the start of a program or erase operation, to return the 
///    device to read mode.
/// -# AMD_ReadDeviceID() is used to retrieve information 
///    about the Flash Device type.
/// -# AMD_ReadManufactoryId() is used to retrieve information 
///    about the Flash Device Manufactory ID.
//------------------------------------------------------------------------------

#ifndef NORFLASHAMD_H
#define NORFLASHAMD_H

#include "at91/memories/norflash/NorFlashCFI.h"
#include "at91/memories/norflash/NorFlashApi.h"

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

void AMD_Reset(struct NorFlashInfo *pNorFlashInfo, unsigned int address);

unsigned int AMD_ReadManufactoryId(struct NorFlashInfo *pNorFlashInfo);

unsigned int AMD_ReadDeviceID(struct NorFlashInfo *pNorFlashInfo);

unsigned char AMD_EraseSector(
    struct NorFlashInfo *pNorFlashInfo, 
    unsigned int sectorAddr);
    
unsigned char AMD_EraseChip(struct NorFlashInfo *pNorFlashInfo);

unsigned char AMD_Write_Data(
    struct NorFlashInfo *pNorFlashInfo,
    unsigned int address,
    unsigned char *buffer,
    unsigned int size);

unsigned char AMD_Program(
    struct NorFlashInfo *pNorFlashInfo,
    unsigned int address,
    unsigned int data);

const struct NorFlashOperations amdOperations;

#endif //#ifndef NORFLASHAMD_H
