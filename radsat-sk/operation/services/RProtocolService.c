/**
 * @file RProtocolService.c
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RProtocolService.h>
#include <RMessage.h>
#include <RRadsat.pb.h>
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
uint8_t protocolGenerate(uint16_t messageTag, uint8_t* wrappedMessage) {

	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return 0;

	// generate new RADSAT-SK message to serialize
	radsat_message rawMessage = { 0 };

	// populate the simple protocol message
	rawMessage.which_service = radsat_message_ProtocolMessage_tag;
	rawMessage.ProtocolMessage.which_message = messageTag;

	// prepare the message
	uint8_t finalSize = messageWrap(&rawMessage, wrappedMessage);

	// return the final size of the message
	return finalSize;
}


/**
 * Handles a generic uplinked message, returning the specific message tag and message type.
 *
 * @param wrappedMessage A pointer to the wrapped message.
 * @param size The size of the given message buffer, in bytes.
 */

void genericHandle(uint8_t* wrappedMessage, uint8_t size){
	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return;

	// generate new RADSAT-SK message to populate
	radsat_message rawMessage = { 0 };

	// unwrap the message
	uint8_t rawSize = messageUnwrap(wrappedMessage, size, &rawMessage);

	// exit if unwrapping failed
	if (rawSize == 0){
		sendNack();
		return;
	}
	// exit if this message is not a protocol message or telecommand message
	if (rawMessage.which_service != radsat_message_ProtocolMessage_tag && rawMessage.which_service != radsat_message_TelecommandMessage_tag){
		sendNack();
		return;
	}
	// if protocol message
	if (rawMessage.which_service == radsat_message_ProtocolMessage_tag){
		// obtain and return the response
		uint8_t response = (uint8_t) rawMessage.ProtocolMessage.which_message;
		switch (response) {
			case (protocol_message_Ack_tag):
				ackReceived();
				debugPrint("Massage Received: Ack\n");
				return;
			case (protocol_message_Nack_tag):
				nackReceived();
				debugPrint("Massage Received: Nack\n");
				return;
			default:
				debugPrint("Massage Received: ???\n");
				sendNack();
				return;
		}
	}

	// if telecommand message
	else if (rawMessage.which_service == radsat_message_TelecommandMessage_tag){
		// obtain the specific telecommand
		uint8_t telecommand = (uint8_t) rawMessage.TelecommandMessage.which_message;
		int error = SUCCESS;
		// execute the telecommands
		switch (telecommand) {

			// indicates that a communication link has been established
			case (telecommand_message_BeginPass_tag):
				// do nothing; this reception of this telecommand already begins the pass mode
				debugPrint("Massage Received: beginPass\n");
				beginPass();
				return;

			// indicates that a telecommands are done; ready for file transfers
			case (telecommand_message_BeginFileTransfer_tag):
				debugPrint("Massage Received: beginFileTransfer\n");
				beginFileTransfer();
				return;

			// indicates that all downlink activities shall be ceased
			case (telecommand_message_CeaseTransmission_tag):
				debugPrint("Massage Received: AceaseTransmissionck\n");
				ceaseTransmission();
				return;

			// indicates that downlink activities may be resumed
			case (telecommand_message_ResumeTransmission_tag):
				debugPrint("Massage Received: resumeTransmission\n");
				resumeTransmission();
				return;

			// provides a new accurate time for the OBC to set itself to
			case (telecommand_message_UpdateTime_tag):
						debugPrint("Massage Received: UpdateTime\n");
				uint32_t time = rawMessage.TelecommandMessage.UpdateTime.unixTime;
				error = updateTime(time);
				if (error) warningPrint("Error Setting time: error= %d", error);
				printTime();
				return;

			// instructs OBC to reset certain components on the Satellite
			case (telecommand_message_Reset_tag):
						debugPrint("Massage Received: Reset\n");
				resetSat();
				return;

			// unknown telecommand
			default:
				debugPrint("Massage unknown send Nack\n");
				sendNack();
				// do nothing; return failure
				return;
		}
	}
	sendNack();
	return;
}


