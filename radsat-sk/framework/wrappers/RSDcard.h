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

#define _BUFF_SIZE ( 2 * 1024 )
#define _SAFE 1

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/


uint32_t SD_init(uint16_t volID);

uint32_t SD_read(uint8_t filename);

uint32_t SD_write(uint8_t filename);


#endif /* RSDCARD_H_ */
