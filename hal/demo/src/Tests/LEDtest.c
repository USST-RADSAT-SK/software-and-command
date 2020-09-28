/*
 * LEDtest.c
 *
 *  Created on: 23-Jan-2013
 *      Author: Akhil Piplani
 */
#include <at91/commons.h>
#include <at91/utility/trace.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <hal/Drivers/LED.h>
#include <hal/boolean.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void taskLED2() {
	while(1) {
		// toggle led-2 and delay
		TRACE_DEBUG(" taskLED2 \n\r");
		LED_toggle(led_2);
		vTaskDelay(250);
	}
}

void taskLED3() {
	while(1) {
		// toggle led-3 and delay
		TRACE_DEBUG(" taskLED3 \n\r");
		LED_toggle(led_3);
		vTaskDelay(500);
	}
}

Boolean LEDtest() {
	xTaskHandle taskLED2handle, taskLED3handle;

	LED_wave(1);
	LED_waveReverse(1);
	LED_wave(1);
	LED_waveReverse(1);

	xTaskGenericCreate(taskLED2, (const signed char*)"taskLED2", 1024, NULL, 2, &taskLED2handle, NULL, NULL);
	xTaskGenericCreate(taskLED3, (const signed char*)"taskLED3", 1024, NULL, 2, &taskLED3handle, NULL, NULL);

	vTaskDelay(10000);

	vTaskDelete(taskLED2handle);
	vTaskDelete(taskLED3handle);

	return TRUE;
}
