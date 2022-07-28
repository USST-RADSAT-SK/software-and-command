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
#include <RObc.h>
#include <RErrorManager.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** How many telemetry readings to collect per hour. */
#define TELEMETRY_READINGS_PER_HOUR			(10)

/** Telemetry Collection Task delay (in ms). */
#define TELEMETRY_COLLECTION_TASK_DELAY_MS	(MS_PER_HOUR / TELEMETRY_READINGS_PER_HOUR)


static battery_telemetry battery_telemetry_R;

static antenna_telemetry antenna_telemetry_R;

static transceiver_telemetry transceiver_telemetry_R;

static eps_telemetry pdb_telemetry_R;

static obc_telemetry obc_telemetry_R;

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

			error = antennaTelemetry(&antenna_telemetry_R);
			if (error != SUCCESS) {
				errorReportModule(moduleAntenna,error);
			}

			error = batteryTelemetry(&battery_telemetry_R);
			if (error != SUCCESS) {
				errorReportModule(moduleBattery,error);
			}

			error = transceiverTelemetry(&transceiver_telemetry_R);
			if (error != SUCCESS) {
				errorReportModule(moduleTransceiver,error);
			}
			error = pdbTelemetry(&pdb_telemetry_R);
			if (error != SUCCESS) {
				errorReportModule(modulePdb,error);
			}

			error = obcTelemetry(&obc_telemetry_R);
			if (error != SUCCESS) {
				errorReportModule(moduleObc,error);
			}


			error = fileTransferAddMessage(&antenna_telemetry_R, sizeof(antenna_telemetry_R),file_transfer_message_AntennaTelemetry_tag );
			if (error != SUCCESS) {
				errorReportModule(moduleFileTransferService,error);
			}

			error = fileTransferAddMessage(&battery_telemetry_R, sizeof(battery_telemetry_R),file_transfer_message_BatteryTelemetry_tag );
			if (error != SUCCESS) {
				errorReportModule(moduleFileTransferService,error);
			}

			error = fileTransferAddMessage(&transceiver_telemetry_R, sizeof(transceiver_telemetry_R),file_transfer_message_TransceiverTelemetry_tag );
			if (error != SUCCESS) {
				errorReportModule(moduleFileTransferService,error);
			}

			error = fileTransferAddMessage(&pdb_telemetry_R, sizeof(pdb_telemetry_R),file_transfer_message_EpsTelemetry_tag );
			if (error != SUCCESS) {
				errorReportModule(moduleFileTransferService,error);
			}

			error = fileTransferAddMessage(&obc_telemetry_R, sizeof(obc_telemetry_R),file_transfer_message_ObcTelemetry_tag );
			if (error != SUCCESS) {
				errorReportModule(moduleFileTransferService,error);
			}


			
		}

		vTaskDelay(TELEMETRY_COLLECTION_TASK_DELAY_MS);
	}
}
