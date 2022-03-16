/**
 * @file RFileTransferService.h
 * @date February 25, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RFILETRANSFERSERVICE_H_
#define RFILETRANSFERSERVICE_H_

#include <stdint.h>
#include <RFileTransfer.pb.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t fileTransferNextFrame(uint8_t* frame);
uint8_t fileTransferCurrentFrame(uint8_t* frame);

int fileTransferAddMessage(const void* message, uint8_t size, uint16_t messageTag);


#endif /* RFILETRANSFERSERVICE_H_ */
