/**
 * @file TProtobuf.c
 * @date March 26, 2021
 * @author Tyrel Kostyk (tck290)
 * @brief Unit test the Protobuf functionality.
 */

#include <TProtobuf.h>


bool test_protoEncode( void )
{
	// create message
	RadsatMessage msg;
	msg.which_topic = RadsatMessage_fileTransferMessage_tag;
	msg.topic.fileTransferMessage.which_message = FileTransferMessage_fileTransferPacket_tag;
	msg.topic.fileTransferMessage.message.fileTransferPacket.data.bytes[0] = 0xAA;

	// create buffer to encode into
	uint8_t myBuffer[200];

	// test encoding
	uint8_t protoSuccess = protoEncode(&msg, &myBuffer);

	// check encoding results
	if (!protoSuccess)
		return false;

	// check header
	RProtoHeader header = (RProtoHeader*)myBuffer;
	if (header->preamble != PROTO_PREAMBLE)
		return false;
	if (header->topicTag != RadsatMessage_fileTransferMessage_tag)
		return false;
	if (header->messageTag != FileTransferMessage_fileTransferPacket_tag)
		return false;

	// all checks passed
	return true;
}


bool test_protoDecode( void )
{
	return false;
}
