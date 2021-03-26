#include <kheap.h>
#include <linked_list.h>
#include <thread.h>
#include <string.h>
#include <mm.h>
#include <kdef.h>
#include <kprint.h>
#include <scheduler.h>
#include <shared_pool.h>
#include <atomic.h>
#include <processor.h>

#define PREPPED_STACK_PARAM_NUM 4

static shared_pool_t thread_pool;

void init_threading()
{
	if(!shared_pool_init(&thread_pool, MAX_THREAD_COUNT, sizeof(tib_t)))
		PANIC("Init threading failed to allocate thread pool\n");
}

tib_t * create_thread(void (*enter)(), const char * name)
{
	
	processor_t * proc = processor_get_info();
	
	tib_t * thread = shared_pool_get_next(&thread_pool);
	if (thread == NULL)
		PANIC("Could not allocate thread from shared pool\n");
	
	thread->tid = shared_pool_get_index(&thread_pool, thread);

	uint32_t new_stack_addr = (uint32_t)kmalloc_page() + PAGE_SIZE; //start at top of page
	/*
	uint32_t new_esp = new_stack_addr - 
			  (PREPPED_STACK_PARAM_NUM * sizeof(register_t));
	*/
	//prep_stack_frame(thread, enter, new_stack_addr);
	
	thread->cpu_state.cr3 = get_cr3(); //same page directory
	thread->cpu_state.cr2 = get_cr2();
	thread->cpu_state.cr0 = get_cr0();
	thread->cpu_state.eflags = get_eflags();
	thread->cpu_state.eip = (uint32_t) enter;
	thread->cpu_state.esp = new_stack_addr; //sp at our preped stack
	thread->enter = enter;
	memcpy(thread->name, name, TASK_NAME_LEN);

	kprint(INFO, "Thread %s: tid %d created on proc %d\n", name, thread->tid, proc->id);
	
	return thread;
}

void thread_enter(tib_t * tib)
{

	kprint(INFO, "Thread tid:%d entered\n", tib->tid);
	tib->state = THREAD_RUNNING;
	
	timer_lapic_start();
	scheduler_leave();

	tib->enter();

	VERIFY_UNREACHED();
}

/*
tib_t *create_task(void (*main)(), priority_t starting_priority, 
				  const char *name)
{
	tib_t *task = 
		kmalloc(sizeof(tib_t));
	memset(task, 0, sizeof(tib_t));
	memcpy(task->name, name, TASK_NAME_LEN);
	
	task->starting_priority = starting_priority;
	task->current_priority = NOT_SCHEDULED; //meaning not queued or blocked

	uint32_t new_stack_addr = (uint32_t)kmalloc_page() + PAGE_SIZE; //start at top of page
	uint32_t new_esp = new_stack_addr - 
			  (PREPPED_STACK_PARAM_NUM * sizeof(unsigned int));
	//kprint(INFO, "PREPING STACK AT %x with new ESP %x\n", new_stack_addr, new_esp);
	
	prep_stack_frame(task, main, new_stack_addr);
	
	task->cpu_state.cr3 = get_cr3(); //same page directory
	task->cpu_state.cr2 = get_cr2();
	task->cpu_state.cr0 = get_cr0();
	task->cpu_state.eflags = get_eflags();
	task->cpu_state.eip = (uint32_t) start_task;
	task->cpu_state.esp = new_esp; //sp at our preped stack
	
	task->id = -1;
	task->main = main;
	init_circ_buf(MESSAGE_BUF_LEN, sizeof(message_t), &task->msg_buf);
	return NULL;
} */