#ifndef _KHEAP_H
#define _KHEAP_H
#include <stddef.h>
#include <stdint.h>

#define IS_ALIGNED(x, size) ((x) % (size) == 0)

#define ROUND_DOWN(x, size) ((x) - ((x) % (size)))

#define ROUND_UP(x, size) IS_ALIGNED((x), (size)) ? \
		(x) : ((x) + ((size) - ((x) % (size)))) 

#define ALIGN_DOWN(x, size) ROUND_DOWN((x) + (size) - 1, (size))

#define ALIGN_UP(x, size) ROUND_UP((x), (size))

#define PAGE_SIZE 4096
#define VIRTUAL_BASE_ADDR 0xC0000000

uint32_t early_kmalloc(size_t size);
uint32_t early_kmalloc_pages(int num_pages);
#endif
