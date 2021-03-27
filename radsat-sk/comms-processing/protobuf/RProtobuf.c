/**
 * @file RProtobuf.c
 * @date Jan 25, 2021
   @author Tyrel Kostyk (tck290)
 */

#include <RProtobuf.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Encode a raw protobuf message with NanoPB into a buffer, and encode the header as well.
 *
 * @param rawMessage The raw (unencoded) RadSat struct message to encode.
 * @param messageSize The size (in bytes) of the message, pre-encoding.
 * @param outgoingBuffer The buffer that will hold the encoded message & header.
 * @return true if the encoded process is successful; false otherwise.
 */
uint8_t protoEncode( RadsatMessage* rawMessage, uint8_t* outgoingBuffer )
{
	// ensure incoming buffer is not NULL
	assert( rawMessage );
	assert( outgoingBuffer );

	// create stream object
	pb_ostream_t stream = pb_ostream_from_buffer( (uint8_t*)rawMessage, PROTO_MAX_ENCODED_SIZE );

	// encode the message into the byte array
	uint8_t protoSuccess = pb_encode( &stream, RadsatMessage_fields, &outgoingBuffer );

	return protoSuccess;
}


/**
 * Decode an encoded protobuf message, and confirm that the contents match the header.
 *
 * @param incomingBuffer The buffer containing the encoded message/header.
 * @param decodedMessage The message that will be populated with the decoded message.
 * @return true if the message is decoded successfully, false otherwise.
 */
uint8_t protoDecode( uint8_t* incomingBuffer, RadsatMessage* decodedMessage )
{
	// ensure incoming buffer is not NULL
	assert( incomingBuffer );
	assert( decodedMessage );

	// create stream object
	pb_istream_t stream = pb_istream_from_buffer( (uint8_t*)incomingBuffer, PROTO_MAX_ENCODED_SIZE );

	// encode the message into the byte array
	return pb_decode( &stream, RadsatMessage_fields, &decodedMessage );
}
