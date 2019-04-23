#ifndef _KERN_TTY_H
#define _KERN_TTY_H
#include <stddef.h>
#include <stdint.h>

void term_init(void);
void term_write(const char* data, size_t size);
void term_writestring(const char* data);
void term_putchar(char c);
void term_move_cursor(unsigned short pos);
#endif
