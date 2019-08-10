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
#include "../include/pit.h"
#include "../include/linked_list.h"
#include "../include/task.h"

extern task_control_block_t *main_task;
extern task_control_block_t *other_task;
extern task_control_block_t *current_task;
extern task_control_block_t *next_task;

void main1()
{
	for (;;) {
		kprint(INFO, "Othertask!!\n");
		disable_int();
		
		next_task = main_task;
		yield();
		enable_int();
		kprint(INFO, "Other task returned\n");
	}
}

void kmain(multiboot_info_t* mbt, unsigned int magic)
{
	
	extern void heap_top();
	extern void heap_bottom();
	extern void stack_top();
	
	uint32_t mem_limit;
	
	multiboot_memory_map_t* mmap = mbt->mmap_addr;
	
	kprint(INFO, "MEM SIZE: %x\n", mbt->mem_upper * 1024);
	mem_limit = mbt->mem_upper * 1024;
	init_early_heap();
	init_mem_manager(mem_limit); //Physical page alloc
	init_kern_paging(); //Map kernel pages to table
	
	init_kheap(); //Regular kheap
	init_page_heap();
	
	init_multitasking();
	create_task(main1, "main1");
	
	disable_int();
	for (int i = 0; i < 10; i++) {
		kprint(INFO, "MAIN! %d\n", i);
		disable_int();
		next_task = other_task;
		yield();
		enable_int();
		//kprint(INFO, "MAIN RETURNED! %d\n");
	}
	//yield();
	//yield();

	PANIC("KMAIN STOP"); 
	
	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
