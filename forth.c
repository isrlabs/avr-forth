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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>

#include "stack.h"
#include "util.h"


/*
 * The following definitions contain keywords.
 */
const char	DUP[] = "DUP";


const char	INVALID_IMMEDIATE[] = "EIMM\r\n";
const char	BADMEM[] = "BADMEM\r\n";


/*
 * forth_init carries out all the initialisation needed for the Forth
 * interpreter. The serial console should be available by the time this
 * is called.
 */
void
forth_init(void)
{
	stack_new();
}


/*
 * forth_process_himm attempts to extract an unsigned 16-bit integer
 * from a word. The word is expected to be in hexadecimal format.
 */
static void
forth_process_himm(char *word, uint8_t l)
{
	uint8_t		i, j;
	uint16_t	digit;
	uint16_t	n = 0;

	if (l > 4) {
		serial_write((char *)INVALID_IMMEDIATE, 6);
		return;
	}

	for (i = 0; i < l; i++) {
		digit = (uint16_t)(word[i] & 0xff);
		if ((digit > 47) && (digit < 58)) {
			digit -= 48;
		} else if ((digit > 64) && (digit < 91)) {
			digit -= 55;
		} else {
			serial_write((char *)INVALID_IMMEDIATE, 6);
			return;
		}

		for (j = 1; j < (l - i); j++) {
			digit <<= 4;
		}

		n += digit;
	}

	stack_push(n);
}


/*
 * forth_process_dimm attempts to extract an unsigned 16-bit integer
 * from a word. The word is expected to be in decimal format.
 */
static void
forth_process_dimm(char *word, uint8_t l)
{
	uint8_t		i, j;
	uint16_t	digit;
	uint16_t	n = 0;

	if (l > 5) {
		serial_write((char *)INVALID_IMMEDIATE, 6);
		return;
	}

	for (i = 0; i < l; i++) {
		digit = (uint16_t)(word[i] & 0xff);
		if ((digit > 47) && (digit < 58)) {
			digit -= 48;
		} else {
			serial_write((char *)INVALID_IMMEDIATE, 6);
			return;
		}

		for (j = 1; j < (l - i); j++) {
			digit += (digit + (digit << 3));
		}

		n += digit;
	}

	stack_push(n);
}


/*
 * prn peeks at the stack and writes the topmost value to the serial
 * console in hexadecimal form.
 */
static void
prn(void)
{
	char		buf[7];
	uint16_t	n = 0;
	uint8_t		l = 0;

	n = stack_peek();

	snprintf(buf, 7, "%X\r\n", n);
	l = strnlen(buf, 6);

	serial_write(buf, l);
}


/*
 * write_mem sets the port specified by loc to val.
 */
static void
write_mem(uint8_t loc, uint8_t val)
{
	switch (loc) {
	case 0:
		/* DDRA = val; */
		break;
	case 1:
		/* PORTA = val; */
		break;
	case 2:
		DDRB = val;
		break;
	case 3:
		PORTB = val;
		break;
	case 4:
		DDRC = val;
		break;
	case 5:
		PORTC = val;
		break;
	case 6:
		DDRD = val;
		break;
	case 7:
		PORTD = val;
		break;
	default:
		serial_write((char *)BADMEM, 8);
	}
}


/*
 * toggle_mem toggles the port specified by loc to val.
 */
static void
toggle_mem(uint8_t loc, uint8_t val)
{
	switch (loc) {
	case 0:
		/* DDRA ^= val; */
		break;
	case 1:
		/* PORTA ^= val; */
		break;
	case 2:
		DDRB ^= val;
		break;
	case 3:
		PORTB ^= val;
		break;
	case 4:
		DDRC ^= val;
		break;
	case 5:
		PORTC ^= val;
		break;
	case 6:
		DDRD ^= val;
		break;
	case 7:
		PORTD ^= val;
		break;
	default:
		serial_write((char *)BADMEM, 8);
	}
}


/*
 * or_mem bitwise ORs val into the port specified by loc.
 */
static void
or_mem(uint8_t loc, uint8_t val)
{
	switch (loc) {
	case 0:
		/* DDRA |= val; */
		break;
	case 1:
		/* PORTA |= val; */
		break;
	case 2:
		DDRB |= val;
		break;
	case 3:
		PORTB |= val;
		break;
	case 4:
		DDRC |= val;
		break;
	case 5:
		PORTC |= val;
		break;
	case 6:
		DDRD |= val;
		break;
	case 7:
		PORTD |= val;
		break;
	default:
		serial_write((char *)BADMEM, 8);
	}
}


/*
 * and_mem bitwise ANDs val with the port specified by loc.
 */
