/*!
 * @file	LED.h
 * @brief	LED control.
 * @warning User may not change the entries in this header file. Values such as
 * define preprocessor directives are for information and not for configuration.
 * @date	Feb 16, 2012
 * @author	Akhil Piplani
 */

#ifndef LED_H_
#define LED_H_

typedef enum _LED {
	led_1 = 0,//!< led_1
	led_2 = 1,//!< led_2
	led_3 = 2,//!< led_3
	led_4 = 3 //!< led_4
} LED;

/*!
 * Initializes the LEDs by setting the corresponding pins as outputs on
 * the PIO controller and setting the correct logic state of the pins.
 */
void LED_start(void);

/*!
 * Makes the specified LED glow.
 * @param led LED number that should glow.
 */
void LED_glow(LED led);

/*!
 * Makes the specified LED dark.
 * @param led LED number that should be dark.
 */
void LED_dark(LED led);

/*!
 * Toggles the specified LED (Glow->Dark or Dark->Glow).
 * @param led LED number that should toggle.
 */
void LED_toggle(LED led);

/*!
 * Makes all LEDs switch on and off in a sequence.
 * @param times Number of times the wave should be repeated.
 */
void LED_wave(unsigned int times);

/*!
 * Makes all LEDs switch on and off in the reverse sequence as LED_wave.
 * @param times Number of times the wave should be repeated.
 */
void LED_waveReverse(unsigned int times);

#endif /* LED_H_ */
