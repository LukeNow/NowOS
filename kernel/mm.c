#include <stddef.h>
#include <stdint.h>
#include "../include/kprint.h"
#include "../include/kdef.h"
#include "../include/mm.h"

uint32_t phys_page_num;
uint8_t *mem_tbl;
uint32_t tbl_size;
uint8_t* free_ptr;

extern void mem_tbl_bottom();

void mem_manage_init(uint32_t phys_size)
{
	mem_tbl = (uint8_t *)mem_tbl_bottom;
	phys_page_num = phys_size / PAGE_SIZE;
	tbl_size = (phys_page_num / 8);
	free_ptr = mem_tbl;

	kprint(INFO, "MEM INIT. mem_tbl: %x tbl_size: %d\n", 
			mem_tbl, tbl_size);
}

void set_tbl(uint32_t phys_addr) 
{
	unsigned int tbl_index;
	unsigned int byte_index;
	uint8_t temp_entry;

	tbl_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) / 8;
	byte_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) % 8;
	
	temp_entry = *(mem_tbl + tbl_index);
	*(mem_tbl + tbl_index) = (temp_entry | (1 << byte_index));
}

void free_tbl(uint32_t phys_addr)
{
	unsigned int tbl_index;
	unsigned int byte_index;
	uint8_t temp_entry;

	tbl_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) / 8;
	byte_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) % 8;
	
	if (free_ptr > mem_tbl + tbl_index)
		free_ptr = (mem_tbl + tbl_index);

	temp_entry = *(mem_tbl + tbl_index);
	*(mem_tbl + tbl_index) = (temp_entry & ~(1 << byte_index));
}

unsigned int check_tbl(uint32_t phys_addr)
{
	unsigned int tbl_index;
	unsigned int byte_index;
	unsigned int ret;
	uint8_t temp_entry;
	
	tbl_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) / 8;
	byte_index = PHYS_ADDR_TO_PAGE_INDEX(phys_addr) % 8;
	
	temp_entry = *(mem_tbl + tbl_index);
	ret = (temp_entry & (1 << byte_index));
	ret = ret >> byte_index;
	return ret;
}

static inline uint32_t index_to_addr(unsigned int tbl_index, 
			    unsigned int byte_index)
{
	return ((tbl_index * 8) + byte_index) * PAGE_SIZE;
}

void update_free_ptr()
{
	uint8_t temp_entry = *free_ptr;
	
	while (free_ptr < (mem_tbl + tbl_size)) {
		if (temp_entry != 0xFFFF){
			return;
		}
		*++free_ptr = temp_entry;
	}
	PANIC("WE COULDNT FIND ANY MORE FREE SPACE");
}

uint32_t get_free_page_mem()
{
	uint8_t temp_entry;
	uint32_t tbl_index;
	uint32_t ret_addr;
	
	uint32_t byte_index = -1; //set so we know if we didnt change
	unsigned int i = 0;

	tbl_index =(uint32_t)(free_ptr - mem_tbl);
	if (mem_tbl[tbl_index] == 0xFFFF) {
		update_free_ptr();
		WARN("Free pointer updated?");
	}
	
	tbl_index =(uint32_t)(free_ptr - mem_tbl);
	if (mem_tbl[tbl_index] == 0xFFFF) {
		PANIC("MEM_TBL NOT FREE");
	}
	
	temp_entry = mem_tbl[tbl_index];
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
	set_tbl(ret_addr);
	return ret_addr;
}

void release_page_mem(uint32_t phys_addr)
{
	free_tbl(phys_addr);
}
