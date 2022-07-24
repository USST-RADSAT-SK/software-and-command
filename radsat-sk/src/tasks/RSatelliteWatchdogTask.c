/**
 * @file RSatelliteWatchdogTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RSatelliteWatchdogTask.h>
#include <RCommunicationTasks.h>
#include <RCommon.h>
#include <RBattery.h>
#include <RTranseiver.h>
#include <RPdb.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** Satellite Watchdog Task delay (in ms). */
#define SATELLITE_WATCHDOG_TASK_DELAY_MS	(15)



/** Abstraction of the states of operation */
typedef enum _oper_state_t {
	stateNormalMode			= 0,	///> All tasks free to run
	stateSafeMode			= 1,	///> No Downlinking or payload collection  
	stateActiveFullCommunicationMode		= 2,	///> No tasks allowed except for WDOG Petting
	stateActiveHalfCommunicationMode	= 3,	///> No tasks or downlinking allowed except for WDOG Petting
} oper_state_t;


/** Wrapper structure for states of operation */
typedef struct _operation_state_t {
	oper_state_t mode;					///> The current state of operation
} operation_state_t;


/** Operation structure */
static operation_state_t state = { 3 };




/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void SatelliteWatchdogTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	int error = 0;
	int communicationFlag = 1;
	int safeFlag = 1;

	while (1) {

		// TODO: implement petting satellite watchdogs

				debugPrint("SatelliteWatchdogTask(): About to pet external satellite watchdogs.\n");

		//pet watchdogs for all components 

		error = batteryPetWatchDog();

		if (error != SUCCESS) {
			// TODO: implement error manager
			error;
		}

		error = transceiverPetWatchDogs();

		if (error != SUCCESS) {
			// TODO: implement error manager
			error;
		}

		error = pdbPetWatchdog();

		if (error != SUCCESS) {
			// TODO: implement error manager
			error;
		}

		// TODO: implement petting camera watchdog

		// Set the comunication flag depending on current state
		if (communicationPassModeActive()) {
			communicationFlag = 1;
		}
		else {
			communicationFlag = 0;
		}

		// Set the safe flag depending on current state
		batteryIsNotSafe(&safeFlag);


		if (communicationFlag == 0 && safeFlag == 0) {
			state.mode = 0;
		}
		else if (communicationFlag == 0 && safeFlag == 1){
			state.mode = 1;
		}
		else if (communicationFlag == 1 && safeFlag == 0){
			state.mode = 2;
		}
		else if (communicationFlag == 1 && safeFlag == 1){
			state.mode = 3;
		}
		



		vTaskDelay(SATELLITE_WATCHDOG_TASK_DELAY_MS);
	}
}


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Indicate if the Satellite is currently in a communications mode.
 *
 * @returns 1 (true) if Satellite is uplinking or downlinking; 0 (false) otherwise.
 */
uint8_t comunicationMode(void){

	if (operation_state_t.mode == 2 || operation_state_t.mode == 3) {
		return 1;
	}
	else{
		return 0;
	}

}


/**
 * Indicate if the Satellite is currently in a safe mode.
 *
 * @returns 1 (true) check if the safe flag is rased meaning the current battery voltage is under 6.5V ; 0 (false) otherwise.
 */
uint8_t safeMode(void){

	if (operation_state_t.mode == 1 || operation_state_t.mode == 3) {
		return 1;
	}
	else{
		return 0;
	}

}
