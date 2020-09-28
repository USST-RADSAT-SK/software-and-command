/*!
 * @file	ADC.h
 * @brief	Driver for reading Analog data (including OBC board temperature) using the ADC.
 * @date	Feb 26, 2013
 * @author	Akhil Piplani
 */

#ifndef ADC_H_
#define ADC_H_

#include <freertos/FreeRTOS.h>
#include <at91/commons.h>
#include "hal/boolean.h"

/*!
 * The number of buffer elements in the circular buffer created by the ADC driver.
 * This buffer is used to store the ADC samples. Using a circular buffer here allows
 * the ADC driver to be samples into one element of the buffer while the application
 * reads from another element.
 */
#define ADC_CIRCULAR_BUFFER_ELEMENTS	4

/*!
 * Reference voltage used for the ADC. This can be used to convert the raw ADC output
 * to milliVolts. The following formulae apply: \n
 * @verbatim
 * For 10-bit resolution:\n
 * milliVolts = ((ADC_REFERENCE_VOLTAGE * rawValue)/0x3FF)\n
 * For 8-bit resolution:\n
 * milliVolts = ((ADC_REFERENCE_VOLTAGE * rawValue)/0xFF)\n
 * @endverbatim
 */
#define ADC_REFERENCE_VOLTAGE			2500

typedef struct _ADCparameters {
	unsigned int sampleRate; //!< Sample rate in Hz. Limits: Minimum 4030Hz, Maximum 75000Hz(10-bit resolution) or 250000Hz(8-bit resolution)
	Boolean resolution10bit; //! Set to TRUE for 10-bit ADC resolution and FALSE for 8-bit ADC resolution.

	/*!
	 * Number of channels to be sampled. Valid values are 1, 2, 3, 4, 8. Values of 5, 6 or 7 are not allowed.
	 * This is because ADC channels are doubled from 4 to 8 using an external multiplexer.
	 * Therefore when >4 channels are sampled, all channels are enabled at the ADC.
	 */
	unsigned int channels;

	/*!
	 * Number of samples to be stored in a buffer for this driver for each channel.
	 * As DMA is used for saving the ADC samples, a larger value here would lead to fewer
	 * interrupts upon completion of a DMA transfer.\n
	 *
	 * It is recommended to use a value of 2048 here.
	 * If 10-bit resolution is used, samplesInBufferPerChannel * channels should be less than 32768
	 * If  8-bit resolution is used, samplesInBufferPerChannel * channels should be less than 65536
	 */
	unsigned int samplesInBufferPerChannel;

	/*!
	 * Function that will be called by the Interrupt Service Routine of the ADC driver.
	 * A call to this function indicated reception of new ADC samples and a pointer to
	 * these is passed as a parameter.
	 * This function should be fast as it will be called from an ISR.
	 *
	 * @param adcSamples Pointer to the latest ADC samples buffer. \n
	 * Size of this buffer in bytes is: \n
	 * 10-bit resolution: channels * samplesInBufferPerChannel * 2 \n
	 * 8-bit resolution:  channels * samplesInBufferPerChannel \n
	 *
	 * The user should cast this pointer to (unsigned char*) if 8-bit resolution is used
	 * and to (unsigned short*) if 10-bit resolution is used.
	 *
	 * Let SnCm mean sample-number-n for channel-number-m.
	 * Then the samples in the buffer are arranged in the following manner: \n
	 * S0C0, S0C1, S0C2, ... , S0CM, S1C0, S1C1....... SNCM.
	 */
	void (*callback)(SystemContext context, void* adcSamples);
} ADCparameters;

/*!
 * Converts a raw 10-bit ADC sample to millivolts.
 * @param adcRawSample Raw 10-bit ADC sample.
 * @return The converted ADC sample in millivolts.
 */
unsigned int ADC_ConvertRaw10bitToMillivolt( unsigned short adcRawSample );

/*!
 * Converts a raw 8-bit ADC sample to millivolts.
 * @param adcRawSample Raw 8-bit ADC sample.
 * @return The converted ADC sample in millivolts.
 */
unsigned int ADC_ConvertRaw8bitToMillivolt( unsigned char adcRawSample );

/*!
 * Initializes the ADC driver and begins a single shot acquisition of 10-bit ADC samples for all 8 channels.
 * @param adcSamples Pointer to an array which will contain the sampled ADC data.
 * @return -2 when input buffer is NULL, -1 when ADC is busy with continuous mode, 0 on success.
 */
int ADC_SingleShot( unsigned short adcSamples[8] );

/*!
 * Initializes the ADC driver and begins periodically storing the ADC samples in a buffer.
 * A callback is called each time a set of ADC samples is buffered.
 * @see ADCparameters
 * @param params ADCparameters structure containing all the parameters required to properly initialize the driver.
 * @return -2 when input parameters are invalid, -1 when memory allocation fails, 0 on success.
 */
int ADC_start(ADCparameters params);

/*!
 * De-initializes the ADC driver and stops the collection of ADC samples.
 */
void ADC_stop(void);

#endif /* ADC_H_ */
