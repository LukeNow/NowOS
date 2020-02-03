#include <kdef.h>
#include <kprint.h>
#include <task.h>
#include <linked_list.h>
#include <machine.h>
#include <string.h>
#include <timer.h>
#include <scheduler.h>
#include <pit.h>
#include <kheap.h>

#define GET_FIRST_TASK(list) \
	(task_control_block_t *)(((linked_list_t)(list))->start_ptr->data)

#define QUEUE_NUM 3

LINKED_LIST_INIT(ready_list_0);
LINKED_LIST_INIT(ready_list_1);
LINKED_LIST_INIT(ready_list_2);

LINKED_LIST_INIT(blocked_list);

task_control_block_t *timer_task_tcb;
task_control_block_t *idle_task_tcb;

task_control_block_t *current_task;
task_control_block_t *next_task;

int scheduler_lock_counter = 0;
int scheduler_postponed_counter = 0;
int scheduler_postponed_flag = 0;

static linked_list_t *queue_num_to_list(int queue_num)
{
	switch (queue_num) {
		case 0:
			return &ready_list_0;
		case 1:
			return &ready_list_1;
		case 2:
			return &ready_list_2;
		default:
			return NULL;
	}
}

task_control_block_t *name_to_tcb(char *name)
{
	for (int i = 0; i < QUEUE_NUM; i++) {
		linked_list_t *ready_list = queue_num_to_list(i);
		linked_list_for_each(index, ready_list) {
			task_control_block_t *task = index->data;
			if (strcmp(name, task->name) == 0)
				return task;
		}
	}
		
	linked_list_for_each(index, &blocked_list) {
		task_control_block_t *task = index->data;
		if (strcmp(name, task->name) == 0)
			return task;
	}

	return NULL;
}

static void update_next_task()
{	
	/* Get the next available task from the highest priority, non-empty
	 * queue. If the task that is found is the same as current task, 
	 * cycle the linked list to make sure that we can schedule other tasks
	 * that might have just been queued */
	for (int i = 0; i < QUEUE_NUM; i++) {
		linked_list_t * ready_list = queue_num_to_list(i);
		if (ready_list->size != 0) {
			next_task = linked_list_get(0, ready_list);
			if (next_task == current_task && ready_list->size != 1) {
				linked_list_cycle(ready_list);
				next_task = linked_list_get(0, ready_list);
				//kprint(INFO, "Next task is %s\n", next_task->name);
				//print_ready_queue(0);
			}
			return;
		}
	}
	/* If we didnt find a ready task, we schedule the idle task */
	next_task = idle_task_tcb;
}

task_control_block_t * get_current_task()
{
	return current_task;
}

void print_ready_queue(int queue_num)
{
	linked_list_t *ready_list = 
		queue_num_to_list(queue_num);
	kprint(INFO, "Printing out queue num %d\n", queue_num);
	linked_list_for_each(index, ready_list) {
		task_control_block_t *task = index->data;
		kprint(INFO, "Task: %s\n", task->name);
	}
}

void print_scheduler_state()
{
	kprint(INFO, "*** Printing scheduler state ***\n");
	kprint(INFO, "scheduler lock counter %d\n", scheduler_lock_counter);
	kprint(INFO, "scheduler postponed counter %d\n", scheduler_postponed_counter);
	kprint(INFO, "scheduler postponed flag %d\n", scheduler_postponed_flag);

	kprint(INFO, "interrupts: %s\n", (get_int() == INT_SET) ? "ACTIVE" : "DISABLED");
	print_ready_queue(0);
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
	/* Decrement counters initially */
	scheduler_postponed_counter--;
	scheduler_lock_counter--;
	
	ASSERT(scheduler_postponed_counter >= 0);
	ASSERT(scheduler_lock_counter >= 0);
	
	if (scheduler_lock_counter == 0 &&
	    scheduler_postponed_flag != 0) {
		scheduler_postponed_flag = 0;
		
		//TODO Do we switch tasks immediately?
		/*
		soft_lock_scheduler();
		schedule();
		soft_unlock_scheduler();
		*/
	}
	/* If the counter is zero, we soft unlock the scheduler 
	 * like we normally would */
	else if (scheduler_lock_counter == 0) {
		enable_int();
	}
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

/***
 * General task scheduling functions
 ***/
void schedule_task_ready(int queue_num, task_control_block_t *task)
{
	int blocked_index = linked_list_search(task, &blocked_list);
	if (blocked_index != -1)
		linked_list_remove(blocked_index, &blocked_list);
	
	linked_list_t *ready_list = queue_num_to_list(queue_num);
	ASSERT(ready_list);
	
	int ready_index = linked_list_search(task, ready_list);
	/* If the task is not in the queue, then we enqueue it */
	if (ready_index == -1){
		linked_list_enqueue(task, ready_list);
	}
}

void schedule_task_blocked(task_control_block_t *task)
{
	int ready_index = -1;
	linked_list_t *ready_list = 
		queue_num_to_list(task->current_priority);
	
	if (ready_list != NULL) {
		ready_index = linked_list_search(task, ready_list);
	}
	if (ready_index != -1)
		linked_list_remove(ready_index, ready_list);
	
	linked_list_enqueue(task, &blocked_list);
}

void unschedule_task(task_control_block_t *task)
{
	int ready_index = -1;
	linked_list_t *ready_list = 
		queue_num_to_list(task->current_priority);

	if (ready_list != NULL)
		ready_index = linked_list_search(task, ready_list);
	int blocked_index = linked_list_search(task, &blocked_list);
	if (ready_index != -1)
		linked_list_remove(ready_index, ready_list);
	if (blocked_index != -1)
		linked_list_remove(blocked_index, &blocked_list);
}

void init_scheduler(task_control_block_t *first_task)
{
	soft_lock_scheduler();
	
	/* Create and schedule idle task 
	 * Priority is -1 meaning it is never actually in any priority queue. 
	 * This means that we never ever schedule idle task and just jump to it
	 * whenver we dont have anything to do */
	idle_task_tcb = create_task(idle_task, NOT_SCHEDULED, "idle_task");
	idle_task_tcb->current_priority = NOT_SCHEDULED;
	idle_task_tcb->state = IDLE;
	
	/* Finally schedule the first task */
	first_task->current_priority = 0;
	first_task->state = READY;
	schedule_task_ready(first_task->starting_priority, first_task);
	
	/* Initialize scheduler variables */
	current_task = first_task;
	next_task = NULL;

	soft_unlock_scheduler();
}
