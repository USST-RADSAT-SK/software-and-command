/*
 * gecko_driver.h
 *
 *  Created on: 20 dec. 2017
 *      Author: pbot
 */

#ifndef SRC_SCS_GECKO_GECKO_DRIVER_H_
#define SRC_SCS_GECKO_GECKO_DRIVER_H_

#include <hal/boolean.h>
#include <hal/Drivers/SPI.h>

#include <stdint.h>

typedef enum GECKO_Endiannes_t
{
	GECKO_Endianness_big,
	GECKO_Endianness_little,
}
GECKO_Endianness_t;

typedef struct __attribute__ ((__packed__)) _GECKO_Telemetry_t
{
	uint32_t uptime;
	uint16_t input5v_voltage;
	uint16_t input5v_current;
	uint16_t fpga1v_voltage;
	uint16_t fpga1v_current;
	uint16_t fpga1v8_voltage;
	uint16_t fpga1v8_current;
	uint16_t fpga2v5_voltage;
	uint16_t fpga2v5_current;
	uint16_t fpga3v3_voltage;
	uint16_t fpga3v3_current;
	uint16_t flash1v8_voltage;
	uint16_t flash1v8_current;
	uint16_t flash3v3_voltage;
	uint16_t flash3v3_current;
	uint16_t sensor1v8_voltage;
	uint16_t sensor1v8_current;
	uint16_t sensor3v3_voltage;
	uint16_t sensor3v3_current;
	uint16_t sensor_pix_voltage;
	uint16_t sensor_pix_current;
	uint16_t flash_vtt09;
	uint16_t temp_sm_u3_ab;
	uint16_t temp_sm_u3_bc;
	uint16_t temp_reg_u6;
	uint16_t temp_reg_u8;
	uint16_t temp_flash;
	uint16_t temp_fpga;
	uint16_t temp_sensor;
} GECKO_Telemetry_t;

typedef struct __attribute__ ((__packed__)) _GECKO_Telemetry_eng_t
{
	uint32_t uptime;
	float input5v_voltage;
	float input5v_current;
	float fpga1v_voltage;
	float fpga1v_current;
	float fpga1v8_voltage;
	float fpga1v8_current;
	float fpga2v5_voltage;
	float fpga2v5_current;
	float fpga3v3_voltage;
	float fpga3v3_current;
	float flash1v8_voltage;
	float flash1v8_current;
	float flash3v3_voltage;
	float flash3v3_current;
	float sensor1v8_voltage;
	float sensor1v8_current;
	float sensor3v3_voltage;
	float sensor3v3_current;
	float sensor_pix_voltage;
	float sensor_pix_current;
	float flash_vtt09;
	float temp_sm_u3_ab;
	float temp_sm_u3_bc;
	float temp_reg_u6;
	float temp_reg_u8;
	float temp_flash;
	float temp_fpga;
	float temp_sensor;
} GECKO_Telemetry_eng_t;

/**
 *  @brief      Initialise the SCS Gecko driver with the corresponding SPI slave parameters.
 *  @param[in]  slaveParams SPIslaveParameters structure which should be used by the SPI driver calls.
 *  @return     0 = no error; -1 = error
 */
int GECKO_Init( const SPIslaveParameters* slaveParams, uint8_t numSystems );

/**
 *  @brief      Get a 32-bit register value from the Gecko subsystem.
 *  @param[in]  addr Address of registers to get.
 *  @param[out] data Data of the register to get.
 *  @param[in]  endian Endianness of how data should be read.
 *  @return     0 = no error; -1 = error, positive values defined in <hal/errors.h>.
 */
int GECKO_GetReg( uint8_t index, uint8_t addr, uint32_t *data, GECKO_Endianness_t endian );

/**
 *  @brief      Set a 32-bit register value on the Gecko subsystem.
 *  @param[in]  addr Address of registers to set.
 *  @param[in]  data Data of the register to set.
 *  @param[in]  endian Endianness of how data should be saved.
 *  @return     0 = no error; -1 = error, positive values defined in <hal/errors.h>.
 */
