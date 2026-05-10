/**
 * Name: _________
 * Course: CPE2610
 * Assignment: Lab2
 * Dependancies: uart.h, regaddr.h
 * Description: C terminal read and write code(polling driven)
 */

#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "stm32f411xe.h" // Provides USART_TypeDef and GPIO_TypeDef

// 1. Replaced manual static pointers with standard CMSIS register access
// USART2 is already defined as ((USART_TypeDef *) USART2_BASE)

int _read(int file, char *ptr, int len)
{
    int byteCnt = 0;
    for (int i = 0; i < len; i++)
    {
        *ptr = usart2Getch();
        byteCnt++;
        if (*ptr == '\n') break;
        ptr++;
    }
    return byteCnt;
}

int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
    {
        usart2Putch(*ptr++);
    }
    return len;
}

char usart2Getch() {
    // 2. Use USART_SR_RXNE instead of RXNE_MASK
    while (!(USART2->SR & USART_SR_RXNE));

    char c = (char)(USART2->DR & 0xFF);
    usart2Putch(c); // Echo back

    if (c == '\r') {
        usart2Putch('\n');
        c = '\n';
    }
    return c;
}

void usart2Putch(char c) {
    // 3. Use USART_SR_TXE instead of TXE_MASK
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = (uint16_t)c;
}

void initUsart2() {
    // 4. Use RCC structure for clock enabling
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // 5. Use GPIOA structure for Pin Multiplexing (PA2, PA3)
    // AFR[0] is equivalent to AFRL (Low register for pins 0-7)
    GPIOA->AFR[0] &= ~(0xFFU << (2 * 4));
    GPIOA->AFR[0] |= (0x77U << (2 * 4));

    // 6. Set Mode to Alternate Function (10) for PA2 and PA3
    GPIOA->MODER &= ~(0xFU << (2 * 2));
    GPIOA->MODER |= (0xAU << (2 * 2)); // 0b1010 shifted

    // 7. Setup USART2 using standard bit definitions
    // UE: USART Enable, TE: Transmitter Enable, RE: Receiver Enable
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    // F_CPU and BAUD should be defined in your header
    USART2->BRR = F_CPU / BAUD;

    setvbuf(stdout, NULL, _IONBF, 0);
}
