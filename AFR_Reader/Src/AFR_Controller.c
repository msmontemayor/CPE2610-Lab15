/*
 * AFR_Controller.c
 *
 *  Created on: May 5, 2026
 *      Author: montemayorm
 *
 *******************************************************************************
 * I wanted to give this its own header mostly so i dont forget how this is
 * supposed to work the next time i come back to this.
 *
 * How does a Lambda sensor work?
 *     In its simplest explanation, it works by injecting current through a pump
 *     cell until the nernst cell reads stoichiometric (typically 450mV). when
 *     the nernst cell is at the correct reading, you can measure the current
 *     being fed to the pump cell and determine your lambda and from there, your
 *     AFR.
 *
 *     this video was quite helpful
 *     https://www.youtube.com/watch?v=zjefDrFH_6c
 */

#include "stm32f411xe.h"
#include "AFR_Controller.h"
#include "timer.h"

static float Lambda = 1.0;
static uint16_t heater_counter = 0;

/**
 * @brief converts ADC values to 3.3v scale then to their 5v scale before
 * the 5v divider
 */
float ADC_To_Volt(uint32_t raw_adc) {
    float pin_voltage = ((float)raw_adc / ADC_RES) * VREF;
    return pin_voltage * DIVIDER_RATIO;
}

/**
 * @brief initializes the gpio for all pin functions
 * pa0 - pump current ADC, IP
 * pb0 - nernst cell adc
 * pb1 - virtual ground ref
 * pa8 - pump cell driver
 * pa9 - heater driver
 * pb12- nernst pulse
 */
void AFR_controller_init(void) {
    // enable clock for gpio A and B
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

    // set gpio to analog for adcs
    // pa0 - Pump Current
    GPIOA->MODER |= (3 << (0));
    // pb0 - Nernst Voltage
    // pb1 - Virtual Ground Reference
    GPIOB->MODER |= (3 << (0)) | (3 << (1 * 2));

    // set pwm for output pins
    //using tim1 channels 1 and 2
    // pa8 - Pump driver
    // pa9 - heater driver

    // set alternate function mode
    GPIOA->MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2))); // clear
    GPIOA->MODER |=  ((2 << (8 * 2)) | (2 << (9 * 2))); // set to af

    // set alternate function
    GPIOA->AFR[1] &= ~((0xF << 0) | (0xF << 4)); //clear
    GPIOA->AFR[1] |=  (1 << 0) | (1 << 4);   // af1 = Tim1

    // set output for determining resistance of the nernst cell
    // pb12 - nernst pulse
    GPIOB->MODER &= ~(3 << (12 * 2)); //clear
    GPIOB->MODER |=  (1 << (12 * 2)); // set to output
    GPIOB->OTYPER &= ~(1 << 12);      // push-pull to drive high and low


    //enable clock and power ADC1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    ADC1->CR2 = ADC_CR2_ADON;

    //TIM1->BDTR |= TIM_BDTR_MOE;
}

/*
 * @brief reads an adc channel
 * @param channel the channel to read
 * @returns the adc value from the channel
 */
uint32_t read_ADC(uint32_t channel) {
    ADC1->SQR3 = channel;
    ADC1->CR2 |= ADC_CR2_SWSTART;

    //wait for adc to finish
    while (!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}

/**
 * @brief measures resistance by sending a voltage pulse through the nernst cell
 * @returns the measured voltage difference caused by the pulse
 */
float get_nernst_resistance() {
    uint32_t v1 = read_ADC(8);     	// read from pb0
    GPIOB->BSRR = GPIO_BSRR_BS12;	// set pin high
    delay_ms(1);
    uint32_t v2 = read_ADC(8);		//read from pb0 again
    GPIOB->BSRR = GPIO_BSRR_BR12;   // set pin low

    return ADC_To_Volt(v2) - ADC_To_Volt(v1);
}

/**
 * @brief calculates lambda from the measured current of the pump cell
 * @param current_ma the measured current of the pump cell
 * @returns lambda
 */
float interpolate_lambda(float current_ma) {
    // too rich
    if (current_ma <= ip_table[0]){
    	return lambda_table[0];
    }
    // too lean
    if (current_ma >= ip_table[TABLE_SIZE - 1]){
    	return lambda_table[TABLE_SIZE - 1];
    }

    // find which points lambda is between
    for (int i = 0; i < TABLE_SIZE - 1; i++) {
        if (current_ma >= ip_table[i] && current_ma <= ip_table[i+1]) {
            //y = y0 + (x - x0) * ((y1 - y0) / (x1 - x0))
            float x0 = ip_table[i];
            float x1 = ip_table[i+1];
            float y0 = lambda_table[i];
            float y1 = lambda_table[i+1];

            return y0 + (current_ma - x0) * ((y1 - y0) / (x1 - x0));
        }
    }
    return 1.0; //return 1 (stoich) if theres an issue
}

/**
 * @brief simple pump control loop
 */
void AFR_pump_control() {
	//read virtual gnd from pb1
    float vgnd = ADC_To_Volt(read_ADC(9));

    //offset nernst reading by virtual gnd reading
    float nernst = ADC_To_Volt(read_ADC(8)) - vgnd;

    if (nernst < (TARGET_NERNST - DEADBAND)){
    	//set duty cycle to above 50% to push current through pump
    	TIM1->CCR1 = 650;
    }
    else if (nernst > (TARGET_NERNST + DEADBAND)){
    	//set duty cycle to below 50% to pull current through pump
    	TIM1->CCR1 = 350;
    }

    // read the voltage coming from pump cell op-amp
    float ip_volts = ADC_To_Volt(read_ADC(0)) - vgnd;

    //convert read volts to ma using sensor resistor and opamp gain
    float ip_ma = ip_volts / (61.9 * 11.0) * 1000.0;

    //linearly interpolate lambda from ma
    Lambda = interpolate_lambda(ip_ma);
}

/**
 * @brief gets the current lambda value
 */
float get_lambda(){
	return 1.0;
}

/**
 * @brief simple heater control
 */
void AFR_control_heater() {
    float res_v = get_nernst_resistance();

    if (res_v > (TARGET_IP_RES + DEADBAND)){
    	TIM1->CCR2 = 800; // heat on
    }

    else if (res_v < (TARGET_IP_RES - DEADBAND)){
    	TIM1->CCR2 = 0;   // heat off
    }
}

/**
 * @brief 10ms tim3 ISR
 */
void TIM3_IRQHandler() {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;

        //every 10ms run the pump control scheme
        AFR_pump_control();

        // every 200ms run heater control scheme as well
        if (++heater_counter >= 20) {
            AFR_control_heater();
            heater_counter = 0;
        }
    }
}
