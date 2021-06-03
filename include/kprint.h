#ifndef _KERN_PRINT_H
#define _KERN_PRINT_H
#include <stddef.h>
#include "../include/machine.h"

#define KLOG_MAX_LEN 128;
#define KLOG_MAX_PARAM 4;

typedef enum LOG_LEVEL {
	INFO = 0,
	WARN = 1,
	ERROR = 2
} LOG_LEVEL;

typedef struct klog_msg {
	LOG_LEVEL level;
	char * str;
	int param1;
	int param2;
	int param3;
	int param4;
} klog_msg;

#define PANIC(x) do {kprint(ERROR, "%s:%d %s\n", __FILE__,__LINE__, (x)); \
		_panic(); } while(0)

#define WARN(x) kprint(WARN, "%s:%d %s\n", __FILE__, __LINE__, (x))

#define VERIFY_UNREACHED() do {kprint(ERROR, "%s:%d Unreached location entered\n", __FILE__,__LINE__); \
		_panic(); __builtin_unreachable(); } while(0) 

#define ASSERT(x) \
	do {if (!(x)) PANIC("Assertion is false"); } while (0)

void kprint(LOG_LEVEL level, char* format, ...);


#endif
