/**
 * @file RSPI.c
 * @date November 12, 2021
 * @author Atharva P. Kulkarni (iya789)
 */

#include <stdint.h>
#include "RSPI.h"

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initiates SPI communications
 * @param bus: The SPI bus we are using on the OBC
 * @param spi1_max_cs: The maximum number of chip selects on the OBC
 * @return An integer error code. 0 indicates success, 1 for failure
 */

uint32_t spiInit(SPIbus bus, SPIslave spi1_max_cs) {

    uint16_t err = SPI_start(bus, spi1_max_cs);

   if(err == 0) {
		return err;
	}
	else {
		return 1;
	}

}

/**
 * Sends and Recieves data over SPI
 * @param tx: SPItransfer structure specifying the details of the tasks
 * @return An integer error code. 0 indicates success, 1 for failure
 */

uint32_t spiTransmitRecieve(SPItransfer *tx){

    uint16_t err = SPI_writeRead(tx);

    if(err == 0) {
		return err;
	}
	else {
		return 1;
	}

}
