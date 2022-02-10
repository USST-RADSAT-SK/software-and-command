/**
 * @file RProtobuf.c
 * @date January 25, 2021
 * @author Tyrel Kostyk (tck290)
 */

#include <RProtobuf.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Encode (serialize) a raw protobuf message with NanoPB into a buffer.
 *
 * @param rawMessage The raw (non-serialized) RadsatMessage struct to encode.
 * @param outgoingBuffer The buffer that will hold the encoded message.
 * @return The size of the encoded message; 0 if encoding failed.
 */
uint8_t protoEncode(RadsatMessage* rawMessage, uint8_t* outgoingBuffer) {

	// ensure incoming buffers are not NULL
	if (rawMessage == 0 || outgoingBuffer == 0)
		return 0;

	// create stream object
	pb_ostream_t stream = pb_ostream_from_buffer((uint8_t*)rawMessage, PROTO_MAX_ENCODED_SIZE);

	// encode the message into the byte array
	if (pb_encode(&stream, RadsatMessage_fields, outgoingBuffer))
		return stream.bytes_written;

	// if the encoding failed, return 0
	return 0;
}


/**
 * Decode an encoded protobuf message.
 *
 * @param incomingBuffer The buffer containing the encoded message (no header).
 * @param decodedMessage The message that will be populated with the decoded message.
 * @return true (1) if the message is decoded successfully, false (0) otherwise.
 */
uint8_t protoDecode(uint8_t* incomingBuffer, RadsatMessage* decodedMessage) {

	// ensure incoming buffers are not NULL
	if (incomingBuffer == 0 || decodedMessage == 0)
		return 0;

	// create stream object
	pb_istream_t stream = pb_istream_from_buffer((uint8_t*)incomingBuffer, PROTO_MAX_ENCODED_SIZE);

	// encode the message into the byte array
	uint8_t success = pb_decode(&stream, RadsatMessage_fields, decodedMessage);

	return success;
}
