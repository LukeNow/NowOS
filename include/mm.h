#ifndef _MM_H
#define _MM_H
#include <stdint.h>

void mem_manage_init(uint32_t phys_size);
uint32_t get_free_page_mem();
void release_page_mem(uint32_t phys_addr);
#endif
