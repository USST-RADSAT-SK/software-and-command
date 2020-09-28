/*
 * ADCtest.c
 *
 *  Created on: 26-Feb-2013
 *      Author: Akhil Piplani
 */

#include <at91/commons.h>
#include <at91/utility/trace.h>

#include <hal/boolean.h>
#include <hal/Drivers/ADC.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <stdlib.h>
#include <string.h>

static xSemaphoreHandle ADCsemaphore = NULL;
static void *CurrentADCsamplesBuffer = NULL;

static ADCparameters ADCparams;

void adcCallback(SystemContext context, void* adcSamples) {
	signed portBASE_TYPE flag = pdFALSE;

	CurrentADCsamplesBuffer = adcSamples;

	if(context == task_context) {
		xSemaphoreGive(ADCsemaphore);
	}
	else {
		xSemaphoreGiveFromISR(ADCsemaphore, &flag);
	}
}

unsigned int convertToMilliVolts_10bit(unsigned short rawValue)
{
	return((ADC_REFERENCE_VOLTAGE * (unsigned int)rawValue)/0x3FF);
}

unsigned int convertToMilliVolts_8bit(unsigned char rawValue)
{
	return((ADC_REFERENCE_VOLTAGE * (unsigned int)rawValue)/0xFF);
}

int ADCtest_initalize() {
	int retVal = 0;
	//unsigned int ADCSamplesBufferSize;

	ADCparams.channels = 8;
	ADCparams.resolution10bit = FALSE;
	ADCparams.sampleRate = 75000;
	ADCparams.samplesInBufferPerChannel = 2048;
	ADCparams.callback = adcCallback;

	//ADCSamplesBufferSize = ADCparams.samplesInBufferPerChannel * ADCparams.channels; // Number of samples in a buffer produced by the ADC driver.

	vSemaphoreCreateBinary(ADCsemaphore);
	if(ADCsemaphore == NULL) {
		printf("\n ADCtest_initalize: vSemaphoreCreateBinary failed! \n");
		retVal = -4;
	}
	if(xSemaphoreTake(ADCsemaphore, (portTickType)10) != pdTRUE) {
		printf("\n ADCtest_initalize: xSemaphoreTake failed! \n");
		retVal = -3;
	}

	retVal = ADC_start(ADCparams);
	if(retVal != 0) {
		printf("\n ADCtest_initalize: ADC_start returned %d \n", retVal);
	}

	return retVal;
}

void ADCtest_printReadout() {
	unsigned char  *ADCsampleUchar = NULL;
	unsigned short *ADCsampleUshort = NULL;
	unsigned int i;

	if(xSemaphoreTake(ADCsemaphore, (portTickType)portMAX_DELAY) != pdTRUE) {
		printf("\n ADCtest_printReadout: xSemaphoreTake failed! \n");
		return;
	}

	// Print the first 4 samples for each channel from the current element of the circular buffer.
	// Use an unsigned char pointer here if ADCparams.resolution10bit is FALSE. Otherwise use an unsigned short pointer.
	if(ADCparams.resolution10bit == FALSE) {
		ADCsampleUchar = CurrentADCsamplesBuffer;
	}
	else {
		ADCsampleUshort = CurrentADCsamplesBuffer;
	}

	printf("\n ADC samples:");
	for(i=0; i<(ADCparams.channels * 4); i++) {
		if((i % ADCparams.channels) == 0) {
			printf("\n\r");
		}

		if(ADCparams.resolution10bit == FALSE) {
			printf("%04dmV ", convertToMilliVolts_8bit(ADCsampleUchar[i]));
		}
		else {
			printf("%04dmV ", convertToMilliVolts_10bit(ADCsampleUshort[i]));
		}
	}
}

void taskADCtest() {
	printf(" This test will start the capture of Analog samples into a buffer and print the first sample's value for each ADC channel. \n\r");
	ADCtest_initalize();

	while(1) {
		ADCtest_printReadout();
	}
}

Boolean ADCtest() {
	xTaskHandle taskADCtestHandle;

	xTaskGenericCreate(taskADCtest, (const signed char*)"taskADCtest", 1024, NULL, 2, &taskADCtestHandle, NULL, NULL);
//	xTaskGenericCreate(taskADCtest2, (const signed char*)"taskADCtest", 1024, NULL, 2, &taskADCtestHandle, NULL, NULL);

	return FALSE;
}

Boolean ADCtestSingleShot()
{
	unsigned short adcSamples[8];

	unsigned int i;

	printf("ADC Channels:\n\r");

	ADC_SingleShot( adcSamples );

	for( i=0; i < 8; i++ )
	{
		printf("Channel %d : %u mV\n\r", i, ADC_ConvertRaw10bitToMillivolt( adcSamples[i] ) );
	}

	return TRUE;
}
