/**
 * @file RTelemetryCollectionTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */


#include <RTelemetryCollectionTask.h>
#include <RSatelliteWatchdogTask.h>
#include <RFileTransferService.h>
#include <RFileTransfer.h>
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

static battery_telemetry battery_telemetry_R;

static antenna_telemetry antenna_telemetry_R;

static transceiver_telemetry transceiver_telemetry_R;

static eps_telemetry pdb_telemetry_R;

static obc_telemetry obc_telemetry_R;



void printBatteryTelemetry(void) {
	infoPrint(" ---Battery Telemetry---\n\
		outputVoltageBatteryBus = %f\n\
		outputVoltage5VBus = %f\n\
		outputVoltage3V3Bus = %f\n\
		outputCurrentBatteryBus = %f\n\
		outputCurrent5VBus = %f\n\
		outputCurrent3V3Bus = %f\n\
		batteryCurrentDirection = %f\n\
		motherboardTemp = %f\n\
		daughterboardTemp1 = %f\n\
		daughterboardTemp2 = %f\n\
		daughterboardTemp3 = %f",
		battery_telemetry_R.outputVoltageBatteryBus,
		battery_telemetry_R.outputVoltage5VBus,
		battery_telemetry_R.outputVoltage3V3Bus,
		battery_telemetry_R.outputCurrentBatteryBus,
		battery_telemetry_R.outputCurrent5VBus,
		battery_telemetry_R.outputCurrent3V3Bus,
		battery_telemetry_R.batteryCurrentDirection,
		battery_telemetry_R.motherboardTemp,
		battery_telemetry_R.daughterboardTemp1,
		battery_telemetry_R.daughterboardTemp2,
		battery_telemetry_R.daughterboardTemp3
	);
}

void printTransceiverTelemetry(void) {
	infoPrint(" ---Transceiver Telemetry---\n\
		transceiver_telemetry_R.receiver.rxDoppler = %f\n\
		transceiver_telemetry_R.receiver.rxRssi = %f\n\
		transceiver_telemetry_R.receiver.busVoltage = %f\n\
		transceiver_telemetry_R.receiver.totalCurrent = %f\n\
		transceiver_telemetry_R.receiver.txCurrent = %f\n\
		transceiver_telemetry_R.receiver.rxCurrent = %f\n\
		transceiver_telemetry_R.receiver.powerAmplifierCurrent = %f\n\
		transceiver_telemetry_R.receiver.powerAmplifierTemperature = %f\n\
		transceiver_telemetry_R.receiver.boardTemperature = %f\n\
		transceiver_telemetry_R.receiver.uptime = %lu\n\
		transceiver_telemetry_R.receiver.frames = %lu\n\
		transceiver_telemetry_R.transmitter.reflectedPower = %f\n\
		transceiver_telemetry_R.transmitter.forwardPower = %f\n\
		transceiver_telemetry_R.transmitter.busVoltage = %f\n\
		transceiver_telemetry_R.transmitter.totalCurrent = %f\n\
		transceiver_telemetry_R.transmitter.txCurrent = %f\n\
		transceiver_telemetry_R.transmitter.rxCurrent = %f\n\
		transceiver_telemetry_R.transmitter.powerAmplifierCurrent = %f\n\
		transceiver_telemetry_R.transmitter.powerAmplifierTemperature = %f\n\
		transceiver_telemetry_R.transmitter.boardTemperature = %f\n\
		transceiver_telemetry_R.transmitter.uptime = %lu",
		transceiver_telemetry_R.receiver.rxDoppler,
		transceiver_telemetry_R.receiver.rxRssi,
		transceiver_telemetry_R.receiver.busVoltage,
		transceiver_telemetry_R.receiver.totalCurrent,
		transceiver_telemetry_R.receiver.txCurrent,
		transceiver_telemetry_R.receiver.rxCurrent,
		transceiver_telemetry_R.receiver.powerAmplifierCurrent,
		transceiver_telemetry_R.receiver.powerAmplifierTemperature,
		transceiver_telemetry_R.receiver.boardTemperature,
		transceiver_telemetry_R.receiver.uptime,
		transceiver_telemetry_R.receiver.frames,
		transceiver_telemetry_R.transmitter.reflectedPower,
		transceiver_telemetry_R.transmitter.forwardPower,
		transceiver_telemetry_R.transmitter.busVoltage,
		transceiver_telemetry_R.transmitter.totalCurrent,
		transceiver_telemetry_R.transmitter.txCurrent,
		transceiver_telemetry_R.transmitter.rxCurrent,
		transceiver_telemetry_R.transmitter.powerAmplifierCurrent,
		transceiver_telemetry_R.transmitter.powerAmplifierTemperature,
		transceiver_telemetry_R.transmitter.boardTemperature,
		transceiver_telemetry_R.transmitter.uptime
	);
}

