/*  comms-processing_tasks.c

    Created by Minh Phong Bill Truong on April 28th 2020
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio.h>
#include "comms-processing_tasks.h"

//==============================================================================
//                                   External
//==============================================================================
/**
 * This Queue is used to store received and unprocessed packets
 */
QueueHandle_t rawPacketReceiveQueue;

/**
 * This Queue is used to store encoded packets that will be used by transmitTask
 */
QueueHandle_t packetsToTransmit;

/**
 * Used to control when the encoderTask can run
 */
SemaphoreHandle_t encodeSignal;

/**
 * 1us delay
 */
const TickType_t xDelay = 1000; // TODO: find actual number for 1us delay

//==============================================================================
//                                  FUNCTIONS
//==============================================================================

/**
 *  @brief Call functions required to encode packet
 *  
 *  The encoder task receives the data that is to be sent down (clarify) and calls the
 *  required functions to handle the formatting of the data into a packet. 
 * 
 *  @header	"software-and-command/main-applicatoion/comms-processing_tasks.c"
 *	@param	Unused
 *	@pre	None
 * 	@post	Formatted packet is stored in memory
 * 	@return	None
 */
void encodeTask( void* pvparameters ) 
{
    uint16_t  rawOutgoingPacket;
    uint16_t  encodedOutgoingPacket;

    while(1) 
    {
        if ( RAW_PACKETS_QUEUED )
        {
            // Wait until signal to encode packet is given.
            xSemaphoreTake( encodeSignal, portMAX_DELAY )
        }
        else
        {
            // Execute any tasks of equal priority that are ready now.
            vTaskDelay( xDelay );
        }

        // Wait for portMAX_Delay period for data to become available on the queue.
        xQueueReceive( rawPacketReceiveQueue, rawOutgoingPacket, portMAX_DELAY );

        &encodedOutgoingPacket = encodeData( &rawOutgoingPacket ); // Stub function call
        &encodedOutgoingPacket = encode( encodedOutgoingPacket ); // Stub function call

        // Send data to the tail of the encoded packets queue.
        xQueueSendToBack( packetsToTransmit, encodedOutgoingPacket, portMAX_DELAY);

        xSemaphoreGive( encodeSignal );
    }
}