static void
and_mem(uint8_t loc, uint8_t val)
{
	switch (loc) {
	case 0:
		/* DDRA |== val; */
		break;
	case 1:
		/* PORTA |== val; */
		break;
	case 2:
		DDRB &= val;
		break;
	case 3:
		PORTB &= val;
		break;
	case 4:
		DDRC &= val;
		break;
	case 5:
		PORTC &= val;
		break;
	case 6:
		DDRD &= val;
		break;
	case 7:
		PORTD &= val;
		break;
	default:
		serial_write((char *)BADMEM, 8);
	}
}


/*
 * read_mem returns the current value for the port specified by loc.
 */
static void
read_mem(uint16_t loc)
{
	switch (loc) {
	case 0:
		/* loc = DDRA & 0xff */;
		break;
	case 1:
		/* loc = PORTA & 0xff */;
		break;
	case 2:
		loc = DDRB & 0xff;
		break;
	case 3:
		loc = PORTB & 0xff;
		break;
	case 4:
		loc = DDRC & 0xff;
		break;
	case 5:
		loc = PORTC & 0xff;
		break;
	case 6:
		loc = DDRD & 0xff;
		break;
	case 7:
		loc = PORTD & 0xff;
		break;
	default:
		serial_write((char *)BADMEM, 8);
		return;
	}

	stack_push(loc);
}


/*
 * process words of length 1.
 */
static void
forth_process_word1(char *word)
{
	uint16_t	x;

	if ('+' == word[0]) {
		x = stack_pop();
		x += stack_pop();
		stack_push(x);
		return;
	}

	if ('-' == word[0]) {
		x = stack_pop();
		x = stack_pop() - x;
		stack_push(x);
		return;
	}

	if ('~' == word[0]) {
		x = stack_pop();
		stack_push(~x);
		return;
	}

	if ('R' == word[0]) {
		x = stack_pop();
		read_mem(x);
		return;
	}

	if ('W' == word[0]) {
		x = stack_pop();
		write_mem((uint8_t)(stack_pop() & 0xff), (uint8_t)(x & 0xff));
		return;
	}

	if ('!' == word[0]) {
		x = stack_pop();
		toggle_mem((uint8_t)(stack_pop() & 0xff), (uint8_t)(x & 0xff));
		return;
	}

	if ('|' == word[0]) {
		x = stack_pop();
		or_mem((uint8_t)(stack_pop() & 0xff), (uint8_t)(x & 0xff));
		return;
	}

	if ('&' == word[0]) {
		x = stack_pop();
		and_mem((uint8_t)(stack_pop() & 0xff), (uint8_t)(x & 0xff));
		return;
	}

	if ('.' == word[0]) {
		prn();
		return;
	}

	serial_write("UNK\r\n", 5);
}


/*
 * process words of length 2.
 */
static void
forth_process_word2(char *word)
{
	uint16_t	x;

	if (('<' == word[0]) && ('<' == word[1])) {
		x = stack_pop();
		x = stack_pop() << x;
		stack_push(x);
		return;
	}

	if (('>' == word[0]) && ('>' == word[1])) {
		x = stack_pop();
		x = stack_pop() >> x;
		stack_push(x);
		return;
	}

	serial_write("UNK\r\n", 5);
}


/*
 * process words of length 3
 */
static void
forth_process_word3(char *word)
{
	if (0 == strncmp(DUP, word, 3)) {
		stack_push(stack_peek());
		return;
	}

	serial_write("UNK\r\n", 5);
}


/*
 * forth_process_word takes a word as input and its length and
 * determines what is to be done with the word.
 */
static void
forth_process_word(char *word, uint8_t l)
{
	if ('$' == word[0]) {
		forth_process_himm(word+1, l-1);
		return;
	}

	if ('#' == word[0]) {
		forth_process_dimm(word+1, l-1);
		return;
	}

	switch (l) {
	case 1:
		forth_process_word1(word);
		break;
	case 2:
		forth_process_word2(word);
		break;
	case 3:
		forth_process_word3(word);
		break;
	default:
		serial_write("UNK\r\n", 5);
	}
}


/*
 * forth_read_word reads a new word from the serial console.
 */
static void
forth_read_word(void)
{
	char	buf[8];
	uint8_t		l = 0;
	char		c = 0;

	memset(buf, 0, 8);
	/*
	 * words are delineated by spaces and may be (in this implementation)
	 * at most 8 characters.
	 */
	while (l < 8) {
		c = serial_read();
		if ((0x0a == c) || (0x0d == c)) {
			serial_write(" ", 1);
			break;
		}
		serial_write(&c, 1);

		if (0x20 == c) {
			break;
		}

		if ((0x08 == c) || (0x7f == c)) {
			if (l > 0) {
				l--;
			}
			continue;
		}

		if ((c > 0x60) && (c < 0x7b)) {
			c ^= 0x20;
		}

		buf[l] = c;
		l++;
	}

	serial_write("OK\r\n", 4);
	forth_process_word(buf, l);
}


/*
 * forth_repl is a loop that continually reads and processes words.
 */
void
forth_repl(void)
{
	serial_write("READY\r\n", 7);

	while (1) {
		forth_read_word();
	}
}

