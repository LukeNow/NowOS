#ifndef _MM_H
#define _MM_H
#include <stdint.h>

void mem_set_tbl(uint32_t phys_addr);
void mem_free_tbl(uint32_t phys_addr);
uint32_t mem_check_tbl(uint32_t phys_addr);
uint32_t mem_get_free_page_phys();
uint32_t mem_get_free_page_virt();
void mem_release_page_phys(uint32_t phys_addr);
void mem_release_page_virt(uint32_t virt_addr);
void init_mem_manager(uint32_t phys_size);
#endif
