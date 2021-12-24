/**
 * @file RDosimeter.c
 * @date February 11, 2020
 * @author Tyrel Kostyk
 */

#ifdef DONT_COMPILE_THE_FOLLOWING_CODE

// TODO: update this code to use real code instead of pseudocode


//==============================================================================
//                                   GLOBALS
//==============================================================================

/**
 *  The slave addresses of the two Melanin-Dosimeter boards.
 *	TODO: confirm once actual addresses are determined
 */
uint8_t dosimeterBoardSlaveAddr[2] = {
	DOSIMETER_1_I2C_SLAVE_ADDR,
	DOSIMETER_2_I2C_SLAVE_ADDR
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
	for ( uint8_t dosimeterBoard = 0; dosimeterBoard < DOSIMETER_COUNT; dosimeterBoard++ ) {

    	// request data from each sensor on a particular board
		for ( uint8_t adcChannel = 0; adcChannel < 8; adcChannel++ ) {

			// write command to dosimeter board using i2c
			uint8_t i2cResult = i2c->writeByte( dosimeterBoardSlaveAddr[dosimeterBoard],
												dosimeterCommandBytes[adcChannel]
											  	);

			// check for success of I2C command
			if ( i2cResult != 0 ) {
				// 0 = success code; anything else is a failure
				// TODO: replace assertion with logging and proper error handling
				ASSERT("ERROR");
			}

			// TODO: Do we need to add any delays? Don't think so, but should confirm

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

#endif	// DONT_COMPILE_THE_FOLLOWING_CODE

