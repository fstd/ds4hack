//device identified as ATmega32 (sig: 1e 95 2)
//fuse bits: low: ff, high: cf

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "max3421e_regs.h"

#define VERSION "dev"

#include "uart_intr.h"
#include "spi.h"
#include "io.h"

#define COUNTOF(ARR) (sizeof (ARR) / sizeof (ARR)[0])

static volatile bool s_int = false;

static void setup(void);
static void banner(void);
static void halt(void);
void ui_cmd(const char *ln);
void ui(void);
static uint8_t maxtx(uint8_t regno, bool wr, uint8_t data, uint8_t *stat);
static void init3421(void);

static void
W(uint8_t regno, uint8_t data)
{
	maxtx(regno, 1, data, NULL);
}

static uint8_t
R(uint8_t regno)
{
	return maxtx(regno, 0, 0, NULL);
}

static bool dbg = true;

#define D(...) do { if (dbg) uart_printf(__VA_ARGS__); } while (0)

static uint8_t
maxtx(uint8_t regno, bool wr, uint8_t data, uint8_t *stat)
{
	uint8_t cmd = (regno << 3) | (wr?2:0);

	io_set(SPI_SS, 0);
	_delay_us(1);
	uint8_t status = spi_tx(cmd);
	uint8_t reply = spi_tx(data);
	if (stat)
		*stat = status;
	_delay_us(1);
	io_set(SPI_SS, 1);

	if (wr) {
		D("W: R%1 <- %1 (stat: %1)\r\n", regno, data, status);
	} else {
		D("R: R%1: %1 (stat: %1)\r\n", regno, reply, status);
	}

	return reply;
}

static void
setup(void)
{

	cli();
	io_setup();
	uart_init(1); // 500 kbaud
	sei();
	spi_init();
	MCUCR |= 1<<ISC11 | 1<<ISC10;
	GICR |= 1<<INT1;
	init3421();
	banner();
}

static void
init3421(void)
{
	W(PINCTL, 1<<FDUPSPI | 1<<POSINT);
	W(USBCTL, 1<<CHIPRES);
	W(USBCTL, 0x00);
	while (!(R(USBIRQ) & 1<<OSCOKIRQ)); // wait for PLL
	W(IOPINS1, 0x00); // 7seg off
	W(IOPINS2, 0x00); // Vbus off
	_delay_ms(200);
	W(IOPINS2, 1<<GPOUT7); // Vbus on
	if (R(REVISION) != 0x13) {
		uart_printf("init error\r\n");
		halt();
	}
	W(CPUCTL, 1<<IE);
	uart_printf("init ok\r\n");
}

static void
banner(void)
{
	uart_printf("\r\n###############\r\n");
	uart_printf("# ds4hack "VERSION" #\r\n");
	uart_printf("###############\r\n");
}

static void
halt(void)
{
	uart_printf("\r\nsystem halted\r\n");
	uart_flush();
	cli();
	for (;;);
}


void
ui_cmd(const char *ln)
{
	if (ln[0] == 'h') {
		halt();
	} else if (ln[0] == 'd') {
		for (int i = 1; i <= 31; i++) {
			if (i == 3 || i == 5 || (i >= 8 && i <= 12) || i == 19)
				continue;
			uint8_t status = 0xaa;
			uint8_t r = maxtx(i, 0, 0, &status);
			uart_printf("reg: %1, content: %1, status: %1\r\n",
				i, r, status);
		}
	} else if (ln[0] == 'r') {
		uint8_t reg;
		sscanf(ln, "r %"SCNu8, &reg);
		uint8_t r = maxtx(reg, 0, 0, NULL);
		uart_printf("%1\r\n", r);
	} else if (ln[0] == 'w') {
		uint8_t reg, data;
		sscanf(ln, "w %"SCNu8" %"SCNu8, &reg, &data);
		uint8_t r = maxtx(reg, 1, data, NULL);
		uart_printf("ok\r\n");
	} else if (ln[0] == 'R') {
		TCCR1B &= ~(1<<CS10 | 1<<CS11 | 1<<CS12);
		cli();
		WDTCR |= 1<<WDE;
		for (;;);
	}
}

void
ui(void)
{
	static char linebuf[64];
	static size_t li;
	static bool prompted;

	if (!prompted) {
		uart_put('>');
		uart_put('>');
		uart_put(' ');
		prompted = true;
	}

	while (uart_avail()) {
		char c = uart_get();
		if (c == '\r') {
			if (li == sizeof linebuf)
				li--;

			linebuf[li] = 0;
			ui_cmd(linebuf);
			prompted = false;
			li = 0;
			continue;
		}

		if (li < sizeof linebuf)
			linebuf[li++] = c;
	}
}

int
main(void)
{
	setup();

	for (;;) {
		if (s_int) {
			s_int = false;
			D("int!\r\n");
		}
		ui();
	}
}

ISR(INT1_vect)
{
	s_int = true;
}
