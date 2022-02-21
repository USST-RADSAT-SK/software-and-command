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
	if (rawMessage.which_service != RadsatMessage_telecommandMessage_tag)
		return 0;

	// obtain the specific telecommand
	uint8_t telecommand = (uint8_t) rawMessage.telecommandMessage.which_message;

	// execute the telecommands
	switch (telecommand) {

		// indicates that a communication link has been established
		case (TelecommandMessage_beginPass_tag):
			// do nothing; this reception of this telecommand already begins the pass mode
			break;

		// indicates that a telecommands are done; ready for file transfers
		case (TelecommandMessage_beginFileTransfer_tag):
			// do nothing; higher level tasks will handle
			break;

		// indicates that all downlink activities shall be ceased
		case (TelecommandMessage_ceaseTransmission_tag):
			// do nothing; higher level tasks will handle
			break;

		// indicates that downlink activities may be resumed
		case (TelecommandMessage_ResumeTransmission_tag):
			// do nothing; higher level tasks will handle
			break;

		// provides a new accurate time for the OBC to set itself to
		case (TelecommandMessage_updateTime_tag):
			// TODO: implement functionality
			break;

		// instructs OBC to reset certain components on the Satellite
		case (TelecommandMessage_reset_tag):
			// TODO: implement functionality
			break;

		// unknown telecommand
		default:
			// do nothing; return failure
			return 0;
	}

	return telecommand;
}

