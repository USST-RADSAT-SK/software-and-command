/**
 * @file RMessage.h
 * @date February 20, 2022
 * @author Tyrel Kostyk (tck290)
 * Last edited 27 July 2022 by Brian Pitzel
 */

#include <RMessage.h>
#include <RCommon.h>
#include <RXorCipher.h>
#include <hal/Timing/Time.h>

/**
 * Encode (serialize) a raw protobuf message with NanoPB into a buffer.
 *
 * @param rawMessage The raw (non-serialized) radsat_message struct to encode.
 * @param outgoingBuffer The buffer that will hold the encoded message.
 * @return The size of the encoded message (max 255); 0 if encoding failed.
 */
static uint8_t getRadsatMessageSize(radsat_tag_t tag) {
	switch (tag) {
	case file_transfer_ObcTelemetry_tag:
		return file_transfer_ObcTelemetry_size;
	case file_transfer_TransceiverTelemetry_tag:
		return file_transfer_TransceiverTelemetry_size;
	case file_transfer_CameraTelemetry_tag:
		return file_transfer_CameraTelemetry_size;
	case file_transfer_EpsTelemetry_tag:
		return file_transfer_EpsTelemetry_size;
	case file_transfer_BatteryTelemetry_tag:
		return file_transfer_BatteryTelemetry_size;
	case file_transfer_AntennaTelemetry_tag:
		return file_transfer_AntennaTelemetry_size;
	case file_transfer_DosimeterData_tag:
		return file_transfer_DosimeterData_size;
	case file_transfer_ImagePacket_tag:
		return file_transfer_ImagePacket_size;
	case file_transfer_ModuleErrorReport_tag:
		return file_transfer_ModuleErrorReport_size;
	case file_transfer_ComponentErrorReport_tag:
		return file_transfer_ComponentErrorReport_size;
	case file_transfer_ErrorReportSummary_tag:
		return file_transfer_ErrorReportSummary_size;
	case file_transfer_adcs_burst_tag:
		return file_transfer_adcs_burst_size;
	case protocol_Ack_tag:
		return protocol_Ack_size;
	case protocol_Nack_tag:
		return protocol_Nack_size;
	case telecommand_BeginPass_tag:
		return telecommand_BeginPass_size;
	case telecommand_BeginFileTransfer_tag:
		return telecommand_BeginFileTransfer_size;
	case telecommand_CeaseTransmission_tag:
		return telecommand_CeaseTransmission_size;
	case telecommand_UpdateTime_tag:
		return telecommand_UpdateTime_size;
	case telecommand_Reset_tag:
		return telecommand_Reset_size;
	default:
		return 0;
	}
}


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
uint8_t messageWrap(radsat_message* rawMessage, radsat_sk_raw_message_t* wrappedMessage) {

	// ensure the input pointers are not NULL
	if (rawMessage == NULL || wrappedMessage == NULL){
		warningPrint("NULL Parameters");
		return 0;
	}
/*
	debugPrint("file_transfer_ObcTelemetry_size         = %d\n",file_transfer_ObcTelemetry_size         );
	debugPrint("file_transfer_TransceiverTelemetry_size = %d\n",file_transfer_TransceiverTelemetry_size );
	debugPrint("file_transfer_CameraTelemetry_size      = %d\n",file_transfer_CameraTelemetry_size      );
	debugPrint("file_transfer_EpsTelemetry_size         = %d\n",file_transfer_EpsTelemetry_size         );
	debugPrint("file_transfer_BatteryTelemetry_size     = %d\n",file_transfer_BatteryTelemetry_size     );
	debugPrint("file_transfer_AntennaTelemetry_size     = %d\n",file_transfer_AntennaTelemetry_size     );
	debugPrint("file_transfer_DosimeterData_size        = %d\n",file_transfer_DosimeterData_size        );
	debugPrint("file_transfer_ImagePacket_size          = %d\n",file_transfer_ImagePacket_size          );
	debugPrint("file_transfer_ModuleErrorReport_size    = %d\n",file_transfer_ModuleErrorReport_size    );
	debugPrint("file_transfer_ComponentErrorReport_size = %d\n",file_transfer_ComponentErrorReport_size );
	debugPrint("file_transfer_ErrorReportSummary_size   = %d\n",file_transfer_ErrorReportSummary_size   );
*/
	// serialize the raw message with NanoPB Protobuf encoding
	uint8_t encodedSize = getRadsatMessageSize(rawMessage->which_service);

	// create stream object
	memcpy(wrappedMessage->body, rawMessage, encodedSize);

	if (encodedSize == 0) {
		errorPrint("Encode Error Size == 0");
		return 0;
	}

	// populate the message header
	wrappedMessage->preamble = RADSAT_SK_MESSAGE_PREAMBLE;
	wrappedMessage->size = (uint8_t) encodedSize;
	Time_getUnixEpoch(&wrappedMessage->timestamp);

	// calculate the CRC of entire message (except for preamble and crc itself)
	wrappedMessage->crc = crcFast((uint8_t*)&wrappedMessage->size,
						  (int)(wrappedMessage->size + RADSAT_SK_HEADER_SIZE - RADSAT_SK_HEADER_CRC_OFFSET));


	return wrappedMessage->size + RADSAT_SK_HEADER_SIZE;
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
uint8_t messageUnwrap(uint8_t* wrappedMessage, uint8_t size, radsat_message* rawMessage) {

	// ensure the input pointers are not NULL
	if (wrappedMessage == 0 || rawMessage == 0){
		errorPrint("Input Pointers are NULL\n");
		return 0;
	}

	radsat_sk_raw_message_t mediumRareMessage = { 0 };
	memcpy(&mediumRareMessage, wrappedMessage, size);

	// decrypt entire message
	int error = xorDecrypt((uint8_t*)&mediumRareMessage, size);
	if (error){
		errorPrint("XOR Error\n");
		return 0;
	}

	// confirm preamble
	if (mediumRareMessage.preamble != RADSAT_SK_MESSAGE_PREAMBLE){
		errorPrint("Bad Preamble\n");
		return 0;
	}

	// calculate the CRC of entire message (except for preamble and crc itself)
	crc_t localCrc = crcFast((uint8_t*)&mediumRareMessage.size, (int)(mediumRareMessage.size + RADSAT_SK_HEADER_SIZE - RADSAT_SK_HEADER_CRC_OFFSET));


	// confirm locally-calculated CRC with the one sent with the message header
	if (mediumRareMessage.crc != localCrc) {
		errorPrint("CRC failure\n");
		return 0;
	}

	// deserialize the encoded message with NanoPB Protobuf decoding
	memcpy(rawMessage, mediumRareMessage.body, mediumRareMessage.size);
	// return the size of the message itself
	return mediumRareMessage.size;
}