void printPdbTelemetry(void) {
	infoPrint(" ---Pdb Telemetry---\n\
		pdb_telemetry_R.sunSensorData.BCR1Voltage = %f\n\
		pdb_telemetry_R.sunSensorData.SA1ACurrent = %f\n\
		pdb_telemetry_R.sunSensorData.SA1BCurrent = %f\n\
		pdb_telemetry_R.sunSensorData.BCR2Voltage = %f\n\
		pdb_telemetry_R.sunSensorData.SA2ACurrent = %f\n\
		pdb_telemetry_R.sunSensorData.SA2BCurrent = %f\n\
		pdb_telemetry_R.sunSensorData.BCR3Voltage = %f\n\
		pdb_telemetry_R.sunSensorData.SA3ACurrent = %f\n\
		pdb_telemetry_R.sunSensorData.SA3BCurrent = %f\n\
		pdb_telemetry_R.outputVoltageBCR = %f\n\
		pdb_telemetry_R.outputVoltageBatteryBus = %f\n\
		pdb_telemetry_R.outputVoltage5VBus = %f\n\
		pdb_telemetry_R.outputVoltage3V3Bus = %f\n\
		pdb_telemetry_R.outputCurrentBCR_mA = %f\n\
		pdb_telemetry_R.outputCurrentBatteryBus = %f\n\
		pdb_telemetry_R.outputCurrent5VBus = %f\n\
		pdb_telemetry_R.outputCurrent3V3Bus = %f\n\
		pdb_telemetry_R.PdbTemperature = %f",
		pdb_telemetry_R.sunSensorData.BCR1Voltage,
		pdb_telemetry_R.sunSensorData.SA1ACurrent,
		pdb_telemetry_R.sunSensorData.SA1BCurrent,
		pdb_telemetry_R.sunSensorData.BCR2Voltage,
		pdb_telemetry_R.sunSensorData.SA2ACurrent,
		pdb_telemetry_R.sunSensorData.SA2BCurrent,
		pdb_telemetry_R.sunSensorData.BCR3Voltage,
		pdb_telemetry_R.sunSensorData.SA3ACurrent,
		pdb_telemetry_R.sunSensorData.SA3BCurrent,
		pdb_telemetry_R.outputVoltageBCR,
		pdb_telemetry_R.outputVoltageBatteryBus,
		pdb_telemetry_R.outputVoltage5VBus,
		pdb_telemetry_R.outputVoltage3V3Bus,
		pdb_telemetry_R.outputCurrentBCR_mA,
		pdb_telemetry_R.outputCurrentBatteryBus,
		pdb_telemetry_R.outputCurrent5VBus,
		pdb_telemetry_R.outputCurrent3V3Bus,
		pdb_telemetry_R.PdbTemperature
	);
}

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

		error = antennaTelemetry((void*)&antenna_telemetry_R);
			//NA-----------
		if (error != SUCCESS) {
			warningPrint("antennaTelemetry error = %d", error);
		} else {
			infoPrint("antennaTelemetry collected");
		}
		error = fileTransferAddMessage(&antenna_telemetry_R, sizeof(antenna_telemetry_R),file_transfer_AntennaTelemetry_tag );

		error = batteryTelemetry((void*)&battery_telemetry_R);
		printBatteryTelemetry(); // Bat telem -----------
		if (error != SUCCESS) {
			warningPrint("batteryTelemetry error = %d", error);
		} else {
			infoPrint("batteryTelemetry collected");
		}
		error = fileTransferAddMessage(&battery_telemetry_R, sizeof(battery_telemetry_R),file_transfer_BatteryTelemetry_tag );

		error = transceiverTelemetry((void*)&transceiver_telemetry_R);
		printTransceiverTelemetry(); // TRXVU Telen
		if (error != SUCCESS) {
			warningPrint("transceiverTelemetry error = %d", error);
		} else {
			infoPrint("transceiverTelemetry collected");
		}
		error = fileTransferAddMessage(&transceiver_telemetry_R, sizeof(transceiver_telemetry_R),file_transfer_TransceiverTelemetry_tag );

		error = pdbTelemetry((void*)&pdb_telemetry_R);
		printPdbTelemetry();
		if (error != SUCCESS) {
			warningPrint("pdbTelemetry error = %d", error);
		} else {
			infoPrint("pdbTelemetry collected");
		}
		error = fileTransferAddMessage(&pdb_telemetry_R, sizeof(pdb_telemetry_R),file_transfer_EpsTelemetry_tag );

		error = obcTelemetry((void*)&obc_telemetry_R);
		if (error != SUCCESS) {
			warningPrint("obcTelemetry error = %d", error);
		} else {
			infoPrint("obcTelemetry collected");
		}
		error = fileTransferAddMessage(&obc_telemetry_R, sizeof(obc_telemetry_R),file_transfer_ObcTelemetry_tag );



		vTaskDelay(TELEMETRY_COLLECTION_TASK_DELAY_MS);
	}
}
