/*
 * PWMtest.c
 *
 *  Created on: 27-Feb-2013
 *      Author: Akhil Piplani
 */

#include <at91/utility/trace.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <hal/boolean.h>
#include <hal/Drivers/ADC.h>
#include <hal/Drivers/PWM.h>

void taskPWMtest() {
	unsigned int pwmVals[6] = {0};
	int retVal = 0;

	TRACE_DEBUG("taskPWMtest: Starting \n\r");

	PWM_start(TC_UPCOUNT_MAXVAL, 0); // This should make the PWM frequency = 1.007kHz.

	while(1) {
		vTaskDelay(100);

		//TRACE_DEBUG("taskPWMtest: Setting Duty cycle of: %d, %d, %d, %d, %d, %d percent.\n\r", pwmVals[0], pwmVals[1], pwmVals[2], pwmVals[3], pwmVals[4], pwmVals[5]);
		retVal = PWM_setDutyCycles(pwmVals);
		if(retVal != 0) {
			TRACE_WARNING("taskPWMtest: PWM_setDutyCycles returned %d \n\r", retVal);
			while(1);
		}

		pwmVals[0] = (pwmVals[0] + 5) % 100; // Increase by 5%

		// Set all the other channels to some increments over the duty cycle of the first channel
		// The output values will roll over if the increment causes them to exceed 100 (e.g. (95+10)%100 = 5)
		pwmVals[1] = (pwmVals[0] + 10) % 100;
		pwmVals[2] = (pwmVals[0] + 20) % 100;
		pwmVals[3] = (pwmVals[0] + 30) % 100;
		pwmVals[4] = (pwmVals[0] + 40) % 100;
		pwmVals[5] = (pwmVals[0] + 50) % 100;
	}
}

Boolean PWMtest() {
	xTaskHandle taskPWMtestHandle;

	xTaskGenericCreate(taskPWMtest, (const signed char*)"taskPWMtest", 1024, NULL, 2, &taskPWMtestHandle, NULL, NULL);

	return FALSE;
}
