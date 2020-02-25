/*  payload_processing.c

	Main location for functions controlling and interacting with the
	experimental Melanin-Dosimeter Payload boards. Data is read from all eight
	channels on both boards, via the ads7828 ADC from Texas Instruments (one per
	board). Data is recorded daily (every 24 hours), with timing controlled by
	the higher-level FreeRTOS tasks.

	Created by Tyrel Kostyk on February 11th 2020
 */


#include <housekeeping/i2c.h>
#include <housekeeping/memory.h>


//==============================================================================
//                                   GLOBALS
//==============================================================================

/**
 *  The slave addresses of the two Melanin-Dosimeter boards.
 *	TODO: confirm once actual addresses are determined
 */
uint8_t dosimeterBoardSlaveAddr[2] = {
	0x90,  // board one - 1001.0000
	0x92   // board two - 1001.0010
};


/**
 *  Byte commands to request data from each of the 8 channels on each
 *	Melanin-Dosimeter board. See datasheet for details.
 */
uint8_t dosimeterCommandBytes[8] = {
	0x84,     // 0 - 1000.0100
	0xC4,     // 1 - 1100.0100
	0x94,     // 2 - 1001.0100
	0xD4,     // 3 - 1101.0100
	0xA4,     // 4 - 1010.0100
	0xE4,     // 5 - 1110.0100
	0xB4,     // 6 - 1011.0100
	0xF4      // 7 - 1111.0100
};


//==============================================================================
//                                  FUNCTIONS
//==============================================================================

/** @brief Request and store readings from all Melanin-Dosimeter channels.
 *
 *	Iterates through each channel on both boards to collect and store payload
 *	data from the experimental Melanin-Dosimeter boards. Relies on asynchronous
 *	I2C calls to send commands and receive the information in a response.
 *
 *	@header	"software-and-command/payload_processing/payload_processing.h"
 *	@param	void
 *	@pre	Melanin-Dosimeter boards are powered on. No initialization necessary
 * 	@post	Payload data is stored in non-volatile memory (TODO: location TBD)
 * 	@return	void
 */
void requestReadingsAllChannels( void )
{
	// iterate through both melanin-dosimeter boards
	for ( uint8_t dosimeterBoard = 0; dosimeterBoard < 2; dosimeterBoard++ ) {

    	// request data from each sensor on a particular board
		for ( uint8_t adcChannel = 0; adcChannel < 8; adcChannel++ ) {

			// write command to dosimeter board using i2c
			uint8_t i2cResult = i2c->writeByte( dosimeterBoardSlaveAddr[dosimeterBoard], dosimeterCommandBytes[adcChannel] );

			if ( i2cResult != 0 ) {
				// 0 = success code; anything else is a failure
				// TODO: replace assertion with logging and proper error handling
				ASSERT("ERROR");
			}

			// send the recieve command over i2c to dosimeter board; store return values
			// high byte contains bits 11-8 (most signifigant), low contains bits 7-0
			// TODO: modify in the future to work with real I2C commands
			uint8_t conversionResultHighByte = i2c->readByte( dosimeterBoardSlaveAddr[dosimeterBoard] );
			uint8_t conversionResultLowByte = i2c->readByte( dosimeterBoardSlaveAddr[dosimeterBoard] );

			// check for zero values in response (values should never be zero)
			if ( conversionResultHighByte == 0 ) {
				// reading should never be 0
				// TODO: replace assertion with logging and proper error handling
				ASSERT("WARNING");
			}
			else if ( conversionResultLowByte == 0 ) {
				// reading should never be 0
				// TODO: replace assertion with logging and proper error handling
				ASSERT("WARNING");
			}

			// combine high and low values
			uint16_t conversionResultTotal = ( ( resultByteHigh & 0x0F ) << 8 ) + resultByteLow;

			// scale conversion result to final voltage value
			float voltageResult = DOSIMETER_REFERENCE_VOLTAGE * ( conversionResultTotal / 255.0 )

			// store final voltage result in memory
			storePayload( dosimeterBoard, adcChannel, voltageResult );
    }
  }
}
