#ifndef _PAGING_H
#define _PAGING_H

#include <stdint.h>

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR PAGES_PER_TABLE

typedef uint32_t page_tbl_entry_t;

typedef struct page_tbl {
	page_tbl_entry_t page_tbl[PAGES_PER_TABLE];
}page_tbl_t;

typedef uint32_t page_dir_entry_t;

typedef struct page_dir {
	page_dir_entry_t page_dir[PAGES_PER_DIR];
}page_dir_t;

void set_page_tbl_entry(page_tbl_entry_t *entry, uint32_t phys_addr, 
			int read_write, int privledge, int write_through, 
			int cache_disabled);

void set_page_dir_entry(page_dir_entry_t *entry, uint32_t phys_addr,
			 int read_write, int privledge, int write_through,
			 int cache_disabled, int page_size);


void init_kern_paging();
#endif
