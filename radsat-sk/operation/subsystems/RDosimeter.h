/**
 * @file RDosimeter.h
 * @date February 11, 2020
 * @author Tyrel Kostyk
 */

#ifndef RDOSIMETER_H_
#define RDOSIMETER_H_

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/**
 * Reference voltage for the Melanin-Dosimeter boards. Sensors return relative
 * 8-bit values. E.g. 127 out of 255 would be 50%, representing 1.65V
 */
#define DOSIMETER_REFERENCE_VOLTAGE	((float)3.30)

/** I2C Slave Address for Dosimeter Board One */
#define DOSIMETER_1_I2C_SLAVE_ADDR (0x4A)
/** I2C Slave Address for Dosimeter Board Two */
#define DOSIMETER_2_I2C_SLAVE_ADDR (0x48)
/** Dosimeter Board Count (how many Dosimeter boards are there) */
#define DOSIMETER_COUNT (2)


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void requestReadingsAllChannels(void);


#endif /* RDOSIMETER_H_ */
