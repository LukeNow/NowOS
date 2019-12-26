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
#include "../include/process.h"
#include "../include/circ_buf.h"
#include "../include/ktest.h"

/* Use the label addresses as the addresses for the start and end points of 
 * important areas of memory */
extern void heap_top();
extern void heap_bottom();
extern void stack_top();

extern task_control_block_t *current_task;

void test_task_1()
{
	for (int i = 0; i < 10; i++) {
		/*
		if (i == 1) {
			block_task(SLEEPING);
		} */
		
		kprint(INFO, "Test task 1 Enter %d\n", i);
		
		kprint(INFO, "BLOCKING PROCESS\n");
		block_process();
		kprint(INFO, "TASK WOKEN UP\n");
		kprint(INFO, "Test task 1 leave %d\n", i);
	}
}

void test_task_2()
{
	for (int i = 0; i < 10; i++) {
		
		
		if (i == 0) {
			sleep_for(10000);
			kprint(INFO, "Test task 2 WOKEN UP AT %d\n", current_task->time_used);
		}
		
		kprint(INFO, "Test task 2 Enter %d\n", i);
		soft_lock_scheduler();
		schedule();
		soft_unlock_scheduler();
		kprint(INFO, "Test Task 2 leave %d\n", i);
	}
}

static void init_multitasking()
{
	task_control_block_t * first_task = init_tasking();
	init_scheduler(first_task);
	init_processes(first_task);
}

void kmain(multiboot_info_t* mbt, unsigned int magic)
{
	int rc;
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
		
#ifdef TEST
	run_ktest_suite();
#endif
	
	proc_id_t id1 = create_process(test_task_1, 0, 0, "test_task_1");
	if (id1 != 1) {
		PANIC("PROC CREATION FAILURE");
	}

	rc = start_process(id1);
	if (rc == FAILURE) {
		PANIC("PROC START FAILURE");
	}
	
	for (int i = 0; i < 20; i++) {
		
		id_t id = MAKE_ID(id1, 0);
		/*
		if (i == 10) {
			unblock_task(name_to_tcb("test_task_1"));
		} */
		/*
		if (i == 15) {
			sleep_for(100000);
		} */
		kprint(INFO, "MAIN enter %d\n", i);
		soft_lock_scheduler();
		schedule();
		soft_unlock_scheduler(); 
		kprint(INFO, "Main leave %d\n", i);
	
		kprint(INFO, "KMAIN unblocking proc %d task %d\n", id1, 0);
		rc = unblock_process(id);
		if (rc == FAILURE) {
			kprint(ERROR, "Unblock process failed\n");
		}
	}
	
	PANIC("KMAIN STOP"); 
	
	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
