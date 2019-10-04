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
#include "../include/scheduler.h"
#include "../include/string.h"
#include "../include/timer.h"

void do_work()
{	
	for (int i = 0; i < 100000; i++) {
		enable_int();
		nop();
		disable_int();
	}
}

void main1()
{
	for (int i = 0; i < 10; i++) {
		
		
		if (i == 1) {
			block_task(SLEEPING);
			//sleep_for(1000);
		}
		
		kprint(INFO, "OTHERTASK1 Enter %d\n", i);
		soft_lock_scheduler();
		//disable_int();
		schedule();
		soft_unlock_scheduler();
		//enable_int();
		kprint(INFO, "OTHERTASK1 leave %d\n", i);
		//i++;


	}
}

extern task_control_block_t *current_task;
void main2()
{
	for (int i = 0; i < 10; i++) {
		
		
		if (i == 0) {
			//block_task(SLEEPING);
			sleep_for(10000);
			kprint(INFO, "WOKEN UP AT %d\n", current_task->time_used);
		}
		
		kprint(INFO, "OTHERTASK2 Enter %d\n", i);
		soft_lock_scheduler();
		//disable_int();
		schedule();
		soft_unlock_scheduler();
		//enable_int();
		kprint(INFO, "OTHERTASK2 leave %d\n", i);
		//i++;


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
	
	task_control_block_t *temp_task;
	soft_lock_scheduler();
	temp_task = create_task(main1, 0, "main1");
	schedule_task_ready(0, temp_task);
	soft_unlock_scheduler();

	soft_lock_scheduler();
	temp_task = create_task(main2, 0, "main2");
	schedule_task_ready(0, temp_task);
	soft_unlock_scheduler();
	
	for (int i = 0; i < 20; i++) {
		//do_work();
		if (i == 10) {
			unblock_task(name_to_tcb("main1"));
		}
		
		if (i == 15) {
			sleep_for(100000);
		}
		kprint(INFO, "MAIN enter %d\n", i);
		//disable_int();
		soft_lock_scheduler();
		schedule();
		soft_unlock_scheduler(); 
		//enable_int();
		kprint(INFO, "Main leave %d\n", i);
		
		
	}
	
	
	PANIC("KMAIN STOP"); 
	
	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
