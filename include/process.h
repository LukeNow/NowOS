#ifndef _PROCESS_H
#define _PROCESS_H

#include "../include/task.h"
#include "../include/kdef.h"
#include "../include/ipc.h"

typedef struct process_t {
	task_control_block_t * proc_tasks[MAX_TASKS_NUM];
	proc_id_t process_id;
	unsigned int task_num;
	flags_t flags;
	message_buf_t message_buf;
	char name[PROC_NAME_LEN];
} process_t;

proc_id_t create_process(void (*main)(), priority_t starting_priority, 
			  flags_t flags, char *name);
int start_process(proc_id_t id);
void fork_process_task(process_t *process);
void init_processes();
#endif
