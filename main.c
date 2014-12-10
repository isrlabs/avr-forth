#include <avr/io.h>
#include <avr/wdt.h>


#include "util.h"
#include "forth.h"


int	main(void) __attribute__((noreturn));


static void
init(void)
{
	wdt_reset();
	MCUSR = 0;
	wdt_disable();
	serial_init();
}


int
main(void)
{
	init();

	serial_write("OK\r\n", 4);

	forth_repl();

	while (1) ;
}
