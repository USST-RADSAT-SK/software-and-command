/**
 * @file RMessageOutput.c
 * @date March 10, 2022
 * @author Matthew Buglass (mab839)
 */


#include <stdint.h>
#include <RMessageOutput.h>
#include <RFileTransferService.h>
#include <RProtocolService.h>
#include <RMessage.h>
#include <RUart.h>
#include <RRadsat.pb.h>


/***************************************************************************************************
                                             DEFINITIONS
***************************************************************************************************/



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/*
 * Prints all prepared messages in the message suite
 */
void sendAll(void) {
	sendAck();
	sendNack();
	sendFileTransferMessages();
}

/*
 * Prints an ACK message
 */
void printAck(void) {
	uint8_t wrappedMessage[RADSAT_SK_MAX_MESSAGE_SIZE] = { 0 };
	uint8_t size = protocolGenerate(ProtocolMessage_ack_tag, wrappedMessage);

	uartTransmit(UART_DEBUG_BUS, wrappedMessage, size);
}


/*
 * Prints a NACK message
 */
void sendNack(void) {
	uint8_t wrappedMessage[RADSAT_SK_MAX_MESSAGE_SIZE] = { 0 };
	uint8_t size = protocolGenerate(ProtocolMessage_nack_tag, wrappedMessage);

	uartTransmit(UART_DEBUG_BUS, wrappedMessage, size);
}


/*
 * Prints a series of FileTransferMessages
 *
 * The Messages don't have meaningful data, other than a unique value
 * is used for each struct member so that data can be validated through
 * the serialization process.
 */
void sendFileTransferMessages(void) {

	// Dosimeter data Message
	RadsatMessage mockDosimeterData = { 0 };
	mockDosimeterData.which_service = RadsatMessage_service_fileTransferMessage_MSGTYPE;

	mockDosimeterData.fileTransferMessage.dosimeterData.boardOne.voltageChannelZero = 1.0;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardOne.voltageChannelOne = 1.1;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardOne.voltageChannelTwo = 1.2;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardOne.voltageChannelThree = 1.3;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardOne.voltageChannelFour = 1.4;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardOne.voltageChannelFive = 1.5;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardOne.voltageChannelSix = 1.6;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardOne.voltageChannelSeven = 1.7;

	mockDosimeterData.fileTransferMessage.dosimeterData.boardTwo.voltageChannelZero = 2.0;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardTwo.voltageChannelOne = 2.1;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardTwo.voltageChannelTwo = 2.2;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardTwo.voltageChannelThree = 2.3;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardTwo.voltageChannelFour = 2.4;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardTwo.voltageChannelFive = 2.5;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardTwo.voltageChannelSix = 2.6;
	mockDosimeterData.fileTransferMessage.dosimeterData.boardTwo.voltageChannelSeven = 2.7;

	uint8_t wrappedMockDosimeterData[RADSAT_SK_MAX_MESSAGE_SIZE] = { 0 };
	messageWrap(&mockDosimeterData, wrappedMockDosimeterData);

	int dosimiterSize = fileTransferAddMessage(wrappedMockDosimeterData,
												RADSAT_SK_MAX_MESSAGE_SIZE,
												FileTransferMessage_dosimeterData_tag);

	if (dosimiterSize != 0) {
		uint8_t dosimeterFrame[RADSAT_SK_MAX_MESSAGE_SIZE] = { 0 };

		int dosimeterFrameSize = fileTransferNextFrame(&dosimeterFrame);

		if (dosimeterFrameSize != 0) {
			uartTransmit(UART_DEBUG_BUS, dosimeterFrame, dosimeterFrameSize);
		}
		else {
			printf("Error in getting dosimeter frame");
		}

	}
	else {
		printf("Error in creating a dosimeter message");
	}

	// OBC Telemetry
	RadsatMessage mockObcTelemetry;

	mockObcTelemetry.fileTransferMessage.obcTelemetry.mode = 0;
	mockObcTelemetry.fileTransferMessage.obcTelemetry.uptime = 1;
	mockObcTelemetry.fileTransferMessage.obcTelemetry.rtcTime = 2;
	mockObcTelemetry.fileTransferMessage.obcTelemetry.rtcTemperature = 3;

	uint8_t wrappedMockObcTelemetry[RADSAT_SK_MAX_MESSAGE_SIZE] = { 0 };
	messageWrap(&mockObcTelemetry, wrappedMockObcTelemetry);

	int obcTelemetrySize = fileTransferAddMessage(wrappedMockObcTelemetry,
													RADSAT_SK_MAX_MESSAGE_SIZE,
													FileTransferMessage_dosimeterData_tag);

	if (obcTelemetrySize != 0) {
		uint8_t obcTelemetryFrame[RADSAT_SK_MAX_MESSAGE_SIZE] = { 0 };

		int obcTelemetryFrameSize = fileTransferNextFrame(&obcTelemetryFrame);

		if (obcTelemetryFrameSize != 0) {
			uartTransmit(UART_DEBUG_BUS, obcTelemetryFrame, obcTelemetryFrameSize);
		}
		else {
			printf("Error in getting obcTelemetry frame");
		}

	}
	else {
		printf("Error in creating a obcTelemetry message");
	}


	// Transceiver Telemetry
	RadsatMessage mockTransceiverTelemetry;

	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.rx_doppler = 1.0;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.rx_rssi = 1.1;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.bus_volt = 1.2;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.vutotal_curr = 1.3;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.vutx_curr = 1.4;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.vurx_curr = 1.5;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.vupa_curr = 1.6;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.pa_temp = 1.7;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.board_temp = 1.8;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.uptime = 2;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.receiver.frames = 3;


	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.tx_reflpwr = 4.0;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.tx_fwrdpwr = 4.1;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.bus_volt = 4.2;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.vutotal_curr = 4.3;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.vutx_curr = 4.4;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.vurx_curr = 4.5;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.vupa_curr = 4.6;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.pa_temp = 4.7;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.board_temp = 4.8;
	mockTransceiverTelemetry.fileTransferMessage.transceiverTelemetry.transmitter.uptime = 5;

	uint8_t wrappedMockTransceiverTelemetry[RADSAT_SK_MAX_MESSAGE_SIZE] = { 0 };
	messageWrap(&mockTransceiverTelemetry, wrappedMockTransceiverTelemetry);

	int transceiverTelemetrySize = fileTransferAddMessage(wrappedMockTransceiverTelemetry,
															RADSAT_SK_MAX_MESSAGE_SIZE,
															FileTransferMessage_dosimeterData_tag);

	if (transceiverTelemetrySize != 0) {
		uint8_t transceiverFrame[RADSAT_SK_MAX_MESSAGE_SIZE] = { 0 };

		int transceiverTelemetryFrameSize = fileTransferNextFrame(&transceiverFrame);

		if (transceiverTelemetryFrameSize != 0) {
			uartTransmit(UART_DEBUG_BUS, transceiverFrame, transceiverTelemetryFrameSize);
		}
		else {
			printf("Error in getting transceiverTelemetry frame");
		}

	}
	else {
		printf("Error in creating a transceiverTelemetry message");
	}


}
