/*!
 * @file	RTC.h
 * @brief	Communicates with DS3234 RTC over SPI.
 * @author	Akhil Piplani
 * @date	Feb 11, 2011
 * @see		RTC.c
 */

#ifndef RTC_H_
#define RTC_H_

// The only thing used from Time.h is the LocalTime structure
#include "hal/Timing/Time.h"

/*!
 * Initializes the communication with the RTC DS3234.
 * If SPI bus0 is not initialized yet, it will be initialized.
 * @return -1 if initializing the SPI bus fails, 0 on success.
 */
int RTC_start(void);

/*!
 * Stops the RTC interface.
 */
void RTC_stop(void);

/*!
 * @brief Sets the time inside the RTC.
 * @param[in] time Pointer to an array whose value is going to be stored inside the RTC.
 * @return -2 if the input time was invalid, -1 if starting the SPI transfer failed, 0 on success.
 */
int RTC_setTime(const Time *time);

/*!
 * @brief This function gets the time inside the RTC.
 * @param[out] time Pointer to an LocalTime structure (timeStamp is not calculated).
 * @return -2 if starting SPI transfer fails, -1 if the time received from the RTC is out of range, 0 on success.
 */
int RTC_getTime(Time *time);

/*!
 * @brief Tests the RTC functionality whether it is still working fine or not.
 * @note This test may work incorrectly if other tasks are also using the SPI peripheral.
 * This is because the function sets the time at the RTC, reads it back
 * and checks it back to see if it's within the range of time that was set.
 * @return -2 if starting SPI transfer fails, -1 for incorrect time, 0 for normal.
 */
int RTC_testGetSet(void);

/*!
 * @brief Tests the RTC time by fetching the seconds value,
 * waiting 1 second and checking again.
 * @note This test may work incorrectly if other tasks are also using the SPI peripheral
 * as this can cause an unpredictable delay till the SPI transfer actually occurs.
 * @return -2 if SPI transfer to RTC failed, -1 for incorrect time, 0 for normal.
 */
int RTC_testSeconds(void);

/*!
 * Prints the seconds value at the RTC.
 * This function should be used for testing or demonstration only.
 */
void RTC_printSeconds(void);

/*!
 * @brief Checks a LocalTime structure for validity by checking if the
 * stored time is in the expected range.
 * @param[in] time RTC time structure to be checked.
 * @return -1 if the time stored in the structure is invalid, 0 otherwise
 */
int RTC_checkTimeValid(const Time *time);

/*!
 * @brief Reads 10bit temperature sensor measurement inside the RTC.
 * @param[in,out] temperature Location where temperature should be stored.
 * @return -1 if SPI transfer could not be started,
 * 0 on success.
 */
int RTC_getTemperature(float *temperature);
#endif /* RTC_H_ */
