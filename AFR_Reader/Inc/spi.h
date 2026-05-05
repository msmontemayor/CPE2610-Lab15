/**
 ******************************************************************************
 * @file           : spi.h
 * @author         : Carson Schur
 * @brief          : spi headers
 ******************************************************************************
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f411xe.h"
#include <stdint.h>
#include <stdio.h>

#define SPI_PORT            SPI1
#define SPI_GPIO            GPIOA

#define SPI_SCK_PIN         5
#define SPI_MISO_PIN        6
#define SPI_MOSI_PIN        7

#define SPI_CS_GPIO         GPIOA
#define SPI_CS_PIN          4

#define SPI_AF_NUM          5
#define SPI_DUMMY_BYTE      0x00

/**
 * @brief Initialize SPI1 and its GPIO pins for polling-based SPI communication.
 */
void spi_init(void);

/**
 * @brief Send one byte over SPI and return the received byte.
 * @param tx_byte Byte to transmit.
 * @return Byte received during the transfer.
 */
uint8_t spi_transfer_byte(uint8_t tx_byte);

/**
 * @brief Perform a simple SPI transaction of fixed length.
 * @param len Number of bytes to transfer.
 * @param tx Pointer to transmit data. Use NULL to send dummy bytes.
 * @param rx Pointer to receive buffer. Use NULL to ignore received data.
 */
void spi_transaction_simple(int len, const uint8_t *tx, uint8_t *rx);

/**
 * @brief Transfer one byte on SPI using polling.
 * @param tx Byte to transmit.
 * @return Byte received during the transfer.
 */
uint8_t spi_xfer_byte(uint8_t tx);

/**
 * @brief Perform a general SPI write/read transaction with optional overlap.
 * @param write_length Number of bytes to write.
 * @param write_data Pointer to transmit buffer.
 * @param read_length Number of bytes to read.
 * @param read_data Pointer to receive buffer.
 * @param overlap Number of transfers where write and read happen at the same
 * time.
 * @return 0 on success, negative value on invalid input.
 */
int spi_wr(int write_length, uint8_t *write_data,
           int read_length, uint8_t *read_data,
           int overlap);

/**
 * @brief Print an array of bytes to the terminal.
 * @param label Text label printed before the array.
 * @param data Pointer to the byte array.
 * @param len Number of elements in the array.
 */
void print_array(const char *label, uint8_t *data, int len);

#endif /* SPI_H_ */
