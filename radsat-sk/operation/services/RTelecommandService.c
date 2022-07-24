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

/**
 * Process (and execute where necessary) received telecommands.
 *
 * @param wrappedMessage A pointer to a wrapped (encrypted, etc.) RADSAT-SK message.
 * @param size The size (in bytes) of the wrapped message.
 * @return The tag of the processed telecommand (0 on failure).
 */
uint8_t telecommandHandle(uint8_t* wrappedMessage, uint8_t size) {

	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return 0;

	// generate new RADSAT-SK message to populate
	radsat_message rawMessage = { 0 };

	// unwrap the message
	uint8_t rawSize = messageUnwrap(wrappedMessage, size, &rawMessage);

	// exit if unwrapping failed
	if (rawSize == 0)
		return 0;

	// exit if this message is not a telecommand message
	if (rawMessage.which_service != radsat_message_TelecommandMessage_tag)
		return 0;

	// obtain the specific telecommand
	uint8_t telecommand = (uint8_t) rawMessage.TelecommandMessage.which_message;

	// execute the telecommands
	switch (telecommand) {

		// indicates that a communication link has been established
		case (telecommand_message_BeginPass_tag):
			debugPrint("in RTelecommandService.c: Message received was of type BeginPass.\n");
			// do nothing; this reception of this telecommand already begins the pass mode
			break;

		// indicates that a telecommands are done; ready for file transfers
		case (telecommand_message_BeginFileTransfer_tag):
			debugPrint("in RTelecommandService.c: Message received was of type BeginFileTransfer.\n");
			// do nothing; higher level tasks will handle
			break;

		// indicates that all downlink activities shall be ceased
		case (telecommand_message_CeaseTransmission_tag):
			debugPrint("in RTelecommandService.c: Message received was of type CeaseTransmission.\n");
			// do nothing; higher level tasks will handle
			break;

		// indicates that downlink activities may be resumed
		case (telecommand_message_ResumeTransmission_tag):
			debugPrint("in RTelecommandService.c: Message received was of type ResumeTransmission.\n");
			// do nothing; higher level tasks will handle
			break;

		// provides a new accurate time for the OBC to set itself to
		case (telecommand_message_UpdateTime_tag):
			debugPrint("in RTelecommandService.c: Message received was of type UpdateTime.\n");
			// TODO: implement functionality
			break;

		// instructs OBC to reset certain components on the Satellite
		case (telecommand_message_Reset_tag):
			debugPrint("in RTelecommandService.c: Message received was of type Reset.\n");
			// TODO: implement functionality
			break;

		// unknown telecommand
		default:
			debugPrint("in RTelecommandService.c: Message received was not of known type. Telecommand variable has value %d.\n", telecommand);
			// do nothing; return failure
			return 0;
	}

	return telecommand;
}

