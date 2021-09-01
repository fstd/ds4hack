#ifndef SPI_H
#define SPI_H 1

#include <stdint.h>

void spi_init(void);

uint8_t spi_tx(uint8_t byte);

#endif /* SPI_H */

