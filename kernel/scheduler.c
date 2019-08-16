#include <stdint.h>
#include <stddef.h>
#include "../include/kprint.h"
#include "../include/task.h"
#include "../include/linked_list.h"
#include "../include/machine.h"
#include "../include/string.h"
#include "../include/timer.h"
#include "../include/scheduler.h"
#include "../include/pit.h"
#include "../include/kdef.h"
#include "../include/kheap.h"

#define GET_FIRST_TASK(list) \
	(task_control_block_t *)(((linked_list_t)(list))->start_ptr->data)

LINKED_LIST_INIT(ready_list);
LINKED_LIST_INIT(blocked_list);
LINKED_LIST_INIT(timer_list);

task_control_block_t *timer_task;
task_control_block_t *current_task;
task_control_block_t *next_task;

int scheduler_lock_counter = 0;
int scheduler_postponed_counter = 0;
int scheduler_postponed_flag = 0;

time_t last_update_time = 0;

static void update_time_used()
{
	uint32_t current_time = timer_get_ns();
	uint32_t time_elapsed = current_time - current_task->last_time;
	
	current_task->last_time = current_time;
	current_task->time_used += time_elapsed;
	
	kprint(INFO, "%s: time elapsed: %d\n", current_task->name, 
	       current_task->time_used);
}

static void update_next_task()
{
	next_task = linked_list_get(0, &ready_list);
	
	/* If the next task is he same as current task and this is
	 * not the only ready task, cycle the list to get a different
	 * task that was probably just queued */
	if (next_task == current_task && ready_list.size != 1) {
		linked_list_cycle(&ready_list);
		next_task = linked_list_get(0, &ready_list);
	}

}

static void pit_handler()
{	
	if (timer_task != NULL && 
	    linked_list_search(timer_task, &blocked_list) != -1) {
		soft_lock_scheduler();
		schedule_task_ready(timer_task);
		timer_task->state = READY;
		soft_unlock_scheduler();
	}
}

static void update_timer_list()
{
	for (;;) {
		time_t curr_time = timer_get_ns();
		time_t time_elapsed = curr_time - last_update_time;
		last_update_time = curr_time;

		hard_lock_scheduler();
		linked_list_for_each(index, &timer_list) {
			wait_control_block_t *wcb = 
				(wait_control_block_t *)index->data;
			wcb->time_left -= time_elapsed;
			if (wcb->time_left <= 0) {
				int timer_index = linked_list_search(wcb, &timer_list);
				ASSERT(timer_index != -1);
				linked_list_remove(timer_index, &timer_list);
				kfree(wcb);
				unblock_task(wcb->task);
			}
		}
		hard_unlock_scheduler();
		block_task(PAUSED);
	}
}

void soft_lock_scheduler() 
{
	disable_int();
	scheduler_lock_counter++;
}


void soft_unlock_scheduler()
{
	scheduler_lock_counter--;
	ASSERT(scheduler_lock_counter >= 0);
	if (scheduler_lock_counter == 0) {
		enable_int();
	}
}

void hard_lock_scheduler()
{
	soft_lock_scheduler();
	scheduler_postponed_counter++;
}

void hard_unlock_scheduler()
{
	scheduler_postponed_counter--;
	ASSERT(scheduler_postponed_counter >= 0);
	if (scheduler_lock_counter == 0) {
		if (scheduler_postponed_flag != 0) {
			scheduler_postponed_flag = 0;
			//schedule();
		}
	}
	soft_unlock_scheduler();
}

task_control_block_t *name_to_tcb(char *name)
{
	linked_list_for_each(index, &ready_list) {
		task_control_block_t *task = index->data;
		if (strcmp(name, task->name) == 0)
			return task;
	}
	
	linked_list_for_each(index, &blocked_list) {
		task_control_block_t *task = index->data;
		if (strcmp(name, task->name) == 0)
			return task;
	}

	return NULL;
}

void sleep_for(time_t ns_to_wait)
{
	wait_control_block_t *wcb = 
		kmalloc(sizeof(wait_control_block_t));
	wcb->task = current_task;
	wcb->time_left = ns_to_wait;
	linked_list_enqueue(wcb, &timer_list);
	block_task(SLEEPING);
}

/* MUST be called with soft_scheduler_lock called */
void schedule()
{
	/* If there is a hard scheduler lock being held
	 * do not switch_task() */
	if (scheduler_postponed_counter != 0) {
		scheduler_postponed_flag = 1;
		return;
	}
	
	/* Update ready list to get next ready task */
	update_next_task();
	
	update_time_used();
	/* If the next task happens to be the same task as our
	 * current one, there is no need to switch tasks */
	if (next_task == current_task) {
		/* Record time usage starting now */
		current_task->last_time = timer_get_ns();
		return;
	}
	switch_task();
	/* record start of time usage period */
	current_task->last_time = timer_get_ns();
}

void schedule_task_ready(task_control_block_t *task)
{
	int blocked_index = linked_list_search(task, &blocked_list);
	if (blocked_index != -1)
		linked_list_remove(blocked_index, &blocked_list);
	linked_list_enqueue(task, &ready_list);
}

void schedule_task_blocked(task_control_block_t *task)
{
	int ready_index = linked_list_search(task, &ready_list);
	if (ready_index != -1)
		linked_list_remove(ready_index, &ready_list);
	
	linked_list_enqueue(task, &blocked_list);
}

void unschedule_task(task_control_block_t *task)
{
	int ready_index = linked_list_search(task, &ready_list);
	int blocked_index = linked_list_search(task, &blocked_list);
	if (ready_index != -1)
		linked_list_remove(ready_index, &ready_list);
	if (blocked_index != -1)
		linked_list_remove(blocked_index, &blocked_list);
}

void block_task(task_state_t new_state)
{
	soft_lock_scheduler();
	schedule_task_blocked(current_task);
	current_task->state = new_state;
	schedule();
	/* The task unblocks here, record start
	 * of time usage period */
	current_task->last_time = timer_get_ns(); 
	soft_unlock_scheduler();
}

void unblock_task(task_control_block_t *task)
{
	ASSERT(task->state == SLEEPING || 
		task->state == PAUSED);
	soft_lock_scheduler();
	schedule_task_ready(task);
	current_task->state = READY;
	schedule();
	soft_unlock_scheduler();
}

void init_scheduler(task_control_block_t *first_task)
{
	/* Create timer task that will keep track of sleeping processes 
	 * and register it so that it gets called on PIT ticks*/
	timer_task = create_task(update_timer_list, "timer_task");
	register_pit_handler(pit_handler);
	
	soft_lock_scheduler();
	current_task = first_task;
	next_task = NULL;
	first_task->state = READY;
	linked_list_enqueue(first_task, &ready_list);
	soft_unlock_scheduler();
}
