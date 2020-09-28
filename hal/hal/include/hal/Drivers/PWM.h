/*!
 * @file	PWM.h
 * @brief	Pulse Width Modulation control.
 * @warning User may not change the entries in this header file. Values such as
 * define preprocessor directives are for information and not for configuration.
 * @date	Feb 27, 2013
 * @author	Akhil Piplani
 */

#ifndef PWM_H_
#define PWM_H_

/*!
 * Maximum count value allowed at the Timer-Counter peripheral.
 */
#define TC_UPCOUNT_MAXVAL	0xFFFF

/*!
 * Initializes the PWM driver that uses the Timer-Counter (TC) peripherals
 * TC0, TC1 and TC2 and provides 6 PWM signals.
 * The Timer will count from 0 up to maxUpCount and use the selected clock
 * as an input to the timer module.
 * The PWM lines are set when the counter reaches maxUpCount and cleared
 * when the counter reaches the specified duty-cycle values.
 * The combination of maxUpCount and timerClock affects the PWM resolution
 * and frequency. \n
 *
 * pwm_frequency = timerClockSpeed / maxUpCount
 *
 * @param[in] maxUpCount The maximum value up to which the timer will count.
 * The maximum value of this input is 0xFFFF and minimum 1.
 * @param[in] timerClock The input clock of the timers to be used.
 * The input value selects the timerClockSpeed by the following factors:
 * 0 = BOARD_MCK/2, 1 = BOARD_MCK/8, 2 = BOARD_MCK/32, 3 = BOARD_MCK/128
 * Other values are invalid. Normally, BOARD_MCK = 132MHz.
 * Note that BOARD_MCK is not the processor clock, which is normally 400MHz.
 * @return -1 if maxUpCount is greater than 0x3FF or maxUpCount is
 * less than 1 or timerClock is greater than 3, \n
 * 0 on success.
 *
 * @note Very low MaxUpCount will lead to a low resolution of the PWM duty-cycles.
 */
int PWM_start(unsigned short maxUpCount, unsigned int timerClock);

/*!
 * Initializes the PWM driver that uses the Timer-Counter (TC) peripherals
 * TC0, TC1 and TC2 and provides 6 PWM signals.
 * This function will select the input clock automatically to achieve the
 * desired PWM frequency.
 * The Timer will count up from 0 to MaxUpCount.
 * The PWM lines are set when the counter reaches maxUpCount and cleared
 * when the counter reaches the specified duty-cycle values.
 *
 * @param[in] frequency The desired frequency of the PWM signal. Maximum: BOARD_MCK/4
 * minimum BOARD_MCK/(128*0xFFFF). Normally, BOARD_MCK = 132MHz.
 * Note that BOARD_MCK is not the processor clock, which is normally 400MHz.
 * @return Negative values signify error, positive values signify the MaxUpCount value.
 * -1 is returned when the desired frequency cannot be achieved.
 *
 * @note Very low MaxUpCount will lead to a low resolution of the PWM duty-cycles.
 */
int PWM_startAuto(unsigned int frequency);

/*!
 * Sets the duty cycle of 6 PWM channels based on the input.
 * @param[in] channels Pointer to an array of 6 unsigned integers containing DutyCycle values in percent.
 * @return -1 if any of the input duty cycles is >100,
 * 0 on success.
 */
int PWM_setDutyCycles(unsigned int *channels);

/*!
 * Sets the duty cycle of 6 PWM channels based on the input.
 * @param[in] channels Pointer to an array of 6 unsigned integers containing
 * DutyCycle values in raw form. The relation between raw inputs and percentage is:
 * @verbatim
   channelsRaw[i] = channels[i] * maxUpCount / 100;
 * @endverbatim
 * Here, maxUpCount (specified in PWM_start or returned by PWM_startAuto) is the
 * maximum value the raw input should take.
 * @return -1 if any of the input duty cycles is > MaxUpCount,
 * 0 on success
 */
int PWM_setRawDutyCycles(unsigned short *channels);

/*!
 * Sets all channel duty cycles to 0 percent.
 */
void PWM_setAllDutyCyclesZero(void);

/*!
 * Sets all channel duty cycles to 100 percent.
 * This will cause the PWM lines to stay at logic high.
 */
void PWM_setAllDutyCyclesFull(void);

/*!
 * Stops the PWM driver and sets all outputs to logic 0.
 */
void PWM_stop(void);

#endif /* PWM_H_ */
