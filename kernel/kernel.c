#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/tty.h"

void kernel_main(void)
{
	term_init();
	term_writestring("Hello world!\n");
	term_move_cursor(0);
	return;

}
