/**
 * @file RFileTransferService.c
 * @date February 25, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RFileTransferService.h>
#include <RCommon.h>
#include <RTransceiver.h>
#include <RMessage.h>
#include <string.h>
#include <RCommon.h>
#include <RFram.h>


/**
 * Ensure that our message sizes never exceed the transceiver's max frame size.
 * NOTE: This check allows for 15 bytes of overhead (i.e. from the header) which is more than we currently need. */
#if ((PROTO_MAX_ENCODED_SIZE + 15) > (TRANCEIVER_TX_MAX_FRAME_SIZE))
#error "Encoded protobuf message size (plus header) exceeds maximum transceiver frame size!! Reduce size of header or max protobuf messages"
#endif


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** The max number of frames locally stored at one time. */
#define MAX_FRAME_COUNT 		(200)

/** Error code for internal issues regarding frame cursors (wrap-around, etc.). */
#define ERROR_CURSOR			(-1)

/** Error code for failed message wrapping. */
#define ERROR_MESSAGE_WRAPPING	(-2)

/** Struct that defines a prepared FRAM frame and its total size. */
typedef struct _fram_frame_t {
	uint8_t size;								///> The size (in bytes) of the entire frame
	uint8_t data[TRANCEIVER_TX_MAX_FRAME_SIZE];	///> The buffer holding the prepared frame
} fram_frame_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Increment the internal FIFO and provide the frame at that location.
 *
 * Invalidates (deletes) the previous frame before moving on.
 *
 * @param frame Pointer to a buffer that the frame will be placed into. Set by function.
 * @return The size of the frame placed into the buffer; 0 on error.
 */
uint8_t fileTransferNextFrame(uint8_t* frame) {
	// get the read cursor from FRAM
	uint16_t frameReadCursor = 0;
	framRead(&frameReadCursor, FRAM_READ_CURSOR_ADDR, 2);

	// ensure that there is a valid frame ahead
	uint16_t nextFrameReadCursor = frameReadCursor + 1;
	if (nextFrameReadCursor == MAX_FRAME_COUNT)
		nextFrameReadCursor = 0;

	fram_frame_t fram_frame = {0};
	void* framDataAddr = FRAM_DATA_START_ADDR + (nextFrameReadCursor * FRAM_DATA_FRAME_SIZE);
	framRead(&fram_frame, framDataAddr, FRAM_DATA_FRAME_SIZE);

	if (fram_frame.size == 0)
		return SUCCESS;

	// invalidate the current (now previous frame)
	fram_frame_t emptyFrame = {0};
	framDataAddr = FRAM_DATA_START_ADDR + (frameReadCursor * FRAM_DATA_FRAME_SIZE);
	framWrite(&emptyFrame, framDataAddr, FRAM_DATA_FRAME_SIZE);

	// increment the read cursor
	frameReadCursor++;
	if (frameReadCursor == MAX_FRAME_COUNT)
		frameReadCursor = 0;
	// save read cursor value in FRAM
	framWrite(&frameReadCursor, FRAM_READ_CURSOR_ADDR, 2);

	// transfer the new (now current) frame into the provided buffer
	memcpy(frame, fram_frame.data, fram_frame.size);

	return fram_frame.size;
}


/**
 * Provide the current frame.
 *
 * @param frame Pointer to a buffer that the frame will be placed into. Set by function.
 * @return The size of the frame placed into the buffer; 0 on error.
 */
uint8_t fileTransferCurrentFrame(uint8_t* frame) {
	// get the read cursor from FRAM
	uint16_t frameReadCursor = 0;
	framRead(&frameReadCursor, FRAM_READ_CURSOR_ADDR, 2);

	// read current frame from FRAM
	fram_frame_t fram_frame = {0};
	void* framDataAddr = FRAM_DATA_START_ADDR + (frameReadCursor * FRAM_DATA_FRAME_SIZE);
	framRead(&fram_frame, framDataAddr, FRAM_DATA_FRAME_SIZE);

	// only provide a frame if the cursor is pointing at a valid frame
	if (fram_frame.size > 0)
		memcpy(frame, fram_frame.data, fram_frame.size);

	// return the size of the frame
	return fram_frame.size;
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
	if (message == 0) {
		return E_INPUT_POINTER_NULL;
	}

	// ensure size parameter is valid
	if (size == 0 || size > (uint8_t)PROTO_MAX_ENCODED_SIZE)
		return E_PARAM_OUTOFBOUNDS;

	// get the write & read cursors from FRAM
	uint16_t frameWriteCursor = 0;
	uint16_t frameReadCursor = 0;
	framRead(&frameWriteCursor, FRAM_WRITE_CURSOR_ADDR, 2);
	framRead(&frameReadCursor, FRAM_READ_CURSOR_ADDR, 2);

	// ensure we are not about to overwrite frames that have not been read
	if (frameWriteCursor == frameReadCursor)
		return ERROR_CURSOR;

	// create new RADSAT-SK message to populate
	radsat_message newMessage = { 0 };
	newMessage.which_service = radsat_message_FileTransferMessage_tag;
	newMessage.FileTransferMessage.which_message = messageTag;

	// internal message data will go immediately after the "which message" property of the struct
	// TODO: confirm that this works
	void* newMessageAddr = &(newMessage.FileTransferMessage.which_message) + sizeof(newMessage.FileTransferMessage.which_message);
	memcpy(newMessageAddr, message, size);

	// wrap new message
	fram_frame_t fram_frame = {0};
	fram_frame.size = messageWrap(&newMessage, fram_frame.data);

	// write data in FRAM
	void* framDataAddr = FRAM_DATA_START_ADDR + (frameWriteCursor * FRAM_DATA_FRAME_SIZE);
	framWrite(&fram_frame, framDataAddr, FRAM_DATA_FRAME_SIZE);

	// return error if message wrapping failed
	if (fram_frame.size == 0)
		return ERROR_MESSAGE_WRAPPING;

	// upon success, increment cursor
	frameWriteCursor++;
	if (frameWriteCursor == MAX_FRAME_COUNT)
		frameWriteCursor = 0;
	// save write cursor value in FRAM
	framWrite(&frameWriteCursor, FRAM_WRITE_CURSOR_ADDR, 2);

	// return success
	return SUCCESS;
}


/**
 * Resets the file transfer content in FRAM to the following initial values:
 *  - Write Cursor = 1
 *  - Read Cursor  = 0
 *  - All data frames are filled with 0.
 */
void fileTransferReset(void) {
	// reset the cursors to their default values
	uint16_t defaultCursors[2] = {1,0};
	framWrite(&defaultCursors, FRAM_WRITE_CURSOR_ADDR, 4);

	// reset all data frames to zero
	fram_frame_t emptyFrame = {0};
	for(int i=0; i<MAX_FRAME_COUNT; i++) {
		void* framDataAddr = FRAM_DATA_START_ADDR + (i * FRAM_DATA_FRAME_SIZE);
		framWrite(&emptyFrame, framDataAddr, FRAM_DATA_FRAME_SIZE);
	}
}
