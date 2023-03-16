/**
 * @file RFileTransferService.h
 * @date February 25, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RFILETRANSFERSERVICE_H_
#define RFILETRANSFERSERVICE_H_

#include <stdint.h>
#include <RFileTransfer.h>




/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

short fileTransferNextFrame(uint8_t* frame);
short fileTransferCurrentFrame(uint8_t* frame);

int fileTransferAddMessage(const void* message, uint8_t size, uint16_t messageTag);

void fileTransferReset(void);


#endif /* RFILETRANSFERSERVICE_H_ */

