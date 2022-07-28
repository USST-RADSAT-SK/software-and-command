/**
 * @file RFram.h
 * @date May 18 2021
 * @author Tyrel Kostyk
 */

#ifndef RFRAM_H_
#define RFRAM_H_

#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** FRAM address of the write cursor. */
#define FRAM_WRITE_CURSOR_ADDR	(0x00)

/** FRAM address of the read cursor. */
#define FRAM_READ_CURSOR_ADDR	(0x01)

/** FRAM start address of the data. */
#define FRAM_DATA_START_ADDR	(0x02)


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int framInit(void);
int framRead(uint8_t* data, uint32_t address, uint32_t size);
int framWrite(uint8_t* data, uint32_t address, uint32_t size);


#endif /* RFRAM_H_ */
