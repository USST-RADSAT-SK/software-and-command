/**
 * @file RUart.h
 * @date May 5, 2021
 * @author Addi Amaya (caa746) & Jacob Waskowic (jaw352)
 */

#ifndef RUART_H_
#define RUART_H_

#include <stdint.h>
#include <hal/Drivers/UART.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define UART_CAMERA_BUS		((UARTbus) bus0_uart)


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int uartInit(UARTbus bus);
int uartTransmit(UARTbus bus, const uint8_t* data, uint16_t size);
int uartReceive(UARTbus bus, uint8_t* data, uint16_t size);

#endif /* RUART_H_ */
