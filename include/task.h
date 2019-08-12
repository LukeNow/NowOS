#ifndef _TASK_H
#define _TASK_H
#include <stdint.h>
#include "../include/processor.h"

#define TASK_NAME_LEN 64

typedef struct task_control_block {
	cpu_state_t cpu_state;
	char name[TASK_NAME_LEN];
}task_control_block_t;

void switch_task();
void prep_stack_frame(task_control_block_t *task, void (*main)(), 
		      uint32_t stack_addr);
void create_task(void (*main)(), const char *name);
void start_task(void (*main)(), task_control_block_t *task);
void destroy_task(task_control_block_t *task);
void yield();
void init_multitasking();
#endif