/**
 ******************************************************************************
 * @file           : itron.h
 * @author         : Carson Schur
 * @brief          : itron headers
 ******************************************************************************
 */

#ifndef ITRON_H_
#define ITRON_H_

#include "stm32f411xe.h"
#include <stdint.h>

#define VFD_BUSY_GPIO GPIOA
#define VFD_BUSY_PIN  6
#define VFD_RESET_GPIO GPIOB
#define VFD_RESET_PIN  10

#define VFD_LAMBDA_CHAR 0x80

int vfd_wait_ready(void);
int vfd_write_byte(uint8_t data);
void vfd_init(void);
void vfd_write_array(const uint8_t *data, int len);
void vfd_write_string(const char *str);
void vfd_clear(void);
void vfd_set_cursor(uint16_t x, uint8_t y_line);
void vfd_define_lambda_char(void);
void vfd_test(void);
void vfd_display_afr(float lambda);


#endif /* ITRON_H_ */
