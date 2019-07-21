#ifndef _KHEAP_H
#define _KHEAP_H
#include <stddef.h>
#include <stdint.h>

uint32_t early_kmalloc(size_t size);
uint32_t early_kmalloc_pages(int num_pages);
#endif
