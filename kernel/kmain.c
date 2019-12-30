#include <kdef.h>
#include <tty.h>
#include <kprint.h>
#include <machine.h>
#include <processor.h>
#include <io.h>
#include <multiboot.h>
#include <kheap.h>
#include <string.h>
#include <mm.h>
#include <kdef.h>
#include <paging.h>
#include <sorted_array_list.h>
#include <pit.h>
#include <linked_list.h>
#include <task.h>
#include <scheduler.h>
#include <string.h>
#include <timer.h>
#include <process.h>
#include <circ_buf.h>
#include <ktest.h>
#include <ipc.h>
#include <system.h>

/* Use the label addresses as the addresses for the start and end points of 
 * important areas of memory */
extern void heap_top();
extern void heap_bottom();
extern void stack_top();

extern task_control_block_t *current_task;

void test_task_1()
{
	for (int i = 0; i < 2; i++) {
		
		kprint(INFO, "Test task 1 Enter %d\n", i);
		id_t curr_id = get_current_id();
		id_t main_id = MAKE_ID(0, 0);

		message_t in_msg;
		async_receive_msg(main_id, &in_msg, 0);
		kprint(INFO, "Task1: Message received Body %d\n", in_msg.body[0]);


		int body = 10;
		MAKE_MESSAGE(out_msg, curr_id, 0, &body);
		
		kprint(INFO, "Task1: Sending message to kmain with %d\n", body);
		async_send_msg(main_id, &out_msg, 0);

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
	
	//Init kmain pid 0 
	init_multitasking();
	
	id_t system_id = create_system_process();
	kprint(INFO, "System proc created with pid %d\n", GET_PROC_ID(system_id));
	
	id_t timer_id = create_timer_process();
	kprint(INFO, "Timer proc created with pid %d\n", GET_PROC_ID(timer_id));
	
#ifdef TEST
	run_ktest_suite();
#endif
	
	proc_id_t id1 = create_process(test_task_1, 0, 0, "test_task_1");
	if (id1 == FAIL_ID) {
		PANIC("PROC CREATION FAILURE");
	}
	
	rc = start_process(id1);
	if (rc == FAILURE) {
		PANIC("PROC START FAILURE");
	}
	
	for (int i = 0; i < 2; i++) {
		
		message_t in_msg;
		id_t out_id = MAKE_ID(id1, 0);
		kprint(INFO, "MAIN enter %d\n", i);
		
		int body = 5;
		MAKE_MESSAGE(out_msg, out_id, 0, &body);
		
		kprint(INFO, "Kmain: sent msg with %d\n", body);
		async_send_msg(out_id, &out_msg, 0);
		
		async_receive_msg(out_id, &in_msg, 0);
		kprint(INFO, "Kmain: received msg with %d\n", in_msg.body[0]);
		
		/*
		soft_lock_scheduler();
		schedule();
		soft_unlock_scheduler(); 
		*/

		kprint(INFO, "Main leave %d\n", i);
		
		/*
		kprint(INFO, "KMAIN unblocking proc %d task %d\n", id1, 0);
		rc = unblock_process(id);
		if (rc == FAILURE) {
			kprint(ERROR, "Unblock process failed\n");
		}*/
	}
	
	print_scheduler_state();
	
	PANIC("KMAIN STOP"); 
	
	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
