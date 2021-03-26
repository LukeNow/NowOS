#ifndef _SCHEDULER_H
#define _SCHEDULER_H 
#include <kdef.h>
#include <thread.h>
#include <machine.h>

void scheduler_async_leave();
void scheduler_leave();
void scheduler_leave_and_queue(tib_t * from_thread);
void scheduler_push_ready_thread(tib_t * thread);
void scheduler_async_enter(int_state_t * state);
void scheduler_yield();
void scheduler_init();

#endif
