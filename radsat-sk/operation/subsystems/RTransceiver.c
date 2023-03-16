/**
 * @file RTransceiver.h
 * @date December 23, 2021
 * @author Tyrel Kostyk (tck290), Matthew Buglass (mab839) and Atharva Kulkarni (iya789)
 */

#include <RTransceiver.h>
#include <RI2c.h>
#include <satellite-subsystems/IsisTRXVU.h>
#include <hal/errors.h>
#include <string.h>
#include <RCommon.h>


/***************************************************************************************************
                                  PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

/** Index of our Transceiver; the IsisTRXVU.h SSI module allows for multiple TRX instances */
#define TRANSCEIVER_INDEX 0

#define TRX_WDOG_RESET_CMD_CODE	0xCC
#define TRX_WDOG_RESET_CMD_SIZE	1

/** For manual testing of messages */
#define N_MESSAGES 5

uint8_t msg1[15] = {0x19, 0x21, 0x2f, 0x08, 0x07, 0xa6, 0xff, 0xb7, 0x62, 0x1b, 0x05, 0x0b, 0x03, 0x09, 0x0a}; // beginPass
uint8_t msg2[13] = {0x19, 0x21, 0xd2, 0x85, 0x05, 0xb3, 0xff, 0xb7, 0x62, 0x1b, 0x03, 0x13, 0x01}; // beginFileTransfer
uint8_t msg3[13] = {0x19, 0x21, 0x10, 0x2a, 0x05, 0x76, 0xff, 0xb7, 0x62, 0x0b, 0x03, 0x0b, 0x01}; // ack
uint8_t msg4[13] = {0x19, 0x21, 0x10, 0x2a, 0x05, 0x76, 0xff, 0xb7, 0x62, 0x0b, 0x03, 0x0b, 0x01}; // ack
uint8_t msg5[13] = {0x19, 0x21, 0x10, 0x2a, 0x05, 0x76, 0xff, 0xb7, 0x62, 0x0b, 0x03, 0x0b, 0x01}; // ack

int messageLengths[] = { 15, 13, 13, 13, 13 }; // in bytes

uint8_t* messages[N_MESSAGES] = {msg1, msg2, msg3, msg4, msg5};

int curMsgIdx = 0;



