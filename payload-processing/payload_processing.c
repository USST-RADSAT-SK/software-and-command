

#include <housekeeping/i2c.h>
#include <housekeeping/memory.h>

// request a reading from one channel
void requestReadingsAllChannels( void )
{
  for (int board = 0; board < 2; board++) {
    // iterate through both boards

    for (int channel = 0; channel < 8; channel++) {
      // write command to dosimeter board using i2c
      uint8_t result = i2c->writeByte(dosimeterBoardSlaveAddrsWrite[board], dosimeterCommandBytes[channel]);

      if ( result ) {
        // 0 = failure
        ASSERT("ERROR");
      }

      // write recieve command over i2c to board
      uint8_t conversionResultByteHigh = i2c->readByte(dosimeterBoardSlaveAddrsRead[board]);
      uint8_t conversionResultByteLow = i2c->readByte(dosimeterBoardSlaveAddrsRead[board]);

      // combine high and low bytes
      uint16_t conversionResultTotal = ( resultByteHigh & 0x0F ) + resultByteLow;

      // scale conversion result to final voltage value
      float voltageResult = 3.30 * ( conversionResultTotal / 255.0 )

      // store final voltage result in memory
      storePayload( board, channel, voltageResult );
    }
  }
}
