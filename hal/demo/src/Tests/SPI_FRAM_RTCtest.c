/*
 * SPIandFRAMtest.c
 *
 *  Created on: 25-Jan-2013
 *      Author: Akhil Piplani
 */

#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <freertos/projdefs.h>

#include <hal/interruptPriorities.h>
#include <hal/boolean.h>
#include <hal/Drivers/SPI.h>
#include <hal/Storage/FRAM.h>
#include <hal/Timing/RTC.h>
#include <hal/Utility/util.h>

#include <string.h>
#include <stdio.h>

#define TEST_RTC	1

#define FRAM_TEST_TRANSACTION_SIZE	1024

static unsigned char FRAMreadData[FRAM_TEST_TRANSACTION_SIZE] = {0};
static unsigned char FRAMwriteData[FRAM_TEST_TRANSACTION_SIZE] = {0};
static unsigned char FRAMwriteVerifyData[FRAM_TEST_TRANSACTION_SIZE] = {0};
static Boolean FRAMtestOnce = FALSE;

void SPIcallback(SystemContext context, xSemaphoreHandle semaphore) {
	signed portBASE_TYPE flag = pdFALSE;

	if(context == task_context) {
		xSemaphoreGive(semaphore);
	}
	else {
		xSemaphoreGiveFromISR(semaphore, &flag);
	}
}

void taskSPItest1() {
	int retValInt = 0;
	unsigned int i, j=0;
	SPIslaveParameters slaveParams;
	SPItransfer spiTransfer;
	xSemaphoreHandle txSemaphore = NULL;
	unsigned char readData[32] = {0}, writeData[32] = {0}, writeData2[32] = {0};
	TRACE_DEBUG("\n\r taskSPItest1: Starting. \n\r");

	writeData[0] = 0xEF;
	for(i=1; i<sizeof(writeData); i++) {
		writeData[i] = (unsigned char)(i*2);
		writeData2[i] = (unsigned char)(i*4);
	}

	vSemaphoreCreateBinary(txSemaphore);
	if(txSemaphore == NULL) {
		TRACE_WARNING("\n\r taskSPItest1: vSemaphoreCreateBinary failed! \n\r");
		while(1);
	}

	slaveParams.bus    = bus1_spi;
	slaveParams.mode   = mode0_spi;
	slaveParams.slave  = slave1_spi;
	slaveParams.dlybs  = 0;
	slaveParams.dlybct = 0;
	slaveParams.busSpeed_Hz = 600000;
	slaveParams.postTransferDelay = 4;

	spiTransfer.slaveParams = &slaveParams;
	spiTransfer.callback  = SPIcallback;
	spiTransfer.readData  = readData;
	spiTransfer.writeData = writeData;
	spiTransfer.transferSize = 10;
	spiTransfer.semaphore  = txSemaphore;

	while(1) {
		if(j%2 == 0) {
			spiTransfer.writeData = writeData;
		}
		else {
			spiTransfer.writeData = writeData2;
		}
		j++;

		if(xSemaphoreTake(txSemaphore, (portTickType)10) != pdTRUE) {
			TRACE_WARNING("\n\r taskSPItest1: xSemaphoreTake failed! \n\r");
			while(1);
		}
		retValInt = SPI_queueTransfer(&spiTransfer);
		if(retValInt != 0) {
			TRACE_WARNING("\n\r taskSPItest1: SPI_queueTransfer returned: %d! \n\r", retValInt);
			while(1);
		}
		else {
			// Make use of the transfer-time: Prepare the other writeBuffer while the transfer is in progress.
			if(j%2 == 0) {
				for(i=1; i<sizeof(writeData2); i++) {
					writeData2[i]++;
				}
			}
			else {
				for(i=1; i<sizeof(writeData); i++) {
					writeData[i]++;
				}
			}

			// Block on the semaphore waiting for the transfer to finish
			if(xSemaphoreTake(txSemaphore, (portTickType)portMAX_DELAY) != pdTRUE) {
				TRACE_WARNING("\n\r taskSPItest1: xSemaphoreTake failed! \n\r");
				while(1);
			}
			xSemaphoreGive(txSemaphore);
		}

		//TRACE_DEBUG(" taskSPItest1: received back: \n\r");
		//TRACE_DEBUG("0x%X ", readData[0]);
		for(i=1; i<spiTransfer.transferSize; i++) {
			//TRACE_DEBUG("0x%X ", readData[i]);
		}

		//TRACE_DEBUG(" \n\r\n\r");
		vTaskDelay(5);
	}
}

