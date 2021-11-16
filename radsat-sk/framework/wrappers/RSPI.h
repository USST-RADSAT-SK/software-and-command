/**
 * @file RSPI.c
 * @date November 12, 2021
 * @author Atharva P. Kulkarni (iya789)
 */

#ifndef SPI_H_
#define SPI_H_

#include <hal/Drivers/SPI.h>
#include <freertos/FreeRTOS.h>

#define SPI_MAX_CS 8
#define BUS 0

/***************************************************************************************************
                                             FUNCTION DECLARATIONS
***************************************************************************************************/

uint32_t spiInit();
uint32_t spiTransmitRecieve();

#endif /* SPI_H_ */