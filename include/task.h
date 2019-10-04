#ifndef _TASK_H
#define _TASK_H
#include <stdint.h>
#include "../include/processor.h"
#include "../include/kdef.h"

#define TASK_NAME_LEN 64

typedef int priority_t;

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
