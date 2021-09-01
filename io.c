#include "io.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <avr/io.h>
#include <avr/sfr_defs.h>

struct io {
	uint8_t outaddr;
	uint8_t inaddr;
	uint8_t pinmask;
	uint8_t output; // 0: input, 1: output-low, 2: output-high
};

static struct io s_io[NUM_IOIDS];

static uint8_t outaddr(char pletter);
static uint8_t inaddr(char pletter);

void
io_setup(void)
{
	#define SETUP(NAME, PLETTER, BITNO, OUTPUT, INITVAL)     \
	    s_io[NAME].outaddr = outaddr(#PLETTER[0]);           \
	    s_io[NAME].inaddr = inaddr(#PLETTER[0]);             \
	    s_io[NAME].pinmask = 1<<BITNO;                       \
	    s_io[NAME].output = !!OUTPUT;                        \
	    PORT ## PLETTER |= INITVAL << P ## PLETTER ## BITNO; \
	    DDR ## PLETTER |= OUTPUT << DD ## PLETTER ## BITNO;

	#define IN(NAME, PLETTER, BITNO, PULLUP) \
	    SETUP(NAME, PLETTER, BITNO, 0, PULLUP)

	#define OUT(NAME, PLETTER, BITNO, INITVAL) \
	    SETUP(NAME, PLETTER, BITNO, 1, INITVAL)

		#include "iodef.x.h"

	#undef IN
	#undef OUT
	#undef SETUP
}

// for inputs, this sets the pull-up.  for outputs, -1 means toggle
void
io_set(uint8_t ioid, int8_t v)
{
	if (s_io[ioid].output) {
		if (v == -1)
			v = 2-s_io[ioid].output;
		s_io[ioid].output = 1+!!v;
	}

	if (v)
		_SFR_IO8(s_io[ioid].outaddr) |= s_io[ioid].pinmask;
	else
		_SFR_IO8(s_io[ioid].outaddr) &= ~s_io[ioid].pinmask;

}

// for outputs, this reads the value we're outputting
bool
io_get(uint8_t ioid)
{
	uint8_t ioaddr =
	    s_io[ioid].output ? s_io[ioid].outaddr : s_io[ioid].inaddr;

	return _SFR_IO8(ioaddr) & s_io[ioid].pinmask;
}

static uint8_t
outaddr(char pletter)
{
	switch (pletter) {
	case 'a': case 'A':
		return _SFR_IO_ADDR(PORTA);
	case 'b': case 'B':
		return _SFR_IO_ADDR(PORTB);
	case 'c': case 'C':
		return _SFR_IO_ADDR(PORTC);
	case 'd': case 'D':
		return _SFR_IO_ADDR(PORTD);
	}

	return 0;
}

static uint8_t
inaddr(char pletter)
{
	switch (pletter) {
	case 'a': case 'A':
		return _SFR_IO_ADDR(PINA);
	case 'b': case 'B':
		return _SFR_IO_ADDR(PINB);
	case 'c': case 'C':
		return _SFR_IO_ADDR(PINC);
	case 'd': case 'D':
		return _SFR_IO_ADDR(PIND);
	}

	return 0;
}

