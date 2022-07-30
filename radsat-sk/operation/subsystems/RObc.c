/**
 * @file RObc.c
 * @date April 2, 2022
 * @author Matthew Buglass (mab839)
 */

#include <RObc.h>
#include <RCommon.h>
#include <hal/supervisor.h>



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Retrieve Temperature telemetry from OBC
 *
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int obcTelemetry(obc_telemetry_t* telemetry){
	supervisor_housekeeping_t houseKeeping;
	int error = Supervisor_getHousekeeping(&houseKeeping, SUPERVISOR_INDEX);
	if (error != SUCCESS) {
		// TODO: record errors (if present) to System Manager
		return error;
	}

	int16_t adcValue[SUPERVISOR_NUMBER_OF_ADC_CHANNELS] = {0};
	Supervisor_calculateAdcValues(&houseKeeping, adcValue);

	telemetry->supervisorUptime    = houseKeeping.fields.supervisorUptime;
	telemetry->IobcUptime          = houseKeeping.fields.iobcUptime;
	telemetry->IobcPowerCycleCount = houseKeeping.fields.iobcResetCount;
	telemetry->temperature = adcValue[_temperature_measurement];

	telemetry->voltage3v3In = adcValue[_voltage_measurement_3v3in];
	telemetry->voltage3v3 	= adcValue[_voltage_measurement_3v3];
	telemetry->voltage1v8 	= adcValue[_voltage_measurement_1v8];
	telemetry->voltage1v0 	= adcValue[_voltage_measurement_1v0];
	telemetry->current3v3 = adcValue[_current_measurement_3v3];



	return SUCCESS;
}

#ifdef TEST
void debugPrintObcTelemetry(obc_telemetry_t* telemetry){
	debugPrint("Supervisor Uptime       : %03d:%02d:%02d \n\r", (int)(telemetry->supervisorUptime / 3600), (int)(telemetry->supervisorUptime % 3600) / 60, (int)(telemetry->supervisorUptime % 3600) % 60);
	debugPrint("IOBC Uptime             : %03d:%02d:%02d \n\r", (int)(telemetry->IobcUptime / 3600), (int)(telemetry->IobcUptime % 3600) / 60, (int)(telemetry->IobcUptime % 3600) % 60);
	debugPrint("IOBC Power Cycle Count  : %d \n\r", (int)(telemetry->IobcPowerCycleCount));

	debugPrint("\n\r Analog to Digital Channels\n\r");
	debugPrint("temperature  : %d C \n\r\n\r", telemetry->temperature);
	debugPrint("voltage 3v3in: %d mV \n\r", telemetry->voltage3v3In);
	debugPrint("voltage 2v5  : %d mV \n\r", telemetry->voltage3v3);
	debugPrint("voltage 1v8  : %d mV \n\r", telemetry->voltage1v8);
	debugPrint("voltage 1v0  : %d mV \n\r", telemetry->voltage1v0);
	debugPrint("current 3v3  : %d mA \n\r", telemetry->current3v3);
}
#endif
