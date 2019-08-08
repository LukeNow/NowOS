#include <stddef.h>
#include <stdint.h>
#include "../include/kprint.h"
#include "../include/kdef.h"
#include "../include/string.h"
#include "../include/mm.h"


uint8_t *free_ptr;
uint8_t *phys_mem_tbl;
uint32_t phys_page_num;
uint32_t tbl_size;

extern void heap_bottom();
extern int heap_ptr;

unsigned int pages_mapped = 0;

static inline uint32_t index_to_addr(unsigned int tbl_index, 
			    unsigned int byte_index)
{
	return ((tbl_index * 8) + byte_index) * PAGE_SIZE;
}

static void update_free_ptr()
{
	uint8_t temp_entry = *free_ptr;
	
	while (free_ptr < (phys_mem_tbl + tbl_size)) {
		if (temp_entry != 0xFF){
			return;
		}
		temp_entry = *++free_ptr;
	}

	PANIC("WE COULDNT FIND ANY MORE FREE SPACE");
}

void init_mem_manager(uint32_t phys_size)
{
	phys_mem_tbl = (uint8_t *)heap_ptr;
	phys_size = ALIGN_DOWN(phys_size, PAGE_SIZE);
	phys_page_num = phys_size / PAGE_SIZE;
	tbl_size = (phys_page_num / 8);
	free_ptr = phys_mem_tbl;
	memset(phys_mem_tbl, 0, tbl_size);
	heap_ptr += tbl_size;



	kprint(INFO, "MM: Mem table start addr: %x\n", phys_mem_tbl);
	kprint(INFO, "MM: Mem table size: %d bytes, mem phys_page_num: %x\n",
	       tbl_size, phys_page_num);
}

void mem_set_tbl(uint32_t phys_addr) 
{
	unsigned int tbl_index;
	unsigned int byte_index;
	uint8_t temp_entry;

	tbl_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) / 8;
	byte_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) % 8;
	
	temp_entry = *(phys_mem_tbl + tbl_index);
	
	if ((temp_entry & (1 << byte_index)) == 0){
		pages_mapped++;
	}
	
	*(phys_mem_tbl + tbl_index) = (temp_entry | (1 << byte_index));
}

void mem_free_tbl(uint32_t phys_addr)
{
	unsigned int tbl_index;
	unsigned int byte_index;
	uint8_t temp_entry;

	tbl_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) / 8;
	byte_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) % 8;
	
	if (free_ptr > phys_mem_tbl + tbl_index)
		free_ptr = (phys_mem_tbl + tbl_index);

	
	temp_entry = *(phys_mem_tbl + tbl_index);
	
	if ((temp_entry & (1 << byte_index)) != 0){ //Keeping track
		pages_mapped--;
	}

	*(phys_mem_tbl + tbl_index) = (temp_entry & ~(1 << byte_index));
}

uint32_t mem_check_tbl(uint32_t phys_addr)
{
	unsigned int tbl_index;
	unsigned int byte_index;
	unsigned int ret;
	uint8_t temp_entry;
	
	tbl_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) / 8;
	byte_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) % 8;
	
	temp_entry = *(phys_mem_tbl + tbl_index);
	ret = (temp_entry & (1 << byte_index));
	ret = ret >> byte_index;
	return ret;
}

uint32_t mem_get_free_page_phys()
{
	uint8_t temp_entry;
	uint32_t tbl_index;
	uint32_t ret_addr;
	
	uint32_t byte_index = -1; //set so we know if we didnt change
	unsigned int i = 0;

	tbl_index =(uint32_t)(free_ptr - phys_mem_tbl);
	if (phys_mem_tbl[tbl_index] == 0xFF) {
		update_free_ptr();
	}
	
	tbl_index =(uint32_t)(free_ptr - phys_mem_tbl);
	if (phys_mem_tbl[tbl_index] == 0xFF) {
		PANIC("MEM_TBL NOT FREE???");
	}
	
	temp_entry = phys_mem_tbl[tbl_index];
	for (i = 0; i < 8; i++) {
		if ((temp_entry & (1 << i)) == 0) {
			byte_index = i;
			break;
		}
	}
	
	if (byte_index == -1) {
		PANIC("BYTE_INDEX NOT SET");
	}

	ret_addr = index_to_addr(tbl_index, byte_index);
	mem_set_tbl(ret_addr);
	return ret_addr;
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
