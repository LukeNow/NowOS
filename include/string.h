#ifndef _STRING_H
#define _STRING_H
#include <stddef.h>
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* __restrict dest, const void* __restrict src, size_t size);
void* memmove(void* dest, const void* src, size_t size);
void* memset(void* bufptr, int value, size_t size);
size_t strlen(const char* str);
#endif 
