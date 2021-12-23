
/**
 * @file RCamera.c
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153)
 */


#include <RCamera.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define CAPTURE_IMAGE_CMD_SIZE	((uint8_t) 3)

#define CAMERA_ONE	((uint8_t) 0)
#define CAMERA_TWO	((uint8_t) 1)

#define IMAGE_SENSOR	((uint8_t) CAMERA_TWO)
#define SUN_SENSOR		((uint8_t) CAMERA_ONE)


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int cameraCaptureImage(camera_slot_t slot) {
	// build our camera command
	uint8_t commandBuffer[CAPTURE_IMAGE_CMD_SIZE];
	// which camera to capture from
	commandBuffer[0] = IMAGE_SENSOR;
	//


}



