#ifndef UART_INTR_H
#define UART_INTR_H 1

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <avr/interrupt.h>

/* uart_init - Initialize usart and set baud rate
 *   `ubrr` - Value of the UBRR register as described in AVR's datasheets
 */
void uart_init(uint16_t ubrr);

/* uart_recv - Retrieve received data to application
 *   `data` - Pointer to a buffer for the result to be stored in - must be at
 *              least `max` bytes of size
 *   `max`  - Maximum number of bytes to retrieve
 * Returns the number of bytes actually retrieved and stored in `data`. This
 *   value may well be less than `max`, down to and including zero, if there
 *   is less/no data available in the internal receive buffer. In other words,
 *   we do not block under no circumstances.
 */
uint8_t uart_recv(uint8_t *data, uint8_t max);

/* uart_send - Buffer data for asynchronous transmission
 *   `data` - Pointer to a buffer holding `max` bytes of data to be transmitted
 *   `max`  - Maximum number of bytes to buffer for transmission
 * Returns the number of bytes actually buffered. This value may well be less
 *   than `max`, down to and including zero, if there is not enough space avail-
 *   able in the internal transmit buffer. In other words, we do not block under
 *   no circumstances.
 */
uint8_t uart_send(uint8_t *data, uint8_t max);

/* uart_avail - Determine number of bytes available to read
 * Returns the number of bytes which can currently be read in one go by
 *   `uart_recv()`. A subsequent call to uart_recv() is guaranteed to deliver
 *   at least this many bytes, if requested.
 */
uint8_t uart_avail(void);

/* uart_put - Convenience function for single-byte blocking data transmission
 *   `data` - Data byte to be transmitted
 * This function still transmits asynchronously, however, it ensures that the
 *   given data byte is actually buffered. I.e. we block until there is space
 *   available in the internal transmit buffer.
 */
void uart_put(uint8_t data);

/* uart_get - Convenience function for single-byte blocking data reception
 * Returns a byte read from the uart
 * This function blocks until there is at least one data byte available in
 *   the internal rx buffer
 */
uint8_t uart_get(void);

void uart_sendall(uint8_t *data, uint8_t num);

void uart_printf(const char *fmt, ...);
bool uart_hasline(void);
void uart_getline(char *dest, size_t destsz);
void uart_flush(void);
void uart_puts(const char *line);

/* ISR USART_RXC - Called when incoming data arrives */
ISR(USART_RXC_vect);

/* ISR USART_UDRE - Called as long as the data register is empty */
ISR(USART_UDRE_vect);

#endif /* UART_INTR_H */
