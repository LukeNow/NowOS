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
	message_buf_t msg_buf;
	char name[PROC_NAME_LEN];
} process_t;

proc_id_t create_process(void (*main)(), priority_t starting_priority, 
			  flags_t flags, char *name);

process_t * get_process_from_pid(proc_id_t pid);
id_t get_current_id();
void destroy_process(proc_id_t id);
int start_process(proc_id_t id);
void fork_process_task(process_t *process);
void unregister_process_task(task_control_block_t * task);
void block_process();
int unblock_process(id_t id);
int start_process(proc_id_t id);
void yield_process();

void init_processes();
#endif
