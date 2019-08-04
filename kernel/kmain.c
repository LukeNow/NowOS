#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/tty.h"
#include "../include/kprint.h"
#include "../include/machine.h"
#include "../include/processor.h"
#include "../include/io.h"
#include "../include/multiboot.h"
#include "../include/kheap.h"
#include "../include/string.h"
#include "../include/mm.h"
#include "../include/kdef.h"
#include "../include/paging.h"
#include "../include/sorted_array_list.h"
#include "../include/test.h"

#define SECTOR_SIZE 512
#define ARRAY_EXPAND_SIZE (SECTOR_SIZE / sizeof(void*))

void kmain(multiboot_info_t* mbt, unsigned int magic)
{
	
	extern void heap_top();
	extern void heap_bottom();
	extern void stack_top();
	
	extern unsigned int pages_mapped;
	
	uint32_t mem_limit;
	int i = 0;
	int addr = 0;
	
	multiboot_memory_map_t* mmap = mbt->mmap_addr;
	
	kprint(INFO, "MEM SIZE: %x\n", mbt->mem_upper * 1024);
	mem_limit = mbt->mem_upper * 1024;
	init_early_kheap();
	init_mem_manager(mem_limit); //Physical page alloc
	init_kern_paging(); //Map kernel pages to table
	

	test();

	init_kheap(); //Regular kheap
	
	PANIC("KMAIN STOP");
	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
