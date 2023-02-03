/**
 * @file RDosimeter.h
 * @date December 28, 2021
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#ifndef RDOSIMETER_H_
#define RDOSIMETER_H_

#include <stdint.h>
#include <RFileTransfer.pb.h>



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

int dosimeterData(dosimeter_data* data);
int dosimeterCollectData(void);
int16_t dosimeterTemperature(dosimeterBoard_t board);
void printDosimeterData(dosimeter_data* data);


#endif /* RDOSIMETER_H_ */
