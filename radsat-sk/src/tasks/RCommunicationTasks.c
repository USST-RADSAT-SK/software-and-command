/**
 * @file RCommunicationTask.c
 * @date December 23, 2021
 * @author Tyrel Kostyk
 */

#include <RTransceiver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <RCommunicationTasks.h>

#define PASS_TIMER_ID 1
#define MAX_PASS_LENGTH 240000 // 4 minutes in ms

static int passtime = 0;
static xTimerHandle passTimer;

static communications_t comms_data;
/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void resetPassData(communications_t comms);
static void startPassMode(void);
static void vCallbackFunction( xTimerHandle timer );

/***************************************************************************************************
											 PUBLIC API
***************************************************************************************************/

void receiverTask(void* parameters)
{
	(void)parameters;
	resetPassMode(comms_data); // Set to non-transmitting mode when initialized

	int error = 0;

	while(1) {


		vTaskDelay(1);
	}
}

void transmitterTask(void* parameters)
{
	(void)parameters;

	int error = 0;

	while(1) {

		vTaskDelay(1);
	}
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Reset the structure for coordinating down and uplinking
 *
 * @param comms A communications_t struct that holds the data for coordination communications.
 */
static void resetPassData(communications_t comms) {
	comms.mode = 0;
	comms.telecommand.transmit = 0;
	comms.telecommand.response = 0;
	comms.fileTransfer.transmit = 0;
	comms.fileTransfer.response = 0;
}


/**
 * Callback function for the pass timer that resets structs to a neutral state in preparation for the next pass
 *
 * @param timer A handle for a timer. However this is implicitly called and is passed without parameters
 * 				to xTimerCreate()
 */
static void vCallbackFunction( xTimerHandle timer ) {
	resetPassData(comms_data);
}

/**
 * Starts a timer for the pass timeout
 */
static void startPassMode(void) {
	if (passtime == 0) {
		passtime = 1;

		if (passTimer == NULL) {
			// If the timer was not created yet, create it
			passTimer = xTimerCreate("passTimer", MAX_PASS_LENGTH, pdFALSE, PASS_TIMER_ID, vCallbackFunction);
			xTimerStart(passTimer, 0);
		}
		else {
			// Otherwise restart it
			xTimerReset(passTimer, 0);
		}
	}
}




