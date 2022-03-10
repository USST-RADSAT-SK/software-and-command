/**
 * @file RMessageOutput.c
 * @date March 10, 2022
 * @author Matthew Buglass (mab839)
 */


#include <stdint.h>
#include <RMessageOutput.h>
#include <RFileTransferService.h>
#include <RProtocolService.h>


/***************************************************************************************************
                                             DEFINITIONS
***************************************************************************************************/



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/*
 * Prints all prepared messages in the message suite
 */
void printAll(void) {
	printAck();
	printNack();
	printFileTransferMessages();
}

/*
 * Prints an ACK message
 */
void printAck(void) {
	uint8_t wrappedMessage[ProtocolMessage_size];
	protocolGenerate(ProtocolMessage_ack_tag, wrappedMessage);

	printf(wrappedMessage);
}


/*
 * Prints a NACK message
 */
void printNack(void) {
	uint8_t wrappedMessage[ProtocolMessage_size];
	protocolGenerate(ProtocolMessage_nack_tag, wrappedMessage);

	printf(wrappedMessage);
}


/*
 * Prints a series of FileTransferMessages
 *
 * The Messages don't have meaningful data, other than a unique value
 * is used for each struct member so that data can be validated through
 * the serialization process.
 */
void printFileTransferMessages(void) {

	// Dosimeter data Message
	DosimeterData mockDosimeterData;

	mockDosimeterData.boardOne.voltageChannelZero = 1.0;
	mockDosimeterData.boardOne.voltageChannelOne = 1.1;
	mockDosimeterData.boardOne.voltageChannelTwo = 1.2;
	mockDosimeterData.boardOne.voltageChannelThree = 1.3;
	mockDosimeterData.boardOne.voltageChannelFour = 1.4;
	mockDosimeterData.boardOne.voltageChannelFive = 1.5;
	mockDosimeterData.boardOne.voltageChannelSix = 1.6;
	mockDosimeterData.boardOne.voltageChannelSeven = 1.7;

	mockDosimeterData.boardTwo.voltageChannelZero = 2.0;
	mockDosimeterData.boardTwo.voltageChannelOne = 2.1;
	mockDosimeterData.boardTwo.voltageChannelTwo = 2.2;
	mockDosimeterData.boardTwo.voltageChannelThree = 2.3;
	mockDosimeterData.boardTwo.voltageChannelFour = 2.4;
	mockDosimeterData.boardTwo.voltageChannelFive = 2.5;
	mockDosimeterData.boardTwo.voltageChannelSix = 2.6;
	mockDosimeterData.boardTwo.voltageChannelSeven = 2.7;

	int dosimiterSize = fileTransferAddMessage(&mockDosimeterData,
													sizeof(mockDosimeterData),
													FileTransferMessage_dosimeterData_tag);

	if (dosimiterSize != 0) {

	}

	// OBC Telemetry
	ObcTelemetry mockObcTelemetry;

	mockObcTelemetry.mode = 0;
	mockObcTelemetry.uptime = 1;
	mockObcTelemetry.rtcTime = 2;
	mockObcTelemetry.rtcTemperature = 3;

	int obcTelemetrySize = fileTransferAddMessage(&mockObcTelemetry,
													sizeof(mockObcTelemetry),
													FileTransferMessage_obcTelemetry_tag);


	// Transceiver Telemetry
	TransceiverTelemetry mockTransceiverTelemetry;

	mockTransceiverTelemetry.receiver.rx_doppler = 1.0;
	mockTransceiverTelemetry.receiver.rx_rssi = 1.1;
	mockTransceiverTelemetry.receiver.bus_volt = 1.2;
	mockTransceiverTelemetry.receiver.vutotal_curr = 1.3;
	mockTransceiverTelemetry.receiver.vutx_curr = 1.4;
	mockTransceiverTelemetry.receiver.vurx_curr = 1.5;
	mockTransceiverTelemetry.receiver.vupa_curr = 1.6;
	mockTransceiverTelemetry.receiver.pa_temp = 1.7;
	mockTransceiverTelemetry.receiver.board_temp = 1.8;
	mockTransceiverTelemetry.receiver.uptime = 2;
	mockTransceiverTelemetry.receiver.frames = 3;


	mockTransceiverTelemetry.transmitter.tx_reflpwr = 4.0;
	mockTransceiverTelemetry.transmitter.tx_fwrdpwr = 4.1;
	mockTransceiverTelemetry.transmitter.bus_volt = 4.2;
	mockTransceiverTelemetry.transmitter.vutotal_curr = 4.3;
	mockTransceiverTelemetry.transmitter.vutx_curr = 4.4;
	mockTransceiverTelemetry.transmitter.vurx_curr = 4.5;
	mockTransceiverTelemetry.transmitter.vupa_curr = 4.6;
	mockTransceiverTelemetry.transmitter.pa_temp = 4.7;
	mockTransceiverTelemetry.transmitter.board_temp = 4.8;
	mockTransceiverTelemetry.transmitter.uptime = 5;

	int transceiverTelemetrySize = fileTransferAddMessage(&mockTransceiverTelemetry,
															sizeof(mockTransceiverTelemetry),
															FileTransferMessage_transceiverTelemetry_tag);


}
