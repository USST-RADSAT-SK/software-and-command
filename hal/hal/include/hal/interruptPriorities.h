/*!
 * @file	interruptPriorities.h
 * @date	June, 2012
 * @author	Akhil Piplani
 *
 * @brief Interrupt priorities of peripherals kept at the same place for easy tweaking.
 */

#ifndef INTERRUPTPRIORITIES_H_
#define INTERRUPTPRIORITIES_H_

// There are 8 priority levels, lowest = 0 = AT91C_AIC_PRIOR_LOWEST and highest = 7 = AT91C_AIC_PRIOR_HIGHEST
// FreeRTOS is configured to use the lowest one in the FreeRTOS source code
#define SDCARD_PRIORITY				(AT91C_AIC_PRIOR_LOWEST+1)
#define ADC_PRIORITY				(AT91C_AIC_PRIOR_LOWEST+3)
#define I2C_PRIORITY				(AT91C_AIC_PRIOR_LOWEST+4)
#define SPI0_PRIORITY				(AT91C_AIC_PRIOR_LOWEST+2)
#define SPI1_PRIORITY				(AT91C_AIC_PRIOR_LOWEST+2)
#define UART0_PRIORITY				(AT91C_AIC_PRIOR_LOWEST+2)
#define UART2_PRIORITY				(AT91C_AIC_PRIOR_LOWEST+2)
// ADC doesn't use an interrupt

#endif /* INTERRUPTPRIORITIES_H_ */
