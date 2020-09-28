/*
 * cspaceADCS.h
 *
 *  Created on: Jul 4, 2012
 *      Author: marcoalfer
 */

#ifndef ESLADCSDEMO_H_
#define ESLADCSDEMO_H_

#include <hal/boolean.h>

/***
 * Starts demo.
 * Calls Init and Menu in sequence.
 * Returns FALSE on failure to initialize.
 */
Boolean cspaceADCSdemoMain(void);

/***
 * Initializes the cspaceADCS subsystem driver.
 * Returns FALSE on failure.
 *
 * note:
 * Depends on an initialized I2C driver.
 * Initialize the I2C interface once before using
 * any of the subsystem library drivers
 */
Boolean cspaceADCSdemoInit(void);

/***
 * Loop producing an interactive
 * text menu for invoking subsystem functions
 * note:
 * Depends on an initialized cspaceADCS subsystem driver.
 */
void cspaceADCSdemoLoop(void);

/***
 * (obsolete) Legacy function to start interactive session
 * Always returns TRUE
 *
 * Note:
 * Use cspaceADCSdemoMain instead.
 */
Boolean cspaceADCStest(void);

#endif /* ESLADCSDEMO_H_ */