void taskSPItest2() {
	int retValInt = 0;
	unsigned int i;
	SPIslaveParameters slaveParams;
	SPItransfer spiTransfer;
	unsigned char readData[64] = {0}, writeData[64] = {0};
	TRACE_DEBUG("\n\r taskSPItest2: Starting. \n\r");

	writeData[0] = 0xEF;
	for(i=1; i<sizeof(writeData); i++) {
		writeData[i] = (unsigned char)(i);
		readData[i] = 0xEF;
	}

	slaveParams.bus    = bus1_spi;
	slaveParams.mode   = mode0_spi;
	slaveParams.slave  = slave1_spi;
	slaveParams.dlybs  = 1;
	slaveParams.dlybct = 1;
	slaveParams.busSpeed_Hz = 600000;
	slaveParams.postTransferDelay = 0;

	spiTransfer.slaveParams = &slaveParams;
	spiTransfer.callback  = SPIcallback;
	spiTransfer.readData  = readData;
	spiTransfer.writeData = writeData;
	spiTransfer.transferSize = 10;

	while(1) {

		retValInt = SPI_writeRead(&spiTransfer);
		if(retValInt != 0) {
			TRACE_WARNING("\n\r taskSPItest2: SPI_queueTransfer returned: %d! \n\r", retValInt);
			while(1);
		}

		//TRACE_DEBUG(" taskSPItest2: received back: \n\r");
		//TRACE_DEBUG("0x%X ", readData[0]);
		for(i=1; i<spiTransfer.transferSize; i++) {
			//TRACE_DEBUG("0x%X ", readData[i]);
			writeData[i]++;
			readData[i] = 0xEF;
		}
		writeData[i]++;

		//TRACE_DEBUG(" \n\r\n\r");
		vTaskDelay(5);
	}
}

void taskFRAMtest() {
	unsigned int i = 0, j = 0;
	FRAMblockProtect blocks;
	int retVal;
	unsigned char deviceID[9] = {0};
	unsigned int size = FRAM_TEST_TRANSACTION_SIZE;
	unsigned int address = 0x10000;

	TRACE_DEBUG(" Starting FRAM test \n\r");
	retVal = FRAM_start();
	if(retVal != 0) {
		TRACE_WARNING(" Error during FRAM_start: %d \n\r", retVal);
		while(1);
	}


	//while(1)
	{
		retVal = FRAM_getDeviceID(deviceID);
		if(retVal != 0) {
			TRACE_WARNING(" Error during FRAM_getDeviceID: %d \n\r", retVal);
			while(1);

		}
		TRACE_DEBUG_WP("Device ID: ");
		for(i=0; i<sizeof(deviceID); i++) {
			TRACE_DEBUG_WP("0x%02X ", deviceID[i]);
		}
		TRACE_DEBUG_WP("\n\r");
		TRACE_DEBUG_WP("Maximum address: 0x%08x\n\r", FRAM_getMaxAddress());
		vTaskDelay(500);
	}

	// Unprotect all blocks
	blocks.fields.blockProtect = 0;
	retVal = FRAM_protectBlocks(blocks);
	if(retVal != 0) {
		TRACE_WARNING(" Error during FRAM_protectBlocks: %d \n\r", retVal);
		while(1);
	}

	// TODO: Test block-protection!

	while(1) {
		retVal = FRAM_read(FRAMreadData, address, size);
		if(retVal != 0) {
			TRACE_WARNING(" Error during FRAM_read: %d \n\r", retVal);
			while(1);
		}

		TRACE_DEBUG_WP("\n\r FRAM readPacket contents: \n\r");
		for(i=0; i<size; i++) {
			TRACE_DEBUG_WP("0x%02X, ", FRAMreadData[i]);
			FRAMwriteData[i] = i*2 + j;
			FRAMwriteVerifyData[i] = FRAMwriteData[i] + 1;
			FRAMreadData[i] = 0;
		}
		TRACE_DEBUG_WP(" \n\r");

		retVal = FRAM_write(FRAMwriteData, address, size);
		if(retVal != 0) {
			TRACE_WARNING(" Error during FRAM_write: %d \n\r", retVal);
			while(1);
		}

		retVal = FRAM_writeAndVerify(FRAMwriteVerifyData, address, size);
		if(retVal != 0) {
			TRACE_WARNING(" Error during FRAM_writeAndVerify: %d \n\r", retVal);
			//while(1);
		}

		j++;

		if(FRAMtestOnce == TRUE) {
			break;
		}

		vTaskDelay(1000);
	}

}

