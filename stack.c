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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "node.h"
#include "stack.h"
#include "util.h"


/*
 * This stack code originally operated on heap-allocated structures,
 * but those proved to be problematic with the limited memory
 * available. A few performance tweaks were tested out, but a single
 * fixed-size array turned out to be the most effective solution here.
 */
uint16_t	stack[STACK_SIZE];
uint8_t		sp = 0; /* The stack pointer. */


/*
 * stack_new initialises the stack. This wipes the stack's memory
 * and resets the stack pointer to zero.
 */
void
stack_new()
{
	memset(stack, 0, STACK_SIZE*(sizeof(uint16_t)));
	sp = 0;
}


/*
 * stack_push pushes a new value onto the stack.
 */
void
stack_push(uint16_t val)
{
	if (sp == STACK_SIZE) {
		STACK_OVERFLOW;
	}

	stack[sp] = val;
	sp++;
}


/*
 * stack_pop remove the first value from the stack and returns it.
 */
uint16_t
stack_pop()
{
	if (0 == sp)
		STACK_UNDERFLOW;

	sp--;
	return stack[sp];
}


/*
 * stack_peek returns the top value from the stack.
 */
uint16_t
stack_peek()
{
	if (0 == sp)
		STACK_UNDERFLOW;

	return stack[sp-1];
}


/*
 * stack_empty returns non-zero if the stack is empty, and zero if
 * the stack is empty.
 */
int
stack_empty()
{
	return 0 == sp;
}


#ifdef DEBUG
/*
 * stack_dump prints the stack out.
 */
void
stack_dump()
{
	char	buf[10];
	uint8_t	i = sp-1;

	while (i < STACK_SIZE) {
		snprintf(buf, 10, "%02x %04x\r\n", i, stack[i]);
		serial_write(buf, 9);
		i--;
	}
}
#endif
