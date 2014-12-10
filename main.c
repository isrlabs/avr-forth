#include <avr/io.h>
#include <avr/wdt.h>


#include "util.h"
#include "forth.h"


int	main(void) __attribute__((noreturn));


/*
 * init performs the necessary initialisation; the watchdog is reset
 * and cleared, in the event that the bootup is occurring after an
 * avr_abort. The serial console is also set up, which requires that
 * BAUD is a valid definition.
 */
static void
init(void)
{
	wdt_reset();
	MCUSR = 0;
	wdt_disable();
	serial_init();
}


/*
 * main indicates that the microcontroller has started up and launches
 * the Forth REPL.
 */
int
main(void)
{
	init();

	serial_write("OK\r\n", 4);

	forth_repl();

	while (1) ;
}

