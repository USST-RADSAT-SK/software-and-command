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

/**
 * Wrap a raw RADSAT-SK message, preparing it for downlink.
 *
 * This function will serialize the message with NanoPB Protobuf encoding, populate and prepend
 * a message header, and generate a CRC of the message.
 *
 * @param rawMessage The raw RADSAT-SK message, including all desired data to be downlinked.
 * @param wrappedMessage The final wrapped message. Filled by function.
 * @return The total size of the message, including the header. 0 on failure.
 */
uint8_t messageWrap(RadsatMessage* rawMessage, uint8_t* wrappedMessage) {

	// ensure the input pointers are not NULL
	if (rawMessage == 0 || wrappedMessage == 0)
		return 0;

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

	return header->size + RADSAT_SK_HEADER_SIZE;
}


/**
 * Unwrap a message, extracting its contents after being uplinked.
 *
 * This message decrypts the message using a simple XOR Cipher, extracts and validates the data
 * found in the message header (including the CRC), and then deserializes the message with NanoPB
 * Protobuf encoding.
 *
 * @param wrappedMessage The message to unwrap.
 * @param size The size of the full message buffer.
 * @param rawMessage The final extracted message. Filled by function.
 * @return The size of the message, not including the header. 0 on failure.
 */
uint8_t messageUnwrap(uint8_t* wrappedMessage, uint8_t size, RadsatMessage* rawMessage) {

	// ensure the input pointers are not NULL
	if (wrappedMessage == 0 || rawMessage == 0)
		return 0;

	// decrypt entire message
	int error = xorDecrypt(wrappedMessage, size);
	if (error)
		return 0;

	// access the message header; obtain size, confirm preamble and CRC
	radsat_sk_header_t *header = (radsat_sk_header_t *)wrappedMessage;

	// confirm preamble
	if (header->preamble != RADSAT_SK_MESSAGE_PREAMBLE)
		return 0;

	// locally calculate the CRC of the entire message (except for preamble and crc itself)
	crc_t localCrc = crcFast(&wrappedMessage[RADSAT_SK_HEADER_CRC_OFFSET],
							 (int)(size - RADSAT_SK_HEADER_CRC_OFFSET));

	// confirm locally-calculated CRC with the one sent with the message header
	if (header->crc != localCrc)
		return 0;

	// deserialize the encoded message with NanoPB Protobuf decoding
	error = protoDecode(&wrappedMessage[RADSAT_SK_HEADER_SIZE], rawMessage);
	if (error)
		return 0;

	// return the size of the message itself
	return header->size;
}

