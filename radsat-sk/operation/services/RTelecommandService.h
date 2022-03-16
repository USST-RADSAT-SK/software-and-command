/**
 * @file RTelecommandService.h
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RTELECOMMANDSERVICE_H_
#define RTELECOMMANDSERVICE_H_

#include <RTelecommands.pb.h>
#include <stdint.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t telecommandHandle(uint8_t* wrappedMessage, uint8_t size);


#endif /* RTELECOMMANDSERVICE_H_ */
