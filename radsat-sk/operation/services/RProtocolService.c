/**
 * @file RProtocolService.c
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RProtocolService.h>
#include <RMessage.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Generates a wrapped Protocol Service message.
 *
 * @param messageTag The tag of the specific Protocol Service message to generate.
 * @param wrappedMessage A buffer for the generated message. Filled by function.
 * @return The final size of the generated message; 0 on failure.
 */
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
	uint8_t finalSize = messageWrap(&rawMessage, wrappedMessage);

	// return the final size of the message
	return finalSize;
}


/**
 * Handles a wrapped Protocol Service message, returning the specific Protocol Service message tag.
 *
 * @param wrappedMessage A pointer to the wrapped message.
 * @param size The size of the given message buffer, in bytes.
 * @return The specific Protocol Service message tag. 0 on failure.
 */
uint8_t protocolHandle(uint8_t* wrappedMessage, uint8_t size) {

	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return 0;

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


