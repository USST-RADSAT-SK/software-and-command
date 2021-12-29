/**
 * @file RDosimeter.h
 * @date December 28, 2021
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#ifndef RDOSIMETER_H_
#define RDOSIMETER_H_

#include <stdint.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** The two Melanin-Dosimeter boards. */
typedef enum _dosimeterBoard {
	dosimeterBoardOne,
	dosimeterBoardTwo,
	dosimeterBoardCount,
} dosimeterBoard_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int dosimeterCollectData(void);
uint16_t dosimeterTemperature(dosimeterBoard_t board);


#endif /* RDOSIMETER_H_ */
