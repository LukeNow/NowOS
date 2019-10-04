#ifndef _SCHEDULER_H
#define _SCHEDULER_H 
#include "../include/task.h"
#include "../include/kdef.h"

typedef struct wait_control_block {
	task_control_block_t *task;
	time_t time_left;
}wait_control_block_t;

extern task_control_block_t *current_task;
extern task_control_block_t *next_task;

void soft_lock_scheduler();
void soft_unlock_scheduler();
void hard_lock_scheduler();
void hard_unlock_scheduler();


void sleep_for(time_t ns_to_wait);

void schedule();
void schedule_task_ready(int queue_num, task_control_block_t *task);
void schedule_task_blocked(task_control_block_t *task);
void unschedule_task(task_control_block_t *task);
void block_task(task_state_t new_state);
void unblock_task(task_control_block_t *task);
void init_scheduler();
task_control_block_t *name_to_tcb(char *name);
void print_ready_queue(int queue_num);
#endif
