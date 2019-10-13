#include <stddef.h>
#include "../include/kdef.h"
#include "../include/timer.h"
#include "../include/pit.h"
#include "../include/linked_list.h"
#include "../include/scheduler.h"
#include "../include/kheap.h"
#include "../include/kprint.h"

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
		//block_task(PAUSED); 
		//TODO we need to implement proper messaging so that interrupt
		//handlers can schedule the timer_task
		soft_lock_scheduler();
		schedule();
		soft_unlock_scheduler();
	}

}

size_t timer_get_ms()
{
	return pit_get_ms();
}

size_t timer_get_ns()
{
	return pit_get_ns();
}
