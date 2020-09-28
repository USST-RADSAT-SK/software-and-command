/*
 * boardTest.c
 *
 *  Created on: 05-Mar-2014
 *      Author: apip
 */

#include "ADCtest.h"
#include "SDCardTest.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <hal/Drivers/I2C.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/PWM.h>
#include <hal/Drivers/SPI.h>
#include <hal/Drivers/ADC.h>
#include <hal/Timing/RTC.h>
#include <hal/Storage/FRAM.h>
#include <hal/supervisor.h>

#include <string.h>

#define FRAM_TEST_TRANSACTION_SIZE	1024
static unsigned char FRAMwriteVerifyData[FRAM_TEST_TRANSACTION_SIZE] = {0};

void taskBoardTest() {
	unsigned int i = 0, j;
	int retVal;
	float temperature;
	unsigned int pwmVals[6] = {16, 32, 48, 64, 80, 96};
	unsigned int dummyI2Cdata = 0xAA55AA55;
	supervisor_housekeeping_t supervisorHK;
	short supervisorADCvalues[SUPERVISOR_NUMBER_OF_ADC_CHANNELS] = {0};

	memset(&supervisorHK, 0, sizeof(supervisorHK));

	printf("\n The board test combines the tests for the following parts: I2C, SD-Cards, PWM, ADC, LEDs, RTC, FRAM, Supervisor \n");
	printf(" Note: This is not a comprehensive test for the board!\n The following tests must be performed to fully verify the board: \n");
	printf(" UART, Aux Pins, USB Device, USB Master, memtest. \n");

	printf(" The board test will now start. Please verify the following: \n");
	printf("   1) There should be an attempt for I2C transaction to slave 0x20. \n");
	printf("   2) LED's should wave. \n");
	printf("   3) PWM pins should have a frequency of 1.007kHz and duty cycles of %d, %d, %d, %d, %d, %d \n", pwmVals[0], pwmVals[1], pwmVals[2], pwmVals[3], pwmVals[4], pwmVals[5]);
	printf("   4) ADC readouts, RTC temperature and Supervisor telemetry being printed is within expected range. \n");
	printf("   5) There are no failure messages being printed. \n");

	vTaskDelay(5000); // Delay to give the user to read the above before test results fill their screen.

	LED_start();
	LED_wave(2);

	retVal = I2C_start(100, 50);
	if(retVal != 0) {
		printf("\n Error during I2C_start: %d! \n", retVal);
	}

	retVal = SPI_start(bus0_spi, slave0_spi);
	if(retVal != 0) {
		printf("\n Error during SPI_start: %d! \n", retVal);
	}

	retVal = RTC_start();
	if(retVal != 0) {
		printf("\n Error during SPI_start: %d! \n", retVal);
	}

	retVal = FRAM_start();
	if(retVal != 0) {
		printf(" Error during FRAM_start: %d \n", retVal);
	}

	retVal = Supervisor_start(NULL, 0);
	if(retVal != 0) {
		printf(" Error during Supervisor_start: %d \n", retVal);
	}

	// PWM test only needs to be initiated once.
	PWM_start(TC_UPCOUNT_MAXVAL, 0); // This should make the PWM frequency = 1.007kHz.
	retVal = PWM_setDutyCycles(pwmVals);
	if(retVal != 0) {
		printf("PWM test failed! \n");
	}

	retVal = ADCtest_initalize();
	if(retVal != 0) {
		printf(" Error during ADCtest_initalize: %d \n", retVal);
	}

	while(1) {

		// Supervisor housekeeping telemetry.
		Supervisor_getHousekeeping(&supervisorHK, 255);
		Supervisor_calculateAdcValues(&supervisorHK, supervisorADCvalues);
		printf("Supervisor Uptime       : %03d:%02d:%02d \n\r", (int)(supervisorHK.fields.supervisorUptime / 3600), (int)(supervisorHK.fields.supervisorUptime % 3600) / 60, (int)(supervisorHK.fields.supervisorUptime % 3600) % 60);
		printf("Analog to Digital Channels [Update Flag: 0x%02X] \n\r", supervisorHK.fields.adcUpdateFlag);
		printf("_temperature_measurement: %04d | %d C \n\r\n\r", supervisorHK.fields.adcData[_temperature_measurement], supervisorADCvalues[_temperature_measurement]);

		printf("_voltage_measurement_3v3in: %04d | %d mV \n\r", supervisorHK.fields.adcData[_voltage_measurement_3v3in], supervisorADCvalues[_voltage_measurement_3v3in]);
		printf("_voltage_reference_2v5    : %04d | %d mV \n\r", supervisorHK.fields.adcData[_voltage_reference_2v5], supervisorADCvalues[_voltage_reference_2v5]);
		printf("_voltage_measurement_rtc  : %04d | %d mV \n\r", supervisorHK.fields.adcData[_voltage_measurement_rtc], supervisorADCvalues[_voltage_measurement_rtc]);
		printf("_voltage_measurement_3v3  : %04d | %d mV \n\r", supervisorHK.fields.adcData[_voltage_measurement_3v3], supervisorADCvalues[_voltage_measurement_3v3]);
		printf("_voltage_measurement_1v8  : %04d | %d mV \n\r", supervisorHK.fields.adcData[_voltage_measurement_1v8], supervisorADCvalues[_voltage_measurement_1v8]);
		printf("_voltage_measurement_1v0  : %04d | %d mV \n\r\n\r", supervisorHK.fields.adcData[_voltage_measurement_1v0], supervisorADCvalues[_voltage_measurement_1v0]);

		printf("_current_measurement_3v3  : %04d | %d mA \n\r", supervisorHK.fields.adcData[_current_measurement_3v3], supervisorADCvalues[_current_measurement_3v3]);
		printf("_current_measurement_1v8  : %04d | %d mA \n\r", supervisorHK.fields.adcData[_current_measurement_1v8], supervisorADCvalues[_current_measurement_1v8]);
		printf("_current_measurement_1v0  : %04d | %d mA \n\r\n\r", supervisorHK.fields.adcData[_current_measurement_1v0], supervisorADCvalues[_current_measurement_1v0]);


		// RTC test
		retVal = RTC_testGetSet();
		if(retVal != 0) {
			printf("RTC test Failed! \n");
		}
		else {
			printf("RTC test OK \n");
		}

		RTC_getTemperature(&temperature);
		printf("RTC temperature: %fC \n", temperature);

		// FRAM test
		for(j=0; j<FRAM_TEST_TRANSACTION_SIZE; j++) {
			FRAMwriteVerifyData[j] = i+j;
		}
		retVal = FRAM_writeAndVerify(FRAMwriteVerifyData, 0x10000, FRAM_TEST_TRANSACTION_SIZE);
		if(retVal != 0) {
			printf("FRAM test Failed! \n");
		}
		else {
			printf("FRAM test OK \n");
		}

		// LED test.
		LED_wave(2);

		// ADC readouts.
		ADCtest_printReadout();

		// No error checking on I2C as it will fail if there is no slave but can still be verified to be working using a scope.
		I2C_write(0x20, (unsigned char*)&dummyI2Cdata, sizeof(dummyI2Cdata));

		// SD-Card / File system test.
//		SDCardTest();

		i++;

		vTaskDelay(5000);
	}
}

Boolean boardTest() {
	xTaskHandle taskHandle;

	xTaskGenericCreate(taskBoardTest, (const signed char*)"taskBoardTest", 4096, NULL, 2, &taskHandle, NULL, NULL);

	return FALSE;
}
