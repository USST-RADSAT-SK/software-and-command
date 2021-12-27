/**
 * @file RTransceiver.h
 * @date December 23, 2021
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#include <RTransceiver.h>
#include <RI2c.h>
#include <string.h>
#include <float.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define TRX_RX_CMD_WRITE_SIZE	1
#define TRX_TX_CMD_WRITE_SIZE	1

typedef struct _rx_rx_command_t {
	uint8_t readSize;
	uint8_t code;
	uint8_t destination;
} rx_command_t;

static const rx_command_t getNumberOfFramesInRxBuffer = {
		.readSize = 1,
		.code = 0x21,
		.destination = TRANSCEIVER_RX_I2C_SLAVE_ADDR,
};

static const rx_command_t receiveOldestFrameInRxBuffer = {
		.readSize = TRX_RECEIVER_FRAME_PREAMBLE_SIZE,	// first read obtains size (and other info) of the frame
		.code = 0x22,
		.destination = TRANSCEIVER_RX_I2C_SLAVE_ADDR,
};

static const rx_command_t deleteOldestFrameInRxBuffer = {
		.readSize = 1,
		.code = 0x24,
		.destination = TRANSCEIVER_RX_I2C_SLAVE_ADDR,
};

static const rx_command_t powerCycle = {
		.readSize = 0,
		.code = 0xAB,
		.destination = TRANSCEIVER_RX_I2C_SLAVE_ADDR,
};

static const rx_command_t rxTelemetry = {
		.readSize = 18,
		.code = 0x1A,
		.destination = TRANSCEIVER_RX_I2C_SLAVE_ADDR,
};

static const rx_command_t rxUpTime = {
		.readSize = 4,
		.code = 0x40,
		.destination = TRANSCEIVER_RX_I2C_SLAVE_ADDR,
};

static const rx_command_t sendFrame = {
		.readSize = 1,
		.code = 0x10,
		.destination = TRANSCEIVER_TX_I2C_SLAVE_ADDR,
};

static const rx_command_t txTelemetry = {
		.readSize = 18,
		.code = 0x25,
		.destination = TRANSCEIVER_TX_I2C_SLAVE_ADDR,
};

static const rx_command_t txTelemetryLastTransmit = {
		.readSize = 18,
		.code = 0x26,
		.destination = TRANSCEIVER_TX_I2C_SLAVE_ADDR,
};

static const rx_command_t txUpTime = {
		.readSize = 4,
		.code = 0x40,
		.destination = TRANSCEIVER_TX_I2C_SLAVE_ADDR,
};

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS                                       
***************************************************************************************************/
void convertRxTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);

void convertTxTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);

void convertSharedTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/** @brief Gets the number of frames in the Receiver's message buffer.
 *
 *	@param	void 
 *	@pre	Receiver has N messages in it's buffer.
 * 	@post	Receiver has N messages in it's buffer.
 * 	@return	N, the number of frames in the Receiver's message buffer.
 */
uint8_t transceiverFrameCount(void) {

	const rx_command_t cmd = getNumberOfFramesInRxBuffer;

	uint8_t writeData[TRX_RX_CMD_WRITE_SIZE] = { cmd.code };
	uint8_t readData[TRX_RX_CMD_WRITE_SIZE] = {0};

	int error = i2cTalk(cmd.destination, TRX_RX_CMD_WRITE_SIZE, cmd.readSize, writeData, readData, 0);
	if (error != 0)
		return 0;

	uint8_t numberOfFrames = readData[1];

	return numberOfFrames;
}

/** @brief Gets next frame from the receiver buffer and the size of the message.
 *
 *	Retreives information from the receiver message buffer, places it 
 *  in the provided buffer and returns the message size. 
 *
 *	@param	msgBuffer A 200 byte array to copy the message into. 
 *	@pre	Receiver has N messages in it's buffer.
 * 	@post	Receiver has N-1 messages in it's buffer.
 * 	@return	The size of the message in bytes.
 */
