#include <kdef.h>
#include <linked_list.h>
#include <process.h>
#include <kheap.h>
#include <kprint.h>
#include <task.h>
#include <scheduler.h>

process_t * process_tbl[MAX_PROC_NUM];

static proc_id_t get_next_proc_id()
{
	for (int i = 0; i < MAX_PROC_NUM; i++) {
		if (process_tbl[i] == NULL) {
			return i;
		}
	}

	return -1;
}


process_t * get_process_from_pid(proc_id_t pid)
{
	ASSERT(pid >= 0 || pid < MAX_PROC_NUM);
	return process_tbl[pid];
}

id_t get_current_id()
{
	task_control_block_t * task = get_current_task();
	return task->id;
}

proc_id_t create_process(void (*main)(), priority_t starting_priority, 
			  flags_t flags, char *name)
{
	proc_id_t pid = get_next_proc_id();
	if (pid == -1) {
		return -1;
	}

	process_t * proc = kmalloc(sizeof(process_t));
	memset(proc, 0, sizeof(process_t));

	proc->proc_tasks[0] = create_task(main, starting_priority, name);
	proc->proc_tasks[0]->id = MAKE_ID(pid, 0);
	proc->task_num = 1;

	proc->process_id = pid;
	proc->flags = flags;
	init_circ_buf(MESSAGE_BUF_LEN, sizeof(message_t), &proc->msg_buf);
	memcpy(proc->name, name, PROC_NAME_LEN);

	process_tbl[proc->process_id] = proc;

	return pid;
}

void destroy_process(proc_id_t id) 
{
	ASSERT(id >= 0 && id < MAX_PROC_ID);
	
	proc_id_t pid = GET_PROC_ID(id);
	task_id_t tid = GET_TASK_ID(id);
	
	process_t * proc = process_tbl[id];
	if (proc == NULL) {
		kprint(ERROR, "Proc %d not found\n", id);
		return;
	}
#ifdef debug
	for (int i = 0; i < MAX_TASKS_NUM; i++) {
		if (proc->proc_tasks[i] != NULL) {
			PANIC("Still active task");
		}
	}
#endif
	
	/* Unregister */
	process_tbl[id] = NULL;
	destroy_circ_buf(&proc->msg_buf);
	kfree(proc);
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

/* Block the current running task
 *
 * TODO should this be here? Its the same as block_task...
 * Do we want the process interface to be the main interface that
 * we interact with blocking things */
void block_process()
{
	block_task(SLEEPING);
}

/* Wakeup the proc_task that is specified by the ID */
int unblock_process(id_t id)
{
	proc_id_t pid = GET_PROC_ID(id);
	task_id_t tid = GET_TASK_ID(id);
	
	if (pid >= MAX_PROC_NUM || tid >= MAX_TASKS_NUM) {
		kprint(ERROR, "Unblock process PID or TID out of range pid %d tid %d\n", 
				pid, tid);
		return FAILURE;
	}
	process_t * proc = process_tbl[pid];
	
	if (proc == NULL) {
		kprint(ERROR, "Wakeup process: couldnt find proc, Deleted?\n");
		return FAILURE;
	}
	
	task_control_block_t * task = proc->proc_tasks[tid];
	if (task == NULL) {
		kprint(ERROR, "Wakeup process: couldnt find task\n");
		return FAILURE;
	}
	
	/* Already in ready queue, Could have been queued somewhere else*/
	if (task->state == READY ) {
		ASSERT(task->current_priority != NOT_SCHEDULED);
		return SUCCESS;
	}
	
	unblock_task(task);
	return SUCCESS;
}

void fork_process_task(process_t *process)
{
	kprint(INFO, "Fork process not implemented\n");
}

void yield_process()
{
	soft_lock_scheduler();
	schedule();
	soft_unlock_scheduler();
}

/* We are just unregistering the task from the process control
 * block and updating fields. We are not freeing and structures */
void unregister_process_task(task_control_block_t * task)
{
	//TODO Do we inform other tasks/procs through messages
	//That we are ending this task?
	
	ASSERT(task);

	proc_id_t pid = GET_PROC_ID(task->id);
	task_id_t tid = GET_TASK_ID(task->id);
	
	
	process_t * proc = process_tbl[pid];
	if (!proc) {
		PANIC("unregister_process_task: could not find proc\n");
		return;
	}
	
	/* invalidate the process */
	proc->proc_tasks[pid] = NULL;
	proc->task_num--;
	
	if (proc->task_num == 0) {
		destroy_process(pid);
	}
#if DEBUG
	kprint(INFO, "Unregistered proc %d task %d\n", pid, tid);
#endif
}

void init_processes(task_control_block_t * first_task)
{
	memset(process_tbl, 0, sizeof(process_tbl));
	process_t * proc = kmalloc(sizeof(process_t));
	memset(proc, 0, sizeof(process_t));

	proc->process_id = 0; //First proc
	proc->proc_tasks[0] = first_task;
	proc->proc_tasks[0]->id = MAKE_ID(0, 0);
	proc->task_num = 1;
	init_circ_buf(MESSAGE_BUF_LEN, sizeof(message_t), &proc->msg_buf);
	memcpy(proc->name, "Main process", PROC_NAME_LEN);
	
	process_tbl[0] = proc;
}
