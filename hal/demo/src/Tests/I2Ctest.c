/*
 * I2Ctest.c
 *
 *  Created on: 23-Jan-2013
 *      Author: Akhil Piplani
 */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <at91/commons.h>
#include <at91/utility/trace.h>

#include <hal/boolean.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/I2C.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void I2Ccallback(SystemContext context, xSemaphoreHandle semaphore) {
	signed portBASE_TYPE flag = pdFALSE;

	if(context == task_context) {
		xSemaphoreGive(semaphore);
	}
	else {
		xSemaphoreGiveFromISR(semaphore, &flag);
	}
	//TRACE_DEBUG(" I2C_callbackTask1: Transfer complete. \n\r");
}

int doBlockingI2CTransfer(xSemaphoreHandle semaphore, I2CgenericTransfer *tx) {
	int retVal;

	if(xSemaphoreTake(semaphore, (portTickType)10) != pdTRUE) { // This should never happen if we coded things correctly.
		TRACE_WARNING(" doBlockingI2CTransfer: xSemaphoreTake failed! \n\r");
		while(1);
	}

	retVal = I2C_queueTransfer(tx);
	if(retVal != 0) { // This would never happen unless there are >64 transfers queued into the driver.
		return retVal;
	}

	// Block on the semaphore waiting for the transfer to finish.
	// This will swap the task out of context and allow other tasks to post their transfers.
	if(xSemaphoreTake(semaphore, (portTickType)portMAX_DELAY) != pdTRUE) { // This should never happen either!
		TRACE_WARNING("\n\r doBlockingI2CTransfer: xSemaphoreTake failed! \n\r");
		while(1);
	}
	xSemaphoreGive(semaphore);
	return 0;
}


/*
 * Here the use of I2C_queueTransfer function is demonstrated by implementing our own I2Ccallback and
 * doBlockingI2CTransfer functions to achieve a blocking transfer.
 * Sometimes, the result of a transfer may not matter to the system, or the task may not want to block
 * on each transfer. For example:
 *
  	if(xSemaphoreTake(semaphore, (portTickType)10) != pdTRUE) { // This should never happen if we coded things correctly.
		TRACE_WARNING(" doBlockingI2CTransfer: xSemaphoreTake failed! \n\r");
		while(1);
	}
  	retVal = I2C_queueTransfer(tx);
	if(retVal != 0) { // This would never happen unless there are >64 transfers queued into the driver.
		printf("taskName: I2C_queueTransfer returned: %d \n\r", retVal);
	}

	// Do something useful here instead of giving up the processor to other tasks like
	// make ADCS calculations; create the data for the next packet
	// and queue that transfer too (using another semaphore and data buffers).
	somethingSmartWhileTransferHappens();

	//  Now try and take the semaphore again, if the callback released the semaphore already, this should return immediately.
	if(xSemaphoreTake(semaphore, (portTickType)portMAX_DELAY) != pdTRUE) { // This should never happen either!
		TRACE_WARNING("\n\r doBlockingI2CTransfer: xSemaphoreTake failed! \n\r");
		while(1);
	}
	xSemaphoreGive(semaphore);
 *
 * However, if blocking transfers is all we need, simply call int I2C_writeRead(I2CwriteReadTransfer *tx, I2CtransferStatus *result);
 * The I2C driver implements the same functions internally.
 */


void taskQueuedI2Ctest1() {
	int retValInt = 0;
	unsigned int i;
	I2CgenericTransfer i2cTx;
	I2CtransferStatus txResult;
	xSemaphoreHandle txSemaphore = NULL;
	unsigned char readData[64] = {0}, writeData[64] = {0};
	TRACE_DEBUG("\n\r taskQueuedI2Ctest1: Starting. \n\r");

	writeData[0] = 0xEF;
	for(i=1; i<sizeof(writeData); i++) {
		writeData[i] = (unsigned char)(i*2);
	}

	vSemaphoreCreateBinary(txSemaphore);
	if(txSemaphore == NULL) {
		TRACE_WARNING("\n\r taskQueuedI2Ctest1: vSemaphoreCreateBinary failed! \n\r");
		while(1);
	}

	i2cTx.callback = I2Ccallback;
	i2cTx.direction = writeRead_i2cDir;
	i2cTx.readData = readData;
	i2cTx.readSize = 2;
	i2cTx.writeData = writeData;
	i2cTx.writeSize = 3;
	i2cTx.writeReadDelay = 0;
	i2cTx.slaveAddress = 0x41;
	i2cTx.result = &txResult;
	i2cTx.semaphore = txSemaphore;

	while(1) {
		//TRACE_DEBUG(" taskQueuedI2Ctest1 \n\r");

		retValInt = doBlockingI2CTransfer(txSemaphore, &i2cTx);
		if(retValInt != 0) {
			TRACE_WARNING("\n\r taskQueuedI2Ctest1: I2C_queueTransfer returned: %d! \n\r", retValInt);
			while(1);
		}
		else {
			if(txResult==error_i2c || txResult==readError_i2c || txResult==writeError_i2c) {
				TRACE_WARNING("\n\r taskQueuedI2Ctest1: transfer error! \n\r");
			}
		}

		//TRACE_DEBUG(" taskQueuedI2Ctest1: received back: \n\r");
		//TRACE_DEBUG("0x%X ", readData[0]);
		for(i=1; i<i2cTx.readSize; i++) {
			//TRACE_DEBUG("0x%X ", readData[i]);
			writeData[i]++;
		}
		writeData[i]++;

		//TRACE_DEBUG(" \n\r\n\r");
		vTaskDelay(5);
	}
}