int GECKO_SetReg( uint8_t index, uint8_t addr, uint32_t data, GECKO_Endianness_t endian );

/**
 *  @brief Get the software build version from the version register.
 */
uint8_t GECKO_GetVersionSwBuild( uint8_t index );

/**
 *  @brief Get the software version minor from the version register.
 */
uint8_t GECKO_GetVersionSwMinor( uint8_t index );

/**
 *  @brief Get the software version major from the version register.
 */
uint8_t GECKO_GetVersionSwMajor( uint8_t index );

/**
 *  @brief Get the hardware version from the version register.
 */
uint8_t GECKO_GetVersionHW( uint8_t index );

/**
 * @brief Get telemetry data from the gecko camera
 */
int GECKO_GetTelemetry( uint8_t index, GECKO_Telemetry_t* tlm );

/**
 * @brief Get telemetry data from the gecko camera in engineering values
 */
int GECKO_GetTelemetry_Engineering( uint8_t index, GECKO_Telemetry_eng_t* tlm );

/**
 * @brief Sets output of image data to LVDS bus
 */
int GECKO_SetOutputLVDS( uint8_t index );

/**
 * @brief Sets output of image data to SPI bus
 */
int GECKO_SetOutputSPI( uint8_t index );

/**
 * @brief
 * @param Speed set in binary value from table below. N/A for SPI transfer
		LVDS 	LVDS USART
000:	200 MHz	20 MHz
001:	150 MHz	10 MHz
010:	120 MHz	 5 MHz
011:	100 MHz	 2 MHz
100:	 80 MHz	 1 MHz
101:	 60 MHz	500 kHz
110:	 40 MHz	250 kHz
111:	 30 MHz	115k200
 */
int GECKO_SetOutputSpeed( uint8_t index, uint8_t speed );

/**
 *  @brief Start readout of data from the sensor.
 */
int GECKO_StartReadout( uint8_t index );

/**
 *  @brief Start sampling of data on the sensor.
 */
int GECKO_StartSample( uint8_t index );

/**
 *  @brief Start erasing of sensor data.
 */
int GECKO_StartErase( uint8_t index );

/**
 *  @brief Enable test pattern.
 */
int GECKO_EnableTestPattern( uint8_t index );

/**
 *  @brief Stop readout of data from the sensor.
 */
int GECKO_StopReadout( uint8_t index );

/**
 *  @brief Stop sampling of data on the sensor.
 */
int GECKO_StopSample( uint8_t index );

/**
 *  @brief Stop erasing of sensor data.
 */
int GECKO_StopErase( uint8_t index );

/**
 *  @brief Disable test pattern.
 */
int GECKO_DisableTestPattern( uint8_t index );

/**
 *  @brief Get flag indicating whether flash has been initialised.
 */
int GECKO_GetFlashInitDone( uint8_t index );

/**
 *  @brief Get flag indicating whether data is ready to be read from sensor.
 */
int GECKO_GetReadReady( uint8_t index );

/**
 *  @brief Get flag indicating whether data read is in progress.
 */
int GECKO_GetReadBusy( uint8_t index );

/**
 *  @brief Get flag indicating whether sensor is busy sampling data.
 */
int GECKO_GetSampleBusy( uint8_t index );

/**
 *  @brief Get flag indicating whether flash is being erased.
 */
int GECKO_GetEraseBusy( uint8_t index );

/**
 *  @brief Get flag indicating whether data read from sensor is done.
 */
int GECKO_GetReadDone( uint8_t index );

/**
 *  @brief Get flag indicating whether sensor is done sampling.
 */
int GECKO_GetSampleDone( uint8_t index );

/**
 *  @brief Get flag indicating whether flash erase is done.
 */
int GECKO_GetEraseDone( uint8_t index );

/**
 *  @brief Clear flag indicating whether data read from sensor is done.
 */
