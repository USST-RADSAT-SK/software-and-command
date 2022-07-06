/**
 * @file RDosimeter.c
 * @date December 28, 2021
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#include <RDosimeter.h>
#include <RFileTransferService.h>
#include <RFileTransfer.pb.h>
#include <RI2c.h>
#include <string.h>
#include <RCommon.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/**
 * Reference voltage for the Melanin-Dosimeter boards. Sensors return relative
 * 8-bit values. E.g. 127 out of 255 would be 50%, representing 1.65V
 */
#define DOSIMETER_REFERENCE_VOLTAGE_MV	((float) 3300)

/** Temperature scales slope  (m in y = mx + b) */
#define TEMPERATURE_SCALE_SLOPE		((float) (1 / -13.6))
/** Temperature scales offset (b in y = mx + b) */
#define TEMPERATURE_SCALE_OFFSET	((float) 192.48)

/** I2C Slave Address for Dosimeter Board One */
#define DOSIMETER_1_I2C_SLAVE_ADDR	(0x4A)
/** I2C Slave Address for Dosimeter Board Two */
#define DOSIMETER_2_I2C_SLAVE_ADDR	(0x48)
/** I2C Response for Dosimeter Board readings are 12 bits (2 bytes) long */
#define DOSIMETER_I2C_DELAY			(0)

/** I2C Response for Dosimeter Board readings are 12 bits (2 bytes) long */
#define DOSIMETER_COMMAND_LENGTH			(1)
/** I2C Response for Dosimeter Board readings are 12 bits (2 bytes) long */
#define DOSIMETER_RESPONSE_LENGTH			(2)
/** High byte of 12-bit readings must have top 4 bits masked away */
#define DOSIMETER_RESPONSE_HIGH_BYTE_MASK	(0x0F)

/** ADC takes 12-bit readings */
#define ADC_BIT_RESOLUTION	(12)
/** Used for converting ADC values to real voltage readings */
#define MAX_ADC_VALUE		((float) (1 << 12))


/***************************************************************************************************
                                          PRIVATE GLOBALS
***************************************************************************************************/

/** The eight Dosimeter board channels. */
enum _adcChannels {
	adcChannelZero,		// COTS 2048mV REF IC, Shielding: none
	adcChannelOne,		// COTS 2048mV REF IC, Shielding: 50 mil
	adcChannelTwo,		// COTS 2048mV REF IC, Shielding: 100 mil
	adcChannelThree,	// COTS 2048mV REF IC, Shielding: 200 mil
	adcChannelFour,		// COTS 2048mV REF IC, Shielding: 20 mil
	adcChannelFive,		// RADFET Experimental Dosimeter, Shielding: none
	adcChannelSix,		// COTS 2048mV REF IC, Shielding: 300 mil
	adcChannelSeven,	// Temperature Sensor
	adcChannelCount,
};

/** The temperature ADC channel. */
static uint8_t temperatureSensor = adcChannelSeven;

/** The slave addresses of the two Melanin-Dosimeter boards. */
static uint8_t dosimeterBoardSlaveAddr[dosimeterBoardCount] = {
	DOSIMETER_1_I2C_SLAVE_ADDR,
	DOSIMETER_2_I2C_SLAVE_ADDR
};

/**
 *  Byte commands to request data conversions for each of the 8 channels on each
 *	Melanin-Dosimeter board. See datasheet for details.
 */
static uint8_t dosimeterCommandBytes[adcChannelCount] = {
	0x84,     // Channel 0 - 1000.0100
	0xC4,     // Channel 1 - 1100.0100
	0x94,     // Channel 2 - 1001.0100
	0xD4,     // Channel 3 - 1101.0100
	0xA4,     // Channel 4 - 1010.0100
	0xE4,     // Channel 5 - 1110.0100
	0xB4,     // Channel 6 - 1011.0100
	0xF4      // Channel 7 - 1111.0100
};


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static float convertCountsToVoltage(uint8_t highByte, uint8_t lowByte);
static float convertVoltageToTemperature(float voltage);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Request and store readings from all Melanin-Dosimeter channels.
 *
 * Iterates through each channel on both boards to collect and store payload
 * data from the experimental Melanin-Dosimeter boards. Relies on I2C calls to
 * send commands and receive the information in a response.
 *
 * @pre I2C must be initialized
 * @post If successful, sends new formatted group of readings to the Downlink Manager
 * @param void
 * @return Returns 0 on Success, anything else indicates error (e.g. from I2C).
 * 		   In the event of error, no data will have been sent to the Downlink
 * 		   Manager.
 */
