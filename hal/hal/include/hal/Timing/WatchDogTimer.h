/*!
 * @file WatchDogTimer.h
 * @brief Interface library to kick the external watchdog function of the supervisor chip on the OBC.
 * The external watchdog is frequency based. This means that it will power-cycle the CPU if it is kicked
 * too slow and too fast. The allowed frequency range of the watchdog is 1000Hz(1ms) to 20Hz(50ms).
 * Therefore, a conservative range for kicking the watchdog is 200Hz(5ms) to 25Hz(30ms).
 */

#ifndef WATCHDOGTIMER_H_
#define WATCHDOGTIMER_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "hal/boolean.h"

/*!
 * The external watchdog starts upon power-up by itself.
 * This function initializes the elements needed to be able to kick the watchdog.
 */
void WDT_start(void);

/*!
 * Kicks the watchdog if needed and increments the KickCounter.
 * The external watchdog is kicked only if the KickCounter is equal to or greater than the input parameter N.
 * If the watchdog is kicked, the KickCounter is reset.
 * @param N Number of times this function needs to be called before the watchdog is kicked.
 */
void WDT_kickEveryNcalls(unsigned int N);

/*!
 * Kicks the external watchdog irrespective of the value of the KickCounter.
 * Also resets the KickCounter.
 */
void WDT_forceKick(void);

/*!
 * Force kicks the watchdog if the last kick of the watchdog was more than N ticks ago
 */
void WDT_forceKickEveryNms( portTickType N );

/*!
 * Starts a low-priority FreeRTOS task that kicks the watchdog at the specified interval.
 * Also calls the WDT_start function to ensure the watchdog interface is initialized.
 * An example for initializing the watchdog correctly is:
 * @code
 * WDT_startWatchdogKickTask(10 / portTICK_RATE_MS, FALSE);
 * @endcode
 *
 * @param kickInterval Number of ticks between kicking the watchdog. A conservative range for this parameter is 5ms to 30ms.
 * @param toggleLed1 If this value is set to TRUE, Led-1 is toggled each time the watchdog is kicked.
 * This can be used to visually verify the heartbeat of the OBC.
 * @returns 0 on success and -1 if kick task has not been started
 */
int WDT_startWatchdogKickTask(portTickType kickInterval, Boolean toggleLed1);

/*!
 * Stops the FreeRTOS task that kicks the watchdog
 */
void WDT_stopWatchdogKickTask(void);

#endif /* WATCHDOGTIMER_H_ */
