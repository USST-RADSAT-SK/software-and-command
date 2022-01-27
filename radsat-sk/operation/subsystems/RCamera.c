
/**
 * @file RCamera.c
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153)
 */


#include <RCamera.h>
#include <RUart.h>
#include <hal/errors.h>
#include <string.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
#define startidentifier1        ((uint8_t) 0x1F)
#define startidentifier2        ((uint8_t) 0x7F)

#define telecommandid21         ((uint8_t) 0x15)

#define CAPTURE_IMAGE_CMD_SIZE	((uint8_t) 3)
#define CAPTURE_IMAGE_TC_SIZE	((uint8_t) 8)
#define REQUEST_TELEMETRY_SIZE	((uint8_t) 5)
#define TELEMETRY_ACK_SIZE	    ((uint8_t) 5)

#define CAMERA_ONE	            ((uint8_t) 0)
#define CAMERA_TWO	            ((uint8_t) 1)

#define IMAGE_SENSOR	        ((uint8_t) CAMERA_TWO)
#define SUN_SENSOR	         	((uint8_t) CAMERA_ONE)

#define SRAM1                   ((uint8_t) 0)
#define SRAM2                   ((uint8_t) 1)

#define TOP_HALVE               ((uint8_t) 0)
#define BOTTOM_HALVE            ((uint8_t) 1)

#define endidentifier1          ((uint8_t) 0x1F)
#define endidentifier2          ((uint8_t) 0xFF)

#define telemetryid3            ((uint8_t) 0x83)

/** Invalid input parameters to the Camera module. */
#define ERROR_INVALID_PARAM		(-36)

static const uint8_t ackResponse[TELEMETRY_ACK_SIZE] = { 0x1F, 0x7F, 0x00, 0x1F, 0xFF };


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static int confirmPreviousTelecommand(void);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int sendCameraCaptureImageTc(tc_t slot) {
	// build  camera capture telecommand
	uint8_t commandBuffer[CAPTURE_IMAGE_TC_SIZE];

	// start of message identifiers
	commandBuffer[0] = startidentifier1;
	commandBuffer[1] = startidentifier2;

	// telecommand ID 21
	commandBuffer[2] = telecommandid21;

	//telecommand parameters:

	// which camera to capture from
	commandBuffer[3] = IMAGE_SENSOR;
	// which SRAM to store in
	commandBuffer[4] = SRAM2;
	// which SRAM location to store in
	commandBuffer[5] = TOP_HALVE;

	// end of message identifiers
	commandBuffer[6] = 0x1F;
	commandBuffer[7] = 0xFF;

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, CAPTURE_IMAGE_TC_SIZE);

	tc_t = commandBuffer;
	return tc_t;
}



static int confirmPreviousTelecommand(void) {
	//Requesting telemetry

	uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

	// start of message identifiers
	telemetryBuffer[0] = startidentifier1;
	telemetryBuffer[1] = startidentifier2;

	// telemetry frame id 3
	telemetryBuffer[2] =telemetryid3;

	// end of message identifiers
	telemetryBuffer[3] = endidentifier1;
	telemetryBuffer[4] = endidentifier2;


	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

	if (error)
		return error;

	uint8_t acknowledgeBuffer[TELEMETRY_ACK_SIZE] = { 0 };

	error = uartReceive(UART_CAMERA_BUS, acknowledgeBuffer, TELEMETRY_ACK_SIZE);

	if (memcmp(acknowledgeBuffer, ackResponse, sizeof(acknowledgeBuffer)))
		return ERROR_INVALID_PARAM;

	return 0;

}






