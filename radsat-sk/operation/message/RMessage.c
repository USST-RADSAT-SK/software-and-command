/**
 * @file RMessage.h
 * @date February 20, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RMessage.h>
#include <RXorCipher.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t messageWrap(RadsatMessage* rawMessage, uint8_t* wrappedMessage) {

	// serialize the raw message with NanoPB Protobuf encoding
	uint8_t encodedSize = protoEncode(rawMessage, &wrappedMessage[RADSAT_SK_HEADER_SIZE]);
	if (encodedSize == 0)
		return 0;

	// populate the message header
	radsat_sk_header_t *header = (radsat_sk_header_t *)wrappedMessage;
	header->preamble = RADSAT_SK_MESSAGE_PREAMBLE;
	header->size = (uint8_t) encodedSize;

	// calculate the CRC of entire message (except for preamble and crc itself)
	header->crc = crcFast(&wrappedMessage[RADSAT_SK_HEADER_CRC_OFFSET],
						  (int)(header->size + RADSAT_SK_HEADER_SIZE - RADSAT_SK_HEADER_CRC_OFFSET));

	return header->size;
}


uint8_t messageUnwrap(uint8_t* wrappedMessage, uint8_t size, RadsatMessage* rawMessage) {

	// decrypt entire message
	int error = xorDecrypt(wrappedMessage, size);
	if (error)
		return 0;

	// access the message header; obtain size, confirm preamble and CRC
	radsat_sk_header_t *header = (radsat_sk_header_t *)wrappedMessage;

	if (header->preamble != RADSAT_SK_MESSAGE_PREAMBLE)
		return 0;

	// calculate and confirm the CRC of entire message (except for preamble and crc itself)
	crc_t localCrc = crcFast(&wrappedMessage[RADSAT_SK_HEADER_CRC_OFFSET],
							 (int)(size - RADSAT_SK_HEADER_CRC_OFFSET));

	if (header->crc != localCrc)
		return 0;

	// deserialize the encoded message with NanoPB Protobuf decoding
	error = protoDecode(&wrappedMessage[RADSAT_SK_HEADER_SIZE], rawMessage);
	if (error)
		return 0;

	return header->size;
}

