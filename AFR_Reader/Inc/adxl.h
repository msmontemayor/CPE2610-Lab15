/**
 ******************************************************************************
 * @file           : adxl.h
 * @author         : Carson Schur
 * @brief          : adxl headers
 ******************************************************************************
 */

#ifndef ADXL_H_
#define ADXL_H_

#ifndef ADXL345_H_
#define ADXL345_H_

#define ADXL_REG_DEVID       0x00
#define ADXL_REG_POWER_CTL   0x2D
#define ADXL_REG_DATA_FORMAT 0x31
#define ADXL_REG_DATAX0      0x32

#define ADXL_BW_RATE         0x2C

/* Tap Threshold and Timing */
#define ADXL_REG_THRESH_TAP   0x1D  // Threshold for tap detection
#define ADXL_REG_DUR          0x21  // Max time for an event to be a tap
#define ADXL_REG_LATENT       0x22  // Gap between single and double tap
#define ADXL_REG_WINDOW       0x23  // Window for second tap

/* Control Registers */
#define ADXL_REG_TAP_AXES     0x2A  // Enable/disable axis for tap
#define ADXL_REG_INT_ENABLE   0x2E  // Enable interrupts
#define ADXL_REG_INT_MAP      0x2F  // Map to INT1 or INT2 pin
#define ADXL_REG_INT_SOURCE   0x30  // Read to clear interrupts

#define ADXL_READ_BIT        (1U << 7)
#define ADXL_MULTI_BYTE_BIT  (1U << 6)
#define ADXL_G_PER_LSB       0.004f

void adxl_write_reg(uint8_t reg, uint8_t value);
void adxl_init();
void print_info();
void check_taps();
void init_int_pin();
void read_accel(float *x, float *y, float *z);


#endif /* ADXL345_H_ */

#endif /* ADXL_H_ */
