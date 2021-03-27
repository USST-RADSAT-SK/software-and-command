/**
 * @file RDosimeter.h
 * @date February 11, 2020
 * @author Tyrel Kostyk
 */

#ifndef RDOSIMETER_H_
#define RDOSIMETER_H_

/**
 * Reference voltage for the Melanin-Dosimeter boards. Sensors return relative
 * 8-bit values. E.g. 127 out of 255 would be 50%, representing 1.65V
 */
#define DOSIMETER_REFERENCE_VOLTAGE	((float)3.30)


void requestReadingsAllChannels( void );


#endif /* RDOSIMETER_H_ */
