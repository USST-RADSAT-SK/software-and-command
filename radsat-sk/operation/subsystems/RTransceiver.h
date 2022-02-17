/**
 * @file RTransceiver.h
 * @date December 12, 2021
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RTRANSCEIVER_H_
#define RTRANSCEIVER_H_

#include <stdint.h>
#include <float.h>


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


/** Struct that holds all telemetry for the Receiver */
typedef struct _rx_telemetry_t {
	float rx_doppler; 		///< Rx Telemetry receiver doppler (Hz, +/- 1000).
	float rx_rssi; 			///< Rx Telemetry rssi measurement (dBm, +/- 3 db).
	float bus_volt; 		///< Rx Telemetry bus voltage (V, +/- 0.055).
	float vutotal_curr; 	///< Tx Telemetry total current (mA, +/- 4).
	float vutx_curr; 		///< Tx Telemetry transmitter current (mA, +/- 4).
	float vurx_curr; 		///< Tx Telemetry receiver current (mA, +/- 4).
	float vupa_curr; 		///< Tx Telemetry power amplifier current (mA, +/- 4).
	float pa_temp; 			///< Rx Telemetry power amplifier temperature (C, +/- 1).
	float board_temp; 		///< Rx Telemetry board temperature (C, +/- 1).
	uint32_t uptime;		///< Rx Telemetry uptime (s).
	uint16_t frames;		///< Rx Frames currently in Receive Buffer.
} rx_telemetry_t;

/** Struct that holds all telemetry for the Transmitter */
typedef struct _tx_telemetry_t {
	float tx_reflpwr; 		///< Tx Telemetry reflected power (mW, +/- 150).
	float tx_fwrdpwr; 		///< Tx Telemetry forward power (mW, +/- 150).
	float bus_volt; 		///< Tx Telemetry bus voltage (V, +/- 0.055).
	float vutotal_curr; 	///< Tx Telemetry total current (mA, +/- 4).
	float vutx_curr; 		///< Tx Telemetry transmitter current (mA, +/- 4).
	float vurx_curr; 		///< Tx Telemetry receiver current (mA, +/- 4).
	float vupa_curr; 		///< Tx Telemetry power amplifier current (mA, +/- 4).
	float pa_temp; 			///< Tx Telemetry power amplifier temperature (C, +/- 1).
	float board_temp; 		///< Tx Telemetry board temperature (C, +/- 1).
	uint32_t uptime;		///< Tx Telemetry uptime (s).
} tx_telemetry_t;


/** Struct that holds all telemetry for the Transceiver */
typedef struct _transceiver_telemetry_t {
	tx_telemetry_t tx;	///< Transmitter Telemetry.
	rx_telemetry_t rx;	///< Receiver Telemetry.
} transceiver_telemetry_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int transceiverInit(void);
int transceiverRxFrameCount(uint16_t* numberOfFrames);
int transceiverGetFrame(uint8_t* messageBuffer, uint16_t* sizeOfMessage);
int transceiverSendFrame(uint8_t* message, uint8_t messageSize, uint8_t* slotsRemaining);
int transceiverPowerCycle(void);
int transceiverTelemetry(transceiver_telemetry_t* telemetry);


#endif /* RTRANSCEIVER_H_ */
