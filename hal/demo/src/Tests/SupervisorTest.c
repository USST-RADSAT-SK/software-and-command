/*
 * SupervisorTest.c
 *
 *  Created on: Jun 21, 2013
 *      Author: ffat
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <hal/boolean.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/SPI.h>
#include <hal/Drivers/I2C.h>
#include <hal/supervisor.h>
#include <hal/Utility/util.h>

#include <stdint-gcc.h>
#include <string.h>

static Boolean use_i2c_interface = FALSE;

/*!
 * @brief	Delays in multiples of 1µs based on NOP instructions (calibrated for release mode).
 * @warning Depends on clock speed. Needs to be adjusted.
 * @param[in] delay	Number of µs to wait for.
 */
static void uSdelay(volatile unsigned int delay) {
	delay *= 18;
	while(delay--) {
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
		__asm ("NOP");
	}
}

//
// Task for communicating with supervisor controller.
//
static void taskSupervisorTest()
{
	Boolean continueOperation = TRUE;
	uint8_t supervisorIndex;
	uint8_t supervisorI2Caddress = SUPERVISOR_DEFAULT_I2C_ADDRESS;

	if(use_i2c_interface)
	{
		supervisorIndex = 0;
	}
	else
	{
		supervisorIndex = 255;
	}

	printf("Starting Supervisor Controller test \n\r");
	if(use_i2c_interface)
	{
		Supervisor_start(&supervisorI2Caddress, 1);
	}
	else
	{
		Supervisor_start(NULL, 0);
	}

	/** .......................................................Getting version and configuration........................................................ */
	if(TRUE == continueOperation) {
		// Instance of Supervisor Controller Version and Configuration bytes.
		supervisor_version_configuration_t versionConfiguration;
		// Get the version from Supervisor Controller.
		Supervisor_getVersion(&versionConfiguration, supervisorIndex);

		// Some variables to print date and time.
		char tempStringDate[12] = {0};
		char tempStringTime[9] = {0};
		memcpy(tempStringDate, versionConfiguration.fields.compileInformation, 11);
		memcpy(tempStringTime, (versionConfiguration.fields.compileInformation + 11), 8);

		// Printing the Version and Configuration.
		printf("\n\rVersion and Configuration \n\r");
		printf("SPI Command Status: %d \n\r", versionConfiguration.fields.spiCommandStatus);
		printf("Index of Subsystem: 0x%X \n\r", versionConfiguration.fields.indexOfSubsystem);
		printf("Software Version  : %c.%c.%c_%d \n\r", versionConfiguration.fields.majorVersion, versionConfiguration.fields.minorVersion, versionConfiguration.fields.patchVersion, (int)versionConfiguration.fields.gitHeadVersion);
		printf("Serial Number     : %d \n\r", versionConfiguration.fields.serialNumber);
		printf("Compile Info      : %s_%s \n\r", tempStringDate, tempStringTime);
		printf("Code Type         : %c \n\r", versionConfiguration.fields.codeType);

		continueOperation = (0x1C == versionConfiguration.fields.indexOfSubsystem)? TRUE : FALSE;

		if(continueOperation == FALSE) {
			printf("\n\r Incorrect system index, aborting test. \n\r");
		}
	}

	if(TRUE == continueOperation) {
		supervisor_generic_reply_t reply;

		printf("\n\r [Supervisor Controller Detected] \n\r");

		// Select which test to perform.
		printf("\n\r Select a test to perform: \n\r");
		printf("\t 1) Housekeeping \n\r");
		printf("\t 2) Power Cycle IOBC \n\r");
		printf("\t 3) Reset Supervisor Controller \n\r");
		printf("\t 4) Status Monitoring \n\r");

		unsigned int selection = 0;
		continueOperation = FALSE;
		while(FALSE == continueOperation) {
			continueOperation = UTIL_DbguGetIntegerMinMax(&selection, 1, 5)? TRUE : FALSE;
		}

		// Instance of housekeeping bytes.
		supervisor_housekeeping_t houseKeeping;
		Supervisor_getHousekeeping(&houseKeeping, supervisorIndex);
		unsigned int testRun = 1;
		switch(selection) {
			case 1:
				/** .......................................................Getting telemetry........................................................ */

				// Printing the Housekeeping telemetry.
				printf("\n\r Housekeeping Telemetry \n\r");
				while(1) {
					vTaskDelay(500);

					Supervisor_getHousekeeping(&houseKeeping, supervisorIndex);

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

					int16_t adcValue[SUPERVISOR_NUMBER_OF_ADC_CHANNELS] = {0};
					Supervisor_calculateAdcValues(&houseKeeping, adcValue);
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

				}
				break;
			case 2:
				// Printing the Housekeeping telemetry.
				printf("\n\r Power Cycle IOBC \n\r");
				Supervisor_powerCycleIobc(&reply, supervisorIndex);
				while(1) {
					vTaskDelay(10000);
				}
				break;
			case 3:
				printf("\n\r Resetting the Supervisor Microcontroller \n\r");
				Supervisor_reset(&reply, supervisorIndex);
				while(1) {
					vTaskDelay(10000);
				}
				break;
			case 4:
				while(testRun != 0) {
					unsigned int delayInBetween = 0, seconds = 0, j = 0;
					// Getting the required information.
					printf("Insert delay in us in between watchdog kicks. (range 1 - 1000000) \n\r");
					while(UTIL_DbguGetIntegerMinMax(&delayInBetween, 1, 1000000) == 0);
					printf("Insert how many seconds should this test last. (range 1 - 30) \n\r");
					while(UTIL_DbguGetIntegerMinMax(&seconds, 1, 30) == 0);

					printf("Delay every pulse: %d microseconds | Total time: %d seconds \n\r", delayInBetween, seconds);

					// Printing the Housekeeping telemetry.
					printf("\n\r Status monitoring IOBC Supervisor \n\r");
					while(seconds) {
						WDT_forceKick();
						// check uSdelay with pin.
						uSdelay(delayInBetween);
						j+=delayInBetween;
						if(j >= 1000000) {
							j = 0;
							seconds--;
						}
					}

					printf("Test Complete \n\r");
					printf("\n\r Perform another test? 1 / 0 ? \n\r");
					while(UTIL_DbguGetIntegerMinMax(&testRun, 0, 1) == 0);
				}
				break;
		}
	}
}

#define SUPERVISOR_TEST_I2C_CLOCK_SPEED		100000
#define SUPERVISOR_TEST_I2C_TIMEOUT_MS		10

//
//      Starting the Supervisor Controller Test.
//
Boolean SupervisorTest(Boolean use_i2c)
{
	xTaskHandle taskSupervisorHandle;
	int spiStartStatus, i2cStartStatus;

	use_i2c_interface = use_i2c;

	spiStartStatus = SPI_start(bus0_spi, slave0_spi);
	if(0 != spiStartStatus) {
		TRACE_FATAL("\n\r SupervisorTest: SPI_start returned %d! \n\r", spiStartStatus);
	}

	i2cStartStatus = I2C_start(SUPERVISOR_TEST_I2C_CLOCK_SPEED, SUPERVISOR_TEST_I2C_TIMEOUT_MS);
	if(i2cStartStatus != 0) {
		TRACE_FATAL("\n\r SupervisorTest: I2C_start returned: %d! \n\r", i2cStartStatus);
	}

	// Create the Supervisor Test Task.
	xTaskGenericCreate(taskSupervisorTest, (const signed char*) "Supervisor Test", 8192, NULL, 2, &taskSupervisorHandle, NULL, NULL);

	return FALSE;
}
