/**
 * @file RTransceiver.h
 * @date December 23, 2021
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#include <RTransceiver.h>
#include <RI2c.h>
#include <string.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define TRX_RX_CMD_WRITE_SIZE	1

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


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/** @brief Gets the number of frames in the Receiver's message buffer.
 *
 *	Gets the number of frames in the Receiver's message buffer.
 *
 *	@header	"software-and-command/radsat-sk/operation/subsystems/RTransceiver.h"
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

/** @brief Gets next frame from the receiver buffer and returns the 
 *  size of the message.
 *
 *	Retreives information from the receiver message buffer, places it 
 *  in the provided buffer and returns the message size. 
 *
 *	@header	"software-and-command/radsat-sk/operation/subsystems/RTransceiver.h"
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
 *	@header	"software-and-command/radsat-sk/operation/subsystems/RTransceiver.h"
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


