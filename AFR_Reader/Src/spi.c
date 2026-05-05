/**
 ******************************************************************************
 * @file           : spi.c
 * @author         : Carson Schur
 * @brief          : spi source
 ******************************************************************************
 */


#include "spi.h"

static void spi_cs_low(void) {
    SPI_CS_GPIO->BSRR = (1 << (SPI_CS_PIN + 16));
}

static void spi_cs_high(void) {
    SPI_CS_GPIO->BSRR = (1 << SPI_CS_PIN);
}

void spi_init(void) {
    //  enable clocks for GPIOA, GPIOC, and SPI1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    //  PA5, PA6, PA7 -> alternate function
    SPI_GPIO->MODER &= ~((3 << (2 * SPI_SCK_PIN)) |
                         (3 << (2 * SPI_MISO_PIN)) |
                         (3 << (2 * SPI_MOSI_PIN)));
    SPI_GPIO->MODER |=  ((2 << (2 * SPI_SCK_PIN)) |
                         (2 << (2 * SPI_MISO_PIN)) |
                         (2 << (2 * SPI_MOSI_PIN)));

    // optional: high speed on SPI pins
    SPI_GPIO->OSPEEDR |= ((3 << (2 * SPI_SCK_PIN)) |
                          (3 << (2 * SPI_MISO_PIN)) |
                          (3 << (2 * SPI_MOSI_PIN)));

    //  AF5 on PA5, PA6, PA7
    SPI_GPIO->AFR[0] &= ~((0xF << (4 * SPI_SCK_PIN)) |
                          (0xF << (4 * SPI_MISO_PIN)) |
                          (0xF << (4 * SPI_MOSI_PIN)));
    SPI_GPIO->AFR[0] |=  ((SPI_AF_NUM << (4 * SPI_SCK_PIN)) |
                          (SPI_AF_NUM << (4 * SPI_MISO_PIN)) |
                          (SPI_AF_NUM << (4 * SPI_MOSI_PIN)));

    //  software CS pin as normal output, default high
    SPI_CS_GPIO->MODER &= ~(3 << (2 * SPI_CS_PIN));
    SPI_CS_GPIO->MODER |=  (1 << (2 * SPI_CS_PIN));
    spi_cs_high();

    // pull up and pull down appropriate pins
    SPI_GPIO->OTYPER &= ~((1 << SPI_SCK_PIN) |
                          (1 << SPI_MOSI_PIN));

    SPI_GPIO->PUPDR &= ~((3 << (2 * SPI_SCK_PIN)) |
                         (3 << (2 * SPI_MISO_PIN)) |
                         (3 << (2 * SPI_MOSI_PIN)));

    SPI_CS_GPIO->OTYPER &= ~(1 << SPI_CS_PIN);
    SPI_CS_GPIO->PUPDR &= ~(3 << (2 * SPI_CS_PIN));

    //  configure SPI1 but do not enable yet
    SPI_PORT->CR1 = 0;
    SPI_PORT->CR1 |= SPI_CR1_MSTR;      // master
    SPI_PORT->CR1 |= SPI_CR1_SSM;       // software NSS management
    SPI_PORT->CR1 |= SPI_CR1_SSI;       // internal NSS high
    SPI_PORT->CR1 |= (0x7 << SPI_CR1_BR_Pos);      // baud prescaler,
    SPI_PORT->CR1 |= SPI_CR1_CPOL;
    SPI_PORT->CR1 |= SPI_CR1_CPHA;
    // mode 3 initially
    // DFF = 0 means 8-bit mode
    // LSBFIRST = 0 means MSB first
}

uint8_t spi_transfer_byte(uint8_t tx_byte) {
    // wait until transmit buffer empty
    while (!(SPI_PORT->SR & SPI_SR_TXE)) {
    }

    // write byte
    *((volatile uint8_t *)&SPI_PORT->DR) = tx_byte;

    // wait until receive buffer has data
    while (!(SPI_PORT->SR & SPI_SR_RXNE)) {
    }

    // return received byte
    return *((volatile uint8_t *)&SPI_PORT->DR);
}

void spi_transaction_simple(int len, const uint8_t *tx, uint8_t *rx) {
    SPI_PORT->CR1 |= SPI_CR1_SPE;   // start SPI
    spi_cs_low();                   // assert chip select

    for (int i = 0; i < len; i++) {
        uint8_t tx_byte = (tx != 0) ? tx[i] : SPI_DUMMY_BYTE;
        uint8_t rx_byte = spi_transfer_byte(tx_byte);

        if (rx != 0) {
            rx[i] = rx_byte;
        }
    }

    // wait until transmit buffer empty
    while (!(SPI_PORT->SR & SPI_SR_TXE)) {
    }

    // wait until SPI not busy
    while (SPI_PORT->SR & SPI_SR_BSY) {
    }

    // if needed, clear possible overrun by reading DR then SR
    (void)SPI_PORT->DR;
    (void)SPI_PORT->SR;

    spi_cs_high();                  // deassert chip select
    SPI_PORT->CR1 &= ~SPI_CR1_SPE;  // end SPI
}

uint8_t spi_xfer_byte(uint8_t tx){

    while (!(SPI_PORT->SR & SPI_SR_TXE)) {
    }

    *(__IO uint8_t *)&SPI_PORT->DR = tx;

    while (!(SPI_PORT->SR & SPI_SR_RXNE)) {
    }

    return *(__IO uint8_t *)&SPI_PORT->DR;
}

int spi_wr(int write_length, uint8_t *write_data,
           int read_length, uint8_t *read_data,
           int overlap){

	(void)SPI_PORT->DR;
	(void)SPI_PORT->SR;

    if (write_length < 0 || read_length < 0 || overlap < 0) {
        return -1;
    }

    if (overlap > write_length || overlap > read_length) {
        return -2;
    }

    int total_iters = write_length + read_length - overlap;

    int write_index = 0;
    int read_index  = 0;

    SPI_PORT->CR1 |= SPI_CR1_SPE;
    spi_cs_low();

    for (int i = 0; i < total_iters; i++) {

        uint8_t tx_byte = SPI_DUMMY_BYTE;
        uint8_t rx_byte;

        /*
         * Send real write data while any remains.
         * After that, if still need reads, send dummy bytes to clock them in.
         */
        if (write_index < write_length) {
            tx_byte = write_data[write_index];
            write_index++;
        } else {
            tx_byte = SPI_DUMMY_BYTE;
        }

        rx_byte = spi_xfer_byte(tx_byte);

        /*
         * Start storing received data only after the initial
         * write-only portion is complete.
         *
         * Number of pure write-only iterations = write_length - overlap
         */
        if (i >= (write_length - overlap)) {
            if (read_index < read_length && read_data != 0) {
                read_data[read_index] = rx_byte;
            }
            if (read_index < read_length) {
                read_index++;
            }
        }
    }

    while (!(SPI_PORT->SR & SPI_SR_TXE)) {
    }

    while (SPI_PORT->SR & SPI_SR_BSY) {
    }

    (void)SPI_PORT->DR;
    (void)SPI_PORT->SR;

    spi_cs_high();
    SPI_PORT->CR1 &= ~SPI_CR1_SPE;

    return 0;
}

void print_array(const char *label, uint8_t *data, int len) {
    printf("%s: ", label);
    for (int i = 0; i < len; i++) {
        printf("%u ", data[i]);
    }
    printf("\r\n");
}
