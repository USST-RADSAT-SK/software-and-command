/**
 * @file RProtocolService.h
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RPROTOCOLSERVICE_H_
#define RPROTOCOLSERVICE_H_

#include <RProtocol.pb.h>
#include <stdint.h>


/***************************************************************************************************
                                             DEFINITIONS
***************************************************************************************************/


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t protocolGenerate(uint16_t messageTag, uint8_t* wrappedMessage);
void genericHandle(uint8_t* wrappedMessage, uint8_t size);


#endif /* RPROTOCOLSERVICE_H_ */
