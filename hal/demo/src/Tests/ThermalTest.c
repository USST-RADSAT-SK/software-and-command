/*
 * ThermalTest.c
 *
 *  Created on: 11-Sep-2013
 *      Author: Akhil Piplani
 */

#include "Tests/SDCardTest.h"
#include "Tests/PWMtest.h"

#include <at91/commons.h>
#include <at91/utility/trace.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <hal/boolean.h>
#include <hal/Drivers/ADC.h>
#include <hal/errors.h>
#include <hal/supervisor.h>
#include <hal/Drivers/SPI.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/UART.h>
#include <hal/Drivers/I2C.h>
#include <hal/Timing/RTC.h>
#include <hal/Storage/FRAM.h>
#include <hal/Storage/NORflash.h>

#include <stdlib.h>
#include <string.h>

#define FRAM_TEST_TRANSACTION_SIZE	1024
#define FRAM_TEST_ADDRESS			0x10000
static unsigned char FRAMwriteVerifyData[2][FRAM_TEST_TRANSACTION_SIZE] = {{0}};

static xSemaphoreHandle ADCsemaphore = NULL;
static void *CurrentADCsamplesBuffer = NULL;
static ADCparameters ADCparams;

void thermalADCcallback(SystemContext context, void* adcSamples) {
	signed portBASE_TYPE flag = pdFALSE;

	CurrentADCsamplesBuffer = adcSamples;

	if(context == task_context) {
		xSemaphoreGive(ADCsemaphore);
	}
	else {
		xSemaphoreGiveFromISR(ADCsemaphore, &flag);
	}
}

static void initADCtest() {
	int retVal;
	ADCparams.channels = 8;
	ADCparams.resolution10bit = FALSE;
	ADCparams.sampleRate = 5000;
	ADCparams.samplesInBufferPerChannel = 2048;
	ADCparams.callback = thermalADCcallback;

	vSemaphoreCreateBinary(ADCsemaphore);
	if(ADCsemaphore == NULL) {
		TRACE_WARNING("\n\r initADCtest: vSemaphoreCreateBinary failed! \n\r");
		while(1);
	}
	if(xSemaphoreTake(ADCsemaphore, (portTickType)10) != pdTRUE) {
		TRACE_WARNING("\n\r initADCtest: xSemaphoreTake failed! \n\r");
		while(1);
	}

	retVal = ADC_start(ADCparams);
	if(retVal != 0) {
		printf(" \n\r taskADCtest: ADC_start returned %d \n\r", retVal);
		while(1);
	}
}

static void initRTCandFRAMtest()
{
	int retVal = 0;
	unsigned int i;

	retVal = SPI_start(bus0_spi, slave0_spi);
	if(retVal != 0) {
		TRACE_WARNING("\n\r initRTCandFRAMtest: SPI_start returned %d! \n\r", retVal);
		while(1);
	}

	retVal = RTC_start();
	if(retVal != 0) {
		TRACE_WARNING("\n\r initRTCandFRAMtest: RTC_start returned %d! \n\r", retVal);
		while(1);
	}

	retVal = FRAM_start();
	if(retVal != 0) {
		TRACE_WARNING("\n\r initRTCandFRAMtest: Error during FRAM_start: %d \n\r", retVal);
		while(1);
	}

	for(i=0; i<FRAM_TEST_TRANSACTION_SIZE; i++) {
		FRAMwriteVerifyData[0][i] = 0xAA;
		FRAMwriteVerifyData[1][i] = 0x55;
	}
}

static void initUARTtest()
{
	int retVal;
	UARTconfig configBus0 = {.mode = AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT,
								.baudrate = 115200, .timeGuard = 1, .busType = rs232_uart, .rxtimeout = 0xFFFF};


	UARTconfig configBus2 = {.mode = AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT,
								.baudrate = 115200, .timeGuard = 1, .busType = rs232_uart, .rxtimeout = 0xFFFF};

	retVal = UART_start(bus0_uart, configBus0);
	if(retVal != 0) {
		TRACE_FATAL("\n\r initUARTtest: UART_start returned %d! \n\r", retVal);
	}

	retVal = UART_start(bus2_uart, configBus2);
	if(retVal != 0) {
		TRACE_FATAL("\n\r initUARTtest: UART_start returned %d! \n\r", retVal);
	}
}

