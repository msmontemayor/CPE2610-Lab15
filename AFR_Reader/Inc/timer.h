/**
 ******************************************************************************
 * @file           : timer.h
 * @author         : Carson Schur
 * @brief          : Delay header
 ******************************************************************************
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void timer_init_1ms(void);

uint32_t millis(void);

void delay_ms(uint32_t ms);

void init_timers();

#endif /* TIMER_H_ */
