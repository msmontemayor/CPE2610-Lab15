/*
 * timer.h
 *
 *  Created on: Feb 10, 2026
 *      Author: montemayorm
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

#define F_CPU_TIM 16000000

// function prototypes
void initTim10();
void initTim4Int();
void systick_init();
void AFR_Timers_Init();

void delay_ms(unsigned int delay);
uint32_t millis(void);

#endif /* TIMER_H_ */

