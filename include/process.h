#ifndef _PROCESS_H
#define _PROCESS_H

#include "../include/task.h"
#include "../include/kdef.h"

typedef struct process_t {
	id_t process_id;
	task_control_block_t proc_tasks[MAX_TASKS_NUM];
	unsigned int task_num;
	char name[PROC_NAME_LEN];
}process_t;


#endif