uint8_t transceiverGetFrame(uint8_t* msgBuffer) {

	// ensure the pointer is pointing to a valid buffer
	if (msgBuffer == 0)
		return 0;

	// first read will obtain the size (and other info) of the frame to be read
	const rx_command_t receiveCmd = receiveOldestFrameInRxBuffer;

	uint8_t writeData[TRX_RX_CMD_WRITE_SIZE] = { receiveCmd.code };
	uint8_t readData[receiveCmd.readSize];

	int error = i2cTalk(receiveCmd.destination, TRX_RX_CMD_WRITE_SIZE, receiveCmd.readSize, writeData, readData, 0);
	if (error != 0)
		return 0;

	// get full size of message
	uint8_t sizeOfMessage = readData[0];
	// add it to total frame size to be received (will have to re-receive the first bytes anyway)
	uint16_t sizeOfFrame = receiveCmd.readSize + sizeOfMessage;

	// can't request something larger than the max rx frame size
	if (sizeOfFrame > TRX_RECEIVER_MAX_FRAME_SIZE)
		return 0;

	uint8_t readFullFrame[sizeOfFrame];
	error = i2cTalk(receiveCmd.destination, TRX_RX_CMD_WRITE_SIZE, sizeOfFrame, writeData, readFullFrame, 0);
	if (error != 0)
		return 0;

	// finally, delete the now-received frame
	rx_command_t deleteCmd = deleteOldestFrameInRxBuffer;
	uint8_t deleteCmdBuffer[TRX_RX_CMD_WRITE_SIZE] = { deleteCmd.code };

	error = i2cTransmit(receiveCmd.destination, deleteCmdBuffer, TRX_RX_CMD_WRITE_SIZE);
	if (error != 0) {
		// try again
		error = i2cTransmit(receiveCmd.destination, deleteCmdBuffer, TRX_RX_CMD_WRITE_SIZE);
		if (error != 0)
			return 0;	// TODO: more robust solution (reset Trx...)? Huge problem if we can't delete old frames
	}

	// copy the message into the given buffer (not including the preamble info)
	memcpy(msgBuffer, &readFullFrame[TRX_RECEIVER_FRAME_PREAMBLE_SIZE], sizeOfMessage);

	return sizeOfMessage;
}

/** @brief Emergency function to perform a hardware reset of the transceiver.
 *
 *	Sends a command to the transceiver to perform a full hardware reset (power cycle). 
 *  This is to be used only when a catastrophic error has occured as all frames in 
 *  the receiver buffer will be lost.
 *
 *	@param	void
 *	@pre	Transiever is powered on and receiving commands.
 * 	@post	Transceiver is reset and will need to re-connect.
 * 	@return	void
 */
void transceiverPowerCycle(void) {
	const rx_command_t cmd = powerCycle;

	uint8_t writeData[TRX_RX_CMD_WRITE_SIZE] = { cmd.code };
	uint8_t readData[TRX_RX_CMD_WRITE_SIZE] = {0};

	i2cTalk(cmd.destination, TRX_RX_CMD_WRITE_SIZE, cmd.readSize, writeData, readData, 0);
}

/**
 * Sends a data frame to the Transmitter's buffer
 *
 * @param message buffer that is a maximum of 235 bytes 
 * 		long to be transmitted
 * @param message_size the length of the message in bytes.
 * @return The number of slots left in the transmission buffer.
 * 		255 (0xff) if the frame was not added.
 */
uint8_t transceiverSendFrame(uint8_t* message, uint8_t message_size){
	const rx_command_t cmd = sendFrame;

	uint8_t* code[1] = cmd.code;
	uint8_t i2c_msg_size = TRX_TX_CMD_WRITE_SIZE + message_size;

	// the message over i2c is the command in byte 0 followed by the message
	uint8_t writeData[i2c_msg_size] = { code | message };
	uint8_t readData[TRX_TX_CMD_WRITE_SIZE] = {0};

	uint8_t slots_remaining = i2cTalk(cmd.destination, i2c_msg_size, cmd.readSize, writeData, readData, 0);

	return slots_remaining;
}

/**
 * Gets the receiver's current telemetry
 * 
 * @param telemetryBuffer an 18 byte buffer to store the receiver's telemetry info
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
void transceiverRxTelemetry(uint8_t* telemetryBuffer){}

/**
 * Gets the receiver's current up time
 * 
 * @return Receiver's up time in seconds
 */
uint32_t transceiverRxUpTime(void){}

/**
 * Gets the transmitter's telemetry during its last transmission.
 * 
 * @param telemetryBuffer an buffer to store 9 floats (36 bytes) to store the transmitter's telemetry info
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
void transcevierTxTelemetry(uint8_t* telemetryBuffer){}

/**
 * Gets the transmitter's telemetry during its last transmission.
 * 
 * @param telemetryBuffer an buffer to store 9 floats (36 bytes) to store the transmitter's telemetry info
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
void transcevierTxTelemetryLastTransmit(uint8_t* telemetryBuffer){}

/**
 * Gets the transmitter's current up time
 * 
 * @return Transmitter's up time in seconds
 */
uint32_t transceiverTxUpTime(void){}


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
void convertRxTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);

/**
 * Converts the telemetry representation from the transmitter into 
 * the true data values.
 *
 * @param rawTelemetryBuffer buffer that contains the telemetry from the transmitter.
 * @param trueTelementryBuffer a 36 byte buffer that will hold the true telemetry values.
 * @return void
 */
void convertTxTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);

/**
 * Converts the telemetry representation of the last 7 telemetry values
 *
 * @param rawTelemetryBuffer buffer that contains the last 7 telemetry from the 
 * 		receiver or transmitter.
 * @param trueTelementryBuffer a 24 byte buffer that will hold the true telemetry values.
 * @return void
 */
void convertSharedTelemetry(uint16_t* rawTelemetryBuffer, float32_t* trueTelemetryBuffer);