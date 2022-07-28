/**
 * @file RKey.c
 * @date February 9, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RKey.h>
#include <RFram.h>
#include <RDebug.h>
#include <RErrorManager.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** The hardcoded address of the FIRST copy of the private 1-byte key in FRAM memory. */
#define PRIVATE_KEY_ADDR_ONE	((uint32_t)0x1001AA00)	// TODO: set to real address location

/** The hardcoded address of the SECOND copy of the private 1-byte key in FRAM memory. */
#define PRIVATE_KEY_ADDR_TWO	((uint32_t)0x1001AA04)	// TODO: set to real address location

/** The hardcoded address of the THIRD copy of the private 1-byte key in FRAM memory. */
#define PRIVATE_KEY_ADDR_THREE	((uint32_t)0x1001AA08)	// TODO: set to real address location


/** The 1-byte private key used for the decryption operations. */
static uint8_t key = 0;


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void rewriteKey(uint8_t goodKey, uint32_t badKeyAddress);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Return the Private Key used for decryption, retrieving it from FRAM if necessary.
 *
 * Will use a "best 2 out of 3" voting approach to add redundancy in the case of bit flip
 * errors or memory corruption. If all 3 are different, the first one is return. Retrieval
 * and voting only done on first access per start-up; stored locally afterwards.
 *
 * @return The private key.
 */
uint8_t privateKey(void) {

	// obtain the key from memory when first obtained
	if (key == 0) {
		uint8_t key1, key2, key3 = 0;
		int error = 0;

		// read from FRAM
		error = framRead(&key1, PRIVATE_KEY_ADDR_ONE,   sizeof(key1))
		
		if (error != SUCCESS)
			errorReportModule(moduleFram , error);
			return error;

		error = framRead(&key2, PRIVATE_KEY_ADDR_TWO,   sizeof(key2));

		if (error != SUCCESS)
			errorReportModule(moduleFram , error);
			return error;
		error = framRead(&key3, PRIVATE_KEY_ADDR_THREE, sizeof(key3));

		if (error != SUCCESS)
			errorReportModule(moduleFram , error);
			return error;
		// all of the keys are the same
		if ((key1 == key2) && (key1 == key3) && (key2 == key3)) {
			key = key1;
		}

		// key1 is different
		else if ((key1 != key2) && (key1 != key3) && (key2 == key3)) {

			debugPrint("RKey: key1 is different (expected %d, got %d). Overwriting...\n", key2, key1);

			// rewrite key1 with a valid one
			rewriteKey(key2, PRIVATE_KEY_ADDR_ONE);

			// send key2
			key = key2;
		}

		// key2 is different
		else if ((key1 != key2) && (key1 == key3) && (key2 != key3)) {

			debugPrint("RKey: key2 is different (expected %d, got %d). Overwriting...\n", key1, key2);

			// rewrite key2 with a valid one
			rewriteKey(key1, PRIVATE_KEY_ADDR_TWO);

			// send key1
			key = key1;
		}

		// key3 is different
		else if ((key1 == key2) && (key1 != key3) && (key2 != key3)) {

			debugPrint("RKey: key3 is different (expected %d, got %d). Overwriting...\n", key1, key3);

			// rewrite key3 with a valid one
			rewriteKey(key1, PRIVATE_KEY_ADDR_THREE);

			// send key1
			key = key1;
		}

		// all three are different... yikes
		else {

			debugPrint("RKey: all three keys are different (Key1 = %d, Key2 = %d, Key3 = %d). Sending Key1...\n", key1, key2, key3);


			key = key1;
		}
	}

	return key;
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Rewrite a corrupted Private Key in FRAM with a valid key.
 *
 * @param goodKey A valid private key.
 * @param badKeyAddress The address of the key that is determined to be invalid.
 */
static void rewriteKey(uint8_t goodKey, uint32_t badKeyAddress) {

	// write good key into location of corrupted key
	framWrite(&goodKey, badKeyAddress, sizeof(goodKey));

}
