#ifndef _PAGING_H
#define _PAGING_H
#include <stdint.h>

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR PAGES_PER_TABLE

#define PAGE_READWRITE_BIT 0
#define PAGE_ALLACCESS_BIT 1
#define PAGE_WRITETHROUGH_BIT 2
#define PAGE_CACHEDISABLE_BIT 3
#define PAGE_READWRITE (1 << PAGE_READWRITE_BIT)
#define PAGE_ALLACCESS (1 << PAGE_ALLACCESS_BIT)
#define PAGE_WRITETHROUGH (1 << PAGE_WRITETHROUGH_BIT)
#define PAGE_CACHEDISABLE (1 << PAGE_CACHEDISABLE_BIT)

typedef uint32_t page_tbl_entry_t;

typedef struct page_tbl {
	page_tbl_entry_t page_tbl[PAGES_PER_TABLE];
}page_tbl_t;

typedef uint32_t page_dir_entry_t;

typedef struct page_dir {
	page_dir_entry_t page_dir[PAGES_PER_DIR];
}page_dir_t;

extern void paging_init(page_dir_t *dir);
void map_kern_page(uint32_t virtual_addr, uint32_t phys_addr);
void map_kern_page_flags(uint32_t virtual_addr, uint32_t phys_addr, 
					     flags_t flags);
page_tbl_entry_t * get_tbl_entry(uint32_t virt_addr);						 
void set_kern_dir();
void print_page_tbl_entry(uint32_t virt_addr);
void flush_tlb();
void init_kern_paging();
#endif
