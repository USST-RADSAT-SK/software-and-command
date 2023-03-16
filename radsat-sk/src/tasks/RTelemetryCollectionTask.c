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
#include <RCamera.h>
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
static camera_telemetry camera_telemetry_R;



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
		receiver rxDoppler                    = %f\n\
		receiver rxRssi                       = %f\n\
		receiver busVoltage                   = %f\n\
		receiver totalCurrent                 = %f\n\
		receiver txCurrent                    = %f\n\
		receiver rxCurrent                    = %f\n\
		receiver powerAmplifierCurrent        = %f\n\
		receiver powerAmplifierTemperature    = %f\n\
		receiver boardTemperature             = %f\n\
		receiver uptime                       = %u\n\
		receiver frames                       = %u\n\
		transmitter reflectedPower            = %f\n\
		transmitter forwardPower              = %f\n\
		transmitter busVoltage                = %f\n\
		transmitter totalCurrent              = %f\n\
		transmitter txCurrent                 = %f\n\
		transmitter rxCurrent                 = %f\n\
		transmitter powerAmplifierCurrent     = %f\n\
		transmitter powerAmplifierTemperature = %f\n\
		transmitter boardTemperature          = %f\n\
		transmitter uptime                    = %u\n",
        (float)transceiver_telemetry_R.receiver.rxDoppler    * 13.352 - 22300.0,
        transceiver_telemetry_R.receiver.rxRssi       * 0.03 - 152,
        transceiver_telemetry_R.receiver.busVoltage   * 0.00488,
        transceiver_telemetry_R.receiver.totalCurrent * 0.16643964,
        transceiver_telemetry_R.receiver.txCurrent    * 0.16643964,
        transceiver_telemetry_R.receiver.rxCurrent    * 0.16643964,
        transceiver_telemetry_R.receiver.powerAmplifierCurrent    * 0.16643964,
        transceiver_telemetry_R.receiver.powerAmplifierTemperature      * -0.07669 + 195.6037,
        transceiver_telemetry_R.receiver.boardTemperature   * -0.07669 + 195.6037,
		transceiver_telemetry_R.receiver.uptime,
		transceiver_telemetry_R.receiver.frames,
		transceiver_telemetry_R.transmitter.reflectedPower   * transceiver_telemetry_R.transmitter.reflectedPower * 5.887E-5,
		transceiver_telemetry_R.transmitter.forwardPower     * transceiver_telemetry_R.transmitter.forwardPower * 5.887E-5,
		transceiver_telemetry_R.transmitter.busVoltage       * 0.00488,
		transceiver_telemetry_R.transmitter.totalCurrent     * 0.16643964,
		transceiver_telemetry_R.transmitter.txCurrent        * 0.16643964,
		transceiver_telemetry_R.transmitter.rxCurrent        * 0.16643964,
		transceiver_telemetry_R.transmitter.powerAmplifierCurrent * 0.16643964,
		transceiver_telemetry_R.transmitter.powerAmplifierTemperature * -0.07669 + 195.6037,
		transceiver_telemetry_R.transmitter.boardTemperature * -0.07669 + 195.6037,
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
		pdb_telemetry_R.sunSensorData.BCR1Voltage* 0.0322581,
		pdb_telemetry_R.sunSensorData.SA1ACurrent* 0.0009775,
		pdb_telemetry_R.sunSensorData.SA1BCurrent* 0.0009775,
		pdb_telemetry_R.sunSensorData.BCR2Voltage* 0.0322581,
		pdb_telemetry_R.sunSensorData.SA2ACurrent* 0.0009775,
		pdb_telemetry_R.sunSensorData.SA2BCurrent* 0.0009775,
		pdb_telemetry_R.sunSensorData.BCR3Voltage* 0.0322581,
		pdb_telemetry_R.sunSensorData.SA3ACurrent* 0.0009775,
		pdb_telemetry_R.sunSensorData.SA3BCurrent* 0.0009775,
		pdb_telemetry_R.outputVoltageBCR         * 0.008993157,
		pdb_telemetry_R.outputVoltageBatteryBus  * 0.008978,
		pdb_telemetry_R.outputVoltage5VBus       * 0.005865,
		pdb_telemetry_R.outputVoltage3V3Bus      * 0.004311,
		pdb_telemetry_R.outputCurrentBCR_mA      * 14.662757,
		pdb_telemetry_R.outputCurrentBatteryBus  * 0.005237,
		pdb_telemetry_R.outputCurrent5VBus       * 0.005237,
		pdb_telemetry_R.outputCurrent3V3Bus      * 0.005237,
		pdb_telemetry_R.PdbTemperature           * 0.372434 - 273.15
	);
}


void printAntennaTelemetry(void) {
	infoPrint("uint16_t deployedAntenna1 = %d\n\
    uint16_t deployedAntenna2 = %d\n\
    uint16_t deployedAntenna3 = %d\n\
    uint16_t deployedAntenna4 = %d\n\
    uint16_t armed            = %d\n\
    float boardTemp           = %f\n\
    uint32_t upTime           = %d\n\
    uint16_t deployedAntenna1 = %d\n\
    uint16_t deployedAntenna2 = %d\n\
    uint16_t deployedAntenna3 = %d\n\
    uint16_t deployedAntenna4 = %d\n\
    uint16_t armed            = %d\n\
    boardTemp                 = %f\n\
    uptime                    = %d",
    antenna_telemetry_R.sideA.deployStatus.ant1Undeployed,
    antenna_telemetry_R.sideA.deployStatus.ant1Undeployed,
    antenna_telemetry_R.sideA.deployStatus.ant1Undeployed,
    antenna_telemetry_R.sideA.deployStatus.ant1Undeployed,
    antenna_telemetry_R.sideA.deployStatus.armed,
    antenna_telemetry_R.sideA.boardTemp,
    antenna_telemetry_R.sideA.uptime,
    antenna_telemetry_R.sideB.deployStatus.ant1Undeployed,
    antenna_telemetry_R.sideB.deployStatus.ant1Undeployed,
    antenna_telemetry_R.sideB.deployStatus.ant1Undeployed,
    antenna_telemetry_R.sideB.deployStatus.ant1Undeployed,
    antenna_telemetry_R.sideB.deployStatus.armed,
    antenna_telemetry_R.sideB.boardTemp,
    antenna_telemetry_R.sideB.uptime);
}