int GECKO_ClearReadDone( uint8_t index );

/**
 *  @brief Clear flag indicating whether sensor is done sampling.
 */
int GECKO_ClearSampleDone( uint8_t index );

/**
 *  @brief Clear flag indicating whether flash erase is done.
 */
int GECKO_ClearEraseDone( uint8_t index );

/**
 *  @brief Return the number of 32-bit words left to read per page.
 */
uint16_t GECKO_GetFlashCount( uint8_t index );

/**
 *  @brief Return 32-bit word of image data.
 */
uint32_t GECKO_GetImgData( uint8_t index );

/**
 * @brief Retrieves a bulk of image data in a more optimised way than GECKO_GetImgData
 * It assumes buffer is large enough for wordsToRead*4 bytes.
 * This is a function that can useful to call after reading out the FLASH_COUNT (0x3) register
 */
Boolean GECKO_GetBulkImgData( uint8_t index, uint32_t* buffer, uint16_t wordsToRead );

/**
 *  @brief Get the current image ID.
 */
uint32_t GECKO_GetImageID( uint8_t index );

/**
 *  @brief Set the current image ID.
 */
int GECKO_SetImageID( uint8_t index, uint32_t imageID );

/**
 *  @brief Get the number of frames to capture register.
 */
uint32_t GECKO_GetFrameAmount( uint8_t index );

/**
 *  @brief Set the number of frames to capture register.
 */
int GECKO_SetFrameAmount( uint8_t index, uint32_t frameAmount );

/**
 *  @brief Get the uptime of the subsystem.
 */
uint32_t GECKO_GetUpTime( uint8_t index );

/**
 * @brief Sets one slot of AUX data
 * @param address Address of AUX slot (0-255). 0-7 is reserved by Gecko camera for image settings
 * @param value The value that should be assigned to the slot
 */
int GECKO_SetAUXData( uint8_t index, uint8_t address, uint16_t value );

/**
 * @brief Sets the image type
 * @param type Indicates which image version to transfer
0000: raw
0001: binned
0010: lossless JPEG (depends on availability in gecko firmware)
0110: lossy 2x JPEG (depends on availability in gecko firmware)
1110: lossy 4x JPEG (depends on availability in gecko firmware)
 */
int GECKO_SetImageType( uint8_t index, uint8_t type );

/**
 *  @brief Turn the sensor on.
 */
int GECKO_SensorOn( uint8_t index );

/**
 *  @brief Turn the sensor off.
 */
int GECKO_SensorOff( uint8_t index );

/**
 *  @brief Get the frame rate of the sensor.
 */
uint32_t GECKO_GetFrameRate( uint8_t index );

/**
 *  @brief Set the frame rate of the sensor.
 */
int GECKO_SetFrameRate( uint8_t index, uint32_t frameRate );

/**
 *  @brief Get flag indicating whether sensor is done turning on.
 */
int GECKO_GetOnDone( uint8_t index );

/**
 *  @brief Get flag indicating whether sensor is done training.
 */
int GECKO_GetTrainingDone( uint8_t index );

/**
 *  @brief Get flag indicating whether training had an error.
 */
int GECKO_GetTrainingError( uint8_t index );

/**
 *  @brief Set the ADC gain of the sensor.
 */
int GECKO_SetAdcGain( uint8_t index, uint8_t gain );

/**
 *  @brief Set the PGA gain of the sensor.
 */
int GECKO_SetPgaGain( uint8_t index, uint8_t gain );

/**
 *  @brief Set the offset of the sensor.
 */
int GECKO_SetOffset( uint8_t index, uint16_t offset );

/**
 *  @brief the exposure of the sensor.
 */
int GECKO_SetExposure( uint8_t index, uint32_t exposure );

/**
 *  @brief Get input 5V voltage telemetry.
 */
float GECKO_GetVoltageInput5V( uint8_t index );

/**
 *  @brief Get input 5V current telemetry.
 */
