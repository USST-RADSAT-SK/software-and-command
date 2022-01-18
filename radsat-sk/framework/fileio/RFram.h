/**
 * @file RFram.h
 * @date May 18 2021
 * @author Tyrel Kostyk
 */

#ifndef RFRAM_H_
#define RFRAM_H_

#include <stdint.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int framInit(void);
int framRead(uint8_t* data, uint32_t address, uint32_t size);
int framWrite(uint8_t* data, uint32_t address, uint32_t size);


#endif /* RFRAM_H_ */
