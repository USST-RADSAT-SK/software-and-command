/**
 * @file IsisMTQv1.h
 * @brief ISIS Magnetorquer board version 1
 */

#ifndef ISISMTQV1_H_
#define ISISMTQV1_H_

/**
 *  Enumeration list of ISIS Magneto-torquer Temperature Sensor maximum resolution.
 */
typedef enum
{
    mtq_resolution_8_lsb = 0x00,
    mtq_resolution_4_lsb = 0x01,
    mtq_resolution_2_lsb = 0x02,
    mtq_resolution_1_lsb = 0x03
}isis_mtq_meas_resolution_t;

/**
 *  @brief      Initialize the iMTQ with the corresponding i2cAddress.
 *  @note       This function can only be called once.
 *  @param[in]  address array of iMTQ I2C Address structure.
 *  @param[in]  number number of attached iMTQ in the system to be initialized.
 *  @return     Error code according to <hal/errors.h>
 */
int isisMTQv1_initialize(unsigned char *address, unsigned char number);

/**
 *  @brief      Set the ISIS Magneto-torquer maximum resolution.
 *  @param[in]  index index of ISIS Magneto-torquer I2C bus address.
 *  @param[in]  resolution maximum resolution from ISIS Magneto-torquer.
 *  @return     the error message of the function.
 */
int IsisMTQv1SetResolution(unsigned char index, isis_mtq_meas_resolution_t resolution);

/**
 *  @brief      Obtain ISIS Magnetotorquer temperature.
 *  @param[in]  index index of ISIS Magnetotorquer I2C bus address.
 *  @param[out] temperature raw temperature value from ISIS Magnetotorquer.
 *  @return     the error message of the function.
 */
int IsisMTQv1GetTemperature(unsigned char index, unsigned short* temperature);

#endif /* ISISMTQV1_H_ */
