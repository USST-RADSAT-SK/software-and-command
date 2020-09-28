/*
 * isis_eps_demo.h
 *
 *  Created on: 22 sep. 2015
 *      Author: pbot
 */

#ifndef ISIS_EPS_DEMO_H_
#define ISIS_EPS_DEMO_H_

#include <hal/boolean.h>


/***
 * Starts demo.
 * Calls Init and Menu in sequence.
 * Returns FALSE on failure to initialize.
 */
Boolean isis_eps__demo__main(void);

/***
 * Initializes the isis_epsS subsystem driver.
 * Returns FALSE on failure.
 *
 * note:
 * Depends on an initialized I2C driver.
 * Initialize the I2C interface once before using
 * any of the subsystem library drivers
 */
Boolean isis_eps__demo__init(void);

/***
 * Loop producing an interactive
 * text menu for invoking subsystem functions
 * note:
 * Depends on an initialized isis_eps subsystem driver.
 */
void isis_eps__demo__loop(void);

/***
 * (obsolete) Legacy function to start interactive session
 * Always returns TRUE
 *
 * Note:
 * Use isis_eps__demo__main instead.
 */
Boolean isis_eps__test(void);

#endif /* ISIS_EPS_DEMO_H_ */
