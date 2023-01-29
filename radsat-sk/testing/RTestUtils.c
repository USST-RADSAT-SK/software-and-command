/**
 * @file RTestUtils.c
 * @date January 29, 2023
 * @author Austin Hruska (jah385)
 */




#include <RTestUtils.h>
#include <RCommon.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int testingMenu(unsigned int autoSelection, TestMenuFunction* tests, char** menuTitles, unsigned int num) {
	unsigned int selection = 0;

	while (1) {
		if (!selection) {
			printf( "\n Select a test to perform: \n");
            for (int i = 0; i < num; i++)
                printf("\t%d) %s\n", i+1, tests[i]);
			printf("\t0) EXIT\n");

			while(debugReadIntMinMax(&selection, 0, num) == 0)
				vTaskDelay(MENU_DELAY);
		}

        if (selection == 1){
            tests[selection](RUN_ALL);
        } else if (selection == 0) {
            break;
		} else {
            tests[selection](RUN_SELECTION);
		}

	}
}