float GECKO_GetCurrentInput5V( uint8_t index );

/**
 *  @brief Get FPGA 1V voltage telemetry.
 */
float GECKO_GetVoltageFPGA1V( uint8_t index );

/**
 *  @brief Get FPGA 1V current telemetry.
 */
float GECKO_GetCurrentFPGA1V( uint8_t index );

/**
 *  @brief Get FPGA 1V8 voltage telemetry.
 */
float GECKO_GetVoltageFPGA1V8( uint8_t index );

/**
 *  @brief Get FPGA 1V8 current telemetry.
 */
float GECKO_GetCurrentFPGA1V8( uint8_t index );

/**
 *  @brief Get FPGA 2V5 voltage telemetry.
 */
float GECKO_GetVoltageFPGA2V5( uint8_t index );

/**
 *  @brief Get FPGA 2V5 current telemetry.
 */
float GECKO_GetCurrentFPGA2V5( uint8_t index );

/**
 *  @brief Get FPGA 3V3 voltage telemetry.
 */
float GECKO_GetVoltageFPGA3V3( uint8_t index );

/**
 *  @brief Get FPGA 3V3 current telemetry.
 */
float GECKO_GetCurrentFPGA3V3( uint8_t index );

/**
 *  @brief Get Flash 1V8 voltage telemetry.
 */
float GECKO_GetVoltageFlash1V8( uint8_t index );

/**
 *  @brief Get Flash 1V8 current telemetry.
 */
float GECKO_GetCurrentFlash1V8( uint8_t index );

/**
 *  @brief Get Flash 3V3 voltage telemetry.
 */
float GECKO_GetVoltageFlash3V3( uint8_t index );

/**
 *  @brief Get Flash 3V3 current telemetry.
 */
float GECKO_GetCurrentFlash3V3( uint8_t index );

/**
 *  @brief Get sensor 1V8 voltage telemetry.
 */
float GECKO_GetVoltageSNSR1V8( uint8_t index );

/**
 *  @brief Get sensor 1V8 current telemetry.
 */
float GECKO_GetCurrentSNSR1V8( uint8_t index );

/**
 *  @brief Get sensor Vdd_pix voltage telemetry.
 */
float GECKO_GetVoltageSNSRVDDPIX( uint8_t index );

/**
 *  @brief Get sensor Vdd_pix current telemetry.
 */
float GECKO_GetCurrentSNSRVDDPIX( uint8_t index );

/**
 *  @brief Get sensor 3V3 voltage telemetry.
 */
float GECKO_GetVoltageSNSR3V3( uint8_t index );

/**
 *  @brief Get sensor 3V3 current telemetry.
 */
float GECKO_GetCurrentSNSR3V3( uint8_t index );

/**
 *  @brief Get Flash/RAM reference voltage telemetry.
 */
float GECKO_GetVoltageFlashVTT09( uint8_t index );

/**
 *  @brief Get 3A and 3B SMPS temperature telemetry.
 */
float GECKO_GetTempSMU3AB( uint8_t index );

/**
 *  @brief Get 3B and 3C SMPS temperature telemetry.
 */
float GECKO_GetTempSMU3BC( uint8_t index );

/**
 *  @brief Get U6 regulator temperature telemetry.
 */
float GECKO_GetTempREGU6( uint8_t index );

/**
 *  @brief Get U8 regulator temperature telemetry.
 */
float GECKO_GetTempREGU8( uint8_t index );

/**
 *  @brief Get flash temperature telemetry.
 */
float GECKO_GetTempFlash( uint8_t index );

/**
 *  @brief Get FPGA temperature telemetry.
 */
float GECKO_GetTempFPGA( uint8_t index );

/**
 *  @brief Get image sensor temperature telemetry. NOTE: This is the raw value!
 */
float GECKO_GetTempSensor( uint8_t index );

#endif /* SRC_SCS_GECKO_GECKO_DRIVER_H_ */
