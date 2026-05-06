/*
 * AFR_Sensor.c
 *
 *  Created on: May 4, 2026
 *      Author: montemayorm
 */

#include "AFR_Sensor.h"

static READINGS readings;

/**
 * @brief initialize the afr sensor
 */
void AFR_init(){
    // enable GPIOA and ADC1 Clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // set pa0 and pa1 to analog mode
    GPIOA->MODER |= (GPIO_MODER_MODER0 | GPIO_MODER_MODER1);

    // configure cr1
    // SCAN: scan mode for multiple channels (narrow and wideband)
    // EOCIE: interrupt enable for EOC
    // OVRIE: interrupt enable for Overrun
    ADC1->CR1 = ADC_CR1_SCAN | ADC_CR1_EOCIE | ADC_CR1_OVRIE;

    // configure cr2
    // CONT: continuous conversion
    // EOCS: EOC bit enable
    // ADON: power on ADC
    ADC1->CR2 = ADC_CR2_CONT | ADC_CR2_EOCS | ADC_CR2_ADON;

    // set sample time to 84 cycles, which should be more than quick enough
    // SMPR2 handles channels 0-9
    ADC1->SMPR2 |= (0b100 << ADC_SMPR2_SMP0_Pos) | (0b100 << ADC_SMPR2_SMP1_Pos);

    // configure sequence length
    // SQR1: 0b0001 sets 2 conversions in a sequence
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= (0b0001 << ADC_SQR1_L_Pos);

    // SQR3: first conversion is channel 0 and second conversion is 1
    ADC1->SQR3 &= ~(ADC_SQR3_SQ1 | ADC_SQR3_SQ2);
    ADC1->SQR3 |= (0 << ADC_SQR3_SQ1_Pos) | (1 << ADC_SQR3_SQ2_Pos);

    // Enable interrupt in the NVIC
    NVIC_EnableIRQ(ADC_IRQn);

    //set channel to narrowband
    channel = narrowband;

    // 8. Start Conversions
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

/**
 * returns a struct containing the narrowband and wideband sensor readings
 */
READINGS sensor_read(){

}

/**
 * @brief ISR for EOC and overrun
 */
void ADC_IRQHandler(void) {
    // Check for overrun
    if (ADC1->SR & ADC_SR_OVR) {
        ADC1->SR &= ~ADC_SR_OVR; // Clear flag
        channel = narrowband;
        ADC1->CR2 |= ADC_CR2_SWSTART; // Restart
        return;
    }

    // Check for EOC
    if (ADC1->SR & ADC_SR_EOC) {
        if (channel == narrowband) {
            readings.narrowband = ADC1->DR;
            channel = 1;
        } else {
            readings.wideband = ADC1->DR;
            channel = 0;
        }
    }
}
