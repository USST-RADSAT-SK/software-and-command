/**
 * @file protobuf.c
 *
 *  Created on: Jan 25, 2021
   Author: Ty
 */

#include <RADProtobuf.h>



uint8_t protoEncode(RadsatMessage *rawMessage, uint16_t messageSize, uint8_t *outgoingBuffer)
{
	// ensure incoming buffer is not NULL
	assert( rawMessage );
	assert( outgoingBuffer );

	// encode header
	// ...

	// create stream object
	pb_ostream_t stream = pb_ostream_from_buffer( (uint8_t *)rawMessage, messageSize );

	// encode the message into the byte array
	return pb_encode( &stream, RadsatMessage_fields, &outgoingBuffer );
}


uint8_t protoDecode(uint8_t *incomingBuffer, uint16_t bufferSize, RadsatMessage *decodedMessage)
{
	// ensure incoming buffer is not NULL
	assert( incomingBuffer );
	assert( decodedMessage );

	// encode header
	// ...

	// create stream object
	pb_istream_t stream = pb_istream_from_buffer( incomingBuffer, bufferSize );

	// encode the message into the byte array
	return pb_decode( &stream, RadsatMessage_fields, &decodedMessage );
}
