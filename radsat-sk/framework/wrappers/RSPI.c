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
uint32_t spiInit() {

    uint16_t err = SPI_start();

   if(err == 0) {
		return err;
	}
	else {
		return 1;
	}

}

uint32_t spiTransmitRecieve(){

    uint16_t err = SPI_writeRead();

    if(err == 0) {
		return err;
	}
	else {
		return 1;
	}

    return 0;

}