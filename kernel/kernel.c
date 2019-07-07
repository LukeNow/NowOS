#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/tty.h"
#include "../include/kprint.h"

void kernel_main(void)
{
	term_init();
	kprint("Hello world!\n");
	kerror("There would be an error here\n");
	return;
}
