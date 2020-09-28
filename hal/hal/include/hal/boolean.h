/*
 * boolean.h
 *
 *  Created on: 03-Aug-2012
 *      Author: Akhil Piplani
 */
/*!
 * @file	boolean.h
 * @brief	Exposes some boolean data types.
 *
 * Values like 0xFFFFFFFF and 0xFF are used instead of 1 for reliability as this increases  the
 * number of bit-flips that turn a TRUE into FALSE. Preference is placed within ISIS drivers
 * to use comparisons with FALSE.
 *
 * @verbatim
 	 // Instead of
 	 if(flag == TRUE) {
 	 	 takeAction();
 	 }
 	 // We use
 	 if(flag != FALSE) {
 	 	 takeAction();
 	 }
 	 // Or
	 if(flag) {
 	 	 takeAction();
 	 }
 * @endverbatim
 *
 * @date	Aug 03, 2012
 * @author	Akhil Piplani
 */

#ifndef BOOLEAN_H_
#define BOOLEAN_H_

typedef unsigned int  Boolean;
typedef unsigned char Boolean8bit;

#define TRUE		0xFFFFFFFF
#define FALSE		0

#define TRUE_8BIT	0xFF
#define FALSE_8BIT	0

#endif /* BOOLEAN_H_ */
