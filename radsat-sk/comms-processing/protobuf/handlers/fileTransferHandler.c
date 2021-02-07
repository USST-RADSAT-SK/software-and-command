/*
 * fileTransferHandler.c
 *
 *  Created on: Jan 25, 2021
 *      Author: Ty
 */

#include <protobuf.h>

#include "hal/Drivers/I2C.h"

typedef enum {
	ACK = 0x06,
	NAK = 0x15
} fileTransferResponse;


void fileTransferReceive(radsatMessage *message)
{
	uint8_t topicId = message->which_topic;
	uint8_t messageId = message->topic->FileTransferMessage->which_message;

	// edge case check
	if (topicId == radsatMessage_TelecommandMessage_tag && messageId == telecommandMessage_CeaseTransmission_tag)
		xEventGroupSetBits( CommunicationEventGroup, CEASE_TRANSMISSION );

	switch (messageId) {

	// the response (ACK, etc.) to a file transfer packet
	case (fileTransferMessage_FileTransferResponse_tag):
		// if ACK, raise ACK flag
		if (message->topic->FileTransferMessage->message->FileTransferResponse.response == ACK)
			xEventGroupSetBits( CommunicationEventGroup, ACK_BIT);

		// if NAK, raise NAK flag
		else if (message->topic->FileTransferMessage->message->FileTransferResponse.response == NAK)
			xEventGroupSetBits( CommunicationEventGroup, NAK_BIT);

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

