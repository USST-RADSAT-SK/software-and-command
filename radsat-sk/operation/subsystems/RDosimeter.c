/**
 * @file RDosimeter.c
 * @date December 28, 2021
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#include <RDosimeter.h>
#include <RI2c.h>
#include <string.h>
#include <stdint.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/**
 * Reference voltage for the Melanin-Dosimeter boards. Sensors return relative
 * 8-bit values. E.g. 127 out of 255 would be 50%, representing 1.65V
 */
#define DOSIMETER_REFERENCE_VOLTAGE	((float)3.30)

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

/** The two Melanin-Dosimeter boards. */
enum _dosimeterBoards {
	dosimeterBoardOne,
	dosimeterBoardTwo,
	dosimeterBoardCount,
};

/** The two Melanin-Dosimeter boards. */
enum _adcChannels {
	adcChannelZero,		// COTS 2.048V REF IC, Shielding: none
	adcChannelOne,		// COTS 2.048V REF IC, Shielding: 50 mil
	adcChannelTwo,		// COTS 2.048V REF IC, Shielding: 100 mil
	adcChannelThree,	// COTS 2.048V REF IC, Shielding: 200 mil
	adcChannelFour,		// COTS 2.048V REF IC, Shielding: 20 mil
	adcChannelFive,		// RADFET Experimental Dosimeter, Shielding: none
	adcChannelSix,		// COTS 2.048V REF IC, Shielding: 300 mil
	adcChannelSeven,	// Temperature Sensor
	adcChannelCount,
};

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
 * @return Returns 0 on Success, anything else indicates error.
 */
int requestReadingsAllChannels(void)
{
	uint8_t dataResponse[DOSIMETER_RESPONSE_LENGTH] = { 0 };

	// iterate through both melanin-dosimeter boards
	for (uint8_t dosimeterBoard = dosimeterBoardOne; dosimeterBoard < dosimeterBoardCount; dosimeterBoard++) {

		// request data from each sensor on a particular board
		for (uint8_t adcChannel = adcChannelOne; adcChannel < adcChannelCount; adcChannel++) {

			// reset internal buffer
			memset(dataResponse, 0, DOSIMETER_RESPONSE_LENGTH);

			// tell dosimeter to begin conversion; receive 12-bit data into our internal buffer
			int error = i2cTalk(dosimeterBoardSlaveAddr[dosimeterBoard], DOSIMETER_COMMAND_LENGTH,
								DOSIMETER_RESPONSE_LENGTH, &dosimeterCommandBytes[adcChannel],
								dataResponse, DOSIMETER_I2C_DELAY);

			// check for success of I2C command
			if ( error != 0 )
				return error;

			// extract the high (sent first) and low (sent second) data bytes from the I2C response
			// high byte (top 4 bits of 12-bit value) must be masked & bit-shifted
			uint8_t conversionResultHighByte = ((dataResponse[0] & DOSIMETER_RESPONSE_HIGH_BYTE_MASK) << 8);
			uint8_t conversionResultLowByte = dataResponse[1];

			// combine high and low values
			uint16_t conversionResultTotal = conversionResultHighByte + conversionResultLowByte;

			// scale conversion result (based on reference voltage)
			// TODO: figure out what our external ADC reference voltage is
			float voltageResult = DOSIMETER_REFERENCE_VOLTAGE * ( conversionResultTotal / MAX_ADC_VALUE );

			// TODO: format data into a Protobuf message

			// TODO: send formatted messages to downlink manager

		}
	}

	return 0;
}

