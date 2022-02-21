/**
 * @file RMessage.h
 * @date February 20, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RMessage.h>


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


uint8_t messageUnwrap(uint8_t* wrappedMessage, RadsatMessage* rawMessage) {

	// TODO decrypt entire message

	// TODO confirm preamble, CRC

	// TODO deserialize message

	// TODO fill the given buffer with the final unwrapped message

	return 0;
}

