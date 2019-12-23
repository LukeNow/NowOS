#include "../include/kdef.h"
#include "../include/linked_list.h"
#include "../include/process.h"
#include "../include/kheap.h"
#include "../include/kprint.h"

process_t * process_tbl[MAX_PROC_NUM];
int proc_num = 0;

proc_id_t create_process(void (*main)(), priority_t starting_priority, 
			  flags_t flags, char *name)
{
	ASSERT(proc_num != MAX_PROC_NUM);
	process_t * proc = kmalloc(sizeof(process_t));
	memset(proc, 0, sizeof(process_t));

	proc->proc_tasks[0] = create_task(main, starting_priority, name);
	proc->process_id = proc_num++;
	proc->flags = flags;
	init_circ_buf(MESSAGE_BUF_LEN, sizeof(message_t), &proc->message_buf);
	memcpy(proc->name, name, PROC_NAME_LEN);

	process_tbl[proc->process_id] = proc;

	return proc->process_id;
}

int start_process(proc_id_t id)
{
	ASSERT(id >= 0 && id < MAX_PROC_ID);
	process_t * proc = process_tbl[id];
	if (proc == NULL) {
		kprint(ERROR, "Proc %d not found\n", id);
		return FAILURE;
	}
	start_task(proc->proc_tasks[0]);
	return SUCCESS;
}

void fork_process_task(process_t *process)
{
	kprint(INFO, "Fork process not implemented\n");
}

void init_processes(task_control_block_t * first_task)
{
	memset(process_tbl, 0, sizeof(process_tbl));
	process_t * proc = kmalloc(sizeof(process_t));
	memset(proc, 0, sizeof(process_t));

	proc->process_id = 0; //First proc
	proc->proc_tasks[0] = first_task;
	proc->task_num = 1;
	init_circ_buf(MESSAGE_BUF_LEN, sizeof(message_t), &proc->message_buf);
	memcpy(proc->name, "Main process", PROC_NAME_LEN);
	
	proc_num++;
	process_tbl[0] = proc;
}
