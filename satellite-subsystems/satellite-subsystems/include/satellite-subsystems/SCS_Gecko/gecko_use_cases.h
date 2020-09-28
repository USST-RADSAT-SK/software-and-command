/*
 * gecko_use_cases.h
 *
 *  Created on: 2 jan. 2018
 *      Author: pbot
 */

#ifndef INCLUDE_SATELLITE_SUBSYSTEMS_SCS_GECKO_GECKO_USE_CASES_H_
#define INCLUDE_SATELLITE_SUBSYSTEMS_SCS_GECKO_GECKO_USE_CASES_H_

#include <hal/boolean.h>
#include <stdint.h>
#include "satellite-subsystems/SCS_Gecko/gecko_driver.h"

typedef enum _gecko_lvds_speed
{
	gecko_lvds_20MHz = 0,
	gecko_lvds_10MHz = 1,
	gecko_lvds_5MHz = 2,
	gecko_lvds_2MHz = 3,
	gecko_lvds_1MHz = 4,
	gecko_lvds_500kHz = 5,
	gecko_lvds_250kHz = 6,
	gecko_lvds_115k2Hz = 7,
} gecko_lvds_speed;

typedef enum _gecko_output_type
{
	gecko_output_raw = 0,
	gecko_output_binned = 1,
	gecko_output_jpeg_lossless = 2,
	gecko_output_jpeg_2x = 6,
	gecko_output_jpeg_4x = 14,
} gecko_output_type;

typedef struct __attribute__ ((__packed__)) _GECKO_UC_Telemetry_t
{
	GECKO_Telemetry_t sensor_tlm;
	/**
	 * Command status register (0x02)
	 * + the following bits from sensor status (0x0C)
	 * Bit 31 = Sensor training error
	 * Bit 30 = Sensor training
	 * Bit 29 = Sensor on done
	 * + the following bit from sensor command (0x0A)
	 * Bit 28 = Sensor on
	 */
	uint32_t state;
	/**
	 * Number of bytes of an image read over SPI. Is reset to zero when a new
	 * SPI read image command is initiated
	 */
	uint32_t spi_bytes_read;
} GECKO_UC_Telemetry_t;


int GECKO_UC_Init( const SPIslaveParameters* slaveParamsIn, uint8_t numSystems );

/**
 * @brief Higher level function for taking an image.
 * @details Internally this function executes the following steps:
 * 1. Set sensor PGA, ADC and sensor offset through register 0x0D.
 * 2. Set the exposure time in register 0x0E.
 * 3. Set the number of frames to capture register 0x06 (FRAME_AMOUNT)
 * 4. Set the frame rate to register 0x0B (FRAME_RATE)
 * 5. Switch to Imaging Mode (Sensor ON):
 *    - Note: Sensor settings (PGA, ADC, offset, exposure, frame rate, etc) is only applied when sensor is
 *    	turned ON. If settings are changed, sensor should be turned OFF and ON again.
 *    - In order to switch the sensor ON, set bit 0 of register 0x0A.
 *    - Read bit 17 of register 0x0C to verify sensor is ON and bit 18 of register 0x0C to verify sensor is
 *    	trained correctly.
 * 6. Image to Flash
 *    - Ensure Flash has been initialised. Flash initialisation done is indicated by 16 of 0x02.
 *    - Set the IMAGE_ID address to capture images to, register 0x05 with a 21-bit value.
 *    - Set sensor to capture image data (not test pattern) by clearing bit 4 of register 0x02.
 *    - Start sampling by setting bit 2 of register 0x02.
 *    - Imaging done is indicated by bit 25 of register 0x02.
 * @param adcGain - Digital gain to use when taking images
 * @param pgaGain - Analog gain to use when taking images
 * @param exposure - Exposure to use when taking images
 * @param frameAmount - Number of images to take
 * @param frameRate - Frame rate at which images should be taken
 * @param imageID - Index in memory where image capture should start
 * @return Use case result:
 *   0 = completed successfully
 *  -1 = could not turn off sensor
 *  -2 = could not set ADC gain
 *  -3 = could not set PGA gain
 *  -4 = could not set exposure
 *  -5 = could not set frame amount
 *  -6 = could not set frame rate
 *  -7 = could not turn on sensor
 *  -8 = sensor turn on timeout
 *  -9 = training timeout
 *  -10 = training error
 *  -11 = could not initialise flash
 *  -12 = could not set image ID
 *  -13 = could not disable test pattern
 *  -14 = could not start sampling
 *  -15 = sampling timeout
 *  -16 = could not clear sample flag
 *  -17 = could not turn of sensor
 */