void taskRTCtest() {
	int retVal;
	unsigned int i;
	float temperature;
	Time time;

	TRACE_DEBUG(" Starting RTC test \n\r");

	RTC_start();

	while(1) {
		retVal = RTC_testGetSet(); // This will set the seconds value to 7, amongst others
		if(retVal != 0) {
			TRACE_WARNING("RTC_testGetSet returned: %d \n\r", retVal);
		}

		for(i=0; i<5; i++) { // Should print 7-11 seconds
			retVal = RTC_getTime(&time);
			if(retVal != 0) {
				TRACE_WARNING("RTC_getTime returned: %d \n\r", retVal);
			}
			else {
				TRACE_DEBUG_WP("seconds: %d \n\r", time.seconds);
			}
			vTaskDelay(1001);
		}

		retVal = RTC_getTemperature(&temperature);
		if(retVal != 0) {
			TRACE_WARNING("RTC_getTemp returned: %d \n\r", retVal);
		}
		else {
			TRACE_DEBUG_WP("RTC temperature: %d.", (int)temperature);
			if(temperature < 0) {
				temperature = temperature * (-1);
			}
			temperature = temperature - (unsigned int)temperature;
			TRACE_DEBUG_WP("%d \n\r", (unsigned int)(temperature*100));
		}


	}
}

Boolean SPI_FRAM_RTCtest() {
	int retValInt = 0;

#if TEST_RTC
	xTaskHandle taskRTCtestHandle;
#else
	xTaskHandle taskFRAMtestHandle;
#endif

	retValInt = SPI_start(bus0_spi, slave0_spi);
	if(retValInt != 0) {
		TRACE_WARNING("\n\r SPIandFRAMtest: SPI_start returned %d! \n\r", retValInt);
		while(1);
	}
#if TEST_RTC
	// Perform a basic FRAM test and then test the RTC
	xTaskGenericCreate(taskRTCtest, (const signed char*)"taskRTCtest", 10240, NULL, 2, &taskRTCtestHandle, NULL, NULL);
#else
	//xTaskGenericCreate(taskSPItest1, (const signed char*)"taskSPItest1", 10240, NULL, 2, &taskSPItest1Handle, NULL, NULL);
	//xTaskGenericCreate(taskSPItest2, (const signed char*)"taskSPItest2", 10240, NULL, 2, &taskSPItest2Handle, NULL, NULL);
	xTaskGenericCreate(taskFRAMtest, (const signed char*)"taskFRAMtest", 10240, NULL, 2, &taskFRAMtestHandle, NULL, NULL);
#endif

	return FALSE;
}

