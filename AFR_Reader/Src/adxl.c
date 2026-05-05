/**
 ******************************************************************************
 * @file           : adxl.c
 * @author         : Carson Schur
 * @brief          : adxl source
 ******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>

#include "spi.h"
#include "adxl.h"
#include "timer.h"


void adxl_write_reg(uint8_t reg, uint8_t value) {
    uint8_t tx[2] = {reg, value};
    uint8_t rx[2];
    spi_wr(2, tx, 0, rx, 0);
}

void adxl_init() {
    spi_init();

    // Set to +/- 4g
    adxl_write_reg(ADXL_REG_DATA_FORMAT, 0x09);

    // Put into Measurement Mode
    adxl_write_reg(ADXL_REG_POWER_CTL, 0x08);

    // Set Tap Threshold ~3g
    adxl_write_reg(ADXL_REG_THRESH_TAP, 0x30);

    // Set Duration ~10ms.
    adxl_write_reg(ADXL_REG_DUR, 0x10);

    // Set ~20ms. Time between taps
    adxl_write_reg(ADXL_REG_LATENT, 0x20);

    // Set Time allowed for the second tap ~318ms.
    adxl_write_reg(ADXL_REG_WINDOW, 0x90);

    // Enable Z-axis for tap detection
    adxl_write_reg(ADXL_REG_TAP_AXES, 0x01); // Bit 0 = Z, Bit 1 = Y, Bit 2 = X

    // Map interrupts to INT
    // SINGLE_TAP and DOUBLE_TAP to trigger INT
    adxl_write_reg(ADXL_REG_INT_MAP, 0x00);

    // Enable Interrupts (SINGLE_TAP = 0x40, DOUBLE_TAP = 0x20)
    adxl_write_reg(ADXL_REG_INT_ENABLE, 0x60);

    // enables the FPU (found online)
    SCB->CPACR |= ((3UL << 20) | (3UL << 22));
}

void init_int_pin() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA0 as Input (00)
    GPIOA->MODER &= ~(GPIO_MODER_MODER0_Msk);

    // Optional: Pull-down if the ADXL345 is active-high (default)
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk);
    GPIOA->PUPDR |= (2 << GPIO_PUPDR_PUPD0_Pos);
}

void read_accel(float *x, float *y, float *z) {
    uint8_t tx = 0xF2; // Multi-byte read starting at DATAX0
    uint8_t rx[6];     // 2 bytes per axis

    if (spi_wr(1, &tx, 6, rx, 0) == 0) {
        // Combine bytes (Little Endian)
        int16_t raw_x = (int16_t)((rx[1] << 8) | rx[0]);
        int16_t raw_y = (int16_t)((rx[3] << 8) | rx[2]);
        int16_t raw_z = (int16_t)((rx[5] << 8) | rx[4]);

        // Convert to 'g' (FULL_RES = 4mg per LSB = 0.004g)
        *x = raw_x * 0.004f;
        *y = raw_y * 0.004f;
        *z = raw_z * 0.004f;
    }
}


void check_taps() {
    uint8_t tx = ADXL_REG_INT_SOURCE | ADXL_READ_BIT; // Read bit set
    uint8_t rx[1];

    if (spi_wr(1, &tx, 1, rx, 0) == 0) {
        uint8_t status = rx[0];

        if ((status & 0x20) != 0) {
            printf("DOUBLE TAP detected!\n");
        } else if ((status & 0x40) != 0) {
            printf("SINGLE TAP detected!\n");
        }
    }
}
