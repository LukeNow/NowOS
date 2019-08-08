#include <stddef.h>
#include <stdint.h>
#include "../include/kheap.h"
#include "../include/linked_list.h"
#include "../include/task.h"
#include "../include/string.h"
#include "../include/mm.h"
#include "../include/kdef.h"
#include "../include/kprint.h"

LINKED_LIST_INIT(task_list);
task_control_block_t *main_task;
task_control_block_t *current_task;
task_control_block_t *next_task;

void init_multitasking()
{
	char task_name[TASK_NAME_LEN] = "main_task";
	
	task_control_block_t *main_task_ptr = 
		kmalloc(sizeof(task_control_block_t));
	memset(main_task_ptr, 0, sizeof(task_control_block_t));
	memcpy(main_task_ptr->name, task_name, TASK_NAME_LEN);
	
	linked_list_add(main_task_ptr, &task_list);
	
	main_task = main_task_ptr;
	current_task = main_task;
}

void create_task(void (*main)(), const char *name)
{
	task_control_block_t *task = 
		kmalloc(sizeof(task_control_block_t));
	memset(task, 0, sizeof(task_control_block_t));
	memcpy(task->name, name, TASK_NAME_LEN);

	uint32_t new_stack_addr = mem_get_free_page_virt() + PAGE_SIZE; //start at top of page
	uint32_t new_esp = new_stack_addr - (6 * 4);
	kprint(INFO, "PREPING STACK AT %x with new ESP %x\n", new_stack_addr, new_esp);
	
	disable_int();
	prep_stack_frame(task, main, new_stack_addr);
	enable_int();
	next_task = task;

	task->cpu_state.cr3 = get_cr3(); //same page directory
	task->cpu_state.cr2 = get_cr2();
	task->cpu_state.cr0 = get_cr0();
	task->cpu_state.eax = 0;
	task->cpu_state.ebx = 0;
	task->cpu_state.ecx = 0;
	task->cpu_state.edx = 0;
	task->cpu_state.esi = 0;
	task->cpu_state.edi = 0;
	task->cpu_state.eflags = get_eflags();
	task->cpu_state.eip = (uint32_t) start_task;
	task->cpu_state.esp = new_esp;
	

	//PANIC("SO FAR SO GOOD");
}

void start_task(void (*main)(), task_control_block_t *task)
{
	main();
	destroy_task(task);
}

void destroy_task(task_control_block_t *task)
{
	PANIC("TASK ENDED");
}

