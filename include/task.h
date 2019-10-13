#ifndef _TASK_H
#define _TASK_H
#include <stdint.h>
#include "../include/processor.h"
#include "../include/kdef.h"

#define MESSAGE_BUFF_LEN 4

typedef enum task_state {
	READY, 
	SLEEPING, 
	PAUSED,
	IDLE
}task_state_t;

typedef struct task_control_block {
	cpu_state_t cpu_state;
	task_state_t state;
	time_t time_used;
	time_t last_time;
	priority_t current_priority;
	priority_t starting_priority;
	task_id_t task_id; //task_id of this task
	proc_id_t proc_id; //proc_id that this tasks belongs to
	/* task_id & proc_id make the unique ID for this task */
	char name[TASK_NAME_LEN];
}task_control_block_t;

void switch_task();
void prep_stack_frame(task_control_block_t *task, void (*main)(), 
		      uint32_t stack_addr);
task_control_block_t *create_task(void (*main)(), priority_t starting_priority, 
				  const char *name);
void start_task(void (*main)(), task_control_block_t *task);
void destroy_task(task_control_block_t *task);
void idle_task();
void init_multitasking();
#endif
