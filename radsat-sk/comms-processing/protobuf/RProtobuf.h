/**
 * @file RProtobuf.h
 * @date Jan 25, 2021
   @author Tyrel Kostyk (tck290)
 */

#ifndef RPROTOBUF_H_
#define RPROTOBUF_H_

#include <common.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include <radsat.pb.h>


/***************************************************************************************************
                                             DEFINITIONS
***************************************************************************************************/

#define PROTO_MAX_ENCODED_SIZE	((uint16_t)256)		///> The max size of a message. TODO: Update with real value
#define PROTO_PREAMBLE			((uint16_t)0x2005)	///> The year the USST was founded :)

typedef struct _RProtoHeader {
	uint16_t preamble;		///> A 16 bit flag that defines the "start" of any protobuf message
	uint16_t size;			///> The size of the message in bytes
	uint32_t crc;			///> A 4-byte (32-bit) CRC of the message, not including the header
	uint16_t topicTag;		///> The ID of the topic for which the message belongs to
	uint16_t messageTag;	///> The ID of the message itself
} RProtoHeader;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t protoEncode(RadsatMessage *rawMessage, uint8_t *outgoingBuffer);
uint8_t protoDecode(uint8_t *incomingBuffer, RadsatMessage *decodedMessage);


#endif /* RPROTOBUF_H_ */
