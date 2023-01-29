/**
 * @file RTestUtils.h
 * @date January 29, 2023
 * @author Austin Hruska (jah385)
 */

#ifndef RTESTUTILS_H_
#define RTESTUTILS_H_


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define RUN_SELECTION	0
#define RUN_ALL			1

#define MENU_DELAY		80

typedef int (*TestMenuFunction)(unsigned int autoSelection);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int testingMenu(unsigned int autoSelection, TestMenuFunction* tests, char** menuTitles, unsigned int num);

extern unsigned char debugReadIntMinMax(unsigned int *pValue, unsigned int min, unsigned int max);

#endif /* RTESTUTILS_H_ */