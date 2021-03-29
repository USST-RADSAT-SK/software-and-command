/**
 * @file RParser.h
 * @date January 18 2020
 * @author Tyrel Kostyk
*/

// preprocessor defence against compiling this file multiple times
#ifndef RPARSER_H
#define RPARSER_H

#ifdef DONT_COMPILE_THE_FOLLOWING_CODE

typedef struct Telecommands Telecommand_t;


int parseQueue( xQueueHandle_t queue );


int parseTelecommand( telecommand_t telecommand );

#endif	// DONT_COMPILE_THE_FOLLOWING_CODE

#endif	// RPARSER_H
