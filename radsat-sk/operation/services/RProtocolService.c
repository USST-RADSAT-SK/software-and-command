/**
 * @file RProtocolService.c
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RProtocolService.h>
#include <RMessage.h>
#include <hal/errors.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t protocolGenerate(uint16_t messageTag, uint8_t* wrappedMessage) {

	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return 0;

	// generate new RADSAT-SK message to serialize
	RadsatMessage rawMessage = { 0 };

	// populate the simple protocol message
	rawMessage.which_topic = RadsatMessage_protocolMessage_tag;
	rawMessage.protocolMessage.which_message = messageTag;

	// prepare the message
	uint8_t finalSize = messageWrap(rawMessage, wrappedMessage);

	// return the final size of the message
	return finalSize;
}


uint8_t protocolHandle(uint8_t* wrappedMessage, uint8_t size) {

	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return E_INPUT_POINTER_NULL;

	// generate new RADSAT-SK message to populate
	RadsatMessage rawMessage = { 0 };

	// unwrap the message
	uint8_t rawSize = messageUnwrap(wrappedMessage, size, &rawMessage);

	if (rawSize == 0)
		return 0;

	// obtain and return the response
	uint8_t response = (uint8_t) rawMessage.protocolMessage.which_message;

	return response;
}


