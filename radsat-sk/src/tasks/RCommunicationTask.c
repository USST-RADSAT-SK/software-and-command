/**
 * @file RCommunicationTask.c
 * @date December 23, 2021
 * @author Tyrel Kostyk
 */

#include <RTransceiver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

int passtime = 0;

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void startPassMode(void);


/***************************************************************************************************
											 PUBLIC API
***************************************************************************************************/

void taskCommunication(void* parameters)
{
	(void)parameters;

	int error = 0;

	while(1) {

		// continue receiving frames while they're available
		uint16_t framesReady = 0;
		error = transceiverRxFrameCount(&framesReady);
		while(error == 0 && framesReady > 0) {
			// let system know we've entered a pass
			startPassMode();

			// allocate a buffer
			uint8_t message[TRANCEIVER_RX_MAX_FRAME_SIZE];

			// retrieve a message
			uint16_t sizeOfMessage = 0;
			error = transceiverGetFrame(message, &sizeOfMessage);
			if (error)
				break;

			// TODO: stuff...

			// update frame size for loop condition
			error = transceiverRxFrameCount(&framesReady);
		}

		/*
		while (downlinkFrameCount > 0) {

		}

		*/

		// done uplinking & downlinking; end passtime
		passtime = 0;

		vTaskDelay(1);
	}
}


static void startPassMode(void) {
	if (passtime == 0) {
		passtime = 1;
		//xTimerCreate(12min, endPassMode);
	}
}


