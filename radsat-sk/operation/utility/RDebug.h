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

#define ANSIRED(text)	"\e[101;30m" text "\e[0m"
#define ANSIGRN(text)	"\e[102;30m" text "\e[0m"
#define ANSIYEL(text)	"\e[103;30m" text "\e[0m"

#define INFOTEXT 				"[  info  ]"
#define WARNINGTEXT		ANSIYEL("[WARNING!]")
#define ERRORTEXT		ANSIRED("[ ERROR! ]")



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

#ifdef DEBUG
// #define debugPrint(fmt, ...) 	printf("[  info  ] %s:%s: " fmt, __FILENAME__, __func__, ##__VA_ARGS__)
 #define debugPrint(fmt, ...) 	printf(fmt, ##__VA_ARGS__)
 #define infoPrint(fmt, ...) 	printf(INFOTEXT 	" %s:%d:%s: " fmt "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
 #define warningPrint(fmt, ...) printf(WARNINGTEXT 	" %s:%d:%s: " fmt "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
 #define errorPrint(fmt, ...) 	printf(ERRORTEXT	" %s:%d:%s: " fmt "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
 #define mark					printf("%s:%d:%s\n", __FILENAME__, __LINE__, __func__);
 #define radsatError(error) 	\
	if (error < 0){	\
		printf(ERRORTEXT " %s:%d:%s: ", __FILENAME__, __LINE__, __func__); \
		printResolvedErrorMessage(error);	\
		printf("\n");	\
		return error;	\
	}
 void printTime(void);
#else
 #define debugPrint(fmt, ...)
 #define infoPrint(fmt, ...)
 #define warningPrint(fmt, ...)
 #define errorPrint(error)
 #define mark
 #define radsatError(error) 	\
	if (error < 0) return error;
 #define printTime(void)
#endif

//void debugPrint(const char* stringFormat, ...);

extern unsigned char debugReadIntMinMax(unsigned int *pValue, unsigned int min, unsigned int max);

#endif /* RDEBUG_H_ */
