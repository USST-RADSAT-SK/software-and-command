/*
 * IsisGomEPSdemo.h
 *
 *  Created on: 11 nov. 2013
 *      Author: malv
 */

#ifndef ISISGOMEPSDEMO_H_
#define ISISGOMEPSDEMO_H_

#include <hal/boolean.h>

/***
 * Starts demo.
 * Calls Init and Menu in sequence.
 * Returns FALSE on failure to initialize.
 */
Boolean GomEPSdemoMain(void);

/***
 * Initializes the subsystem driver on the IOBC.
 * Returns FALSE on failure.
 *
 * note:
 * Depends on an initialized I2C driver.
 * Initialize the I2C interface once before using
 * any of the subsystem library drivers
 */
Boolean GomEPSdemoInit(void);

/***
 * Loop producing an interactive
 * text menu for invoking subsystem functions
 * note:
 * Depends on an initialized GomEPS subsystem driver.
 */
void GomEPSdemoLoop(void);

/***
 * (obsolete) Legacy function to start interactive session
 * Always returns TRUE
 *
 * Note:
 * Use GomEPSdemoMain function instead.
 */
Boolean GomEPStest(void);

#endif /* ISISGOMEPSDEMO_H_ */
