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


tib_t * thread_create_thread(void (*enter)(), uint32_t stack_addr, const char * name)
{
	
	processor_t * proc = processor_get_info();
	
	tib_t * thread = shared_pool_get_next(&thread_pool);
	if (thread == NULL)
		PANIC("Could not allocate thread from shared pool\n");
	
	memset(thread, 0, sizeof(tib_t));
	thread->tid = shared_pool_get_index(&thread_pool, thread);

	if (stack_addr == 0)
		stack_addr = (uint32_t)kmalloc_page() + PAGE_SIZE; //start at top of page
	
	ASSERT(stack_addr != 0);

	thread->cpu_state.cr3 = get_cr3(); //same page directory
	thread->cpu_state.cr2 = get_cr2();
	thread->cpu_state.cr0 = get_cr0();
	thread->cpu_state.eflags = get_eflags();
	thread->cpu_state.eip = (uint32_t) enter;
	thread->cpu_state.esp = stack_addr; //sp at our preped stack
	thread->enter = enter;
	memcpy(thread->name, name, TASK_NAME_LEN);

	kprint(INFO, "Thread at addr %x enter is %x esp is %x, name %s\n", thread, enter, stack_addr, name);
	
	return thread;
}

void thread_enter(tib_t * tib)
{

	//kprint(INFO, "Thread tid:%d entered\n", tib->tid);
	tib->state = THREAD_RUNNING;
	
	timer_lapic_start();
	scheduler_leave();

	tib->enter();

	VERIFY_UNREACHED();
}