#ifndef _TASK_H
#define _TASK_H
#include <stdint.h>
#include "../include/processor.h"
#include "../include/kdef.h"
#include "../include/circ_buf.h"

typedef enum task_state {
	READY, 
	SLEEPING, 
	PAUSED,
	IDLE
}task_state_t;

typedef struct task_control_block {
	/* We have assembly functions that require cpu_state 
	 * to be at the top of the struct */
	cpu_state_t cpu_state;
	task_state_t state;
	time_t time_used;
	time_t last_time;
	priority_t current_priority;
	priority_t starting_priority;
	task_id_t task_id; //task_id of this task
	/* task_id & proc_id make the unique ID for this task */
	void (*main)();
	circ_buf_t message_buf;
	char name[TASK_NAME_LEN];
}task_control_block_t;

void switch_task();
void prep_stack_frame(task_control_block_t *task, void (*main)(), 
		      uint32_t stack_addr);
task_control_block_t *create_task(void (*main)(), priority_t starting_priority, 
				  const char *name);
void bootstrap_task(void (*main)(), task_control_block_t *task);
void destroy_task(task_control_block_t *task);
int start_task(task_control_block_t * task);
void idle_task();
task_control_block_t * init_tasking();
#endif
