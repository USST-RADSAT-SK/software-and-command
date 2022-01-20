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

/** Max sizes (in bytes) of uplink frames (from OBC perspective; no AX.25 headers) */
#define TRANCEIVER_RX_MAX_FRAME_SIZE	(200)
/** Max sizes (in bytes) of downlink frames (from OBC perspective; no AX.25 headers) */
#define TRANCEIVER_TX_MAX_FRAME_SIZE	(235)

/** I2C Slave Address for Transceiver Receive Port */
#define TRANSCEIVER_RX_I2C_SLAVE_ADDR (0x60)
/** I2C Slave Address for Transceiver Transmit Port */
#define TRANSCEIVER_TX_I2C_SLAVE_ADDR (0x61)


/** Struct that holds all telemetry for the Transmitter */
typedef struct _tx_telemetry_t {
	float tx_reflpwr; 		///< Tx Telemetry reflected power.
	float tx_fwrdpwr; 		///< Tx Telemetry forward power.
	float bus_volt; 		///< Tx Telemetry bus voltage.
	float vutotal_curr; 	///< Tx Telemetry total current.
	float vutx_curr; 		///< Tx Telemetry transmitter current.
	float vurx_curr; 		///< Tx Telemetry receiver current.
	float vupa_curr; 		///< Tx Telemetry power amplifier current.
	float pa_temp; 			///< Tx Telemetry power amplifier temperature.
	float board_temp; 		///< Tx Telemetry board temperature.
	uint32_t uptime;		///< Tx Uptime in Seconds.
} tx_telemetry_t;

/** Struct that holds all telemetry for the Receiver */
typedef struct _rx_telemetry_t {
	float rx_doppler; 		///< Rx Telemetry receiver doppler.
	float rx_rssi; 			///< Rx Telemetry rssi measurement.
	float bus_volt; 		///< Rx Telemetry bus voltage.
	float vutotal_curr; 	///< Tx Telemetry total current.
	float vutx_curr; 		///< Tx Telemetry transmitter current.
	float vurx_curr; 		///< Tx Telemetry receiver current.
	float vupa_curr; 		///< Tx Telemetry power amplifier current.
	float pa_temp; 			///< Rx Telemetry power amplifier temperature
	float board_temp; 		///< Rx Telemetry board temperature.
	uint32_t uptime;		///< Rx Uptime in Seconds.
	uint16_t frames;		///< Rx Frames currently in Receive Buffer.
} rx_telemetry_t;


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
