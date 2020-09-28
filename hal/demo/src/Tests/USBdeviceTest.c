/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#include <at91/boards/ISIS_OBC_G20/board.h>
#include <at91/peripherals/pio/pio.h>
#include <at91/peripherals/pio/pio_it.h>
#include <at91/peripherals/pmc/pmc.h>
#include <at91/peripherals/aic/aic.h>
#include <at91/utility/trace.h>
#include <at91/usb/device/core/USBD.h>
#include <at91/usb/common/core/USBConfigurationDescriptor.h>
#include <at91/usb/device/hid-mouse/HIDDMouseDriver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/boolean.h>
#include <string.h>

/// Speed of pointer movement X
#define MOUSE_SPEED_X	4
/// Speed of pointer movement Y
#define MOUSE_SPEED_Y	4

/// Use for power management
#define STATE_IDLE    0
/// The USB device is in suspend state
#define STATE_SUSPEND 4
/// The USB device is in resume state
#define STATE_RESUME  5

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------
/// State of USB, for suspend and resume
unsigned char USBState = STATE_IDLE;

//------------------------------------------------------------------------------
//         VBus monitoring (optional)
//------------------------------------------------------------------------------
#if defined(PIN_USB_VBUS)

#define VBUS_CONFIGURE()  VBus_Configure()

/// VBus pin instance.
static const Pin pinVbus = PIN_USB_VBUS;

//------------------------------------------------------------------------------
/// Handles interrupts coming from PIO controllers.
//------------------------------------------------------------------------------
static void ISR_Vbus(const Pin *pPin)
{
	(void)pPin;

    TRACE_INFO("VBUS ");

    // Check current level on VBus
    if (PIO_Get(&pinVbus)) {

        TRACE_INFO("conn\n\r");
        USBD_Connect();
    }
    else {

        TRACE_INFO("discon\n\r");
        USBD_Disconnect();
    }
}

//------------------------------------------------------------------------------
/// Configures the VBus pin to trigger an interrupt when the level on that pin
/// changes.
//------------------------------------------------------------------------------
static void VBus_Configure( void )
{
    TRACE_INFO("VBus configuration\n\r");

    // Configure PIO
    PIO_Configure(&pinVbus, 1);
    PIO_ConfigureIt(&pinVbus, ISR_Vbus);
    PIO_EnableIt(&pinVbus);

    // Check current level on VBus
    if (PIO_Get(&pinVbus)) {

        // if VBUS present, force the connect
        TRACE_INFO("conn\n\r");
        USBD_Connect();
    }
    else {
        USBD_Disconnect();
    }
}

#else
    #define VBUS_CONFIGURE()    USBD_Connect()
#endif //#if defined(PIN_USB_VBUS)

//------------------------------------------------------------------------------
/// Put the CPU in 32kHz, disable PLL, main oscillator
/// Put voltage regulator in standby mode
//------------------------------------------------------------------------------
void LowPowerMode(void)
{
    PMC_CPUInIdleMode();
}
//------------------------------------------------------------------------------

void USBDCallbacks_Resumed(void)
{
    // Initialize LEDs
//    LED_Configure(USBD_LEDPOWER);
//    LED_Set(USBD_LEDPOWER);
//    LED_Configure(USBD_LEDUSB);
//    LED_Clear(USBD_LEDUSB);
    USBState = STATE_RESUME;
}

void USBDCallbacks_Suspended(void)
{
    // Turn off LEDs
//    LED_Clear(USBD_LEDPOWER);
//    LED_Clear(USBD_LEDUSB);
    USBState = STATE_SUSPEND;
}

Boolean USBdeviceTest() {
	unsigned char status = 0, bmButtons = 0;
    signed char dX = MOUSE_SPEED_X, dY = MOUSE_SPEED_Y;
    signed char directionMultiplierX = 1, directionMultiplierY = -1, usbState = 0, lastUsbState = 0;
    unsigned int i = 0;

    printf("\n\r USB device test started \n\r");
    printf(" Please connect the USB Device port of the OBC to a Computer. \n\r");
    printf(" The OBC will show up as a USB mouse and move your cursor in a diamond pattern. \n\r");

    // If they are present, configure Vbus & Wake-up pins
    PIO_InitializeInterrupts(0);

    // HID driver initialization
    HIDDMouseDriver_Initialize();

    // connect if needed
    VBUS_CONFIGURE();
    while (usbState < USBD_STATE_CONFIGURED) {
    	usbState = USBD_GetState();
    	if(usbState != lastUsbState) {
//    		printf("\n\r %d \n\r", usbState);
    		lastUsbState = usbState;
    	}
    }

    for(i=0; i<2000; i++) {
    	do {
            status = HIDDMouseDriver_ChangePoints(bmButtons, dX, dY);
        } while (status != USBD_STATUS_SUCCESS);

        dX = MOUSE_SPEED_X * directionMultiplierX;
        dY = MOUSE_SPEED_X * directionMultiplierY;

        if(i%100 == 0) {
        	directionMultiplierX *= -1;
        }
        else if(i%50 == 0) {
        	directionMultiplierY *= -1;
        }

        if( USBState == STATE_SUSPEND ) {
            TRACE_DEBUG("suspend  !\n\r");
            LowPowerMode();
            USBState = STATE_IDLE;
        }
        if( USBState == STATE_RESUME ) {
            // Return in normal MODE
            TRACE_DEBUG("resume !\n\r");
            USBState = STATE_IDLE;
        }

        vTaskDelay(10);
    }

    return TRUE;
}
