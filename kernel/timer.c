#include <stddef.h>
#include "../include/kdef.h"
#include "../include/timer.h"
#include "../include/pit.h"
#include "../include/linked_list.h"
#include "../include/scheduler.h"
#include "../include/kheap.h"
#include "../include/kprint.h"
#include "../include/process.h"
#include "../include/ipc.h"

LINKED_LIST_INIT(timer_list);
time_t last_update_time = 0;

void timer_add_sleep_list(wait_control_block_t *timer_task)
{
	linked_list_enqueue(timer_task, &timer_list);
}

void update_time_used()
{
	uint32_t current_time = timer_get_ns();
	uint32_t time_elapsed = current_time - current_task->last_time;
	
	current_task->last_time = current_time;
	current_task->time_used += time_elapsed;
	
	/*kprint(INFO, "%s: time elapsed: %d\n", current_task->name, 
	       current_task->time_used); */
}

void timer_task()
{
	for (;;) {
		
		
		kprint(INFO, "Timer task running\n");
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
				unblock_process(wcb->task->id);
			}
		}
		hard_unlock_scheduler();
		
		
		block_task(PAUSED); 
	}

}

void pit_task()
{
	//kprint(INFO, "PIT task called\n");
	unblock_process(TIMER_ID);
}

id_t create_timer_process()
{
	int rc;
	proc_id_t pid = create_process(timer_task, 0, 0, "Timer process");
	if (pid == FAIL_ID) {
		kprint(ERROR, "Failed creating timer process\n");
		return FAIL_ID;
	}
	
	rc = start_process(pid);
	if (rc == FAILURE) {
		kprint(ERROR, "Failed to start timer process\n");
		return FAIL_ID;
	}
	
	/* register the pit handler that will be called on
	 * ever clock tick to unblock the timer task to run */
	register_pit_handler(pit_task);

	return MAKE_ID(pid, 0);
}

time_t timer_get_ms()
{
	return pit_get_ms();
}

time_t timer_get_ns()
{
	return pit_get_ns();
}
