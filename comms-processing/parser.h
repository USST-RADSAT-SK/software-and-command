/* parser.h
 * Created: Saturday, Jan 18th 2020 by Tyrel Kostyk
 *
 * header file for functions relating to parsing of uplink telecommands and data
*/

// preprocessor defence against compiling this file multiple times
#ifndef PARSER_H
#define PARSER_H


typedef struct Telecommands Telecommand_t;


int parseQueue( xQueueHandle_t queue );


int parseTelecommand( telecommand_t telecommand );



#endif // PARSER_H
