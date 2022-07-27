/**
 * @file RProtobuf.c
 * @date January 25, 2021
 * @author Tyrel Kostyk (tck290)
 */

#include <RProtobuf.h>
#include <hal/errors.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Encode (serialize) a raw protobuf message with NanoPB into a buffer.
 *
 * @param rawMessage The raw (non-serialized) radsat_message struct to encode.
 * @param outgoingBuffer The buffer that will hold the encoded message.
 * @return The size of the encoded message (max 255); 0 if encoding failed.
 */
uint8_t protoEncode(radsat_message* rawMessage, uint8_t* outgoingBuffer) {

	// ensure incoming buffers are not NULL
	if (rawMessage == 0 || outgoingBuffer == 0)
		return 0;

	// create stream object
	pb_ostream_t stream = pb_ostream_from_buffer((uint8_t*)rawMessage, PROTO_MAX_ENCODED_SIZE);

	// encode the message into the byte array
	if (pb_encode(&stream, radsat_message_fields, outgoingBuffer))
		return stream.bytes_written;

	// if the encoding failed, return 0
	return 0;
}


/**
 * Decode an encoded protobuf message.
 *
 * @param incomingBuffer The buffer containing the encoded message (no header).
 * @param decodedMessage The message that will be populated with the decoded message.
 * @return 0 if the message is decoded successfully, otherwise error occured.
 */
int protoDecode(uint8_t* incomingBuffer, radsat_message* decodedMessage) {

	// ensure incoming buffers are not NULL
	if (incomingBuffer == 0 || decodedMessage == 0)
		return E_INPUT_POINTER_NULL;

	// create stream object
	pb_istream_t stream = pb_istream_from_buffer((uint8_t*)incomingBuffer, PROTO_MAX_ENCODED_SIZE);

	// decode the message into the empty message struct
	uint8_t success = pb_decode(&stream, radsat_message_fields, decodedMessage);

	debugPrint("success variable: %d \n", success);
	return !success;
}
