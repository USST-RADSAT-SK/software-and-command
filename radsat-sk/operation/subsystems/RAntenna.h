/**
 * @file RAntenna.h
 * @date December 12, 2021
 * @author Addi Amaya (caa746)
 */

#ifndef RANTENNA_H_
#define RANTENNA_H_

#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** I2C slave address for antenna, primary */
#define ANTENNA_I2C_SLAVE_ADDR_PRIMARY (0x31)

/** I2C slave address for antenna, redundant */
#define ANTENNA_I2C_SLAVE_ADDR_REDUNANT (0x32)

/** number of attached Antennas in the system */
#define ANTENNAS_ON_BOARD 4

/** Number of Attempts allowed for deployment */
#define MAX_DEPLOYMENT_ATTEMPTS 5

/** Max Time allowed for a deployment for an Antenna in seconds */
#define MAX_DEPLOYMENT_TIMEOUT 60

/** Antenna Deployment status Struct */
typedef struct _antenna_deployment_status_t {
	int DeployedAntennaOne;
	int DeployedAntennaTwo;
	int DeployedAntennaThree;
	int DeployedAntennaFour;
	int AntennaArmed;
} antenna_deployment_status_t;

/** Struct that holds telemetry for one side of the Antenna */
typedef struct _antenna_telemetry_side_t {
	antenna_deployment_status_t deployStatus;	///< Antenna Deployment status.
	float board_temp; 					 		///< Antenna board temperature.
	uint32_t uptime;							///< Antenna Uptime in Seconds.
} antenna_telemetry_side_t;

/** Struct that holds all telemetry for the Antenna */
typedef struct _antenna_telemetry_t {
	antenna_telemetry_side_t sideA;	///< Antenna Telemetry for side A.
	antenna_telemetry_side_t sideB; ///< Antenna Telemetry for side B.
	uint32_t timestamp; 			///> The time (in seconds since Unix Epoch) that the message was formatted
} antenna_telemetry_t;



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int antennaInit(void);
int antennaDeploymentAttempt(void);
int antennaTelemetry(antenna_telemetry_t* telemetry);


#endif /* RANTENNA_H_ */
