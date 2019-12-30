#include <kdef.h>
#include <kprint.h>
#include <string.h>
#include <mm.h>
#include <byte_index_list.h>

BYTE_INDEX_LIST_INIT(phys_mem_tbl);

uint32_t phys_page_num;
extern int early_heap_ptr;

static inline uint32_t index_to_addr(unsigned int index)
{	
	return (index * PAGE_SIZE);
}

void init_mem_manager(uint32_t phys_size)
{
	uint32_t tbl_size;
	
	phys_size = ALIGN_DOWN(phys_size, PAGE_SIZE);
	phys_page_num = phys_size / PAGE_SIZE;
	tbl_size = (phys_page_num / 8);
	init_byte_index_list((char *)early_heap_ptr, tbl_size, &phys_mem_tbl);
	
	memset(phys_mem_tbl.array, 0, tbl_size);
	early_heap_ptr += tbl_size;

	kprint(INFO, "MM: Mem table start addr: %x\n", phys_mem_tbl.array);
	kprint(INFO, "MM: Mem table size: %d bytes, mem phys_page_num: %x\n",
	       tbl_size, phys_page_num);
}

void mem_set_tbl(uint32_t phys_addr) 
{
	uint32_t index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr);
	byte_index_set_tbl(index, &phys_mem_tbl);
}

void mem_free_tbl(uint32_t phys_addr)
{
	uint32_t index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr);
	byte_index_release_tbl(index, &phys_mem_tbl);
}

uint32_t mem_check_tbl(uint32_t phys_addr)
{
	uint32_t index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr);
	return byte_index_check_tbl(index, &phys_mem_tbl);
}

uint32_t mem_get_free_page_phys()
{
	uint32_t index = byte_index_get_free_slot(&phys_mem_tbl);
	// TODO do we put null addresses as invalid addresses?
	if (index == -1)
		return -1;
	
	return (index * PAGE_SIZE);
}

void mem_release_page_phys(uint32_t phys_addr)
{
	mem_free_tbl(phys_addr);
}

uint32_t mem_get_free_page_virt()
{
	return mem_get_free_page_phys() + KERN_VIRTUAL_ADDR;
}

void mem_release_page_virt(uint32_t virt_addr)
{
	mem_free_tbl(virt_addr - KERN_VIRTUAL_ADDR);
}
