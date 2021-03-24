#ifndef _KERN_PRINT_H
#define _KERN_PRINT_H
#include <stddef.h>
#include "../include/machine.h"

enum LOG_LEVEL {
	INFO = 0,
	WARN = 1,
	ERROR = 2
};

#define PANIC(x) do {kprint(ERROR, "%s:%d %s\n", __FILE__,__LINE__, (x)); \
		_panic(); } while(0)

#define WARN(x) kprint(WARN, "%s:%d %s\n", __FILE__, __LINE__, (x))

#define VERIFY_UNREACHED() do {kprint(ERROR, "%s:%d Unreached location entered\n", __FILE__,__LINE__); \
		_panic(); __builtin_unreachable(); } while(0) 

#define ASSERT(x) \
	do {if (!(x)) PANIC("Assertion is false"); } while (0)

void kprint(enum LOG_LEVEL level, char* format, ...);


#endif
