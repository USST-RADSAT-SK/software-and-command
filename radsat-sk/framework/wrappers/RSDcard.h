/**
 * @file RUart.h
 * @date May 22, 2021
 * @author Addi Amaya (caa746) & Julian Pham (ngp143)
 */

#ifndef RSDCARD_H_
#define RSDCARD_H_

#include <hcc/api_fat.h>
#include <hcc/api_hcc_mem.h>
#include <hcc/api_fat_test.h>
#include <hcc/api_mdriver_atmel_mcipdc.h>

#include <hal/Utility/fprintf.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <at91/utility/assert.h>
#include <stdint.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
#define _BUFF_SIZE ( 2 * 1024 )
#define _SAFE 1
#define ENABLE_SD_TRACES 1


uint32_t SDinit(uint16_t volID);

uint32_t SDstop(uint16_t volID);

uint32_t SDwrite(uint8_t* filename, uint16_t volID, uint8_t* pWriteBuff, uint32_t buff_size, uint16_t numItems);

uint32_t SDread(uint8_t* filename, uint16_t volID, uint8_t* pReadBuff, uint32_t numItems);



#endif /* RSDCARD_H_ */
