/**
 * @file RFileTransferService.c
 * @date February 25, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RCommon.h>
#include <RFileTransferService.h>
#include <RTransceiver.h>
#include <RMessage.h>
#include <string.h>
#include <math.h>
#include <RFram.h>
#include <assert.h>


/**
 * Ensure that our message sizes never exceed the transceiver's max frame size.
 * NOTE: This check allows for 15 bytes of overhead (i.e. from the header) which is more than we currently need. */
//#if ((PROTO_MAX_ENCODED_SIZE + 15) > (TRANCEIVER_TX_MAX_FRAME_SIZE))
//#error "Encoded protobuf message size (plus header) exceeds maximum transceiver frame size!! Reduce size of header or max protobuf messages"
//#endif


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

#define OVERWRITE
#define ERASE_PREVIOUS_FRAME

/** Error code for internal issues regarding frame cursors (wrap-around, etc.). */
#define ERROR_CURSOR			(-1)

/** Error code for failed message wrapping. */
#define ERROR_MESSAGE_WRAPPING	(-2)

typedef uint16_t fileCursor_t;
typedef uint16_t framAddress_t;
typedef uint8_t file_t;

/** Struct that defines a prepared FRAM frame and its total size. */
typedef struct _fram_frame_t {
	uint8_t size;								///> The size (in bytes) of the entire frame
	uint8_t data[TRANCEIVER_TX_MAX_FRAME_SIZE];	///> The buffer holding the prepared frame
} fram_frame_t;


static fileCursor_t frameWriteCursor = 0;
static fileCursor_t frameReadCursor = 0;

//typedef union {
//	fram_frame_t framed;
//	uint8_t raw[FRAM_DATA_FRAME_SIZE];
//}file_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


fileCursor_t cursorIncrement(fileCursor_t value) {
	return value + 1 >= MAX_FRAME_COUNT ? 0 : value + 1;
}

uint16_t cursorToAddress(fileCursor_t cursor) {
	return FRAM_DATA_START_ADDR + cursor * FRAM_DATA_FRAME_SIZE;
}

fileCursor_t cursorDistance(fileCursor_t readCursor, fileCursor_t writeCursor) {
	if ( writeCursor > readCursor )
		return MAX_FRAME_COUNT + writeCursor - readCursor;
	return writeCursor - readCursor;
}

int getCursors(fileCursor_t* readCursor, fileCursor_t* writeCursor){
	int error = SUCCESS;
	if (readCursor != NULL){
		error = framRead((uint8_t*)writeCursor, FRAM_WRITE_CURSOR_ADDR, 2);
		if (error) {
			errorPrint("FRAM READ WRITE CURSOR ERROR = %d, cursor = %hu", error, *writeCursor);
			return error;
		}
	}
	if (writeCursor != NULL){
		error = framRead((uint8_t*)readCursor, FRAM_READ_CURSOR_ADDR, 2);
		if(error){
			errorPrint("FRAM READ READ CURSOR ERROR = %d, cursor = %hu", error, *readCursor);
		}
	}
	return error;
}

void printCursorInfo(void){
	getCursors(&frameReadCursor, &frameWriteCursor);
	uint16_t delta = cursorDistance(frameReadCursor, frameWriteCursor);
	infoPrint("Read: %hu, Write: %hu, Delta: %hu", frameReadCursor, frameWriteCursor, delta);
	return;
}

int writeCursor(fileCursor_t* readCursor, fileCursor_t* writeCursor){
	int error = SUCCESS;
	if (readCursor != NULL){
		error += framWrite((uint8_t*)readCursor, FRAM_READ_CURSOR_ADDR, 2);
	}
	if (writeCursor != NULL){
		error = framWrite((uint8_t*)writeCursor, FRAM_WRITE_CURSOR_ADDR, 2);
	}
	assert(error != E_NOT_INITIALIZED);
	assert(error != E_INDEX_ERROR);
	return error;
}

