/**
 * @file IsisSolarPanel.h
 * @brief ISIS Solar Panel temperature sensors
 */

#ifndef ISISSOLARPANEL_H_
#define ISISSOLARPANEL_H_

#include <hal/Drivers/SPI.h>

/**
 * Retrieve solar panel temperature
 *
 * @param[in] slave SPI chip select channel to which solar panel is connected
 * @param[out] paneltemp Solar panel temperature
 * @return Error code according to <hal/errors.h>
 */
int IsisSolarPanel_getTemperature(SPIslave slave, unsigned short* paneltemp);

#endif /* ISISSOLARPANEL_H_ */
