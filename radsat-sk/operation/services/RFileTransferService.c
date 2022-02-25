/**
 * @file RFileTransferService.c
 * @date February 25, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RFileTransferService.h>
#include <RTransceiver.h>
#include <RMessage.h>
#include <hal/errors.h>
#include <string.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

#define MAX_FRAME_COUNT 200

#define ERROR_CURSOR			(-1)
#define ERROR_MESSAGE_WRAPPING	(-2)

typedef struct _frame_t {
	uint8_t data[TRANCEIVER_TX_MAX_FRAME_SIZE];
	uint8_t size;
} frame_t;

static frame_t frames[MAX_FRAME_COUNT] = {0};

static uint8_t frameWriteCursor, frameReadCursor = 0;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Increment the internal FIFO and provide the latest frame.
 *
 * @param frame Pointer to a buffer that the frame will be placed into. Set by function.
 * @return The size of the frame placed into the buffer; 0 on error.
 */
uint8_t fileTransferNextFrame(uint8_t* frame) {

	// ensure that there is a valid frame ahead
	if (frames[frameReadCursor + 1].size == 0)
		return 0;

	// invalidate the current (now previous frame)
	memset(&frames[frameReadCursor], 0, sizeof(frames[frameReadCursor]));

	// increment the read cursor
	frameReadCursor++;

	// transfer the frame into the provided buffer
	memcpy(frame, frames[frameReadCursor].data, frames[frameReadCursor].size);

	return frames[frameReadCursor].size;
}


/**
 * Provide the current frame.
 *
 * @param frame Pointer to a buffer that the frame will be placed into. Set by function.
 * @return The size of the frame placed into the buffer; 0 on error.
 */
uint8_t fileTransferCurrentFrame(uint8_t* frame) {

	// only provide a frame if the cursor is pointing at a valid frame
	if (frames[frameReadCursor].size > 0)
		memcpy(frame, frames[frameReadCursor].data, frames[frameReadCursor].size);

	// return the size of the frame
	return frames[frameReadCursor].size;
}

/**
 * Prepare a message for downlink and add it to the internal FIFO.
 *
 * @param message Pointer to the raw Protobuf message to be prepared.
 * @param size The size (in bytes) of the message.
 * @param messageTag The Protobuf tag of the message.
 * @return 0 on success, -1 on internal cursor error, -2 on message wrapping error, otherwise see hal/errors.h.
 */
int fileTransferAddMessage(const void* message, uint8_t size, uint16_t messageTag) {

	// ensure input pointer is valid
	if (message == 0)
		return E_INPUT_POINTER_NULL;

	// ensure size parameter is valid
	if (size == 0 || size > PROTO_MAX_ENCODED_SIZE)
		return E_PARAM_OUTOFBOUNDS;

	// ensure we are not about to overwrite frames that have not been read
	if (frameWriteCursor == frameReadCursor)
		return ERROR_CURSOR;

	// create new RADSAT-SK message to populate
	RadsatMessage newMessage = { 0 };
	newMessage.which_service = RadsatMessage_fileTransferMessage_tag;
	newMessage.fileTransferMessage.which_message = messageTag;

	// internal message data will go immediately after the "which message" property of the struct
	// TODO: confirm that this works
	void* newMessageAddr = &(newMessage.fileTransferMessage.which_message) + sizeof(newMessage.fileTransferMessage.which_message);
	memcpy(newMessageAddr, message, size);

	// wrap new message
	frames[frameWriteCursor].size = messageWrap(&newMessage, frames[frameWriteCursor].data);

	// return error if message wrapping failed
	if (frames[frameWriteCursor].size == 0)
		return ERROR_MESSAGE_WRAPPING;

	// upon success, increment cursor
	frameWriteCursor++;
	if (frameWriteCursor == MAX_FRAME_COUNT)
		frameWriteCursor = 0;

	// return success
	return 0;
}