uint8_t readFile(fram_frame_t* dataIn, fileCursor_t cursor) {
	int error = framRead((uint8_t*)dataIn, cursorToAddress(cursor), FRAM_DATA_FRAME_SIZE);
	assert(!error);
	return dataIn->size;
}

int writeFile(fram_frame_t* dataOut, fileCursor_t cursor) {
	int error = framWrite((uint8_t*)dataOut, cursorToAddress(cursor), FRAM_DATA_FRAME_SIZE);
	assert(!error);
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
short fileTransferNextFrame(uint8_t* frame) {
	// get the read cursor from FRAM
	getCursors(&frameReadCursor, &frameWriteCursor);

	if (frameWriteCursor == frameReadCursor) {
		warningPrint("Fram Full Write %u > Read %u", frameWriteCursor, frameReadCursor);
		return ERROR_CURSOR;
	}

	fram_frame_t fram_frame = { 0 };
#ifdef ERASE_PREVIOUS_FRAME
	// invalidate the current (now previous frame)
	writeFile(&fram_frame, frameReadCursor);
#endif

	// increment the read cursor
	frameReadCursor = cursorIncrement(frameReadCursor);

	// save read cursor value in FRAM
	writeCursor(&frameReadCursor, NULL);

	readFile(&fram_frame, frameReadCursor);

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
short fileTransferCurrentFrame(uint8_t* frame) {
	// get the write & read cursors from FRAM
	getCursors(&frameReadCursor, NULL);

	fram_frame_t fram_frame = { 0 };

	readFile(&fram_frame, frameReadCursor);
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
	assert(message != NULL);
	if (message == NULL) {
		errorPrint("message == 0");
		return E_INPUT_POINTER_NULL;
	}
	printCursorInfo();




#ifndef OVERWRITE
	// get the write & read cursors from FRAM
	getCursors(&frameReadCursor, &frameWriteCursor);
	if (cursorIncrement(frameWriteCursor) == frameReadCursor){
		warningPrint("Fram Full Write %u > Read %u", frameWriteCursor, frameReadCursor);
		return ERROR_CURSOR;
	}
#endif /* OVERWRITE */

	// create new RADSAT-SK message to populate
	radsat_message newMessage = { 0 };
	newMessage.which_service = messageTag;

	// internal message data will go immediately after the "which message" property of the struct
	void* newMessageAddr = (&newMessage.which_service) + 1;
	memcpy(newMessageAddr, message, size);

	// wrap new message
	fram_frame_t fram_frame = { 0 };
	fram_frame.size = messageWrap(&newMessage, (radsat_sk_raw_message_t*)&fram_frame.data);

	// write data in FRAM
	getCursors(&frameReadCursor, &frameWriteCursor);
	writeFile(&fram_frame, frameWriteCursor);

	// return error if message wrapping failed
	if (fram_frame.size == 0)
		return ERROR_MESSAGE_WRAPPING;

	// upon success, increment cursor
	frameWriteCursor = cursorIncrement(frameWriteCursor);
#ifdef OVERWRITE
	if (frameWriteCursor == frameReadCursor){
		frameReadCursor = cursorIncrement(frameReadCursor);
	}
	writeCursor(&frameReadCursor, &frameWriteCursor);
#else /* OVERWRITE */
	// save write cursor value in FRAM
	writeCursor(NULL, &frameWriteCursor);
#endif /* OVERWRITE */

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
	fileCursor_t writeStart = 0;
	fileCursor_t readStart = 0;
	writeCursor(&readStart, &writeStart);

	// reset all data frames to zero
	fram_frame_t emptyFrame = { 0 };
	for(int i=0; i<MAX_FRAME_COUNT; i++) {
		uint32_t framDataAddr = FRAM_DATA_START_ADDR + (i * FRAM_DATA_FRAME_SIZE);
		framWrite((uint8_t*)&emptyFrame, framDataAddr, FRAM_DATA_FRAME_SIZE);
	}
}
