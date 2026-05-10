/**
 ******************************************************************************
 * @file           : itron.c
 * @author         : Carson Schur
 * @brief          : itron source
 ******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>

#include "spi.h"
#include "itron.h"
#include "timer.h"

int vfd_wait_ready(void){
    uint32_t timeout = 100000;

    delay_ms(2);

    while (VFD_BUSY_GPIO->IDR & (1 << VFD_BUSY_PIN)) {
        timeout--;

        if (timeout == 0) {
            return -1;
        }
    }

    return 0;
}

int vfd_write_byte(uint8_t data)
{
    if (vfd_wait_ready() != 0) {
        return -1;
    }

    SPI_PORT->CR1 |= SPI_CR1_SPE;

    spi_xfer_byte(data);

    while (SPI_PORT->SR & SPI_SR_BSY) {
    }

    SPI_PORT->CR1 &= ~SPI_CR1_SPE;

    delay_ms(1);

    return 0;
}

void vfd_reset_init(void){
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    VFD_RESET_GPIO->MODER &= ~(3 << (2 * VFD_RESET_PIN));
    VFD_RESET_GPIO->MODER |=  (1 << (2 * VFD_RESET_PIN));   // output mode

    VFD_RESET_GPIO->OTYPER &= ~(1 << VFD_RESET_PIN);        // push-pull
    VFD_RESET_GPIO->OSPEEDR |= (3 << (2 * VFD_RESET_PIN));  // high speed
    VFD_RESET_GPIO->PUPDR &= ~(3 << (2 * VFD_RESET_PIN));   // no pull-up/down

    // Default released from reset
    VFD_RESET_GPIO->BSRR = (1 << VFD_RESET_PIN);
}

void vfd_init(void){
	// Chat GPT wizardry
	SCB->CPACR |= (0xF << 20);
	__DSB();
	__ISB();

	vfd_reset_init();
    // Pull /RESET low
    VFD_RESET_GPIO->BSRR = (1 << (VFD_RESET_PIN + 16));

    // Hold low longer than 1 ms
    delay_ms(5);

    // Release /RESET high
    VFD_RESET_GPIO->BSRR = (1 << VFD_RESET_PIN);

    vfd_define_lambda_char();
}

void vfd_write_array(const uint8_t *data, int len){
    if (data == 0 || len <= 0) {
        return;
    }

    for (int i = 0; i < len; i++) {
    	(void)vfd_write_byte(data[i]);
    }
}

void vfd_write_string(const char *str){
    while (*str != '\0') {
    	(void)vfd_write_byte((uint8_t)*str);
        str++;
    }
}

void vfd_clear(void){
    uint8_t cmd = 0x0C;
    vfd_write_array(&cmd, 1);
    delay_ms(10);
}

void vfd_set_cursor(uint16_t x, uint8_t y_line){
    uint8_t cmd[] = {
        0x1F, 0x24,
        (uint8_t)(x & 0xFF),
        (uint8_t)((x >> 8) & 0xFF),
        y_line,
        0x00
    };

    vfd_write_array(cmd, sizeof(cmd));
}

void vfd_define_lambda_char(void){
    /*
     * Define custom character 0x80 as a rough 5x7 lambda.
     *
     * Command:
     * ESC & a c1 c2 x d1 d2 d3 d4 d5
     *
     * a  = 1      download character type
     * c1 = 0x80   start character code
     * c2 = 0x80   end character code
     * x  = 5      5-dot-wide character
     */
	uint8_t lambda_cmd[] = {
	    0x1B, 0x26,          // ESC &
	    0x01,                // a = character type
	    VFD_LAMBDA_CHAR,     // c1
	    VFD_LAMBDA_CHAR,     // c2
	    0x05,                // x = 5 columns

	    0x82,
	    0x4C,
	    0x30,
	    0x0C,
	    0x02
	};

    uint8_t enable_download_chars[] = {
        0x1B, 0x25, 0x01
    };

    vfd_write_array(lambda_cmd, sizeof(lambda_cmd));
    delay_ms(5);

    vfd_write_array(enable_download_chars, sizeof(enable_download_chars));
    delay_ms(5);
}

void vfd_test(void){
	//vfd_write_byte(0x0C);
	delay_ms(1);
	(void)vfd_write_byte('A');
}

void vfd_display_afr(float lambda)
{
    char line1[20];
    char line2[20];

    float afr = 14.7f * lambda;

    snprintf(line1, sizeof(line1), "AFR: %.1f", afr);
    snprintf(line2, sizeof(line2), ": %.1f", lambda);

    vfd_set_cursor(0, 0);
    vfd_write_string(line1);
    vfd_write_string("   ");

    vfd_set_cursor(0, 1);
    vfd_write_byte(VFD_LAMBDA_CHAR);
    vfd_write_string(line2);
    vfd_write_string("   ");
}