void printCameraTelemetry(void) {
	infoPrint("uptime                  = %d\n\
	power Telemetry current_3V3	       = %u\n\
	power Telemetry current_5V	       = %u\n\
	power Telemetry current_SRAM_1	   = %u\n\
	power Telemetry current_SRAM_2	   = %u\n\
	power Telemetry overcurrent_SRAM_1 = %u\n\
	power Telemetry overcurrent_SRAM_2 = %u\n\
	camera One detectionThreshold      = %u\n\
	camera One autoAdjustMode          = %u\n\
	camera One exposure                = %u\n\
	camera One autoGainControl         = %u\n\
	camera One blueGain                = %u\n\
	camera One redGain                 = %u\n\
	camera Two detectionThreshold      = %u\n\
	camera Two autoAdjustMode          = %u\n\
	camera Two exposure                = %u\n\
	camera Two autoGainControl         = %u\n\
	camera Two blueGain                = %u\n\
	camera Two redGain                 = %u",
	camera_telemetry_R.uptime,
	camera_telemetry_R.powerTelemetry.current_3V3,
	camera_telemetry_R.powerTelemetry.current_5V,
	camera_telemetry_R.powerTelemetry.current_SRAM_1,
	camera_telemetry_R.powerTelemetry.current_SRAM_2,
	camera_telemetry_R.powerTelemetry.overcurrent_SRAM_1,
	camera_telemetry_R.powerTelemetry.overcurrent_SRAM_2,
	camera_telemetry_R.cameraOneTelemetry.detectionThreshold,
	camera_telemetry_R.cameraOneTelemetry.autoAdjustMode,
	camera_telemetry_R.cameraOneTelemetry.exposure,
	camera_telemetry_R.cameraOneTelemetry.autoGainControl,
	camera_telemetry_R.cameraOneTelemetry.blueGain,
	camera_telemetry_R.cameraOneTelemetry.redGain,
	camera_telemetry_R.cameraTwoTelemetry.detectionThreshold,
	camera_telemetry_R.cameraTwoTelemetry.autoAdjustMode,
	camera_telemetry_R.cameraTwoTelemetry.exposure,
	camera_telemetry_R.cameraTwoTelemetry.autoGainControl,
	camera_telemetry_R.cameraTwoTelemetry.blueGain,
	camera_telemetry_R.cameraTwoTelemetry.redGain
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

		infoPrint("TelemetryCollectionTask(): About to collect satellite telemetry data.\n");

		error = antennaTelemetry(&antenna_telemetry_R);
		if (error) warningPrint("antenna Telemetry error = %d", error);
		else infoPrint("antenna Telemetry collected");
		fileTransferAddMessage(&antenna_telemetry_R, sizeof(antenna_telemetry_R),file_transfer_AntennaTelemetry_tag );

		error = transceiverTelemetry((void*)&transceiver_telemetry_R);
		if (error) warningPrint("transceiver Telemetry error = %d", error);
		else infoPrint("transceiver Telemetry collected");
		fileTransferAddMessage(&transceiver_telemetry_R, sizeof(transceiver_telemetry_R),file_transfer_TransceiverTelemetry_tag );

		error = batteryTelemetry((void*)&battery_telemetry_R);
		if (error) warningPrint("battery Telemetry error = %d", error);
		else infoPrint("battery Telemetry collected");
		fileTransferAddMessage(&battery_telemetry_R, sizeof(battery_telemetry_R),file_transfer_BatteryTelemetry_tag );

		error = pdbTelemetry((void*)&pdb_telemetry_R);
		if (error) warningPrint("pdbTelemetry error = %d", error);
		else infoPrint("pdb Telemetry collected");
		fileTransferAddMessage(&pdb_telemetry_R, sizeof(pdb_telemetry_R),file_transfer_EpsTelemetry_tag );

		error = obcTelemetry((void*)&obc_telemetry_R);
		if (error) warningPrint("obcTelemetry error = %d", error);
		else infoPrint("obc Telemetry collected");
		fileTransferAddMessage(&obc_telemetry_R, sizeof(obc_telemetry_R),file_transfer_ObcTelemetry_tag );

		error = getSettings(&camera_telemetry_R);
		if (error) warningPrint("Camera Telemetry error = %d", error);
		else infoPrint("Camera Telemetry collected");
		fileTransferAddMessage(&camera_telemetry_R, sizeof(camera_telemetry_R), file_transfer_CameraTelemetry_tag );

		/*
		printAntennaTelemetry();
		printTransceiverTelemetry();
		printBatteryTelemetry();
		printPdbTelemetry();

		printCameraTelemetry();
		*/
		vTaskDelay(TELEMETRY_COLLECTION_TASK_DELAY_MS);
	}
}
