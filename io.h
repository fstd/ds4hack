#ifndef CNCCTL_IO_H
#define CNCCTL_IO_H 1

#include <stdbool.h>
#include <stdint.h>

enum ioids {
	#define IN(NAME, PLETTER, BITNO, PULLUP) NAME,
	#define OUT(NAME, PLETTER, BITNO, INITVAL) NAME,
	#include "iodef.x.h"
	#undef IN
	#undef OUT
	NUM_IOIDS
};

void io_setup(void);
void io_set(uint8_t ioid, int8_t v);
bool io_get(uint8_t ioid);

#endif
