#ifndef _KERN_PRINT_H
#define _KERN_PRINT_H
#include <stddef.h>
#include "../include/machine.h"

enum LOG_LEVEL {
	INFO = 0,
	WARN = 1,
	ERROR = 2
	
};

#define PANIC(x) kprint(ERROR, "%s:%d %s\n", __FILE__,__LINE__, x); \
		_panic();


void kprint(enum LOG_LEVEL level, char* format, ...);


#endif
