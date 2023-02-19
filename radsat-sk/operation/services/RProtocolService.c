/**
 * @file RProtocolService.c
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RProtocolService.h>
#include <RRadsat.h>
#include <RCommon.h>
#include <RCommunicationStateMachine.h>


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
uint8_t protocolGenerate(commandType_t command, uint8_t* wrappedMessage) {

	// ensure the input pointer is not NULL
	if (wrappedMessage == NULL){
		errorPrint("wrappedMessage is null pointer!!!");
		return 0;
	}

	// generate new RADSAT-SK message to serialize
	radsat_message rawMessage = { 0 };

	// populate the simple protocol message
	switch (command){
		case commandAck:
			rawMessage.which_service = protocol_Ack_tag;
			rawMessage.ProtocolMessage.Ack.resp = 1;
			break;
		case commandNack:
			rawMessage.which_service = protocol_Nack_tag;
			rawMessage.ProtocolMessage.Nack.resp = 1;
			break;
		default:
			errorPrint("Unknown Command: %d!!!", command);
			return 0;
	}

	// prepare the message
	uint8_t finalSize = messageWrap(&rawMessage, (radsat_sk_raw_message_t*)wrappedMessage);


	// return the final size of the message
	return finalSize;
}


/**
 * Handles a generic uplinked message, returning the specific message tag and message type.
 *
 * @param wrappedMessage A pointer to the wrapped message.
 * @param size The size of the given message buffer, in bytes.
 * @param Output pointer variable for the message data.
 */

commandType_t genericHandle(uint8_t* wrappedMessage, uint8_t size, messageSubject_t* messageData) {
	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return E_INPUT_POINTER_NULL;

	// generate new RADSAT-SK message to populate
	radsat_message rawMessage = { 0 };
	//return command
	commandType_t command;
	//message data copy size
	size_t copySize;
	//message data copy location
	void* copyLocation;

	// unwrap the message
	uint8_t rawSize = messageUnwrap(wrappedMessage, size, &rawMessage);

	// exit if unwrapping failed
	if (rawSize == 0){
		warningPrint("Failed to unwrap message = %d", rawMessage.which_service);
		return commandProtoUnwrapError;
	}

	// obtain the specific telecommand
	radsat_tag_t telecommand = rawMessage.which_service;
	// execute the telecommands
	switch (telecommand) {
	// indicates that a communication link has been established
	case (protocol_Ack_tag):
		infoPrint("Massage unpacked: Ack");
		command = commandAck;
		copyLocation = &rawMessage.ProtocolMessage.Ack;
		copySize = sizeof(ack);
		break;
	case (protocol_Nack_tag):
		infoPrint("Massage unpacked: Nack");
		command = commandNack;
		copyLocation = &rawMessage.ProtocolMessage.Nack;
		copySize = sizeof(nack);
		break;
	case (telecommand_BeginPass_tag):
		// do nothing; this reception of this telecommand already begins the pass mode
		infoPrint("Massage unpacked: beginPass, PassTime=%lus", rawMessage.TelecommandMessage.BeginPass.passLength);
		command = commandBeginPass;
		copyLocation = &rawMessage.TelecommandMessage.BeginPass;
		copySize = sizeof(begin_pass);
		break;
	// indicates to start file transfers
	case (telecommand_BeginFileTransfer_tag):
		infoPrint("Massage unpacked: beginFileTransfer");
		command = commandBeginFileTransfer;
		copyLocation = &rawMessage.TelecommandMessage.BeginFileTransfer;
		copySize = sizeof(begin_file_transfer);
		break;
	// indicates that all downlink activities shall be ceased
	case (telecommand_CeaseTransmission_tag):
		infoPrint("Massage unpacked: ceaseTransmission");
		command = commandCeaseTransmission;
		copyLocation = &rawMessage.TelecommandMessage.CeaseTransmission;
		copySize = sizeof(cease_transmission);
		break;
	// provides a new accurate time for the OBC to set itself to
	case (telecommand_UpdateTime_tag):
		infoPrint("Massage unpacked: UpdateTime");
		command = commandUpdateTime;
		copyLocation = &rawMessage.which_service + 1;
		copySize = sizeof(update_time);
		break;
	// instructs OBC to reset certain components on the Satellite
	case (telecommand_Reset_tag):
		infoPrint("Massage unpacked: Reset");
		command = commandReset;
		copyLocation = &rawMessage.TelecommandMessage.Reset;
		copySize = sizeof(reset);
		break;
	// unknown telecommand
	default:
		warningPrint("Telecommand unknown: %d", telecommand);
		return commandUnknownCommand;
	}

	memcpy(messageData, copyLocation, copySize);
	return command;
}


