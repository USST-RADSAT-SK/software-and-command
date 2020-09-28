/**
 * @file IsisSolarPanelv2.h
 * @brief ISIS Solar Panel temperature sensors, version 2 (using an LTC ADC driver)
 */

#ifndef SRC_ISISSOLARPANELV2_H_
#define SRC_ISISSOLARPANELV2_H_

#include <hal/Drivers/SPI.h>

#include <stdint.h>

/** Macro used for converting temperature to floating point value */
#define ISIS_SOLAR_PANEL_CONV ( 1.0 / 1024.0 )

/**
 * Return values for ISIS solar panel function calls.
 */
enum
{
	ISIS_SOLAR_PANEL_ERR_TIMEOUT = -5,
	ISIS_SOLAR_PANEL_ERR_ADC     = -4,
	ISIS_SOLAR_PANEL_ERR_PIN     = -3,
	ISIS_SOLAR_PANEL_ERR_SPI     = -2,
	ISIS_SOLAR_PANEL_ERR_STATE   = -1,
	ISIS_SOLAR_PANEL_ERR_NONE    =  0
};

/**
 * State of the internal temperature sensor.
 */
typedef enum _IsisSolarPanelv2_State_t
{
	ISIS_SOLAR_PANEL_STATE_NOINIT,
	ISIS_SOLAR_PANEL_STATE_SLEEP,
	ISIS_SOLAR_PANEL_STATE_AWAKE,
}
IsisSolarPanelv2_State_t;

/**
 * Channels available for temperature measurements.
 * Note: Some channels might not be in use
 */
typedef enum _IsisSolarPanel_panel_t
{
	ISIS_SOLAR_PANEL_0     = 0,
	ISIS_SOLAR_PANEL_1     = 1,
	ISIS_SOLAR_PANEL_2     = 2,
	ISIS_SOLAR_PANEL_3     = 3,
	ISIS_SOLAR_PANEL_4     = 4,
	ISIS_SOLAR_PANEL_5     = 5,
	ISIS_SOLAR_PANEL_6     = 6,
	ISIS_SOLAR_PANEL_7     = 7,
	ISIS_SOLAR_PANEL_8     = 8,
	ISIS_SOLAR_PANEL_COUNT = 9
}
IsisSolarPanelv2_Panel_t;

/**
 * Initializes the internal hardware and software required for measuring the
 * temperatures of the ISIS solar panels.
 *
 * @param[in] slave SPI slave to which LTC ADC driver is connected
 * @return A value defined by IsisSolarPanelv2_State_t
 */
int IsisSolarPanelv2_initialize( SPIslave slave );

/**
 * Puts the internal temperature sensor to sleep mode. Reducing its power-
 * consumption.
 *
 * @note When in sleep mode, the temperature needs to be woken by a call from
 *   IsisSolarPanelv2_wakeup() before attempting to acquire any temperature
 *   readings. Waking the sensor could take up to 500ms to complete.
 *
 * @return A value defined by IsisSolarPanelv2_State_t
 */
int IsisSolarPanelv2_sleep( void );

/**
 * Wakes the internal temperature sensor from sleep mode. Allowing it to acquire
 * temperature readings from the solar panels.
 *
 * @note Waking the sensor could take up to 500ms to complete.
 *
 * @return A value defined by IsisSolarPanelv2_State_t
 */
int IsisSolarPanelv2_wakeup( void );

/**
 * Acquires the temperature from the specified panel as well as a status byte
 * used for fault reporting. The temperature is reported from -273.16 to
 * 8192 degrees Celcius with a resolution of 1/1024.
 *
 * @param[in] panel The specified panel for which the temperature should be
 *   acquired
 * @param[out] panelTemp The acquired temperature for the specified panel
 * @param[out] status The status of the acquired temperature. Used for fault
 *   reporting.
 * @return A value defined by IsisSolarPanelv2_State_t
 */
int IsisSolarPanelv2_getTemperature( IsisSolarPanelv2_Panel_t panel, int32_t* paneltemp, uint8_t *status );

/**
 * Returns the current state of the internal temperature sensor.
 *
 * @return The state as define by IsisSolarPanelv2_State_t
 */
IsisSolarPanelv2_State_t IsisSolarPanelv2_getState( void );

#endif /* SRC_ISISSOLARPANELV2_H_ */
