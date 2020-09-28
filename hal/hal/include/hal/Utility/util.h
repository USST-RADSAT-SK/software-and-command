/*!
 * @file	util.h
 * @brief	Utility based functions for reading and writing with Debug UART.
 * @date	Apr 06, 2015
 * @author	Pieter Botma
 */
/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef UTIL_H
#define UTIL_H

//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

/**
 * @brief	Writes data a point to location to debug UART as memory words
 * in hexadecimal format
 * @param[in] pBuffer Pointer to memory location where data is stored
 * @param[in] size Size of data to be output in number of bytes
 */
extern void UTIL_DbguDumpMemory(unsigned char *pBuffer, unsigned int size);

/**
 * @brief	Writes data at pointer location to debug UART as individual bytes
 * in hexadecimal format
 * @param[in] pBuffer Pointer to memory location where data is stored
 * @param[in] size Size of data to be output in number of bytes
 */
extern void UTIL_DbguDumpArrayBytes(unsigned char *pBuffer, unsigned int size);

/**
 *  @brief    Reads an integer from the debug UART
 *  @warning  This is a blocking function which will cause the watchdog to reset the
 *  iOBC when the debug connector is not connected and will prevent the RTOS
 *  scheduler and other tasks from running
 *  @param[out] pValue Pointer to storage location for input value
 *  @return   1 if successful, 0 if input is not a number
 */
extern unsigned char UTIL_DbguGetInteger(unsigned int *pValue);

/**
 *  @brief    Reads an integer between a minimum and maximum value from the debug UART
 *  @warning  This is a blocking function which will cause the watchdog to reset the
 *  iOBC when the debug connector is not connected and will prevent the RTOS
 *  scheduler and other tasks from running
 *  @param[in] min Minimum value accepted as input
 *  @param[in] max Maximum value accepted as input
 *  @param[out] pValue Pointer to storage location for input value
 *  @return   1 if successful, 0 if input is not a number or outside of specified range
 */
extern unsigned char UTIL_DbguGetIntegerMinMax(unsigned int *pValue, unsigned int min, unsigned int max);

/**
 *  @brief    Reads a hexadecimal number from the debug UART
 *  @warning  This is a blocking function which will cause the watchdog to reset the
 *  iOBC when the debug connector is not connected and will prevent the RTOS
 *  scheduler and other tasks from running
 *  @param[out] pValue Pointer to storage location for input value
 *  @return   1 if successful, 0 if input is not a number or not a hexadecimal number
 */
extern unsigned char UTIL_DbguGetHexa32(unsigned int *pValue);

/**
 * @brief Reads a string from the debug UART
 * @warning The char array should be(at least) one byte larger than the size parameter
 * in order to accommodate the null terminating character
 * @warning  This is a blocking function which will cause the watchdog to reset the
 * iOBC when the debug connector is not connected and will prevent the RTOS
 * scheduler and other tasks from running
 * @param[in] str Pointer to char array to store received string
 * @param[in] size Size of the string(excluding null terminating character)
 */
extern void UTIL_DbguGetString(char* str, int size);

#endif //#ifndef UTIL_H

