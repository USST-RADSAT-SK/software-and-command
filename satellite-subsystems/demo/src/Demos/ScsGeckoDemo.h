/*
 * ScsGeckoDemo.h
 *
 *  Created on: 29 jan. 2018
 *      Author: pbot
 */

#ifndef DEMOS_SCSGECKODEMO_H_
#define DEMOS_SCSGECKODEMO_H_

#include <hal/boolean.h>

/***
 * Starts demo.
 * Calls Init and Menu in sequence.
 * Returns FALSE on failure to initialize.
 */
Boolean ScsGeckoDemoMain(void);

/***
 * Initializes the SCS Gecko subsystem driver.
 * Returns FALSE on failure.
 *
 * note:
 * Depends on an initialized SPI driver.
 * Initialize the SPI interface once before using
 * any of the subsystem library drivers
 */
Boolean ScsGeckoDemoInit(void);

/***
 * Loop producing an interactive
 * text menu for invoking subsystem functions
 * note:
 * Depends on an initialized SCS Gecko subsystem driver.
 */
void ScsGeckoDemoLoop(void);

/***
 * (obsolete) Legacy function to start interactive session
 * Always returns TRUE
 *
 * Note:
 * Use ScsGeckoDemoMain instead.
 */
Boolean GeckoTest(void);

#endif /* DEMOS_SCSGECKODEMO_H_ */
