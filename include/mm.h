#ifndef _MM_H
#define _MM_H
#include <stdint.h>

void mem_set_tbl(uint32_t phys_addr);
void mem_free_tbl(uint32_t phys_addr);
uint32_t mem_check_tbl(uint32_t phys_addr);
uint32_t mem_get_free_page();
void init_mem_manager(uint32_t phys_size);
void mem_release_page(uint32_t phys_addr);

#endif