static void performSupervisorTest(unsigned char index) {
	supervisor_housekeeping_t houseKeeping = {{0}};
	supervisor_enable_status_t* enableStatus = &(houseKeeping.fields.enableStatus);
	int16_t adcValue[SUPERVISOR_NUMBER_OF_ADC_CHANNELS] = {0};
	int retVal;

	retVal = Supervisor_getHousekeeping(&houseKeeping, index);
	if(retVal != E_NO_SS_ERR) {
		TRACE_WARNING("\n\r performSupervisorTest: Supervisor_getHousekeeping returned: %d! \n\r", retVal);
		return;

	}

	printf("Supervisor Uptime       : %03d:%02d:%02d \n\r", (int)(houseKeeping.fields.supervisorUptime / 3600), (int)(houseKeeping.fields.supervisorUptime % 3600) / 60, (int)(houseKeeping.fields.supervisorUptime % 3600) % 60);
	printf("IOBC Uptime             : %03d:%02d:%02d \n\r", (int)(houseKeeping.fields.iobcUptime / 3600), (int)(houseKeeping.fields.iobcUptime % 3600) / 60, (int)(houseKeeping.fields.iobcUptime % 3600) % 60);
	printf("IOBC Power Cycle Count  : %d \n\r", (int)(houseKeeping.fields.iobcResetCount));


	printf("\n\r Supervisor Enable Status \n\r");
	printf("Power OBC               : %d \n\r", enableStatus->fields.powerObc);
	printf("Power RTC               : %d \n\r", enableStatus->fields.powerRtc);
	printf("Is in Supervisor Mode   : %d \n\r", enableStatus->fields.isInSupervisorMode);
	printf("Busy RTC                : %d \n\r", enableStatus->fields.busyRtc);
	printf("Power off RTC           : %d \n\r", enableStatus->fields.poweroffRtc);

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

static void performUARTtest(Boolean sendPause) {
	int retVal;
	UARTbus  bus = bus0_uart;
	unsigned int bytes = 4, i;
	unsigned char buffer[16] = {0};
	unsigned char pause[] = "PPPP";

	for(i=0; i<2; i++) {
		retVal = UART_read(bus, buffer, bytes);
		if(retVal != 0) {
			TRACE_WARNING("\n\r performUARTtest: UART_read returned: %d for bus %d \n\r", retVal, bus);
		}

		// Send a special pause string over bus2_uart if sendPause is true, loopback otherwise.
		if(sendPause != FALSE && i==1) {
			retVal = UART_write(bus, pause, bytes);
		}
		else {
			retVal = UART_write(bus, buffer, bytes);
		}
		if(retVal != 0) {
			TRACE_WARNING("\n\r performUARTtest: UART_write returned: %d for bus %d \n\r", retVal, bus);
		}

		bus = bus2_uart;
	}
}

static void initSupervisortest(Boolean SPIonly)
{
	int retVal;
	unsigned char supervisorAddress = 0x14;


	retVal = I2C_start(100000, 10);
	if(retVal != 0) {
		TRACE_FATAL("\n\r initSupervisortest: I2C_start returned: %d! \n\r", retVal);
	}

	retVal = SPI_start(bus0_spi, slave0_spi);
	if(retVal != 0) {
		TRACE_FATAL("\n\r initSupervisortest: SPI_start returned %d! \n\r", retVal);
	}

	if(SPIonly != FALSE) {
		retVal = Supervisor_start(&supervisorAddress, 0);
	}
	else {
		retVal = Supervisor_start(&supervisorAddress, 1);
	}
	if(retVal != E_NO_SS_ERR) {
		TRACE_FATAL("\n\r initSupervisortest: Supervisor_initialize returned: %d! \n\r", retVal);
	}
}


void taskThermalTest_UART2andSupervisorSPI()
{
	// Two boards are tested simultaneously, to qualify the different configurations for UART2 transceiver.
	// The second one only tests the UART2 and supervisor interface over SPI using this task.

	int retVal;
	unsigned int bytes = 4;
	unsigned char buffer[16] = {0};
	UARTconfig configBus2 = {. mode = AT91C_US_USMODE_HWHSH | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT,
								.baudrate = 115200, .timeGuard = 1, .busType = rs232_uart, .rxtimeout = 0xFFFF};

	printf("taskThermalTest_UART2andSupervisorSPI: Initializing. \n\r\n\r");

	retVal = UART_start(bus2_uart, configBus2);
	if(retVal != 0) {
		TRACE_WARNING("\n\r taskThermalTest_UART2only: UART_start returned %d! \n\r", retVal);
		while(1);
	}

	initSupervisortest(TRUE); // Testing supervisor over SPI.

	printf("taskThermalTest_UART2andSupervisorSPI: Done initializing. \n\r");

	while(1) {
		retVal = UART_read(bus2_uart, buffer, bytes);
		if(retVal != 0) {
			TRACE_WARNING("\n\r taskThermalTest_UART2only: UART_read returned: %d for bus %d \n\r", retVal, bus2_uart);
		}

		retVal = UART_write(bus2_uart, buffer, bytes);
		if(retVal != 0) {
			TRACE_WARNING("\n\r taskThermalTest_UART2only: UART_write returned: %d for bus %d \n\r", retVal, bus2_uart);
		}

		performSupervisorTest(SUPERVISOR_SPI_INDEX);

		vTaskDelay(10);
	}
}

void taskThermalTest() {
	unsigned int testIteration, i;
	void *framData = 0;
	int retVal, retValFRAM, retValRTC, retValNORFlash;
	unsigned char *ADCsample;

	printf("\n\r taskThermalTest: Initializing. \n\r\n\r");

	initADCtest();
	initRTCandFRAMtest();
	PWMtest(); // This will generate varying waveforms on all PWM channels.
	initUARTtest();
	initSupervisortest(FALSE); // Testing supervisor over I2C.

	retVal = NORflash_start();
	if(retVal != 0) {
		TRACE_WARNING("\n\r taskThermalTest: Error during NORflash_init: %d \n\r", retVal);
		while(1);
	}

	printf("\n\r taskThermalTest: Done initializing. \n\r");

	for(testIteration=0; testIteration<10; testIteration++) {
		// NOR flash test
		retValNORFlash= NORflash_test(testIteration);
		if(retValNORFlash != 0) {
			TRACE_WARNING(" taskThermalTest: Error during NORflash_test: %d \n\r", retValNORFlash);
		}

		// Test FRAM
		if(testIteration%2 == 0) {
			framData = &FRAMwriteVerifyData[0][0];
		}
		else {
			framData = &FRAMwriteVerifyData[1][0];
		}
		retValFRAM = FRAM_writeAndVerify(framData, FRAM_TEST_ADDRESS, FRAM_TEST_TRANSACTION_SIZE);
		if(retValFRAM != 0) {
			TRACE_WARNING(" taskThermalTest: Error during FRAM_writeAndVerify: %d \n\r", retValFRAM);
		}

		// Test RTC
		retValRTC = RTC_testGetSet();
		if(retValRTC != 0) {
			TRACE_WARNING(" taskThermalTest: Error during RTC_testGetSet: %d \n\r", retValRTC);
		}

		// XXX: Test SD-Cards
#if(0)
		fileSystemTest(card0, testIteration, TRUE);
		vTaskDelay(10);
		fileSystemTest(card1, testIteration+1, TRUE);
#endif
		SDCardTest();
		vTaskDelay(10);

		// Test ADC
		if(xSemaphoreTake(ADCsemaphore, (portTickType)portMAX_DELAY) != pdTRUE) {
			TRACE_WARNING("\n\r taskThermalTest: xSemaphoreTake failed! \n\r");
		}
		ADCsample = CurrentADCsamplesBuffer;
		printf("\n\r  taskThermalTest: ADC data: \n\r");
		for(i=0; i<ADCparams.channels; i++) {
			printf("0x%03X ", ADCsample[i]);
			if((i+1)%ADCparams.channels == 0) {
				printf("\n\r");
			}
		}

		// Test LEDs
		LED_wave(1);
		LED_waveReverse(1);
		LED_wave(1);
		LED_waveReverse(1);

		// Test UART
		printf("\n\r taskThermalTest: Testing UART \n\r\n\r");
		performUARTtest(FALSE);

		vTaskDelay(10);

		// Test I2C by talking to the supervisor over I2C.
		performSupervisorTest(0);

		vTaskDelay(10);
	}


	printf("\n\r taskThermalTest: %d iterations finished, expect reboot soon. \n\r", testIteration);

	// Send the string PPPP over UART, this will cause the PC side program to stop sending characters for the UART test.
	printf("\n\r taskThermalTest: Sending Pause command over UART. \n\r");
	performUARTtest(TRUE);

	// This would cause the watchdog to reset the ARM, this is done on purpose to test the watchdog.
	while(1);

}

Boolean thermalTest() {
	xTaskHandle taskThermalTestHandle;

//	xTaskGenericCreate(taskThermalTest, (const signed char*)"taskThermalTest", 1024, NULL, 2, &taskThermalTestHandle, NULL, NULL);
	xTaskGenericCreate(taskThermalTest_UART2andSupervisorSPI, (const signed char*)"taskThermalTest_UART..", 1024, NULL, 2, &taskThermalTestHandle, NULL, NULL);

	return FALSE;
}
