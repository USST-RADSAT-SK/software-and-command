/*
 * protobuf.h
 *
 *  Created on: Jan 25, 2021
 *      Author: Ty
 */

#ifndef PROTOBUF_H_
#define PROTOBUF_H_

#include <common.h>
#include <pb_encode.h>
#include <pb_decode.h>


typedef struct RProtoHeader {
	uint32_t preamble;		///> 0x2005 -> A flag that defines the "start" of a message
	uint32_t crc;			///> A 4-byte (32-bit) CRC of the message, not including the header
	uint16_t topicTag;		///> The ID of the topic for which the message belongs to
	uint16_t messageTag;	///> The ID of the message itself
};


void protoEncode(uint8_t *messageBuffer, uint16_t messageSize, uint8_t *outgoingBuffer);


#endif /* PROTOBUF_H_ */
