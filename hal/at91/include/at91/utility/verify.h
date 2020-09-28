/*
 * verify.h
 *
 *  Created on: 26 aug. 2014
 *      Author: pbot
 */

#ifndef VERIFY_H_
#define VERIFY_H_

#include <stdio.h>
#include "trace.h"

#if defined(NOASSERT)
    #define ASSERT(...)
    #define SANITY_CHECK(...)
#else
    #if (TRACE_LEVEL == 0)
        #define VERIFY( condition, error, label, formatLiteral, ... )                 \
            do {                                                                       \
                if(!(condition))                                                       \
                {                                                                      \
                    retVal = error;                                                    \
                    goto label;                                                        \
                }                                                                      \
            } while( 0 )
    #else
        #define VERIFY( condition, error, label, formatLiteral, ... )                  \
            do {                                                                       \
                if(!(condition))                                                       \
                {                                                                      \
                    TRACE_ERROR("Error [%d] - " formatLiteral, error, ##__VA_ARGS__);  \
                    retVal = error;                                                    \
                    goto label;                                                        \
                }                                                                      \
            } while( 0 )
    #endif

    #if (TRACE_LEVEL == 0)
        #define VERIFY_RET( condition, error, formatLiteral, ... )                     \
            do {                                                                       \
                if(!(condition))                                                       \
                {                                                                      \
                    return error;                                                      \
                }                                                                      \
            } while( 0 )
    #else
        #define VERIFY_RET( condition, error, formatLiteral, ... )                    \
            do {                                                                      \
                if(!(condition))                                                      \
                {                                                                     \
                    TRACE_ERROR("Error [%d] - " formatLiteral, error, ##__VA_ARGS__); \
                    return error;                                                     \
                }                                                                     \
            } while( 0 )
    #endif

	#if (TRACE_LEVEL == 0)
		#define VERIFY_STP( condition, error, formatLiteral, ... )                     \
			do {                                                                       \
				if(!(condition))                                                       \
				{                                                                      \
					while(1);                                                      \
				}                                                                      \
			} while( 0 )
	#else
		#define VERIFY_STP( condition, error, formatLiteral, ... )                    \
			do {                                                                      \
				if(!(condition))                                                      \
				{                                                                     \
					TRACE_ERROR("Error [%d] - " formatLiteral, error, ##__VA_ARGS__); \
					while(1);                                                   \
				}                                                                     \
			} while( 0 )
	#endif
#endif

#endif /* VERIFY_H_ */

