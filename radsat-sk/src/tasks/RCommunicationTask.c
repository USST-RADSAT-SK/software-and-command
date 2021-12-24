/**
 * @file RCommunicationTask.c
 * @date December 23, 2021
 * @author Tyrel Kostyk
 */

#include <RTransceiver.h>


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

	while(1) {

		while(transceiverFrameCount() > 0) {
			// let system know we've entered a pass
			startPassMode();

			// allocate a buffer
			uint8_t message[TRX_RECEIVER_MAX_MSG_SIZE];

			// retrieve a message
			int sizeOfMessage = transceiverGetFrame(message);

			// TODO: stuff...

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


