/**
 * @file RCamera.h
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153)
 */

#ifndef RCAMERA_H_
#define RCAMERA_H_

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

typedef enum _camera_image_slots {
	image_slot_one,
	image_slot_two,
	image_slot_three,
	sun_sensor_slot_one,
	camera_slot_count,
} camera_slot_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int cameraCaptureImage(camera_slot_t slot);
int sendCameraCaptureImageTc(tc_t slot);

#endif /* RCAMERA_H_ */
