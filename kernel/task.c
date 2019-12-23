#include <stddef.h>
#include <stdint.h>
#include "../include/kheap.h"
#include "../include/linked_list.h"
#include "../include/task.h"
#include "../include/string.h"
#include "../include/mm.h"
#include "../include/kdef.h"
#include "../include/kprint.h"
#include "../include/scheduler.h"
#include "../include/timer.h"
#include "../include/ipc.h"

task_control_block_t *create_task(void (*main)(), priority_t starting_priority, 
				  const char *name)
{
	task_control_block_t *task = 
		kmalloc(sizeof(task_control_block_t));
	memset(task, 0, sizeof(task_control_block_t));
	memcpy(task->name, name, TASK_NAME_LEN);
	
	task->starting_priority = starting_priority;
	task->current_priority = -1; //meaning not queued or blocked

	uint32_t new_stack_addr = (uint32_t)kmalloc_page() + PAGE_SIZE; //start at top of page
	uint32_t new_esp = new_stack_addr - (4 * sizeof(unsigned int));
	//kprint(INFO, "PREPING STACK AT %x with new ESP %x\n", new_stack_addr, new_esp);
	
	prep_stack_frame(task, main, new_stack_addr);
	
	/* All other fields will be zero */
	task->cpu_state.cr3 = get_cr3(); //same page directory
	task->cpu_state.cr2 = get_cr2();
	task->cpu_state.cr0 = get_cr0();
	task->cpu_state.eflags = get_eflags();
	task->cpu_state.eip = (uint32_t) start_task;
	task->cpu_state.esp = new_esp; //sp at our preped stack
	
	task->main = main;
	init_circ_buf(MESSAGE_BUF_LEN, sizeof(message_t), &task->message_buf);

	return task;
}

/* Function gets called naturally when a task returns
 * from its function, or if unaturally terminated */
void destroy_task(task_control_block_t *task)
{
	unschedule_task(task);
	destroy_circ_buf(&task->message_buf);
	kfree(task);
}


//TODO Phase out main() parameter in function assmebly function call
/* The main function will run, when it returns, it will fall through to
 * the destroy task function which will automatically clean up for us */
void bootstrap_task(void (*main)(), task_control_block_t *task)
{
	soft_unlock_scheduler();
	task->last_time = timer_get_ns();
	task->current_priority = task->starting_priority;
	/* Start main function of this task */
	task->main();
	ASSERT(current_task->current_priority != -1);
	/* Destory and unschedule this task */
	destroy_task(task);
	soft_lock_scheduler();
	/* Switch to next task */
	schedule();
	PANIC("We reached end of a task that we shouldnt be reaching");
}

int start_task(task_control_block_t * task)
{
	if (task == NULL)
		return FAILURE;

	soft_lock_scheduler();
	schedule_task_ready(task->starting_priority, task);
	soft_unlock_scheduler();
}

void idle_task()
{
	for (;;) {
		soft_unlock_scheduler();
		soft_lock_scheduler();
		schedule();
	}
}

void sleep_for(time_t ns_to_wait)
{
	wait_control_block_t *wcb = 
		kmalloc(sizeof(wait_control_block_t));
	wcb->task = current_task;
	wcb->time_left = ns_to_wait;
	timer_add_sleep_list(wcb);
	block_task(SLEEPING);
}

task_control_block_t * init_tasking()
{
	char task_name[TASK_NAME_LEN] = "main_task";
	
	task_control_block_t *main_task = 
		kmalloc(sizeof(task_control_block_t));
	memset(main_task, 0, sizeof(task_control_block_t));
	memcpy(main_task->name, task_name, TASK_NAME_LEN);
	main_task->starting_priority = 0;
	main_task->current_priority = 0;
	/* All other fields will be zero */
	main_task->cpu_state.cr3 = get_cr3(); //page directory
	main_task->cpu_state.cr2 = get_cr2();
	main_task->cpu_state.cr0 = get_cr0();
	main_task->cpu_state.eflags = get_eflags();
	/* All other registers will be updated */

	return main_task;
}
