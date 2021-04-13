/**
 * @file RImageCaptureTask.c
 * @date April 07, 2021
 * @author Addi Amaya (caa746)
 */



//TODO: Double check telecommand and telemetry text - seems too big
//TODO: Find out where the Queues are being made and make sure they are named correctly


#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <RImageCaptureTask.h>
#include <stdint.h>

/***************************************************************************************************
                                             Global Variables
***************************************************************************************************/

/** flag to check if SRAM 1 top slot is open*/
uint16_t SRAM1Top;
/** flag to check if SRAM 1 bottom slot is open*/
uint16_t SRAM1Bottom;
/** flag to check if SRAM 2 bottom slot is open*/
uint16_t SRAM2Bottom;
/** Extern global variable for CubeSense camera queue */
extern xQueueHandle xQueueCubeSense;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
* used to assist main capture task and organizes which SRAM slot the images take
*
* @note this task required knowledge on how the CubeSense Camera operates. Review Cube sense Camera ICD and User Manual
* @pre requires queue for camera to be established else where
* @post Store one image into one SRAM slot on CubeSense camera
* @param requires a location of the SRAM slot to put the captured task
* @return a flag to determine if it was a successful capture
*/
uint32_t captureLocation(uint64_t *SRAMSlot){
	uint64_t TLcapatureStatus = 0x1F7F941FFF;
	uint8_t resultsCubeSense = 0x1;;
	const xBlockTime = pdMS_TO_TICKS( 200 );


	//Send Capture command
	xQueueSend(xQueueCubeSense, &SRAMSlot, xBlockTime);

	//pending capture
	while (resultsCubeSense == 0x1 )  {
		//Send request for telemetry for successful image capture
		xQueueSend(xQueueCubeSense, &TLcapatureStatus, xBlockTime);
	    //Receive status of image capture
		xQueueReceive(xQueueCubeSense, &resultsCubeSense, xBlockTime);
		resultsCubeSense = resultsCubeSense & 0x000F; //mask with byte 4 for status
	}

	//captured successfully
	if (resultsCubeSense == 0x2){
		 return 1;
	}
	//capture failed
	else{
		return 0;
	}
}

/**
* Task used to tell the CubeSense CubeSat Camera to take a photo and store into a SRAM location
*
* @note this task required knowledge on how the CubeSense Camera operates. Review Cube sense Camera ICD and User Manual
* @pre requires queue for camera to be established else where
* @post Store one image into one SRAM slot on CubeSense camera
* @param requires information if ground stations has asked for a super-resolution photo or not
* @return N/A, all features will be included in global variables
*/
uint32_t captureTask(void *superresolution ){
	uint64_t TCcaptureSRAM10 = 0x1F7F150001FFF; // Telecommand 21, camera 1, SRAM 1, Top Half
	uint64_t TCcaptureSRAM11 = 0x1F7F151001FFF; // Telecommand 21, camera 1, SRAM 1, bottom Half
	uint64_t TCcaptureSRAM21 = 0x1F7F151101FFF; // Telecommand 21, camera 1, SRAM 2, Bottom Half


    while(1){
        if (superresolution == 1){
            captureLocation(&TCcaptureSRAM10);
            captureLocation(&TCcaptureSRAM11);
            captureLocation(&TCcaptureSRAM21);
        }
        else{
        	//logic used to determine if SRAM is considered available or not
            if(SRAM1Top==0){
            	if (captureLocation(&TCcaptureSRAM10)){
            		SRAM1Top = 1;
            	}
            }
            else if(SRAM1Bottom==0){
            	if (captureLocation(&TCcaptureSRAM11)){
					SRAM1Bottom = 1;
				}
            }
            else if(SRAM2Bottom==0){
            	if (captureLocation(&TCcaptureSRAM21)){
					SRAM1Bottom = 1;
				}
            }
            else{
                break; //all photos slots are filled with a good photo
            }
        }
    }
    return 0;
}
