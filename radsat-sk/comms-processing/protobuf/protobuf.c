/*
 * protobuf.c
 *
 *  Created on: Jan 25, 2021
   Author: Ty
 */

#include <protobuf.h>

/* Protobuf Proto Includes */
#include <radsat.pb.h>


//typedef struct {...} MyMessage;

void protoEncode(uint8_t *messageBuffer, uint16_t messageSize, uint8_t *outgoingBuffer)
{
	// ensure incoming buffer is not NULL
	assert(messageBuffer);
	assert(outgoingBuffer);

	if (messageSize < 0)
		return;

	pb_ostream_t stream;
	stream = pb_ostream_from_buffer(messageBuffer, sizeof(messageBuffer));
	pb_encode(&stream, transferResponse_msg, &outgoingBuffer);
}

