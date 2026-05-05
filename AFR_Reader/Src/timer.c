/**
 ******************************************************************************
 * @file           : timer.c
 * @author         : Carson Schur
 * @brief          : delay source
 ******************************************************************************
 */


#include "timer.h"
#include "button.h"
#include "regaddr.h"
#include <stdio.h>

static RCC* const rcc = (RCC*)RCCADDR;
static TIM* const tim10 = (TIM*)TIM10;
static TIM* const tim2 = (TIM*)TIM2;
static volatile uint32_t g_ms = 0;

void timer_init_1ms(void){

	SYSTICK* const systick = (SYSTICK*)SYSTICKADDR;

    // reset timer
    systick->CTRL = 0;

    // load so delay is 1ms
    systick->LOAD = 16000 - 1;

    // clear current value
    systick->VAL = 0;

    // CPU clock, enable interrupt, start counter
    systick->CTRL = SYSTICK_CLKSRC | SYSTICK_TICKINT | SYSTICK_EN;
}

uint32_t millis(void){
    return g_ms;
}

void SysTick_Handler(void){
    g_ms++;
    getButtonStates();
}

void init_timers(){
	//enable timer 10 and tim 2
	rcc->APB2ENR |= (1<<17);
	rcc->APB1ENR |= (1<<0);

	// stop the clock  do this every time you change arr
	tim10->CR1 = 0;
	tim2->CR1 = 0;

	//set prescaler this makes ticks 1us and .1 ms
	tim10->PSC = 15;
	tim2->PSC = 15;

	//set ARR
	int arr = 1000000/440; // this is middle A
	tim10->ARR = arr;
	tim10->CCMR1 |= (1<<3);
	tim10->CR1 |= (1<<7);
	int arr2 = 1000 - 1; // this is 1 ms
	tim2->ARR = arr2;

	// set duty cycle to 50%
	tim10->CCR1 = (uint16_t)((arr + 1) / 2);

	// configure pwm Mode
	tim10->CCMR1 |= (0b110<<4); // PWM mode 1

	// enable output signal
	tim10->CCER |= (1<<0);

	// forces update event
	tim10->EGR = (1<<0);

	// Enable update interrupt
	tim2->DIER |= 1;

	// Enable IRQ in NVIC
	*((volatile uint32_t*)NVIC_ISER0) |= (1 << TIM2_IRQ_NUM);

	// force update event
	tim2->SR &= ~1;
	tim2->EGR = (1<<0);

	// Start timer
	tim2->CR1 |= 1;

}

void delay_ms(uint32_t ms){
    uint32_t start = g_ms;

    while ((g_ms - start) < ms) {
        // wait
    }
}

