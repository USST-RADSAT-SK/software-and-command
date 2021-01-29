/*
 * fileTransferHandler.c
 *
 *  Created on: Jan 25, 2021
 *      Author: Ty
 */

#include <protobuf.h>

#include "hal/Drivers/I2C.h"


void fileTransferReceive(uint16_t messageId)
{

	switch (messageId) {

	// the response (ACK, etc.) to a file transfer packet
	case (fileTransferMessage_FileTransferResponse_tag):
		break;

	// a file transfer packet
	case (fileTransferMessage_FileTransferPacket_tag):
		break;

	// an unknown message
	default:
		assert(false);
		break;
	}

}


void fileTransferSend(uint16_t messageId)
{
	uint8_t *outgoingMessage = (uint8_t *)malloc(fileTransferMessage_size);
	assert(outgoingMessage);
	memset(outgoingMessage, 0, fileTransferMessage_size);


	protoEncode((uint8_t *)outgoingMessage, sizeof(outgoingBuffer), outgoingBuffer);

	// TODO: replace with more generic (custom) radsat-sk I2C call
	I2C_write(0x4F, outgoingBuffer, sizeof(outgoingBuffer));
}