void taskQueuedI2Ctest2() {
	int retValInt = 0;
	unsigned int i;
	I2CgenericTransfer i2cTx;
	I2CtransferStatus txResult;
	xSemaphoreHandle txSemaphore = NULL;
	unsigned char readData[64] = {0}, writeData[64] = {0};
	TRACE_DEBUG("\n\r taskQueuedI2Ctest2: Starting. \n\r");

	writeData[0] = 0xEF;
	for(i=1; i<sizeof(writeData); i++) {
		writeData[i] = (unsigned char)(i*2);
	}

	vSemaphoreCreateBinary(txSemaphore);
	if(txSemaphore == NULL) {
		TRACE_WARNING("\n\r taskQueuedI2Ctest2: vSemaphoreCreateBinary failed! \n\r");
		while(1);
	}

	i2cTx.callback = I2Ccallback;
	i2cTx.direction = writeRead_i2cDir;
	i2cTx.readData = readData;
	i2cTx.readSize = 3;
	i2cTx.writeData = writeData;
	i2cTx.writeSize = 4;
	i2cTx.writeReadDelay = 1;
	i2cTx.slaveAddress = 0x41;
	i2cTx.result = &txResult;
	i2cTx.semaphore = txSemaphore;

	while(1) {
		//TRACE_DEBUG(" taskQueuedI2Ctest2 \n\r");

		retValInt = doBlockingI2CTransfer(txSemaphore, &i2cTx);
		if(retValInt != 0) {
			TRACE_WARNING("\n\r taskQueuedI2Ctest2: I2C_queueTransfer returned: %d! \n\r", retValInt);
			while(1);
		}
		else {
			if(txResult==error_i2c || txResult==readError_i2c || txResult==writeError_i2c) {
				TRACE_WARNING("\n\r taskQueuedI2Ctest2: transfer error! \n\r");
			}
		}

		//TRACE_DEBUG(" taskQueuedI2Ctest2: received back: \n\r");
		//TRACE_DEBUG("0x%X ", readData[0]);
		for(i=1; i<i2cTx.readSize; i++) {
			//TRACE_DEBUG("0x%X ", readData[i]);
			writeData[i]++;
		}
		writeData[i]++;

		//TRACE_DEBUG(" \n\r\n\r");
		vTaskDelay(5);
	}
}

void taskQueuedI2Ctest3() {
	int retValInt = 0;
	unsigned int i;
	I2Ctransfer i2cTx;
	unsigned char readData[64] = {0}, writeData[64] = {0};
	TRACE_DEBUG("\n\r taskQueuedI2Ctest3: Starting. \n\r");

	writeData[0] = 0xEF;
	for(i=1; i<sizeof(writeData); i++) {
		writeData[i] = (unsigned char)(i*2);
	}

	i2cTx.readData = readData;
	i2cTx.readSize = 4;
	i2cTx.writeData = writeData;
	i2cTx.writeSize = 5;
	i2cTx.writeReadDelay = 2;
	i2cTx.slaveAddress = 0x41;

	while(1) {
		//TRACE_DEBUG(" taskQueuedI2Ctest3 \n\r");

		retValInt = I2C_writeRead(&i2cTx); // Use I2C_writeRead instead of our own implementation.
		if(retValInt != 0) {
			TRACE_WARNING("\n\r taskQueuedI2Ctest3: I2C_writeRead returned: %d! \n\r", retValInt);
			while(1);
		}

		//TRACE_DEBUG(" taskQueuedI2Ctest3: received back: \n\r");
		//TRACE_DEBUG("0x%X ", readData[0]);
		for(i=1; i<i2cTx.readSize; i++) {
			//TRACE_DEBUG("0x%X ", readData[i]);
			writeData[i]++;
		}
		writeData[i]++;

		//TRACE_DEBUG(" \n\r\n\r");
		vTaskDelay(5);
	}
}

Boolean I2Ctest() {
	int retValInt = 0;
	xTaskHandle taskQueuedI2Ctest1Handle, taskQueuedI2Ctest2Handle, taskQueuedI2Ctest3Handle;

	retValInt = I2C_start(66000, 10);
	if(retValInt != 0) {
		TRACE_FATAL("\n\r I2Ctest: I2C_start returned: %d! \n\r", retValInt);
	}

	xTaskGenericCreate(taskQueuedI2Ctest1, (const signed char*)"taskQueuedI2Ctest1", 1024, NULL, 2, &taskQueuedI2Ctest1Handle, NULL, NULL);
	xTaskGenericCreate(taskQueuedI2Ctest2, (const signed char*)"taskQueuedI2Ctest2", 1024, NULL, 2, &taskQueuedI2Ctest2Handle, NULL, NULL);
	xTaskGenericCreate(taskQueuedI2Ctest3, (const signed char*)"taskQueuedI2Ctest3", 1024, NULL, 2, &taskQueuedI2Ctest3Handle, NULL, NULL);

	return FALSE;
}
