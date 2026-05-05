/**
 * Name: Carson Schur
 * Course: CPE2610
 * Assignment: Lab2
 * Dependancies: None
 * Description: C terminal code using Usart2
 * Note: NO MODIFICATION REQUIRED
 */

#ifndef UART_H_
#define UART_H_

//Modify if changing the processor clock or target baud rate
#define F_CPU 16000000UL
#define BAUD 57600


// Function prototypes
extern void initUsart2();
extern char usart2Getch();
extern void usart2Putch(char c);

// syscalls overrides
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);


#endif /* UART_H_ */
