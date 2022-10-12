/**
 * @file RTelecommandService.c
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RTelecommandService.h>
#include <RCameraService.h>
#include <RMessage.h>
#include <stdio.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Process (and execute where necessary) received telecommands.
 *
 * @param wrappedMessage A pointer to a wrapped (encrypted, etc.) RADSAT-SK message.
 * @param size The size (in bytes) of the wrapped message.
 * @return The tag of the processed telecommand (0 on failure).
 */
uint8_t telecommandHandle(uint8_t* wrappedMessage, uint8_t size) {
	int error;

	// ensure the input pointer is not NULL
	if (wrappedMessage == 0)
		return 0;

	// generate new RADSAT-SK message to populate
	radsat_message rawMessage = { 0 };

	// unwrap the message
	uint8_t rawSize = messageUnwrap(wrappedMessage, size, &rawMessage);

	// exit if unwrapping failed
	if (rawSize == 0)
		return 0;

	// exit if this message is not a telecommand message
	if (rawMessage.which_service != radsat_message_TelecommandMessage_tag)
		return 0;

	// obtain the specific telecommand
	uint8_t telecommand = (uint8_t) rawMessage.TelecommandMessage.which_message;

	// execute the telecommands
	switch (telecommand) {

		// indicates that a communication link has been established
		case (telecommand_message_BeginPass_tag):
			// do nothing; this reception of this telecommand already begins the pass mode
			break;

		// indicates that a telecommands are done; ready for file transfers
		case (telecommand_message_BeginFileTransfer_tag):
			// do nothing; higher level tasks will handle
			break;

		// indicates that all downlink activities shall be ceased
		case (telecommand_message_CeaseTransmission_tag):
			// do nothing; higher level tasks will handle
			break;

		// indicates that downlink activities may be resumed
		case (telecommand_message_ResumeTransmission_tag):
			// do nothing; higher level tasks will handle
			break;

		// provides a new accurate time for the OBC to set itself to
		case (telecommand_message_UpdateTime_tag):
			// TODO: implement functionality
			break;

		// instructs OBC to reset certain components on the Satellite
		case (telecommand_message_Reset_tag):
			// TODO: implement functionality
			break;



		// TO ADD: Reset cameras
		case (255):
			// TODO: Pass argument (reset option)
			if (!getCubeSenseUsageState()) {
				error = requestReset(2);
				if (error != 0) {
					printf("Error resetting cameras.\n");
				}
			}
			break;

		// TO ADD: Change both cameras' settings
		case (254):
			// TODO: Pass arguments (CameraSettings struct)
			if (!getCubeSenseUsageState()) {
				error = setCamerasSettings((CameraSettings){0});
				if (error != 0) {
					printf("Error updating cameras settings.\n");
				}
			}
			break;

		// TO ADD: Change automatic image capture interval
		case (253):
			// TODO: Pass argument (interval in ms)
			setImageCaptureInterval(0);
			break;

		// TO ADD: Change automatic image download size
		case (252):
			// TODO: Pass argument (0, 1, 2, 3 or 4)
			error = setImageDownloadSize(0);
			if (error != 0) {
				printf("Invalid image download size.\n");
			}
			break;

		// TO ADD: Set image as ready for a new capture
		case (251):
			setImageReadyForNewCapture();
			break;

		// TO ADD: Change image transfer frame index
		case (250):
			// TODO: Pass argument (frame index)
			setImageTransferFrameIndex(0);
			break;

		// TO ADD: Take manual image
		case (249):
			// TODO: Pass arguments?
			if (!getCubeSenseUsageState()) {
				error = requestImageCapture(NADIR_SENSOR, SRAM2, BOTTOM_HALVE);
				if (error != 0) {
					printf("Failed to manually capture an image...\n");
				}
			}
			break;

		// TO ADD: Manually start the download of an image
		case (248):
			// TODO: Pass arguments (image size)
			if (!getCubeSenseUsageState()) {
				error = requestImageDownload(SRAM2, 0);
				if (error != 0) {
					printf("Failed to start the image download...\n");
				}
			}
			break;

		// TO ADD: Update ADCS settings
		case (247):
			// TODO: Pass arguments (nb of measurements in a burst, interval between measurements)
			setADCSCaptureSettings(5, 2000);
			break;

		// TO ADD: Reset the adcs readiness flag so it can take another burst of measurements
		case (246):
			setADCSReadyForNewBurst();
			break;


		// unknown telecommand
		default:
			// do nothing; return failure
			return 0;
	}

	return telecommand;
}

