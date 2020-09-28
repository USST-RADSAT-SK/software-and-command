/*
 * IsisMTQv2demo.h
 *
 *  Created on: 13 mrt. 2015
 *      Author: malv
 */

#ifndef ISISMTQV2DEMO_H_
#define ISISMTQV2DEMO_H_

#include <hal/boolean.h>

/***
 * Starts demo.
 * Calls Init and Menu in sequence.
 * Returns FALSE on failure to initialize.
 */
Boolean IsisMTQv2demoMain(void);

/***
 * Initializes the IMTQv2 subsystem driver.
 * Returns FALSE on failure.
 *
 * note:
 * Depends on an initialized I2C driver.
 * Initialize the I2C interface once before using
 * any of the subsystem library drivers
 */
Boolean IsisMTQv2demoInit(void);

/***
 * Loop producing an interactive
 * text menu for invoking subsystem functions
 * note:
 * Depends on an initialized IsisMTQv2 subsystem.
 */
void IsisMTQv2demoLoop(void);

/***
 * (obsolete) Legacy function to start interactive session
 * Always returns TRUE
 *
 * Note:
 * Use IsisMTQv2demoMain instead.
 */
Boolean IsisMTQv2test(void);


#endif /* ISISMTQV2DEMO_H_ */
