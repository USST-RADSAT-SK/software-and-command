/**
 * @file RDebug.h
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#ifndef RDEBUG_H_
#define RDEBUG_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define ANSIRED(text) "\e[101;30m" text "\e[0m"
#define ANSIGRN(text) "\e[102;30m" text "\e[0m"
#define ANSIYEL(text) "\e[103;30m" text "\e[0m"



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

#ifdef DEBUG
// #define debugPrint(fmt, ...) 	printf("[  info  ] %s:%s: " fmt, __FILENAME__, __func__, ##__VA_ARGS__)
 #define debugPrint(fmt, ...) 	printf(fmt, ##__VA_ARGS__)
 #define infoPrint(fmt, ...) 	printf("[  info  ] %s:%d:%s: " fmt "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
 #define errorPrint(fmt, ...) 	printf(ANSIRED("[ ERROR! ]") " %s:%d:%s: " fmt "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
 #define warningPrint(fmt, ...) printf(ANSIGRN("[WARNING!]") " %s:%d:%s: " fmt "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
// #define errorCheck(error) 		if (error) errorPrint("%d", error)
// #define warningCheck(warning) 	if (warning) warningCheck("%d", warning)
 #define mark					printf("%s:%d:%s\n", __FILENAME__, __LINE__, __func__);
#else
 #define debugPrint(fmt, ...)
 #define infoPrint(fmt, ...)
 #define errorPrint(error)
 #define warningPrint(fmt, ...)
 #define errorCheck(error)
 #define warningCheck(warning)
 #define mark
#endif

//void debugPrint(const char* stringFormat, ...);

extern unsigned char debugReadIntMinMax(unsigned int *pValue, unsigned int min, unsigned int max);

#endif /* RDEBUG_H_ */
