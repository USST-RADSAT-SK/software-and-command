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

/** FRAM address of the write cursor (2 bytes allocated). */
#define FRAM_WRITE_CURSOR_ADDR	(0x00)

/** FRAM address of the read cursor (2 bytes allocated). */
#define FRAM_READ_CURSOR_ADDR	(0x02)

/** FRAM start address of the data. */
#define FRAM_DATA_START_ADDR	(0x04)

/** Size of each data block in FRAM (in bytes). */
#define FRAM_DATA_FRAME_SIZE	(TRANCEIVER_TX_MAX_FRAME_SIZE + 1)


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int framInit(void);
int framRead(uint8_t* data, uint32_t address, uint32_t size);
int framWrite(uint8_t* data, uint32_t address, uint32_t size);


#endif /* RFRAM_H_ */
