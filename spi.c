#include "spi.h"

#include <avr/io.h>

void
spi_init(void)
{
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);
}

uint8_t
spi_tx(uint8_t byte)
{

	SPDR = byte;
	while(!(SPSR & (1<<SPIF)));
	uint8_t res = SPDR;
	return res;
}
