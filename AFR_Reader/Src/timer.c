/*
 * timer.c
 *
 *  Created on: Feb 10, 2026
 *      Author: montemayorm
 *      Purpose: Timer API
 */

#include "timer.h"
#include "stm32f411xe.h"

/**
 * @brief initializes Tim10 to PWM mode
 */
void initTim10(){
	// 1. Enable Clock for TIM10 via RCC
    RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;

    // 2. Configure PWM Mode 1 (OC1M = 110)
    // Reset bits then set PWM mode 1 and enable preload
    TIM10->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_CC1S);
    TIM10->CCMR1 |= (0x6U << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;

    // 3. Enable Auto-reload preload
    TIM10->CR1 |= TIM_CR1_ARPE;

    // 4. Enable Output Channel 1
    TIM10->CCER |= TIM_CCER_CC1E;

    // 5. Generate Update to initialize registers
    TIM10->EGR |= TIM_EGR_UG;
}

/**
 * @brief initializes Timer 4 to be an interrupt event
 */
void initTim4Int(){
	// 1. Enable Clock for TIM4
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // 2. Set Prescaler to achieve 1ms resolution (assuming 16MHz clock)
    TIM4->PSC = (uint32_t)(F_CPU_TIM / 1000) - 1;
    TIM4->ARR = 1; // Trigger every 1 count (effectively 1ms)

    // 3. Enable Update Interrupt
    TIM4->DIER |= TIM_DIER_UIE;

    // 4. Enable TIM4 IRQ in NVIC
    NVIC_EnableIRQ(TIM4_IRQn);

    // 5. Start Timer
    TIM4->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief initialize the systick to a 1ms interrupt
 */
void systick_init(){

	NVIC_SetPriority(SysTick_IRQn, 1);
	// 1. Disable SysTick during configuration
	SysTick->CTRL = 0;

    // 2. Set the reload value for 1ms intervals
    // (F_CPU / 1000) - 1
    SysTick->LOAD = (uint32_t)(F_CPU_TIM / 1000) - 1;

    // 3. Clear current value register
    SysTick->VAL = 0;

    // 4. Configure Control Register
    // SysTick_CTRL_CLKSOURCE_Msk: Use processor clock
    // SysTick_CTRL_TICKINT_Msk:   Enable interrupt
    // SysTick_CTRL_ENABLE_Msk:    Enable the counter
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
}

void AFR_Timers_Init() {
    // 1. Enable Clocks for TIM1 (Advanced) and TIM3 (General Purpose)
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // TIM1 is on APB2
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // TIM3 is on APB1

    /* --- TIM1: PWM Generation (Heater & Pump) --- */
    TIM1->PSC = (F_CPU_TIM / 1000000) - 1;
    TIM1->ARR = 1000 - 1;
    TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
    TIM1->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;
    TIM1->CCR1 = 515;   // neutral starting duty
    TIM1->CCR2 = 0;
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->EGR  |= TIM_EGR_UG;   // force load all shadow registers
    TIM1->CR1  |= TIM_CR1_CEN;

    /* --- TIM3: Control Loop Interrupt (10ms) --- */
    // Set for 100Hz frequency
    TIM3->PSC = (F_CPU_TIM / 10000) - 1; // 10kHz timer clock
    TIM3->ARR = 100 - 1;                       // 100 counts = 10ms (100Hz)

    // Enable Update Interrupt
    TIM3->DIER |= TIM_DIER_UIE;

    // Enable TIM3 IRQ in NVIC
    NVIC_SetPriority(TIM3_IRQn, 2); // Set medium priority
    NVIC_EnableIRQ(TIM3_IRQn);

    // Start Timers
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
}

static volatile uint32_t ticks = 0;

/**
 * @brief systick ISR, increments ticks and gets the button states
 */
void SysTick_Handler(void){
	//get_button_states();
	ticks++;
}

/**
 * @brief returns ticks passed since program started
 * @return ticks
 */
uint32_t millis(void){
	return ticks;
}

/**
 * @brief blocking delay for an amount of ms
 * @param delay the amount of ms to delay
 */
void delay_ms(unsigned int delay){
	unsigned int start = ticks;
	while((ticks-start) < delay){

	}
}
