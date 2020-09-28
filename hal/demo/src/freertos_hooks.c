/**
 * @file  freertos_hooks.c
 * @date  2015/01/08
 * @brief Contains implementation of hooks defined by FreeRTOS.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <at91/utility/exithandler.h>

/*
 * @brief
 *   FreeRTOS stack overflow hook.
 * @detail
 *   This hook is provided by FreeRTOS as a way for the upper layer (user code)
 *   to handle a stack overflow without introducing an dependency in the
 *   FreeRTOS code.
 * @note
 *   Implementing this hook is optional. If it is not implemented, a default
 *   internal implementation will be used, which is similar to this
 *   implementation.
 */
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	printf("\n\r STACK OVERFLOW DETECTED!! \n\r");
	printf(" Culprit task %p name: %s \n\r", pxTask, pcTaskName);
	printf(" !!Restarting Now!! \n\r");
	restart();
}

/*
 * @brief
 *   FreeRTOS idle hook.
 * @brief
 *   This hook is provided by FreeRTOS as a way for the upper layer (user code)
 *   to determine what happens to the iOBC when idle without introducing an
 *   dependency in the FreeRTOS code. This is an ideal place to kick the
 *   watchdog. It is imperative that the hook function does not call any API
 *   functions that might cause the idle task to block (vTaskDelay(), or a queue
 *   or semaphore function with a block time, for example).
 * @note
 *   Implementing this hook is optional. If it is not implemented, a default
 *   internal implementation will be used, which is similar to this
 *   implementation.
 */
void vApplicationIdleHook(void)
{
}

