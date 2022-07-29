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

/** Struct that defines a prepared frame and its total size. */
typedef struct _frame_t {
	uint8_t data[TRANCEIVER_TX_MAX_FRAME_SIZE];	///> The buffer holding the prepared frame
	uint8_t size;								///> The size (in bytes) of the entire frame
} frame_t;


/** Local FIFO containing frames prepared for downlink. */
static frame_t frames[MAX_FRAME_COUNT] = { 0 };

/** Cursor for writing to the frame FIFO (starts ahead of the read cursor). */
static uint8_t frameWriteCursor = 1;

/** Cursor for reading from the frame FIFO. */
static uint8_t frameReadCursor = 0;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initialize the write/read cursors with the last values in FRAM.
 * Requires that FRAM has already been initialized.
 * @return 0 for success, non-zero for failure.
 */
int fileTransferInit(void) {
	uint16_t framCursors[2] = {0};

	// Read the last stored cursor values
	int error = framRead(&framCursors, FRAM_WRITE_CURSOR_ADDR, 4);

	if (error == SUCCESS) {
		frameWriteCursor = 2; //framCursors[0];
		frameReadCursor = 1; //framCursors[1];
	}

	return error;
}

/**
 * Increment the internal FIFO and provide the frame at that location.
 *
 * Invalidates (deletes) the previous frame before moving on.
 *
 * @param frame Pointer to a buffer that the frame will be placed into. Set by function.
 * @return The size of the frame placed into the buffer; 0 on error.
 */
uint8_t fileTransferNextFrame(uint8_t* frame) {

	// ensure that there is a valid frame ahead
	if (frames[frameReadCursor + 1].size == 0)
		return SUCCESS;

	// invalidate the current (now previous frame)
	memset(&frames[frameReadCursor], 0, sizeof(frames[frameReadCursor]));
	//void* framDataAddr = FRAM_DATA_START_ADDR + (frameReadCursor * TRANCEIVER_TX_MAX_FRAME_SIZE);
	//framWrite(0, framDataAddr, TRANCEIVER_TX_MAX_FRAME_SIZE);

	// increment the read cursor
	frameReadCursor++;
	if (frameReadCursor == MAX_FRAME_COUNT)
		frameReadCursor = 0;
	// save read cursor value in FRAM
	framWrite(&frameReadCursor, FRAM_READ_CURSOR_ADDR, 2);

	// transfer the new (now current) frame into the provided buffer
	memcpy(frame, frames[frameReadCursor].data, frames[frameReadCursor].size);
	//void* framDataAddr = FRAM_DATA_START_ADDR + (frameReadCursor * TRANCEIVER_TX_MAX_FRAME_SIZE);
	//framRead(frame, framDataAddr, TRANCEIVER_TX_MAX_FRAME_SIZE);

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
	/*if (frames[frameReadCursor].size > 0) {
		memcpy(frame, frames[frameReadCursor].data, frames[frameReadCursor].size);
	}*/

	// read current frame from FRAM
	void* framDataAddr = FRAM_DATA_START_ADDR + (frameReadCursor * TRANCEIVER_TX_MAX_FRAME_SIZE);
	printf("\n\r Reading data in FRAM at cursor/address: %i/%x \n\r", frameReadCursor, framDataAddr);
	framRead(frame, framDataAddr, TRANCEIVER_TX_MAX_FRAME_SIZE);

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
	if (message == 0) {
		return E_INPUT_POINTER_NULL;
	}

	// ensure size parameter is valid
	if (size == 0 || size > (uint8_t)PROTO_MAX_ENCODED_SIZE)
		return E_PARAM_OUTOFBOUNDS;

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
	frames[frameWriteCursor].size = messageWrap(&newMessage, frames[frameWriteCursor].data);


	// testing purposes only
	/*
	printf("\n\r Read frames[frameWriteCursor].data: \n\r");
	for (uint8_t i=0; i<235; i++) {
		printf("%i=%i   ", i, frames[frameWriteCursor].data[i]);
	}
	*/
	/*
	printf("\n\r frameWriteCursor: %i \n\r", frameWriteCursor);
	printf("\n\r sizeof(frames[frameWriteCursor].data): %i \n\r", sizeof(frames[frameWriteCursor].data));
	*/
	printf("\n\r --- Cursor values --- \n\r");
	uint16_t framCursors[2] = {0};
	framRead(&framCursors, FRAM_WRITE_CURSOR_ADDR, 4);
	printf(" frameWriteCursor: %i \n\r", frameWriteCursor);
	printf(" frameReadCursor: %i \n\r", frameReadCursor);
	printf(" %x (FRAM WRITE):  %i \n\r", FRAM_WRITE_CURSOR_ADDR, framCursors[0]);
	printf(" %x (FRAM READ):   %i \n\r", FRAM_READ_CURSOR_ADDR, framCursors[1]);

	// increment the FRAM address for the new frame
	void* framDataAddr = FRAM_DATA_START_ADDR + (frameWriteCursor * TRANCEIVER_TX_MAX_FRAME_SIZE);
	// write data in FRAM
	printf("\n\r Writing data in FRAM at cursor/address: %i/%x \n\r", frameWriteCursor, framDataAddr);
	framWrite(frames[frameWriteCursor].data, framDataAddr, TRANCEIVER_TX_MAX_FRAME_SIZE);
	for (uint8_t i=0; i<235; i++) {
		printf("%i, ", frames[frameWriteCursor].data[i]);
	}

	// testing purposes only (read back what we wrote in FRAM)
	/*
	printf("\n\r FRAM read frame data at address %x \n\r", framDataAddr);
	uint8_t framData[235] = {0};
	framRead(&framData, framDataAddr, 235);
	for (uint8_t i=0; i<235; i++) {
		printf("%x=%i   ", framDataAddr+i, framData[i]);
	}
	*/


	// return error if message wrapping failed
	if (frames[frameWriteCursor].size == 0)
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

