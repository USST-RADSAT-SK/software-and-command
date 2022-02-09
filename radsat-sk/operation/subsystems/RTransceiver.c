/**
 * @file RTransceiver.h
 * @date December 23, 2021
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#include <RTransceiver.h>
#include <satellite-subsystems/IsisTRXVU.h>
#include <hal/errors.h>
#include <string.h>


/***************************************************************************************************
                                  PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

/** Index of our Transceiver; the IsisTRXVU.h SSI module allows for multiple TRX instances */
#define TRANSCEIVER_INDEX	0

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
		return 0;

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

	int error = IsisTrxvu_initialize(&addresses, &frameLengths, &bitrate, TRANSCEIVER_INDEX);

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

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

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

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

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
 * @param	slotsRemaining The number of remaining slots in the transmitter's buffer. Set by function.
 * @return	Error code; 0 for success, otherwise see hal/error.c
 */
int transceiverSendFrame(uint8_t* message, uint8_t messageSize, uint8_t* slotsRemaining) {

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

	// ensure the pointers are valid
	if (message == 0 || slotsRemaining == 0)
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

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

	// send full hard reset command
	int error = IsisTrxvu_hardReset(TRANSCEIVER_INDEX);

	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Gets the receiver's current telemetry.
 * 
 * @param	telemetry A rx_telemetry struct filled with receiver telemetry data. Set by function.
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverTelemetry(transceiver_telemetry_t* telemetry) {

	// ensure the pointer is valid
	if (telemetry == 0)
		return E_INPUT_POINTER_NULL;

	ISIStrxvuRxTelemetry rawRxTelemetry = { .fields = { 0 } };
	ISIStrxvuTxTelemetry rawTxTelemetry = { .fields = { 0 } };

	int error = IsisTrxvu_rcGetTelemetryAll(TRANSCEIVER_INDEX, &rawRxTelemetry);

	// TODO: record errors (if present) to System Manager
	if (error)
		return error;

	error = IsisTrxvu_tcGetTelemetryAll(TRANSCEIVER_INDEX, &rawTxTelemetry);

	// TODO: record errors (if present) to System Manager
	if (error)
		return error;

	uint16_t uptime = 0;
	error = transceiverRxUpTime(&uptime);
	telemetry->rx.uptime = uptime;

	if (error)
		return error;

	uint16_t rxFrames = 0;
	error = transceiverRxFrameCount(&rxFrames);
	telemetry->rx.frames = rxFrames;

	if (error)
		return error;

	// convert the raw receiver values to true telemetry values
	// TODO: should this be done here? Or by Ground Station?
	telemetry->rx.rx_doppler = rawRxTelemetry.fields.rx_doppler * (rawRxTelemetry.fields.rx_doppler * 0.00005887);
	telemetry->rx.rx_rssi = rawRxTelemetry.fields.rx_rssi * (rawRxTelemetry.fields.rx_rssi * 0.00005887);
	telemetry->rx.bus_volt = rawRxTelemetry.fields.bus_volt * 0.00488;
	telemetry->rx.vutotal_curr = rawRxTelemetry.fields.vutotal_curr * 0.16643964;
	telemetry->rx.vutx_curr = rawRxTelemetry.fields.vutx_curr * 0.16643964;
	telemetry->rx.vurx_curr = rawRxTelemetry.fields.vurx_curr * 0.16643964;
	telemetry->rx.vupa_curr = rawRxTelemetry.fields.vupa_curr * 0.16643964;
	telemetry->rx.pa_temp = rawRxTelemetry.fields.pa_temp * -0.07669 + 195.6037;
	telemetry->rx.board_temp = rawRxTelemetry.fields.board_temp * -0.07669 + 195.6037;

	// convert the raw transmitter values to true telemetry values
	// TODO: should this be done here? Or by Ground Station?
	telemetry->tx.tx_reflpwr = rawTxTelemetry.fields.tx_reflpwr * 13.352 - 22300;
    telemetry->tx.tx_fwrdpwr = rawTxTelemetry.fields.tx_fwrdpwr * 0.03 - 152;
    telemetry->tx.bus_volt = rawTxTelemetry.fields.bus_volt * 0.00488;
	telemetry->tx.vutotal_curr = rawTxTelemetry.fields.vutotal_curr * 0.16643964;
	telemetry->tx.vutx_curr = rawTxTelemetry.fields.vutx_curr * 0.16643964;
	telemetry->tx.vurx_curr = rawTxTelemetry.fields.vurx_curr * 0.16643964;
	telemetry->tx.vupa_curr = rawTxTelemetry.fields.vupa_curr * 0.16643964;
	telemetry->tx.pa_temp = rawTxTelemetry.fields.pa_temp * -0.07669 + 195.6037;
	telemetry->tx.board_temp = rawTxTelemetry.fields.board_temp * -0.07669 + 195.6037;

	uptime = 0;
	error = transceiverTxUpTime(&uptime);
	telemetry->tx.uptime = uptime;

	return error;
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

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

