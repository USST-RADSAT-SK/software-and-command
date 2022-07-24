/**
 * @file RTelemetryCollectionTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */


#include <RTelemetryCollectionTask.h>
#include <RSatelliteWatchdogTask.h>
#include <RFileTransferService.h>
#include <RFileTransfer.pb.h>
#include <RCommon.h>
#include <RAntenna.h>
#include <RBattery.h>
#include <RTransceiver.h>
#include <RPdb.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** How many telemetry readings to collect per hour. */
#define TELEMETRY_READINGS_PER_HOUR			(10)

/** Telemetry Collection Task delay (in ms). */
#define TELEMETRY_COLLECTION_TASK_DELAY_MS	(MS_PER_HOUR / TELEMETRY_READINGS_PER_HOUR)


static battery_status_t battery_telemetry;

static antenna_telemetry_t antenna_telemetry;

static transceiver_telemetry_t transceiver_telemetry;

static EpsTelemetry pdb_telemetry;

/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void TelemetryCollectionTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;
	int error = 0;

	while (1) {

		// TODO: implement telemetry collection for camera

		debugPrint("TelemetryCollectionTask(): About to collect satellite telemetry data.\n");

		if(!(comunicationMode())) {

			error = antennaTelemetry(&antenna_telemetry);
			if (error != SUCCESS) {
				error;
				// TODO: implement error manager
			}

			error = batteryTelemetry(&battery_telemetry);
			if (error != SUCCESS) {
				error;
				// TODO: implement error manager
			}

			error = transceiverTelemetry(&transceiver_telemetry);
			if (error != SUCCESS) {
				error;
				// TODO: implement error manager
			}
			error = pdbTelemetry(&pdb_telemetry);
			if (error != SUCCESS) {
				error;
				// TODO: implement error manager
			}

			error = fileTransferAddMessage(&antenna_telemetry, sizeof(antenna_telemetry),FileTransferMessage_antennaTelemetry_tag );
			if (error != SUCCESS) {
				error;
				// TODO: implement error manager
			}

			error = fileTransferAddMessage(&battery_telemetry, sizeof(battery_telemetry),FileTransferMessage_batteryTelemetry_tag );
			if (error != SUCCESS) {
				error;
				// TODO: implement error manager
			}

			error = fileTransferAddMessage(&transceiver_telemetry, sizeof(transceiver_telemetry),FileTransferMessage_transceiverTelemetry_tag );
			if (error != SUCCESS) {
				error;
				// TODO: implement error manager
			}

			error = fileTransferAddMessage(&pdb_telemetry, sizeof(pdb_telemetry),FileTransferMessage_epsTelemetry_tag );
			if (error != SUCCESS) {
				error;
				// TODO: implement error manager
			}


			// error = batteryIsNotSafe(&safeFlag);
			// if (error != SUCCESS) {
			// 	error;
			// 	// TODO: implement error manager
			// }

			
		}

		vTaskDelay(TELEMETRY_COLLECTION_TASK_DELAY_MS);
	}
}
