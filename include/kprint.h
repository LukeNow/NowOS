#ifndef _KERN_PRINT_H
#define _KERN_PRINT_H
#include <stddef.h>
#include <stdint.h>

enum LOG_LEVEL {
	INFO = 0,
	WARN = 1,
	ERROR = 2
	
};

void kprint(enum LOG_LEVEL level, char* format, ...);
//void kprint(enum LOG_LEVEL level, char* format, ...);
#endif