int dosimeterCollectData(void) {
	int error = SUCCESS;

	// internal buffer for receiving I2C responses
	uint8_t dataResponse[DOSIMETER_RESPONSE_LENGTH] = { 0 };

	// prepare a protobuf struct to populate with data
	dosimeter_data data = { 0 };

	// prepare a 2D array to store the values obtained in the following loops
	float results[dosimeterBoardCount][adcChannelCount] = { 0 };

	// iterate through both melanin-dosimeter boards
	for (uint8_t dosimeterBoard = dosimeterBoardOne; dosimeterBoard < dosimeterBoardCount; dosimeterBoard++) {

		// request data from each sensor on a particular board
		for (uint8_t adcChannel = adcChannelZero; adcChannel < adcChannelCount; adcChannel++) {

			// reset internal buffer
			memset(dataResponse, 0, DOSIMETER_RESPONSE_LENGTH);

			// tell dosimeter to begin conversion; receive 12-bit data into our internal buffer
			error = i2cTalk(dosimeterBoardSlaveAddr[dosimeterBoard], DOSIMETER_COMMAND_LENGTH,
								DOSIMETER_RESPONSE_LENGTH, &dosimeterCommandBytes[adcChannel],
								dataResponse, DOSIMETER_I2C_DELAY);

			// check for success of I2C command
			if (error != 0)
				return error;

			float finalVoltage = convertCountsToVoltage(dataResponse[0], dataResponse[1]);

			// if reading the temperature sensor, convert it to celsius
			if (adcChannel == temperatureSensor)
				finalVoltage = convertVoltageToTemperature(finalVoltage);

			// store result in 2D array
			results[dosimeterBoard][adcChannel] = finalVoltage;

		}
	}

	// format protobuf message with recorded values

	// board one
	data.boardOne.channelZero = results[dosimeterBoardOne][adcChannelZero];
	data.boardOne.channelOne = results[dosimeterBoardOne][adcChannelOne];
	data.boardOne.channelTwo = results[dosimeterBoardOne][adcChannelTwo];
	data.boardOne.channelThree = results[dosimeterBoardOne][adcChannelThree];
	data.boardOne.channelFour = results[dosimeterBoardOne][adcChannelFour];
	data.boardOne.channelFive = results[dosimeterBoardOne][adcChannelFive];
	data.boardOne.channelSix = results[dosimeterBoardOne][adcChannelSix];
	data.boardOne.channelSeven = results[dosimeterBoardOne][adcChannelSeven];

	// board two
	data.boardTwo.channelZero = results[dosimeterBoardTwo][adcChannelZero];
	data.boardTwo.channelOne = results[dosimeterBoardTwo][adcChannelOne];
	data.boardTwo.channelTwo = results[dosimeterBoardTwo][adcChannelTwo];
	data.boardTwo.channelThree = results[dosimeterBoardTwo][adcChannelThree];
	data.boardTwo.channelFour = results[dosimeterBoardTwo][adcChannelFour];
	data.boardTwo.channelFive = results[dosimeterBoardTwo][adcChannelFive];
	data.boardTwo.channelSix = results[dosimeterBoardTwo][adcChannelSix];
	data.boardTwo.channelSeven = results[dosimeterBoardTwo][adcChannelSeven];

	// send formatted protobuf messages to downlink manager
	error = fileTransferAddMessage(&data, sizeof(data), FileTransferMessage_dosimeterData_tag);

	return  error;
}


/**
 * Return the temperature reading from one of the Dosimeter Boards.
 *
 * @param board Which of the two boards to read from.
 * @return The temperature; a float cast as a uint16_t
 */
int16_t dosimeterTemperature(dosimeterBoard_t board) {

	// internal buffer for receiving the I2C responses
	uint8_t dataResponse[DOSIMETER_RESPONSE_LENGTH] = { 0 };

	uint8_t dosimeterBoard = board;
	uint8_t adcChannel = temperatureSensor;

	// tell dosimeter to begin conversion; receive 12-bit data into our internal buffer
	int error = i2cTalk(dosimeterBoardSlaveAddr[dosimeterBoard], DOSIMETER_COMMAND_LENGTH,
						DOSIMETER_RESPONSE_LENGTH, &dosimeterCommandBytes[adcChannel],
						dataResponse, DOSIMETER_I2C_DELAY);

	// return 1 if an error occurs
	if (error != 0)
		return E_GENERIC;

	// obtain the voltage reading
	float voltageReading = convertCountsToVoltage(dataResponse[0], dataResponse[1]);

	// obtain the real temperature
	int16_t temperature = convertVoltageToTemperature(voltageReading);

	return temperature;
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Convert raw ADC counts (0 to 4095) to a real voltage reading (in mV)
 *
 * @note based off of a 3V3 reference voltage (per dosimeter board design)
 *
 * @param highByte High (most significant) byte of 12-bit reading
 * @param lowByte Low (least significant) byte of 12-bit reading
 * @return The real voltage reading of the sensor being measured (in mV)
 */
static float convertCountsToVoltage(uint8_t highByte, uint8_t lowByte) {

	// high byte (top 4 bits of 12-bit value) must be masked & bit-shifted
	uint8_t conversionResultHighByte = ((highByte & DOSIMETER_RESPONSE_HIGH_BYTE_MASK) << 8);
	uint8_t conversionResultLowByte = lowByte;

	// combine high and low values
	float conversionResultTotal = conversionResultHighByte + conversionResultLowByte;

	// convert ADC counts to voltage (in mV)
	float voltageResult = DOSIMETER_REFERENCE_VOLTAGE_MV * ( conversionResultTotal / MAX_ADC_VALUE );

	return voltageResult;
}


/**
 * Convert a voltage (in mV) to temperature (in Celsius).
 *
 * Based off the datasheet for the LMT87 (Section on Trasnfer Function).
 * Note that this function is a linear approximation of a slightly parabolic
 * temperature curve; nonetheless, the result should always be within 1-2
 * degrees Celsius of the true value.
 *
 * @param voltage The voltage reading of the LMT87 (in mV)
 * @return The voltage (in degrees Celsius) as a whole number
 */
static float convertVoltageToTemperature(float voltage) {

	float temperature = (float)( ( voltage * TEMPERATURE_SCALE_SLOPE ) + TEMPERATURE_SCALE_OFFSET );
	return temperature;
}
