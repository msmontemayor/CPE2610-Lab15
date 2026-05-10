/**
 * Name: Carson Schur
 * Course: CPE2610
 * Assignment: Lab2
 * Dependancies: none
 * Description: Portable Header file containing STM32F411 Addresses and Control Bit Masks
 */

#include <stdint.h>
#ifndef REGADDR_H_
#define REGADDR_H_

///Complete this file by adding address and
/// creating masks for necessary control signals.
///Address can be calculated using the Memory Map(Section 2.3)
///  and Register Maps at the end of the documentation for each
///  component.
///This file can be modified to add future registers and masks.


//Systick
#define SYSTICKADDR 0xE000E010

typedef struct{
	volatile uint32_t CTRL;
	volatile uint32_t LOAD;
	volatile uint32_t VAL;
	volatile uint32_t CALIB;
}SYSTICK;

#define SYSTICK_EN (1<<0)
#define SYSTICK_TICKINT (1<<1)
#define SYSTICK_CLKSRC (1<<2)


//RCC
#define RCC_AHB1ENR 0x40023830
#define GPIOAEN_MASK (1<<0)
#define GPIOBEN_MASK (1<<1)

#define RCC_APB1ENR 0x40023840
#define USART2EN_MASK (1<<17)

//GPIOA
#define GPIOA 0x40020000
#define GPIOA_MODER 0x40020000
#define GPIOA_PUPDR 0x4002000c
#define GPIOA_IDR 0x40020010
#define GPIOA_ODR 0x40020014
#define GPIOA_AFRL 0x40020020

//RCC struct
#define RCCADDR 0x40023800
typedef struct{
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t res1;
	volatile uint32_t res2;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t res3;
	volatile uint32_t res4;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t res5;
	volatile uint32_t res6;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t res7;
	volatile uint32_t res8;
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t res9;
	volatile uint32_t res10;
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	volatile uint32_t res11;
	volatile uint32_t res12;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t res13;
	volatile uint32_t res14;
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
	volatile uint32_t res15;
	volatile uint32_t DCKCFGR;
} RCC;

//GPIO struct
typedef struct{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile const uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
} GPIO;

//TIM struct
#define TIM10 0x40014400
#define TIM2  0x40000000
typedef struct{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t RCR;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	volatile uint32_t res1;
	volatile uint32_t DCR;
} TIM;

#define NVIC_ISER0  0xE000E100
#define TIM2_IRQ_NUM 28
#define ADC_IRQ_NUM 18

// ADC struct
#define ADCADDR 0x40012000
typedef struct{
    volatile uint32_t SR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t JOFR1;
    volatile uint32_t JOFR2;
    volatile uint32_t JOFR3;
    volatile uint32_t JOFR4;
    volatile uint32_t HTR;
    volatile uint32_t LTR;
    volatile uint32_t SQR1;
    volatile uint32_t SQR2;
    volatile uint32_t SQR3;
    volatile uint32_t JSQR;
    volatile const uint32_t JDR1;
    volatile const uint32_t JDR2;
    volatile const uint32_t JDR3;
    volatile const uint32_t JDR4;
    volatile const uint32_t DR;
    volatile const uint32_t CCR;
} ADC;

//GPIOB
#define GPIOB 0x40020400
#define GPIOB_MODER 0x40020400
#define GPIOB_PUPDR 0x4002040c
#define GPIOB_IDR 0x40020010
#define GPIOB_ODR 0x40020014
#define GPIOB_AFRL 0x40020020

//UART
#define USART2_SR 0x40004400
#define USART2_DR 0x40004404
#define USART2_BRR 0x40004408
#define USART2_CR1 0x4000440C
#define USART2_CR2 0x40004410
#define USART2_CR3 0x40004414

// CR1 bits
#define UE_MASK (1<<13)
#define TE_MASK (1<<3)
#define RE_MASK (1<<2)

// SR control bits
#define TXE_MASK (1<<7)
#define RXNE_MASK (1<<5)


#endif /* REGADDR_H_ */
