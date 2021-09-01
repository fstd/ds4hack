#include "uart_intr.h"
#include <stdio.h>

// permitted buffer sizes: 1, 2, 4, 8, 16, 32, 64, 128
#ifndef UART_RXBUF_SZ
#warning using default UART_RXBUF_SZ 8
#define UART_RXBUF_SZ 8
#endif

#ifndef UART_TXBUF_SZ
#warning using default UART_TXBUF_SZ 8
#define UART_TXBUF_SZ 8
#endif

#define UART_RXBUF_MASK (UART_RXBUF_SZ - 1) //for cheap modulo operation
#define UART_TXBUF_MASK (UART_TXBUF_SZ - 1) // i.e. & instead of %

#if(UART_TXBUF_SZ & UART_TXBUF_MASK || UART_TXBUF_SZ > 128 || UART_TXBUF_SZ < 1)
#error UART_TXBUF_SZ must be a power of two and between 1 and 128
#endif

#if(UART_RXBUF_SZ & UART_RXBUF_MASK || UART_RXBUF_SZ > 128 || UART_RXBUF_SZ < 1)
#error UART_RXBUF_SZ must be a power of two and between 1 and 128
#endif

static uint8_t rxbuf[UART_RXBUF_SZ]; //receive ring buffer
static uint8_t txbuf[UART_TXBUF_SZ]; //transmit ring buffer

static uint8_t rx_cons_ind; //uart_recv() reads at this offset
static uint8_t rx_prod_ind; //RXC ISR stores at this offset
static uint8_t tx_prod_ind; //uart_send() stores at this offset
static uint8_t tx_cons_ind; //UDRE ISR transmits at this offset

volatile uint8_t rx_count; //number of octets in receive buffer
static volatile uint8_t tx_count; //number of octets in transmit buffer

static void u8tostr(char *buf, uint8_t n);
static int hexdig(uint8_t n);

ISR(USART_RXC_vect) //receive complete, buffer received octet
{
	if (rx_count >= UART_RXBUF_SZ) {
		if(UDR){} //overflow, dummy access UDR to discard octet
	} else {
		rxbuf[rx_prod_ind++ & UART_RXBUF_MASK] = UDR;
		++rx_count;
	}
}

ISR(USART_UDRE_vect) //data reg empty, transmit next one, or mask this interrupt
{
	if (tx_count) {
		UDR = txbuf[tx_cons_ind++ & UART_TXBUF_MASK];
		tx_count--;
	} else //no more data to transmit for now, disable this interrupt
		UCSRB &= ~(1<<UDRIE);
}

void
uart_init(uint16_t ubrr)
{
	UBRRH = ((uint8_t)(ubrr>>8)) & ~(1<<URSEL); //ensure URSEL is 0
	UBRRL = (uint8_t)ubrr;

	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0); // 8N1
}

uint8_t
uart_recv(uint8_t *data, uint8_t max)
{
	uint8_t num = 0; // number of octets read from rx buffer
	uint8_t cnt = rx_count; //cache volatile

	while(cnt-- && max--)
		data[num++] = rxbuf[rx_cons_ind++ & UART_RXBUF_MASK];

	uint8_t s = SREG; cli();   rx_count -= num;   SREG = s;

	return num; //return number of octets we actually got
}

uint8_t
uart_send(uint8_t *data, uint8_t max)
{
	uint8_t num = 0; // number of octets buffered for transmission
	uint8_t cnt = tx_count; //cache volatile

	while(max-- && cnt++ < UART_TXBUF_SZ)
		txbuf[tx_prod_ind++ & UART_TXBUF_MASK] = data[num++];

	uint8_t s = SREG; cli();   tx_count += num;   SREG = s;
	UCSRB |= (1<<UDRIE); //enable data reg empty interrupt

	return num; //return number of octets we actually buffered for tx
}

uint8_t
uart_avail(void)
{
	return rx_count;
}

void
uart_put(uint8_t data)
{
	while(tx_count >= UART_TXBUF_SZ || !uart_send(&data, 1));
}

uint8_t
uart_get(void) //waits
{
	static uint8_t c;
	while(!rx_count || !uart_recv(&c, 1));
	return c;
}

void
uart_sendall(uint8_t *data, uint8_t num)
{
	for (uint8_t c = 0; c < num; c++)
		uart_put(data[c]);
}


size_t
uart_vsprintf(char *buf, const char *fmt, va_list ap)
{
	const char *p = fmt;
	char fc;
	char chunk[17]; //enough to fit "ffffffffffffffff"
	uint64_t u64;
	uint32_t u32;
	uint16_t u16;
	uint8_t u8;
	size_t clen;
	uint8_t n;
	char *obuf = buf;
	while ((fc = *p++)) {
		if (fc == '%') {
			switch ((fc = *p++)) {
			case '%':
				chunk[0] = '%';
				clen = 1;
				break;
			case '1':
				u8 = (uint8_t)va_arg(ap, int);
				u8tostr(chunk, u8);
				clen = 2;
				break;
			case '2':
				u16 = va_arg(ap, uint16_t);
				u8tostr(chunk, u16 >> 8);
				u8tostr(chunk+2, (uint8_t)u16);
				clen = 4;
				break;
			case '3':
			case '4':
				u32 = va_arg(ap, uint32_t);
				n = fc - '0';
				for (uint8_t i = 0; i < n; i++) {
					u8tostr(&chunk[2*(n-i-1)], (uint8_t)(u32));
					u32 >>= 8;
				}
				clen = 2*n;
				break;
			case '5':
			case '6':
			case '7':
			case '8':
				u64 = va_arg(ap, uint64_t);
				n = fc - '0';
				for (uint8_t i = 0; i < n; i++) {
					u8tostr(&chunk[2*(n-i-1)], (uint8_t)(u64));
					u64 >>= 8;
				}
				clen = 2*n;
				break;
			default:
				chunk[0] = '?';
				chunk[1] = '%';
				chunk[2] = fc;
				chunk[3] = '?';
				clen = 4;
				break;
			}

			for (size_t i = 0; i < clen; i++)
				*buf++ = chunk[i];
		} else
			*buf++ = fc;
	}
	*buf = '\0';
	return buf - obuf;
}

void
uart_printf(const char *fmt, ...)
{
	static char buf[64];
	va_list l;
	va_start(l, fmt);
	size_t r = uart_vsprintf(buf, fmt, l);
	va_end(l);
	if ((size_t)r > sizeof buf)
		r = sizeof buf;
	uart_sendall((uint8_t*)buf, (uint8_t)r);
}

bool
uart_hasline(void)
{
	uint8_t cnt = rx_count; //cache volatile
	uint8_t ind = rx_cons_ind; //cache volatile

	while (cnt--)
		if (rxbuf[ind++ & UART_RXBUF_MASK] == '\r')
			return true;

	return false;
}

void
uart_getline(char *dest, size_t destsz)
{
	for (;;) {
		char c = uart_get();

		if (destsz) {
			*dest++ = c;
			destsz--;
		}

		if (c == '\r') {
			dest[-1] = 0;
			break;
		}
	}
}

void
uart_flush(void)
{
	while (tx_count);
}

void
uart_puts(const char *line)
{
	for (char c = *line; c; c = *++line)
		uart_put(c);
	uart_put('\r');
	uart_put('\n');
}

static void
u8tostr(char *buf, uint8_t n)
{
	*buf++ = hexdig((n&0xf0u) >> 4);
	*buf = hexdig(n&0xfu);
}

static int
hexdig(uint8_t n)
{
	if (n <= 9)
		return n+'0';
	else
		return (n-10)+'a';
}
