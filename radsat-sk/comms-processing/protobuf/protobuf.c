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


void TelecommandTask()
{
	// block on pass time (wait until above Saskatoon)

	// block on Telecommand Turn (wait until we're in the Telecommand)
}


void FileTransferTask()
{
	radsatMessage *receivedMessage;
	radsatMessage *outgoingMessage;
	uint8_t *outgoingBuffer;
	uint8_t *receivedBuffer;
	I2Ctransfer *trx;

	while (true) {

		// block on pass mode, etc. etc.

		// set up message
		outgoingMessage->which_topic = radsatMessage_FileTransferMessage_tag;
		outgoingMessage->topic->FileTransferMessage->message->FileTransferPacket.data = getData();
		// ...

		uint32_t dataSize = outgoingMessage->topic->FileTransferMessage->message->FileTransferPacket.data.size;

		// encode
		protoEncode(outgoingMessage, dataSize, outgoingBuffer);

		// send the message (send over I2C to transceiver for downlink)
//		fileTransferSend(outgoingBuffer);
		trx->writeData = outgoingBuffer;
		I2C_writeRead(trx);

		// set up I2C Request to transceiver for RX message
		// wants to receive a FileTransferResponse
		trx.slaveAddress = 0x5A;
		// ...

		I2C_writeRead(trx);

		// copy response into local buffer
		memcpy(receivedBuffer, trx.readData, trx.readSize);

		// decode the received message
		uint8_t protoSuccess = protoDecode(receivedBuffer, trx.readSize, receivedMessage);

		// handle the protobuf message
		if (protoSuccess == true)
			fileTransferReceive(receivedMessage);
	}
}


void protoHandle(radsatMessage *message)
{
	switch(message->which_topic) {

	// file transfer messages
	case(radsatMessage_FileTransferMessage_tag):
		fileTransferReceive(message);
		break;

	// telecommand messages
	case(radsatMessage_TelecommandMessage_tag):
		telecommandReceive(message->topic->TelecommandMessage->which_message);
		break;

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
