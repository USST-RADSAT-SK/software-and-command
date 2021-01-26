/*
 * protobuf.c
 *
 *  Created on: Jan 25, 2021
   Author: Ty
 */

#include <protobuf.h>


void protoEncode(radsatMessage *rawMessage, uint16_t messageSize, uint8_t *outgoingBuffer)
{
	// ensure incoming buffer is not NULL
	assert(rawMessage);
	assert(outgoingBuffer);

	// don't bother if there are no bytes to encode
	if (messageSize == 0)
		return;

	// create stream object
	pb_ostream_t stream = pb_ostream_from_buffer((uint8_t *)rawMessage, messageSize);

	// encode the message into the byte array
	pb_encode(&stream, radsatMessage_fields, &outgoingBuffer);
}


void protoDecode(uint8_t *incomingBuffer, uint16_t bufferSize, radsatMessage *decodedMessage)
{
	// ensure incoming buffer is not NULL
	assert(incomingBuffer);
	assert(decodedMessage);

	// don't bother if there are no bytes to decode
	if (bufferSize == 0)
		return;

	// create stream object
	pb_istream_t stream = pb_istream_from_buffer(incomingBuffer, bufferSize);

	// encode the message into the byte array
	pb_decode(&stream, radsatMessage_fields, &decodedMessage);
}
