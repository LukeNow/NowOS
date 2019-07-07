#ifndef _KERN_PRINT_H
#define _KERN_PRINT_H
#include <stddef.h>
#include <stdint.h>

void kprint(const char* string);
void kerror(const char* string);
#endif
