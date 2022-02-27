/**
 * @file RImageCaptureTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RImageCaptureTask.h>
#include <RCommon.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** How many images to capture per week. */
#define IMAGE_CAPTURES_PER_WEEK	(1)

/** Image Capture Task delay (in ms). */
#define IMAGE_CAPURE_TASK_DELAY_MS	(MS_PER_WEEK / IMAGE_CAPTURES_PER_WEEK)


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void ImageCaptureTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	while (1) {

		// TODO: implement image capture

		vTaskDelay(IMAGE_CAPURE_TASK_DELAY_MS);
	}
}


