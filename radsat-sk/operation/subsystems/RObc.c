/**
 * @file RObc.c
 * @date April 2, 2022
 * @author Matthew Buglass (mab839)
 */

#include <RObc.h>
#include <RCommon.h>
#include <hal/errors.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Timing/RTC.h>



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Retrieve Temperature telemetry from OBC
 *
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int obcTelemetry(obc_telemetry_t* telemetry){
	float temperature = 0;
	int error = RTC_getTemperature(&temperature);
	if (error != SUCCESS) {
		// TODO: record errors (if present) to System Manager
		return error;
	}

	telemetry->temperature = temperature;

	supervisor_housekeeping_t houseKeeping;
	error = Supervisor_getHousekeeping(&houseKeeping, SUPERVISOR_INDEX);
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
	telemetry->voltageRef   = adcValue[_voltage_reference_2v5];
	telemetry->voltageRtc 	= adcValue[_voltage_measurement_rtc];
	telemetry->voltage3v3 	= adcValue[_voltage_measurement_3v3];
	telemetry->voltage1v8 	= adcValue[_voltage_measurement_1v8];
	telemetry->voltage1v0 	= adcValue[_voltage_measurement_1v0];

	telemetry->current3v3 = adcValue[_current_measurement_3v3];
	telemetry->current1v8 = adcValue[_current_measurement_1v8];
	telemetry->current1v0 = adcValue[_current_measurement_1v0];

#ifdef TEST
	printf("Supervisor Uptime       : %03d:%02d:%02d \n\r", (int)(houseKeeping.fields.supervisorUptime / 3600), (int)(houseKeeping.fields.supervisorUptime % 3600) / 60, (int)(houseKeeping.fields.supervisorUptime % 3600) % 60);
	printf("IOBC Uptime             : %03d:%02d:%02d \n\r", (int)(houseKeeping.fields.iobcUptime / 3600), (int)(houseKeeping.fields.iobcUptime % 3600) / 60, (int)(houseKeeping.fields.iobcUptime % 3600) % 60);
	printf("IOBC Power Cycle Count  : %d \n\r", (int)(houseKeeping.fields.iobcResetCount));

	supervisor_enable_status_t* temporaryEnable = &(houseKeeping.fields.enableStatus);
	printf("\n\r Supervisor Enable Status \n\r");
	printf("Power OBC               : %d \n\r", temporaryEnable->fields.powerObc);
	printf("Power RTC               : %d \n\r", temporaryEnable->fields.powerRtc);
	printf("Is in Supervisor Mode   : %d \n\r", temporaryEnable->fields.isInSupervisorMode);
	printf("Busy RTC                : %d \n\r", temporaryEnable->fields.busyRtc);
	printf("Power off RTC           : %d \n\r", temporaryEnable->fields.poweroffRtc);

	printf("\n\r Analog to Digital Channels [Update Flag: 0x%02X] \n\r", houseKeeping.fields.adcUpdateFlag);
	printf("_temperature_measurement: %04d | %d C \n\r\n\r", houseKeeping.fields.adcData[_temperature_measurement], adcValue[_temperature_measurement]);

	printf("_voltage_measurement_3v3in: %04d | %d mV \n\r", houseKeeping.fields.adcData[_voltage_measurement_3v3in], adcValue[_voltage_measurement_3v3in]);
	printf("_voltage_reference_2v5    : %04d | %d mV \n\r", houseKeeping.fields.adcData[_voltage_reference_2v5], adcValue[_voltage_reference_2v5]);
	printf("_voltage_measurement_rtc  : %04d | %d mV \n\r", houseKeeping.fields.adcData[_voltage_measurement_rtc], adcValue[_voltage_measurement_rtc]);
	printf("_voltage_measurement_3v3  : %04d | %d mV \n\r", houseKeeping.fields.adcData[_voltage_measurement_3v3], adcValue[_voltage_measurement_3v3]);
	printf("_voltage_measurement_1v8  : %04d | %d mV \n\r", houseKeeping.fields.adcData[_voltage_measurement_1v8], adcValue[_voltage_measurement_1v8]);
	printf("_voltage_measurement_1v0  : %04d | %d mV \n\r\n\r", houseKeeping.fields.adcData[_voltage_measurement_1v0], adcValue[_voltage_measurement_1v0]);

	printf("_current_measurement_3v3  : %04d | %d mA \n\r", houseKeeping.fields.adcData[_current_measurement_3v3], adcValue[_current_measurement_3v3]);
	printf("_current_measurement_1v8  : %04d | %d mA \n\r", houseKeeping.fields.adcData[_current_measurement_1v8], adcValue[_current_measurement_1v8]);
	printf("_current_measurement_1v0  : %04d | %d mA \n\r\n\r", houseKeeping.fields.adcData[_current_measurement_1v0], adcValue[_current_measurement_1v0]);
#endif

	return SUCCESS;

}

