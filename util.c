/*
 * Copyright (c) 2014 Kyle Isom <kyle@tyrfingr.is>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <avr/io.h>
#ifdef ABORT_LED_PIN
#include <util/delay.h>
#else
#include <avr/wdt.h>
#endif
#include <util/setbaud.h>

#include "util.h"


/*
 * serial_init sets up the serial port for 9600 baud communications.
 * BAUD must be defined prior to calling this.
 */
void
serial_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A &= ~(1 << U2X0);

	UCSR0B = ((1 << TXEN0)|(1 << RXEN0));
	UCSR0C = ((1 << UCSZ01)|(1 << UCSZ00));
}


/*
 * serial_write writes a sequence of characters to the serial port.
 */
void
serial_write(char *s, uint8_t l)
{
	uint8_t	i;

	for (i = 0; i != l; i++) {
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = s[i];
	}
}


/*
 * serial_read blocks until a character is available from the UART,
 * returning that character when it is received.
 */
char
serial_read(void)
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}


/*
 * avr_abort writes the message to the serial console, then triggers
 * a watchdog reset after four seconds.
 */
void
avr_abort(char *m, uint8_t l)
{
	serial_write(m, l);

#ifdef ABORT_LED_PIN
	while (1) {
		ABORT_LED_PORT ^= (1 << LED_PIN);
		_delay_ms(1000);
	}
#else
	wdt_enable(WDTO_4S);
	while (1) ;
#endif
}

