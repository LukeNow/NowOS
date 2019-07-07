#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/tty.h"

void kprint(const char* string)
{
	term_setcolor(VGA_COLOR_WHITE);
	term_writestring(string);
}

void kerror(const char* string)
{
	term_setcolor(VGA_COLOR_RED);
	term_writestring(string);
}
