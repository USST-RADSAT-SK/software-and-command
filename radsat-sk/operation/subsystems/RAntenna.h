/**
 * @file RAntenna.h
 * @date December 12, 2021
 * @author Addi Amaya (caa746)
 */

#ifndef RANTENNA_H_
#define RANTENNA_H_

#include <stdint.h>
#include <RFileTransfer.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** I2C slave address for antenna, primary */
#define ANTENNA_I2C_SLAVE_ADDR_PRIMARY (0x31)

/** I2C slave address for antenna, redundant */
#define ANTENNA_I2C_SLAVE_ADDR_REDUNANT (0x32)

/** number of attached Antennas in the system */
#define ANTENNAS_ON_BOARD 1

/** Number of Attempts allowed for deployment */
#define MAX_DEPLOYMENT_ATTEMPTS 3

/** Max Time allowed for a deployment for an Antenna in seconds */
#define MAX_DEPLOYMENT_TIMEOUT 60

/** Time delay between deployment attempts in milliseconds */
#define INTER_DEPLOYMENT_DELAY_MS 15*1000


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int antennaInit(void);
void antennaDeploymentAttempt(void* parameters);
int antennaTelemetry(antenna_telemetry* telemetry);
int antennaReset(void);

#endif /* RANTENNA_H_ */
