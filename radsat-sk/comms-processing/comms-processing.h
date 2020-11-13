/*  comms-processing_tasks.h
	Created by Minh Phong Bill Truong on April 28th 2020
 */

#ifndef COMMS_PROCESSING_TASKS_H
#define COMMS_PROCESSING_TASKS_H
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

//==============================================================================
//                                DEFINITIONS
//==============================================================================
#define RAW_PACKETS_QUEUED // TODO: Find how to check when there is something to be deparsed

//==============================================================================
//                                PUBLIC GLOBALS
//==============================================================================
/**
 * This Queue is used to store received and unprocessed packets
 */
extern QueueHandle_t rawPacketReceiveQueue;

/**
 * This Queue is used to store deparsed packets that will be used by transmitTask
 */
extern QueueHandle_t packetsToTransmit;

/**
 * Used to 
 */
extern SemaphoreHandle_t deparseSignal;

//==============================================================================
//                                PUBLIC API STUBS
//==============================================================================
void deparseTask( void* pvparameters );

#endif