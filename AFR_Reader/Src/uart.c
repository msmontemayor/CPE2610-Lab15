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
#include "regaddr.h"

//File Static Register Pointers
static volatile const uint32_t* const sr = (uint32_t*) USART2_SR;
static volatile uint8_t* const dr = (uint8_t*) USART2_DR;

// These will override _read and _write in syscalls.c, which are
// prototyped as weak
int _read(int file, char *ptr, int len)
{
	// Modified the for loop in order to get the correct behavior for fgets
	int byteCnt = 0;
	for (int i = 0; i < len; i++)
	{
		byteCnt++;
		*ptr = usart2Getch();
        //Exit with newline character
		if(*ptr == '\n') break;
		//move to next character
        ptr++;
	}
	//return len;
	return byteCnt; // Return byte count
}

int _write(int file, char *ptr, int len)
{
	for (int i = 0; i < len; i++)
	{
		usart2Putch(*ptr++);
	}
	return len;
}

char usart2Getch(){
	//Wait for character to be placed
	while((*(sr)&RXNE_MASK) != RXNE_MASK);

	char c = ((char) *dr);  // Read character from usart
	usart2Putch(c);  // Echo back
	//usart2Putch(c);  // echo back again

	if (c == '\r'){  // If character is CR
		usart2Putch('\n');  // send it
		c = '\n';   // Return LF. fgets is terminated by LF
	}

	return c;
}

void usart2Putch(char c){
	//Wait for empty space to send
	while((*(sr)&TXE_MASK) != TXE_MASK);
	*dr = c;
}

void initUsart2(){
	// Enable clocks for GPIOA and USART2
	*((volatile uint32_t*) RCC_AHB1ENR) |= GPIOAEN_MASK;
	*((volatile uint32_t*) RCC_APB1ENR) |= USART2EN_MASK;

	// Function 7 of PORTA pins is USART
	uint32_t temp;
	volatile uint32_t* const afrl = (uint32_t*) GPIOA_AFRL;
	temp = *afrl;
	temp &= ~ (0xFF<<(2*4)); // Clear the bits associated with PA3 and PA2
	temp |= (0x77<<(2*4));  // Choose function 7 for both PA3 and PA2
	*afrl = temp;

	volatile uint32_t* const moder = (uint32_t*) GPIOA_MODER;
	temp = *moder;
	temp &= ~(0xF<<(2*2));  // Clear mode bits for PA3 and PA2
	temp |= (0b1010<<(2*2));  // Both PA3 and PA2 in alt function mode
	*moder = temp;

	// Set up USART2
	//USART2_init();  //8n1 no flow control
	// over8 = 0..oversample by 16
	// M = 0..1 start bit, data size is 8, 1 stop bit
	// PCE= 0..Parity check not enabled
	// no interrupts... using polling
	*((volatile uint32_t*) USART2_CR1) = UE_MASK|TE_MASK|RE_MASK; // Enable UART, Tx and Rx
	*((volatile uint32_t*) USART2_CR2) = 0;  // This is the default, but do it anyway
	*((volatile uint32_t*) USART2_CR3) = 0;  // This is the default, but do it anyway
	*((volatile uint32_t*) USART2_BRR) = F_CPU/BAUD;

	// Flush output buffer
	 setvbuf(stdout, NULL, _IONBF, 0);
}

