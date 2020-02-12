




uint8_t dosimeterBoardSlaveAddrsWrite[2] = {
  0x90,  // board one - 1001.0000
  0x92   // board two - 1001.0010
};

uint8_t dosimeterBoardSlaveAddrsRead[2] = {
  0x91,  // board one - 1001.0001
  0x93   // board two - 1001.0011
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


void requestReadingsAllChannels( void );
