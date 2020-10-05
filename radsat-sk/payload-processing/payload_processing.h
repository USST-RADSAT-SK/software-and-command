/*  payload_processing.h

	Created by Tyrel Kostyk on February 11th 2020
 */


//==============================================================================
//                                    MACROS
//==============================================================================

/**
 *	Reference voltage for the Melanin-Dosimeter boards. Sensors return relative
 	8-bit values. E.g. 127 out of 255 would be 50%, representing 1.65V
 */
#define DOSIMETER_REFERENCE_VOLTAGE	((float)3.30)


//==============================================================================
//                                FUNCTION STUBS
//==============================================================================

void requestReadingsAllChannels( void );
