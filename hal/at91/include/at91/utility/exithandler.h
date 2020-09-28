/*!
 * @file	exithandler.h
 * @brief	Driver for reading Analog data (including OBC board temperature) using the ADC.
 * @author	Akhil Piplani
 * @date    Jun 22, 2011
 */

#ifndef EXITHANDLER_H_
#define EXITHANDLER_H_

/*!
 * @brief Performs any housekeeping needed before reset and then calls the
 * gracefulReset function. These two functions are not merged to save space
 * on the SRAM0 by placing a small function there.
 */
void restart();

/*!
 * @brief Same as restart, except prints a different message.
 * Do not call this function. To be used only by the Prefetch Abort handler.
 */
void restartPrefetchAbort(); // Only called by Prefetch Abort Handler

/*!
 * @brief Same as restart, except prints a different message.
 * Do not call this function. To be used only by the Data Abort Handler.
 */
void restartDataAbort(); // Only called by Data Abort Handler

/*!
 * @brief Resets the G20 gracefully by first powering down the SDRAM.
 *
 * This function must be located in the SRAM0 because it shuts down the SDRAM!
 * This is done by placing it in the sramfunc section.
 */
void gracefulReset() __attribute__ ((long_call, section (".sramfunc")));

#endif /* EXITHANDLER_H_ */
