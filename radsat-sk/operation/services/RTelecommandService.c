/**
 * @file RTelecommandService.c
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RTelecommandService.h>
#include <RMessage.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t telecommandHandle(uint8_t* wrappedMessage, uint8_t size) {

	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return 0;

	// generate new RADSAT-SK message to populate
	RadsatMessage rawMessage = { 0 };

	// unwrap the message
	uint8_t rawSize = messageUnwrap(wrappedMessage, size, &rawMessage);

	// exit if unwrapping failed
	if (rawSize == 0)
		return 0;

	// exit if this message is not a telecommand message
	if (rawMessage.which_topic != RadsatMessage_telecommandMessage_tag)
		return 0;

	// obtain the specific telecommand
	uint8_t telecommand = (uint8_t) rawMessage.telecommandMessage.which_message;

	// execute the telecommands
	switch (telecommand) {

		// TODO: implement functionality
		case (TelecommandMessage_beginPass_tag):
			break;

		// TODO: implement functionality
		case (TelecommandMessage_beginFileTransfer_tag):
			break;

		// TODO: implement functionality
		case (TelecommandMessage_ceaseTransmission_tag):
			break;

		// TODO: implement functionality
		case (TelecommandMessage_ResumeTransmission_tag):
			break;

		// TODO: implement functionality
		case (TelecommandMessage_updatePassTime_tag):
			break;

		// TODO: implement functionality
		case (TelecommandMessage_reset_tag):
			break;

		// unknown telecommand; return failure
		default:
			return 0;
	}

	return telecommand;
}

