#ifndef _THREAD_H
#define _THREAD_H
#include <stdint.h>
#include <machine.h>
#include <kdef.h>

typedef enum thread_state_t {
	THREAD_READY,
	THREAD_RUNNING,
	THREAD_SLEEPING, 
	THREAD_PAUSED,
	THREAD_IDLE
}thread_state_t;

/* Thread info block */
typedef struct tib_t {
	/* We have assembly functions that require cpu_state 
	 * to be at the top of the struct */
	cpu_state_t cpu_state;
	thread_state_t state;
	uint32_t tid;
	time_t time_used;
	time_t last_time;
	priority_t current_priority;
	priority_t starting_priority;
	bool is_active;
	void (*enter)();
	char name[TASK_NAME_LEN];
} tib_t;

void prep_stack_frame(tib_t * tib, void (*enter)(), 
		      		  uint32_t stack_addr);

void thread_switch_async(tib_t * from_thread, tib_t * to_thread, int_state_t * int_state);
void thread_switch_sync(tib_t * from_thread, tib_t * to_thread);
tib_t * create_thread(void (*enter)(), const char * name);
void _thread_enter(tib_t * tib);
void thread_enter(tib_t * tib);
void init_threading();

#endif
