/**
 * @file RTransceiver.h
 * @date December 12, 2021
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RTRANSCEIVER_H_
#define RTRANSCEIVER_H_

#include <stdint.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define TRX_RECEIVER_FRAME_PREAMBLE_SIZE	(6)

#define TRANCEIVER_RX_MAX_FRAME_SIZE	(200)
#define TRANCEIVER_TX_MAX_FRAME_SIZE	(235)

#define TRX_RECEIVER_MAX_FRAME_SIZE	((uint16_t) TRANCEIVER_RX_MAX_FRAME_SIZE+TRX_RECEIVER_FRAME_PREAMBLE_SIZE)

/** I2C Slave Address for Transceiver Receive Port */
#define TRANSCEIVER_RX_I2C_SLAVE_ADDR (0x60)
/** I2C Slave Address for Transceiver Transmit Port */
#define TRANSCEIVER_TX_I2C_SLAVE_ADDR (0x61)


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int transceiverInit(void);
int transceiverFrameCount(void);
int transceiverGetFrame(uint8_t* msgBuffer);
int transceiverPowerCycle(void);


#endif /* RTRANSCEIVER_H_ */