int GECKO_UC_TakeImage( uint8_t index, uint8_t adcGain, uint8_t pgaGain, uint32_t exposure, uint32_t frameAmount, uint32_t frameRate, uint32_t imageID );

/**
 * @brief Prepares the Gecko camera for taking an image.
 * NOTE: The function will turn on the image sensor, but not sample an image.
 *  This means that the delay between this function and GECKO_UC_TakeImage_StartSample
 *  should be kept at a minimum
 *
 * See GECKO_UC_TakeImage for documentation about the parameters
 */
int GECKO_UC_TakeImage_Prepare( uint8_t index, uint8_t adcGain, uint8_t pgaGain, uint32_t exposure, uint32_t frameAmount, uint32_t frameRate, uint32_t imageID );

/**
 * @brief Starts the sampling of an image
 */
int GECKO_UC_TakeImage_StartSample(uint8_t index);

/**
 * @brief Waits for sampling to finish and then cleans up flags etc. in the Gecko camera
 */
int GECKO_UC_TakeImage_Cleanup(uint8_t index);

/**
 * @brief Higher level function for reading an image.
 * @note The image size is depending on the image type. For RAW images the size is
 * 2048x1088 pixels + 604 byte header = 2,228,828 bytes
 * @details Internally this function executes the following steps:
 * 1. Ensure Flash has been initialised. Flash initialisation done is indicated by 16 of 0x02.
 * 2. Set IMAGE_ID to read from by writing a 12-bit address to register 0x05.
 * 3. Set image type to bit 0-3 in register 0x09
 * 4. Set SPI as output to bit 8 in register 0x02
 * 5. Start read-out by setting bit 1 of register 0x02.
 * 6. Determine if data is ready for reading through monitoring bit 19 of register 0x02. Start SPI read-out
      when bit 19 is set.
 * 7. Read out register 0x03 for the number of words to read out,
      and then read Image data from register 0x04, 32-bits at a time, the provided number of time
 * 8. Read-out complete is indicated by bit 24 of register 0x02. If not complete, repeat step 7
 * @param imageID - ID of image to be read
 * @param buffer - buffer into which image should be read
 * @param size - size of buffer used given in bytes
 * @param imageType - Which image version to request
 * @return Use case result:
 *   0 = completed successfully
 *  -1 = could not initialise flash
 *  -2 = could not set image ID
 *  -3 = could not start readout
 *  -4 = data read timeout
 *  -5 = word count mismatch during read
 *  -6 = page count mismatch during read
 *  -7 = read done flag not set
 *  -8 = could not clear read done flag
 *  -9 = could not set output to SPI
 *  -12= error setting image type
 *  -13= error during image data read
 *  -14= error due to the buffer being too small
 */
int GECKO_UC_ReadImage( uint8_t index, uint32_t imageID, uint32_t *buffer, uint32_t bufferSize, uint32_t *imageSize, gecko_output_type imageType );

/**
 * @brief Higher level function for reading an image over the LVDS interface
 */
int GECKO_UC_ReadImageLVDS( uint8_t index, uint32_t imageID, gecko_lvds_speed lvdsSpeed, gecko_output_type imageType );
//Asynchronous function for reading out over LVDS
int GECKO_UC_ReadImageLVDS_Start( uint8_t index, uint32_t imageID, gecko_lvds_speed lvdsSpeed, gecko_output_type imageType );
int GECKO_UC_ReadImageLVDS_WaitForDone(uint8_t index);

/**
 * @brief Overrides the default LVDS read timeout with the provided number of attempts.
 * One attempt is done every 100ms, thus setting this to 200 means 20 second timeout
 */
void GECKO_UC_SetLVDSReadTimeout(uint16_t attempts);

/**
 * @brief Higher level function for erasing an image.
 * @details Internally this function executes the following steps:
 * 1. Set image ID to erase by writing a 12-bit address to register 0x05.
 * 2. Start erase process by setting bit 3 in register 0x02.
 * 3. Bit 26 of register 0x02 will indicate when IMAGE_ID has been erased
 * @param imageID - ID of image to erase
 * @return Use case result:
 *   0 = completed successfully
 *  -1 = could not set image ID
 *  -2 = could not start erase
 *  -3 = erase timeout
 *  -4 = could not clear erase done flag
 */
int GECKO_UC_EraseBlock( uint8_t index, uint32_t imageID );

/**
 * @brief Provides additional telemetry to temperature, voltage and current
 */
int GECKO_UC_GetTelemetry( uint8_t index, GECKO_UC_Telemetry_t* tlm );

#endif /* INCLUDE_SATELLITE_SUBSYSTEMS_SCS_GECKO_GECKO_USE_CASES_H_ */
