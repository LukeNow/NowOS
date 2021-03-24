#include <kdef.h>
#include <kprint.h>
#include <string.h>
#include <mm.h>
#include <byte_index_list.h>

BYTE_INDEX_LIST_INIT(phys_mem_tbl);

uint32_t phys_page_num;
extern uint32_t early_heap_ptr;

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

bool mem_check_tbl(uint32_t phys_addr)
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

void * mem_alloc_region(uint32_t phys_addr, size_t size, flags_t flags)
{
	size = ALIGN_UP(size, PAGE_SIZE);
	//phys_addr = ALIGN_DOWN(phys_addr, PAGE_SIZE);
	spin_lock(&phys_mem_tbl.lock);

	if (!(flags & MEM_ALREADYEXISTS) & mem_check_tbl(phys_addr)) {
		spin_unlock(&phys_mem_tbl.lock);
		return NULL;
	}

	flags_t map_flags = 0;
	if (flags & MEM_READWRITE)
		map_flags |= PAGE_READWRITE;
	if (flags & MEM_WRITETHROUGH)
		map_flags |= PAGE_WRITETHROUGH;
	if (flags & MEM_CACHEDISABLE)
		map_flags |= PAGE_CACHEDISABLE;

	uint32_t virt_addr = phys_addr;
	if (!(flags & MEM_IDENTITYREGION)) 
		virt_addr += KERN_VIRTUAL_ADDR;
	
	uint32_t ret_addr = virt_addr;

	int num_pages = size / PAGE_SIZE;
	for (int i = 0; i < num_pages; i++) {
		map_kern_page_flags(virt_addr, phys_addr, map_flags);
		mem_set_tbl(phys_addr);

		//kprint(INFO, "IN ALLOC %d\n", i);
		virt_addr += PAGE_SIZE;
		phys_addr += PAGE_SIZE;
	}

	//kprint(INFO, "OUT ALLOC \n");
	flush_tlb();

//	kprint(INFO, "FLUSHED\n");
	spin_unlock(&phys_mem_tbl.lock);

	return ret_addr;
}
