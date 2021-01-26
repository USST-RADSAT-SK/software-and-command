///*
// * fileTransferHandler.c
// *
// *  Created on: Jan 25, 2021
// *      Author: Ty
// */
//
//#include <protobuf.h>
//
//#include "hal/Drivers/I2C.h"
//
//
//void fileTransferHandle(void)
//{
//	transferResponse message;
//
//	message.packetNumber = 10;
//	message.packetType = 0;
//	message.responseType = ACK;
//
////	MyMessage.message = message;
//
//	uint8_t *outgoingBuffer = (uint8_t *)malloc(transferResponse_size);
//	// ...
//
//	protoEncode((uint8_t *)message, sizeof(outgoingBuffer), outgoingBuffer);
//
//	// TODO: replace with more generic (custom) I2C call
//	I2C_write(0x4F, outgoingBuffer, sizeof(outgoingBuffer));
//}
//
//
//typedef enum response_ {
//	ACK = 0,
//	NACK = 1,
//} response_t;
//
