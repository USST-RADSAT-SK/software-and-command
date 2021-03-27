/**
 * @file RParser.c
 * @date January 18 2020
 * @author Tyrel Kostyk
*/

#ifdef DONT_COMPILE_THE_FOLLOWING_CODE

#include <stdio.h>
#include "RParser.h"


// TODO: serialization library? wrapper for a serialization library?
// TODO: API to convert to/from Telecommands struct with some serialization object?


typedef struct Telecommands {
   uint16_t id;             // the actual command (i.e. 0x01FF means "self-destruct")
   uint32_t timestamp;      // timestamp; can be of when tc was sent, when comms line was confirmed, etc.
   uint8_t  intended_tcs;   // how many individual telecommands are intended to be sent this pass (including data?)
   uint8_t  num_in_trans;   // how many individual telecommands have been sent this pass (including this one)
   uint8_t  with_data;      // boolean; is this tc supposed to be accompanied by data (i.e. following this tc?)
   uint32_t data_size_B;    // size of proceeding data (in raw data Bytes)
   uint32_t data_size_F;    // size (in frames of size 200 (235?) Bytes)
   uint8_t  parity;         // boolean; true(1) if tc_id is even. Possible way of implementing simple robustnessa
} Telecommand_t;


xQueueHandle_t queueTelecommands;    // queue (buffer) for holding telecommands to parse


/* @brief: parse top item off of a FreeRTOS FIFO Queue
 *
 *
 */
int parseQueue( xQueueHandle_t queue )
{
  Telecommand_t tc;     // init tc, a telecommand struct object
  serial_tc_t raw_tc;   // init raw_tc, a raw "object" placeholder that needs to be translated into the proper form
  raw_tc = queue.pop()  // get oldest item from queue (FIFO)

  tc = deserialize(raw_tc); // conv raw tc "object" into proper form

  int result = parseTelecommand( tc );

  return result;
}


/* @brief: parse a single telecommand struct
 *
 *
 */
int parseTelecommand( telecommand_t tc )
{
  // check parity bit:
    // goes into loop if ID is even (%2 = 0) and parity is odd (0)
    // or if ID is odd (%2 = 1) and parity is even (1)
  if ( ( tc.id % 2 ) == tc.parity )
  {
    assertError("ERROR - PARITY BIT DOES NOT MATCH PARITY OF TELECOMMAND");
    // TODO: some logging here
  }

  switch ( tc.id )
  {
    case 0x0001:
      // do something...
      return 0;  // success

    case 0x0002:
      // do something...
      return 0;  // success

    default:
      assertError("ERROR - PARITY BIT DOES NOT MATCH PARITY OF TELECOMMAND");
      // TODO: some logging here
      return 1;  // failure
  }
}


int main( int argc, int argv[] ) {

}

#endif	// DONT_COMPILE_THE_FOLLOWING_CODE
