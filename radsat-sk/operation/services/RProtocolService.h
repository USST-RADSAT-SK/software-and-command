/**
 * @file RProtocolService.h
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RPROTOCOLSERVICE_H_
#define RPROTOCOLSERVICE_H_

#include <stdint.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t protocolGenerate(uint16_t messageTag, uint8_t* wrappedMessage);
uint8_t protocolHandle(uint8_t* wrappedMessage, uint8_t size);


#endif /* RPROTOCOLSERVICE_H_ */
