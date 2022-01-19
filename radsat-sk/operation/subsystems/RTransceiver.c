/**
 * @file RTransceiver.h
 * @date December 23, 2021
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#include <RTransceiver.h>
#include <RI2c.h>
#include <satellite-subsystems/IsisTRXVU.h>
#include <hal/errors.h>
#include <string.h>


/***************************************************************************************************
                                  PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

///** Size of all transceiver commands (in bytes) */
//#define TRANSCEIVER_CMD_WRITE_SIZE	1

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
 * 	@return	The number of frames in the Receiver's message buffer (0 on failure or empty buffer).
 */
uint8_t transceiverFrameCount(void) {

	// transceiver must be initialized first
	if (!initialized)
		return 0;

	uint8_t numberOfFrames = 0;
	int error = IsisTrxvu_rcGetFrameCount(TRANSCEIVER_INDEX, &numberOfFrames);
	if (error)
		return 0;

	return numberOfFrames;
}


/** Gets next frame from the receiver buffer and the size of the message.
 *
 *	Retreives information from the receiver message buffer, places it 
 *  in the provided buffer and returns the message size. 
 *
 *	@param	msgBuffer A 200 byte array to copy the message into. 
 *	@pre	Receiver has N messages in it's buffer.
 * 	@post	Receiver has N-1 messages in it's buffer.
 * 	@return	The size of the message in bytes (0 on failure).
 */
uint8_t transceiverGetFrame(uint8_t* messageBuffer) {

	// transceiver must be initialized first
	if (!initialized)
		return 0;

	// ensure the pointer is pointing to a valid buffer
	if (messageBuffer == 0)
		return 0;

	// create an RX Frame struct to receive the frame (and length) into
	ISIStrxvuRxFrame frame = {
		.rx_framedata = messageBuffer,
	};

	int error = IsisTrxvu_rcGetCommandFrame(TRANSCEIVER_INDEX, &frame);
	if (error)
		return 0;

	uint8_t sizeOfMessage = frame.rx_length;

	return sizeOfMessage;
}


/**
 * Sends a data frame to the Transmitter's buffer.
 *
 * @param message buffer that is to be transmitted; must be no longer than 235 bytes long.
 * @param messageSize the length of the message in bytes.
 * @return The number of slots left for incoming frames in the transmission buffer.
 * 		   Will return 0xFF if the frame was not added.
 */
uint8_t transceiverSendFrame(uint8_t* message, uint8_t messageSize) {
//	const trx_command_t cmd = sendFrame;
//
//	uint8_t* code[TRANSCEIVER_CMD_WRITE_SIZE] = cmd.code;
//	uint8_t i2cMsgSize = TRANSCEIVER_CMD_WRITE_SIZE + messageSize;
//
//	// the message over i2c is the command in byte 0 followed by the message
//	uint8_t writeData[i2cMsgSize] = { 0 };
//	// copy in the command code into the first byte of the command to the transceiver
//	memcpy(writeData, code, TRANSCEIVER_CMD_WRITE_SIZE);
//	// copy in the rest of the message to be downlinked by the transceiver
//	memcpy(&writeData[TRANSCEIVER_CMD_WRITE_SIZE], message, messageSize);
//
//	uint8_t readData[TRANSCEIVER_CMD_WRITE_SIZE] = {0};
//
//	uint8_t slotsRemaining = i2cTalk(cmd.destination, i2cMsgSize, cmd.readSize, writeData, readData, 0);

	uint8_t slotsRemaining = 0;
	int error = IsisTrxvu_tcSendAX25DefClSign(TRANSCEIVER_INDEX, message, messageSize, &slotsRemaining);
	if (error)
		return 0xFF;

	return slotsRemaining;
}


/** Emergency function to perform a full hardware reset of the transceiver transmitter and receiver.
 *
 *	Sends a command to the transceiver to perform a full hardware reset (power cycle). 
 *  This is to be used only when absolutely necessary as all frames in the receiver and transmitter
 *  buffer will be lost.
 *
 * 	@post	Transceiver is power-cycled and will need to be re-initialized.
 * 	@return Error code; 0 for success, otherwise see hal/errors.h.
 */
int transceiverPowerCycle(void) {

	// transceiver must be initialized first
	if (!initialized)
		return 0;

	// send full hard reset command
	int error = IsisTrxvu_hardReset(TRANSCEIVER_INDEX);
	if (error)
		return error;

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

