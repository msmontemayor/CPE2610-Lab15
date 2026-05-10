/*
 * AFR_test.c
 *
 *  Created on: May 9, 2026
 *      Author: montemayorm
 */

#include <stdio.h>
#include "AFR_test.h"
#include "AFR_Controller.h"
#include "stm32f411xe.h"
#include "timer.h"

/**
 * @brief test script to test my hardware before final implementation
 */
void AFR_test_script() {

	//disable the tim3 interrupt
    TIM3->CR1 &= ~TIM_CR1_CEN;
    NVIC_DisableIRQ(TIM3_IRQn);

    printf("Controller hardware validation \n");

    getchar();

    // test 1 - virtual gnd
    float vgnd = ADC_To_Volt(read_ADC(9));


    printf("TEST 1: virtual ground\n");
    printf("Measured VGND: %02fV\n", vgnd);

    //check if vgnd is roughly 2.5V
    if (vgnd > 2.3f && vgnd < 2.7f) {
        printf("PASS: VGND near 2.5V\n");
    } else {
        printf("FAIL: VGND not at 2.5V\n");
    }

    //wait for user input before next
    getchar();

    // test 2 -  nernst baseline
    float vs = ADC_To_Volt(read_ADC(8)); // PB0
    printf("TEST 2: Nernst Baseline\n");
    printf("Measured Vs: %.2fV\n", vs);
    printf("Measure Vs pin (PB0) with a multimeter. Does it match measured?\n");

    //wait for enter
    getchar();

    // TEST 3 - Pump Drive
    printf("TEST 3 - Pump driver\n");
    printf("Testing pump push...\n");
    TIM1->CCR1 = 1000; // 100% PWM
    delay_ms(500);
    printf("Measure the output of the Pump Op-Amp.\n");
    printf("Target around 5V, enter when done\n");
    //wait for enter
    getchar();

    printf("Testing pump push...\n");
    TIM1->CCR1 = 0; // 0% PWM
    delay_ms(500);
    printf("Target 0V, enter when done\n");

    //wait for enter
    getchar();

    // pwm back to center
    TIM1->CCR1 = 515;

    // TEST 4 - pulse check
    printf("TEST 4: nernst pulse check\n");
    printf("toggling pb12 every 2 seconds\n");
    printf("check pb12 with a multimeter, should jump 0 to 3.3\n");
    for(int i=0; i<3; i++) {
        GPIOB->BSRR = GPIO_BSRR_BS12;
        printf("high...\n");
        delay_ms(2000);
        GPIOB->BSRR = GPIO_BSRR_BR12;
        printf("low...\n");
        delay_ms(2000);
    }

    printf("validation complete\n");

    //re enable tim3 interrupt
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= TIM_CR1_CEN;
}
