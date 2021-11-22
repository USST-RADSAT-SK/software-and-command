/**
 * @file RSPI.c
 * @date November 12, 2021
 * @author Atharva P. Kulkarni (iya789)
 */

#include <stdint.h>
#include "RSPI.h"gu

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
uint32_t spiInit(SPIbus bus, SPIslave spi1_max_cs) {

    uint16_t err = SPI_start(bus, spi1_max_cs);

   if(err == 0) {
		return err;
	}
	else {
		return 1;
	}

}

uint32_t spiTransmitRecieve(SPItransfer *tx){

    uint16_t err = SPI_writeRead(tx);

    if(err == 0) {
		return err;
	}
	else {
		return 1;
	}

}