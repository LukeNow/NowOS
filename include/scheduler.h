#ifndef _SCHEDULER_H
#define _SCHEDULER_H 
#include <kdef.h>
#include <thread.h>

typedef struct wait_control_block {
	tib_t *task;
	time_t time_left;
}wait_control_block_t;

extern tib_t *current_task;
extern tib_t *next_task;




void scheduler_leave();
void scheduler_leave_and_queue(tib_t * from_thread);
void scheduler_yield();
void scheduler_init();




void soft_lock_scheduler();
void soft_unlock_scheduler();
void hard_lock_scheduler();
void hard_unlock_scheduler();



void sleep_for(time_t ns_to_wait);

tib_t * get_current_task();
void schedule();
void schedule_task_ready(int queue_num, tib_t *task);
void schedule_task_blocked(tib_t *task);
void unschedule_task(tib_t *task);
void init_scheduler();
tib_t *name_to_tcb(char *name);

void print_ready_queue(int queue_num);
void print_scheduler_state();
#endif
