/*
 * IsisTxSdemo.h
 *
 *  Created on: 4 mrt. 2015
 *      Author: malv
 */

#ifndef ISISTXSDEMO_H_
#define ISISTXSDEMO_H_

#include <hal/boolean.h>

/***
 * Starts demo.
 * Calls Init and Menu in sequence.
 * Returns FALSE on failure to initialize.
 */
Boolean IsisTxSdemoMain(void);

/***
 * Initializes the TxS subsystem driver.
 * Returns FALSE on failure.
 *
 * note:
 * Depends on an initialized I2C driver.
 * Initialize the I2C interface once before using
 * any of the subsystem library drivers
 */
Boolean IsisTxSdemoInit(void);

/***
 * Loop producing an interactive
 * text menu for invoking subsystem functions
 * note:
 * Depends on an initialized TxS subsystem driver.
 */
void IsisTxSdemoLoop(void);

/***
 * (obsolete) Legacy function to start interactive session
 * Always returns TRUE
 *
 * Note:
 * Use IsisTxSdemoMain instead.
 */
Boolean TxStest(void);

#endif /* ISISTXSDEMO_H_ */
