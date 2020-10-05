/* parser.h
 * Created: Saturday, Jan 18th 2020 by Tyrel Kostyk
 *
 * header file for functions relating to parsing of uplink telecommands and data
*/

// preprocessor defence against compiling this file multiple times
#ifndef PARSER_H
#define PARSER_H

#ifdef DONT_COMPILE_THE_FOLLOWING_CODE

typedef struct Telecommands Telecommand_t;


int parseQueue( xQueueHandle_t queue );


int parseTelecommand( telecommand_t telecommand );

#endif	// DONT_COMPILE_THE_FOLLOWING_CODE

#endif	// PARSER_H
