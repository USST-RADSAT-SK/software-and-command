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


/***************************************************************************************************
                                  PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

/** Index of our Transceiver; the IsisTRXVU.h SSI module allows for multiple TRX instances */
#define TRANSCEIVER_INDEX	0

#define TRX_WDOG_RESET_CMD_CODE	0xCC
#define TRX_WDOG_RESET_CMD_SIZE	1


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
 * @param	slotsRemaining Remaining slots in the transmitter's buffer. Set by function. Optional; set NULL to skip.
 * @return	Error code; 0 for success, otherwise see hal/error.c
 */
int transceiverSendFrame(uint8_t* message, uint8_t messageSize, uint8_t* slotsRemaining) {

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

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

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

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
	if (error != 0)
			return error;

	return 0;
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

	// ISISpace structs to store receiver and transmitter telemetry
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
	// TODO: confirm that the MCU can accurately perform these calculations
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
	// TODO: confirm that the MCU can accurately perform these calculations
	telemetry->tx.tx_reflpwr = rawTxTelemetry.fields.tx_reflpwr * rawTxTelemetry.fields.tx_reflpwr * 0.0005887;
    telemetry->tx.tx_fwrdpwr = rawTxTelemetry.fields.tx_fwrdpwr * rawTxTelemetry.fields.tx_fwrdpwr * 0.0005887;
    telemetry->tx.bus_volt = rawTxTelemetry.fields.bus_volt * 0.00488;
	telemetry->tx.vutotal_curr = rawTxTelemetry.fields.vutotal_curr * 0.16643964;
	telemetry->tx.vutx_curr = rawTxTelemetry.fields.vutx_curr * 0.16643964;
	telemetry->tx.vurx_curr = rawTxTelemetry.fields.vurx_curr * 0.16643964;
	telemetry->tx.vupa_curr = rawTxTelemetry.fields.vupa_curr * 0.16643964;
	telemetry->tx.pa_temp = (rawTxTelemetry.fields.pa_temp * -0.07669) + 195.6037;
	telemetry->tx.board_temp = (rawTxTelemetry.fields.board_temp * -0.07669) + 195.6037;

	uptime = 0;
	error = transceiverTxUpTime(&uptime);
	telemetry->tx.uptime = uptime;

	return error;
}


int transceiverPetWatchDogs(void) {

	int error = 0;

	// create buffer to hold pet command code
	uint8_t writeData[TRX_WDOG_RESET_CMD_SIZE] = { TRX_WDOG_RESET_CMD_CODE };

	// transmit WDOG pet to receiver module
	error = i2cTransmit(TRANSCEIVER_RX_I2C_SLAVE_ADDR, writeData, sizeof(writeData));

	if (error != 0)
		return error;

	// transmit WDOG pet to transmitter module
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

