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
#define TRANSCEIVER_INDEX			0

/** Track whether the Transceiver has been initialized yet */
static uint8_t initialized = 0;


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS                                       
***************************************************************************************************/

static void convertRxTelemetry(uint16_t* rawTelemetryBuffer, float* trueTelemetryBuffer);

static void convertTxTelemetry(uint16_t* rawTelemetryBuffer, float* trueTelemetryBuffer);

static void convertSharedTelemetry(uint16_t* rawTelemetryBuffer, float* trueTelemetryBuffer);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/** Initializes the Transceiver, setting the appropriate operation settings.
 *
 * 	@return Error code; 0 for success, otherwise see hal/errors.h.
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

	return error;
}


/** Gets the number of frames in the Receiver's message buffer.
 *
 * 	@param	sizeOfMessage The number of frames available in the receiver's buffer. Set by function.
 * 	@return Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverFrameCount(uint16_t* numberOfFrames) {

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

	// ensure the pointer is valid
	if (numberOfFrames == 0)
		return E_INPUT_POINTER_NULL;

	int error = IsisTrxvu_rcGetFrameCount(TRANSCEIVER_INDEX, numberOfFrames);
	return error;
}


/** Gets next frame from the receiver buffer.
 *
 *	@param	messageBuffer A byte array to copy the message into; must accept up to 200 bytes.
 * 	@param	sizeOfMessage The size of the frame received (0 on failure or empty buffer). Set by function.
 * 	@return Error code; 0 for success, otherwise see hal/errors.h.
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

	return error;
}


/**
 * Sends a data frame to the Transmitter's buffer.
 *
 * @param message Buffer that is to be transmitted; must be no longer than 235 bytes long.
 * @param messageSize The length of the outgoing message in bytes.
 * @param slotsRemaining The number of remaining slots in the transmitter's buffer. Set by function.
 * @return Error code; 0 for success, otherwise see hal/error.c
 */
int transceiverSendFrame(uint8_t* message, uint8_t messageSize, uint8_t* slotsRemaining) {

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

	// ensure the pointers are valid
	if (message == 0 || slotsRemaining == 0)
		return E_INPUT_POINTER_NULL;

	int error = IsisTrxvu_tcSendAX25DefClSign(TRANSCEIVER_INDEX, message, messageSize, slotsRemaining);
	return error;
}


/** Emergency function to perform a full hardware reset of the transceiver transmitter and receiver.
 *
 *	Sends a command to the transceiver to perform a full hardware reset (power cycle). 
 *  This is to be used only when absolutely necessary as all frames in the receiver and transmitter
 *  buffer will be lost.
 *
 * 	@post	Transceiver is power-cycled and may take a moment to come back online.
 * 	@return Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverPowerCycle(void) {

	// transceiver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

	// send full hard reset command
	int error = IsisTrxvu_hardReset(TRANSCEIVER_INDEX);
	return error;
}


/**
 * Gets the receiver's current telemetry
 * 
 * @param telemetryBuffer a buffer to store 9 floats (36 bytes) to store the receiver's telemetry info
 * 		[00] Instantanious Doppler offset of the signal at the receiver port (Hz).
 * 		[01] Instantaneous signal strength of the signal at the receiver (dBm).
 * 		[02] Value of the power bus voltage (V).
 * 		[03] Value of the total supply current (mA).
 * 		[04] Value of the transmitter current (mA).
 * 		[05] Value of the receiver current (mA).
 * 		[06] Value of the power amplifier current (mA).
 * 		[07] Value of the power amplifier temperature (C).
 * 		[08] Value of the local oscillator temperature (C).
 * @return void
 */
void transceiverRxTelemetry(float32_t* telemetryBuffer){}

/**
 * Gets the receiver's current up time
 * 
 * @return Receiver's up time in seconds
 */
uint32_t transceiverRxUpTime(void){}

/**
 * Gets the transmitter's telemetry during its last transmission.
 * 
 * @param telemetryBuffer a buffer to store 9 floats (36 bytes) to store the transmitter's telemetry info
 * 		[00] Value of the instantaneous RF reflected power at the transmitter port (mW).
 * 			Only valid during transmission.
 * 		[01] Value of the instantaneous RF forward power at the transmitter port (mW).
 * 			Only valid during transmission.
 * 		[02] Value of the power bus voltage (V).
 * 		[03] Value of the total supply current (mA).
 * 		[04] Value of the transmitter current (mA).
 * 		[05] Value of the receiver current (mA).
 * 		[06] Value of the power amplifier current (mA).
 * 		[07] Value of the power amplifier temperature (C).
 * 		[08] Value of the local oscillator temperature (C).
 * @return void
 */
void transcevierTxTelemetry(float32_t* telemetryBuffer){}

/**
 * Gets the transmitter's telemetry during its last transmission.
 * 
 * @param telemetryBuffer a buffer to store 9 floats (36 bytes) to store the transmitter's telemetry info
 * 		[00] Value of the instantaneous RF reflected power at the transmitter port (mW).
 * 			Only valid during transmission.
 * 		[01] Value of the instantaneous RF forward power at the transmitter port (mW).
 * 			Only valid during transmission.
 * 		[02] Value of the power bus voltage (V).
 * 		[03] Value of the total supply current (mA).
 * 		[04] Value of the transmitter current (mA).
 * 		[05] Value of the receiver current (mA).
 * 		[06] Value of the power amplifier current (mA).
 * 		[07] Value of the power amplifier temperature (C).
 * 		[08] Value of the local oscillator temperature (C).
 * @return void
 */
void transcevierTxTelemetryLastTransmit(float32_t* telemetryBuffer){}

/**
 * Gets the transmitter's current up time
 * 
 * @return Transmitter's up time in seconds
 */
uint32_t transceiverTxUpTime(void) {

}


/***************************************************************************************************
                                          PRIVATE FUNCTIONS                                          
***************************************************************************************************/
/**
 * Converts the telemetry representation from the receiver into 
 * the true data values.
 *
 * @param rawTelemetryBuffer buffer that contains the telemetry from the receiver.
 * @param trueTelementryBuffer a 36 byte buffer that will hold the true telemetry values.
 * @return void
 */
static void convertRxTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);

/**
 * Converts the telemetry representation from the transmitter into 
 * the true data values.
 *
 * @param rawTelemetryBuffer buffer that contains the telemetry from the transmitter.
 * @param trueTelementryBuffer a 36 byte buffer that will hold the true telemetry values.
 * @return void
 */
static void convertTxTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);

/**
 * Converts the telemetry representation of the last 7 telemetry values
 *
 * @param rawTelemetryBuffer buffer that contains the last 7 telemetry from the 
 * 		receiver or transmitter.
 * @param trueTelementryBuffer a 24 byte buffer that will hold the true telemetry values.
 * @return void
 */
static void convertSharedTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);

