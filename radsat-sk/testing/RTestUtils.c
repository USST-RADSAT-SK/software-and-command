/**
 * @file RTestUtils.c
 * @date January 29, 2023
 * @author Austin Hruska (jah385)
 */




#include <RTestUtils.h>
#include <RCommon.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int testingMenu(unsigned int autoSelection, TestMenuFunction* tests, char** menuTitles, unsigned int num) {
	unsigned int selection = autoSelection;

	while (1) {
		if (!selection) {
			printf( "\n Select a test to perform: \n");
            for (unsigned int i = 0; i < num; i++)
                printf("\t%d) %s\n", i+1, menuTitles[i]);
			printf("\t0) <- Return\n");

			while(debugReadIntMinMax(&selection, 0, num) == 0)
				vTaskDelay(MENU_DELAY);
		}

        if (selection == 1){
            tests[selection - 1](RUN_ALL);
        } else if (selection == 0) {
            break;
		} else {
            tests[selection - 1](autoSelection);
		}
        selection = 0;

	}
    return 0;
}