/** Track whether the Transceiver has been initialized yet */
static uint8_t initialized = 0;


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static int transceiverRxUpTime(uint16_t* uptime);
static int transceiverTxUpTime(uint16_t* uptime);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initializes the Transceiver, setting the appropriate operation settings.
 *
 * @note	This does not do any initialization on the actual TRXVU; changes are local to the OBC.
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverInit(void) {

	// only allow initialization once (return without error if already initialized)
	if (initialized)
		return SUCCESS;

	// define I2C addresses for individual receiver and transmitter
	ISIStrxvuI2CAddress addresses = {
		.addressVu_rc = TRANSCEIVER_RX_I2C_SLAVE_ADDR,
		.addressVu_tc = TRANSCEIVER_TX_I2C_SLAVE_ADDR,
	};

	// define max frame sizes for uplink (RX) and downlink (TX) frames
	ISIStrxvuFrameLengths frameLengths = {
		.maxAX25frameLengthRX = TRANCEIVER_RX_MAX_FRAME_SIZE,
		.maxAX25frameLengthTX = TRANCEIVER_TX_MAX_FRAME_SIZE,
	};

	// define bitrate for uplink (RX) and downlink (TX) transmissions
	ISIStrxvuBitrate bitrate = trxvu_bitrate_9600;

	int error = IsisTrxvu_initialize(&addresses, &frameLengths, &bitrate, 1);

	// update flag if successfully initialized
	if (!error)
		initialized = 1;

	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Gets the number of frames in the Receiver's message buffer.
 *
 * @param	numberOfFrames The number of frames available in the receiver's buffer. Set by function.
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverRxFrameCount(uint16_t* numberOfFrames) {

	// ensure the pointer is valid
	if (numberOfFrames == 0)
		return E_INPUT_POINTER_NULL;

	int error = IsisTrxvu_rcGetFrameCount(TRANSCEIVER_INDEX, numberOfFrames);

	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Gets next frame from the receiver buffer.
 *
 * @param	messageBuffer A byte array to copy the message into; must accept up to 200 bytes.
 * @param	sizeOfMessage The size of the frame received (0 on failure or empty buffer). Set by function.
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverGetFrame(uint8_t* messageBuffer, uint16_t* sizeOfMessage) {

	// ensure the pointers are valid
	if (messageBuffer == 0 || sizeOfMessage == 0)
		return E_INPUT_POINTER_NULL;

	// create an RX Frame struct to receive the frame (and length) into
	ISIStrxvuRxFrame frame = {
		.rx_framedata = messageBuffer,
	};

	int error = IsisTrxvu_rcGetCommandFrame(TRANSCEIVER_INDEX, &frame);

	// provide the size of the message to the caller
	*sizeOfMessage = frame.rx_length;


	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Sends a data frame to the Transmitter's buffer.
 *
 * @param	message Buffer that is to be transmitted; must be no longer than 235 bytes long.
 * @param	messageSize The length of the outgoing message in bytes.
 * @param	slotsRemaining Remaining slots in the transmitter's buffer. Set by function. Optional; set NULL to skip.
 * @return	Error code; 0 for success, otherwise see hal/error.c
 */
int transceiverSendFrame(uint8_t* message, uint8_t messageSize, uint8_t* slotsRemaining) {

	// ensure the pointer is valid (slotsRemaining is optional)
	if (message == 0)
		return E_INPUT_POINTER_NULL;

	int error = IsisTrxvu_tcSendAX25DefClSign(TRANSCEIVER_INDEX, message, messageSize, slotsRemaining);

	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Emergency function to perform a full hardware reset of the transceiver transmitter and receiver.
 *
 * Sends a command to the transceiver to perform a full hardware reset (power cycle).
 * This is to be used only when absolutely necessary as all frames in the receiver and transmitter
 * buffer will be lost.
 *
 * @post	Transceiver is power-cycled and may take a moment to come back online.
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverPowerCycle(void) {

	// send full hard reset command
	int error = IsisTrxvu_hardReset(TRANSCEIVER_INDEX);

	// TODO: record errors (if present) to System Manager

	return error;
}

/**
 * Soft Reset the ISIS TRXVU VU_RC and VU_TC.
 *
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverSoftReset(void){

	//send full soft reset command
	int error = IsisTrxvu_softReset(TRANSCEIVER_INDEX);

	//TODO: record errors (if present) to System Manager

	return error;
}

/**
 * Gets the receiver's current telemetry.
 * 
 * @param	telemetry A rx_telemetry struct filled with receiver telemetry data. Set by function.
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverTelemetry(transceiver_telemetry* telemetry) {

	// ensure the pointer is valid
	if (telemetry == 0)
		return E_INPUT_POINTER_NULL;

	// ISISpace structs to store receiver and transmitter telemetry
	ISIStrxvuRxTelemetry rawRxTelemetry = { .fields = { 0 } };
	ISIStrxvuTxTelemetry rawTxTelemetry = { .fields = { 0 } };


	int error = IsisTrxvu_rcGetTelemetryAll(TRANSCEIVER_INDEX, &rawRxTelemetry);
	error = IsisTrxvu_tcGetTelemetryAll(TRANSCEIVER_INDEX, &rawTxTelemetry);
	error = transceiverRxUpTime(&telemetry->receiver.uptime);
	error = transceiverRxFrameCount(&telemetry->receiver.frames);
	error = transceiverTxUpTime(&telemetry->transmitter.uptime);

	/*
	// convert the raw receiver values to true telemetry values
	telemetry->rx.rx_doppler = (rawRxTelemetry.fields.rx_doppler * 13.352) - 223000;
	telemetry->rx.rx_rssi = (rawRxTelemetry.fields.rx_rssi * 0.03) - 152;
	telemetry->rx.bus_volt = rawRxTelemetry.fields.bus_volt * 0.00488;
	telemetry->rx.vutotal_curr = rawRxTelemetry.fields.vutotal_curr * 0.16643964;
	telemetry->rx.vutx_curr = rawRxTelemetry.fields.vutx_curr * 0.16643964;
	telemetry->rx.vurx_curr = rawRxTelemetry.fields.vurx_curr * 0.16643964;
	telemetry->rx.vupa_curr = rawRxTelemetry.fields.vupa_curr * 0.16643964;
	telemetry->rx.pa_temp = (rawRxTelemetry.fields.pa_temp * -0.07669) + 195.6037;
	telemetry->rx.board_temp = (rawRxTelemetry.fields.board_temp * -0.07669) + 195.6037;

	// convert the raw transmitter values to true telemetry values
	telemetry->tx.tx_reflpwr = rawTxTelemetry.fields.tx_reflpwr * rawTxTelemetry.fields.tx_reflpwr * 0.0005887;
    telemetry->tx.tx_fwrdpwr = rawTxTelemetry.fields.tx_fwrdpwr * rawTxTelemetry.fields.tx_fwrdpwr * 0.0005887;
    telemetry->tx.bus_volt = rawTxTelemetry.fields.bus_volt * 0.00488;
	telemetry->tx.vutotal_curr = rawTxTelemetry.fields.vutotal_curr * 0.16643964;
	telemetry->tx.vutx_curr = rawTxTelemetry.fields.vutx_curr * 0.16643964;
	telemetry->tx.vurx_curr = rawTxTelemetry.fields.vurx_curr * 0.16643964;
	telemetry->tx.vupa_curr = rawTxTelemetry.fields.vupa_curr * 0.16643964;
	telemetry->tx.pa_temp = (rawTxTelemetry.fields.pa_temp * -0.07669) + 195.6037;
	telemetry->tx.board_temp = (rawTxTelemetry.fields.board_temp * -0.07669) + 195.6037;
*/

	// convert the raw receiver values to true telemetry values
	telemetry->receiver.rxDoppler	= rawRxTelemetry.fields.rx_doppler;
	telemetry->receiver.rxRssi		= rawRxTelemetry.fields.rx_rssi;
	telemetry->receiver.busVoltage		= rawRxTelemetry.fields.bus_volt;
	telemetry->receiver.totalCurrent	= rawRxTelemetry.fields.vutotal_curr;
	telemetry->receiver.txCurrent		= rawRxTelemetry.fields.vutx_curr;
	telemetry->receiver.rxCurrent		= rawRxTelemetry.fields.vurx_curr;
	telemetry->receiver.powerAmplifierCurrent		= rawRxTelemetry.fields.vupa_curr;
	telemetry->receiver.powerAmplifierTemperature		= rawRxTelemetry.fields.pa_temp;
	telemetry->receiver.boardTemperature	= rawRxTelemetry.fields.board_temp;

	// convert the raw transmitter values to true telemetry values
	telemetry->transmitter.reflectedPower	= rawTxTelemetry.fields.tx_reflpwr;
    telemetry->transmitter.forwardPower		= rawTxTelemetry.fields.tx_fwrdpwr;
    telemetry->transmitter.busVoltage		= rawTxTelemetry.fields.bus_volt;
	telemetry->transmitter.totalCurrent		= rawTxTelemetry.fields.vutotal_curr;
	telemetry->transmitter.txCurrent		= rawTxTelemetry.fields.vutx_curr;
	telemetry->transmitter.rxCurrent		= rawTxTelemetry.fields.vurx_curr;
	telemetry->transmitter.powerAmplifierCurrent	= rawTxTelemetry.fields.vupa_curr;
	telemetry->transmitter.powerAmplifierTemperature	= rawTxTelemetry.fields.pa_temp;
	telemetry->transmitter.boardTemperature	= rawTxTelemetry.fields.board_temp;

	return error;
}


int transceiverResetWatchDogs(void) {

	int error = 0;

	// create buffer to hold reset command code
	uint8_t writeData[TRX_WDOG_RESET_CMD_SIZE] = { TRX_WDOG_RESET_CMD_CODE };

	// transmit WDOG reset_t to receiver module
	error = i2cTransmit(TRANSCEIVER_RX_I2C_SLAVE_ADDR, writeData, sizeof(writeData));

	if (error != 0)
		return error;

	// transmit WDOG reset_t to transmitter module
	error = i2cTransmit(TRANSCEIVER_TX_I2C_SLAVE_ADDR, writeData, sizeof(writeData));

	if (error != 0)
		return error;

	return error;
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
****************************************************************************************************/

/**
 * Gets the receiver's current up time.
 *
 * @param	uptime Receiver's up time in seconds.
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
static int transceiverRxUpTime(uint16_t* uptime) {

	int error = IsisTrxvu_rcGetUptime(TRANSCEIVER_INDEX, (unsigned int *) uptime);

	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * @brief	enable sending of frame bytes for symbol recovery
 */
int startInterFrameFill(void){
	int error = IsisTrxvu_tcSetIdlestate(TRANSCEIVER_INDEX, trxvu_idle_state_on);
	return error;
}


/**
 * @brief	disables sending of frame bytes for symbol recovery
 */
int stopInterFrameFill(void){
	int error = IsisTrxvu_tcSetIdlestate(TRANSCEIVER_INDEX, trxvu_idle_state_off);
	return error;
}


/**
 * @brief	reset the TRXVU beacon timer.
 */
void resetBeacon(void) {
	IsisTrxvu_tcClearBeacon(TRANSCEIVER_INDEX);
}


/**
 * Gets the transmitter's current up time.
 * 
 * @param	uptime transmitter's up time in seconds.
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
static int transceiverTxUpTime(uint16_t* uptime) {

	int error = IsisTrxvu_tcGetUptime(TRANSCEIVER_INDEX, (unsigned int *) uptime);

	// TODO: record errors (if present) to System Manager

	return error;
}

