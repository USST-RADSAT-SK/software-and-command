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
uint8_t uplinkHandle(uint8_t* wrappedMessage, uint8_t size, uint8_t* messageType) {
	debugPrint("got to telecommand handle\n");
	uint8_t rawSize = 0;
	uint8_t returnVal = 1;

	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return 0;

	// generate new RADSAT-SK message to populate
	radsat_message rawMessage = { 0 };

	// unwrap the message
	rawSize = messageUnwrap(wrappedMessage, size, &rawMessage);
	debugPrint("rawSize: %d\n", rawSize);
	// exit if unwrapping failed
	if (rawSize == 0)
		return 0;
	debugPrint("after rawSize\n");

	// if this message is a protocol message
	if (rawMessage.which_service == radsat_message_ProtocolMessage_tag){

			// pass out the message type
			*messageType = radsat_message_ProtocolMessage_tag;

			// obtain and return the response
			uint8_t response = (uint8_t) rawMessage.ProtocolMessage.which_message;

			return response;
	}
	// if this message is a telecommand message
	else if (rawMessage.which_service == radsat_message_TelecommandMessage_tag){

			// pass out the message type
			*messageType = radsat_message_TelecommandMessage_tag;

			// obtain the specific telecommand
			uint8_t telecommand = (uint8_t) rawMessage.TelecommandMessage.which_message;

			// execute the telecommands
			switch (telecommand) {

				// indicates that a communication link has been established
				case (telecommand_message_BeginPass_tag):
					// do nothing; this reception of this telecommand already begins the pass mode
					break;

				// indicates that a telecommands are done; ready for file transfers
				case (telecommand_message_BeginFileTransfer_tag):
					// do nothing; higher level tasks will handle
					break;

				// indicates that all downlink activities shall be ceased
				case (telecommand_message_CeaseTransmission_tag):
					// do nothing; higher level tasks will handle
					break;

				// indicates that downlink activities may be resumed
				case (telecommand_message_ResumeTransmission_tag):
					// do nothing; higher level tasks will handle
					break;

				// provides a new accurate time for the OBC to set itself to
				case (telecommand_message_UpdateTime_tag):
					// TODO: implement functionality
					break;

				// instructs OBC to reset certain components on the Satellite
				case (telecommand_message_Reset_tag):
					// TODO: implement functionality
					break;

				// unknown telecommand
				default:
					// do nothing; return failure
					return 0;
			}

			return telecommand;
		}

	// neither protocol or telecommand, should never happen
	else {
		*messageType = 0;
		return 0;
	}

}

