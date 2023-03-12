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

#define CSI	"\e["



#define CLEAREND	CSI "0K"
#define CLEARHOME	CSI "1K"
#define CLEARLINE	CSI "2K"

/* Text Formatting */
#define ULINE	";1"
#define FAINT	";2"
#define ITALIC	";3"
#define BOLD	";4"
#define BLINKS	";5"
#define BLINKF	";6"
#define STRIKE	";9"

#define BLK "0"
#define RED "1"
#define GRN "2"
#define YEL "3"
#define BLU "4"
#define MAG "5"
#define CYN "6"
#define WHT	"7"

//fflush(stdout);
#define TXTCLR(colour)	";3" colour
#define BTXTCLR(colour)	";9" colour
#define RSTTXT			";39"
#define BGDCLR(colour)	";4" colour
#define BBGDCLR(colour)	";10" colour
#define RSTBGDCLR		";49"

#define RSTALL	";0"

#define ANSISTYLE(settings, text)	CSI settings "m" text CSI RSTALL "m"

#define INFOTEXT 		"[  info  ]"
#define WARNINGTEXT		ANSISTYLE( TXTCLR(BLK) BBGDCLR(YEL), "[WARNING!]")
#define ERRORTEXT		ANSISTYLE( TXTCLR(BLK) BBGDCLR(RED), "[ ERROR! ]")

#define INFOFMT 		" %15s(): "
#define WARNINGFMT 		" %s:%d, %s(): "
#define ERRORFMT 		" %s:%s() at line %d... \n -> "



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

#ifdef DEBUG
	#ifndef TEST
		#define INFOBAR
	#endif
	#define debugPrint(fmt, ...) 	printf(fmt, ##__VA_ARGS__)
	#define infoPrint(fmt, ...) 	printf("\r" INFOTEXT	INFOFMT		fmt "\n", __func__, ##__VA_ARGS__)
	#define warningPrint(fmt, ...)	printf("\r" WARNINGTEXT	WARNINGFMT 	fmt "\n", __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
	#define errorPrint(fmt, ...) 	printf("\r" ERRORTEXT	ERRORFMT 	fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
	#define mark					printf("%d, %s\n", __LINE__, __func__);
	void printTime(void);

	#ifdef INFOBAR
		char* throbber(void);
	#endif
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
void debugPrintHex(uint8_t* values, size_t size);

#endif /* RDEBUG_H_ */
