#ifndef _MM_H
#define _MM_H
#include <stdint.h>
#include <paging.h>
void mem_set_tbl(uint32_t phys_addr);
void mem_free_tbl(uint32_t phys_addr);
bool mem_check_tbl(uint32_t phys_addr);
uint32_t mem_get_free_page_phys();
uint32_t mem_get_free_page_virt();
void mem_release_page_phys(uint32_t phys_addr);
void mem_release_page_virt(uint32_t virt_addr);

#define MEM_READWRITE_BIT 0
#define MEM_ALLACCESS_BIT 1
#define MEM_WRITETHROUGH_BIT 2
#define MEM_CACHEDISABLE_BIT 3
#define MEM_ALREADYEXISTS_BIT 4
#define MEM_IDENTITYREGION_BIT 5

#define MEM_READWRITE (1 << MEM_READWRITE_BIT)
#define MEM_ALLACCESS (1 << MEM_ALLACCESS_BIT) // Set if any ring can access
#define MEM_WRITETHROUGH (1 << MEM_WRITETHROUGH_BIT)
#define MEM_CACHEDISABLE (1 << MEM_CACHEDISABLE_BIT)
#define MEM_ALREADYEXISTS (1 << MEM_ALREADYEXISTS_BIT) // If it already exists, dont fail
#define MEM_IDENTITYREGION (1 << MEM_IDENTITYREGION_BIT) // Identity alloc if set, else could be at any virt address

void * mem_alloc_region(uint32_t phys_addr, size_t size, flags_t flags);

void init_mem_manager(uint32_t phys_size);
#endif
