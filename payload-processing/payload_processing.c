/*  payload_processing.c

	Main entry point for functions controlling and interacting with the
	experimental Melanin-Dosimeter Payload boards.

	Created by Tyrel Kostyk on February 11th 2020
 */


#include <housekeeping/i2c.h>
#include <housekeeping/memory.h>


//==============================================================================
//                                   GLOBALS
//==============================================================================

uint8_t dosimeterBoardSlaveAddr[2] = {
	0x90,  // board one - 1001.0000
	0x92   // board two - 1001.0010
};


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

// request a reading from all channels
void requestReadingsAllChannels( void )
{
	// iterate through both melanin-dosimeter boards
	for ( uint8_t dosimeterBoard = 0; dosimeterBoard < 2; dosimeterBoard++ ) {

    	// request data from each sensor on a particular board
		for ( uint8_t adcChannel = 0; adcChannel < 8; adcChannel++ ) {

	      // write command to dosimeter board using i2c
		  uint8_t i2cResult = i2c->writeByte( dosimeterBoardSlaveAddr[dosimeterBoard], dosimeterCommandBytes[adcChannel] );

	      if ( i2cResult == 0 ) {
	        // 0 = failure code
	        // TODO: replace assertion with logging and proper error handling
	        ASSERT("ERROR");
	      }

	      // write 'recieve' command over i2c to dosimeter board, store return values
	      // consecutive reads; first read is 'high' values, second is 'low' values
	      uint8_t conversionResultByteHigh = i2c->readByte( dosimeterBoardSlaveAddr[dosimeterBoard] );
	      uint8_t conversionResultByteLow = i2c->readByte( dosimeterBoardSlaveAddr[dosimeterBoard] );

	      // combine high and low values
	      uint16_t conversionResultTotal = ( resultByteHigh & 0x0F ) + resultByteLow;

	      // scale conversion result to final voltage value
	      float voltageResult = DOSIMETER_REFERENCE_VOLTAGE * ( conversionResultTotal / 255.0 )

	      // store final voltage result in memory
	      storePayload( dosimeterBoard, adcChannel, voltageResult );
    }
  }
}
