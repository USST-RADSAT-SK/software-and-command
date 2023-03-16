/**
 * @file RTransceiver.h
 * @date December 12, 2021
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RTRANSCEIVER_H_
#define RTRANSCEIVER_H_

#include <stdint.h>
#include <float.h>
#include <RFileTransfer.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** Max sizes (in bytes) of uplink frames (from OBC perspective; no AX.25 headers). */
#define TRANCEIVER_RX_MAX_FRAME_SIZE	(200)
/** Max sizes (in bytes) of downlink frames (from OBC perspective; no AX.25 headers). */
#define TRANCEIVER_TX_MAX_FRAME_SIZE	(235)

/** Max number of frames that the receiver can hold. */
#define TRANCEIVER_RX_MAX_FRAME_COUNT	(40)
/** Max number of frames that the transmitter can hold. */
#define TRANCEIVER_TX_MAX_FRAME_COUNT	(40)

/** I2C Slave Address for Transceiver Receive Port */
#define TRANSCEIVER_RX_I2C_SLAVE_ADDR (0x60)
/** I2C Slave Address for Transceiver Transmit Port */
#define TRANSCEIVER_TX_I2C_SLAVE_ADDR (0x61)




/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int transceiverInit(void);
int transceiverRxFrameCount(uint16_t* numberOfFrames);
int transceiverGetFrame(uint8_t* messageBuffer, uint16_t* sizeOfMessage);
int transceiverSendFrame(uint8_t* message, uint8_t messageSize, uint8_t* slotsRemaining);
int transceiverPowerCycle(void);
int transceiverTelemetry(transceiver_telemetry* telemetry);
int transceiverResetWatchDogs(void);
int transceiverSoftReset(void);
int startInterFrameFill(void);
int stopInterFrameFill(void);


#endif /* RTRANSCEIVER_H_ */
