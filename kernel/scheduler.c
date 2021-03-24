#include <kdef.h>
#include <kprint.h>
#include <thread.h>
#include <linked_list.h>
#include <machine.h>
#include <string.h>
#include <scheduler.h>
#include <pit.h>
#include <kheap.h>
#include <lock.h>
#include <processor.h>
#include <atomic.h>

SPINLOCK_INIT(sched_lock);
LINKED_LIST_INIT(ready_list);

static void idle_loop()
{
    for (;;) {

        kprint(INFO, "IDLE LOOP ON PROC %d\n", processor_get_id());
    
    
    
    
        scheduler_yield();
    }



    VERIFY_UNREACHED();
}


static tib_t * pull_next_runnable_thread()
{
    return linked_list_get(0, &ready_list);
}

void scheduler_leave()
{
    processor_t * proc = processor_get_info();
    proc->in_scheduler = false;
    spin_unlock(&sched_lock);
    enable_int();

    kprint(INFO, "Leaving scheduler %d with thread %d\n", (uint32_t)proc->id, ((tib_t *)(proc->current_thread))->tid);

}
void scheduler_leave_and_queue(tib_t * from_thread)
{
    processor_t * proc = processor_get_info();
    proc->in_scheduler = false;
    from_thread->is_active = false;
    from_thread->state = THREAD_READY;
    linked_list_enqueue(from_thread, &ready_list);
    spin_unlock(&sched_lock);
    enable_int();

    kprint(INFO, "L N Q scheduler %d with thread %d\n", (uint32_t)proc->id, from_thread->tid);
}

void scheduler_yield()
{
    disable_int(); 
    
    processor_t * proc = processor_get_info();
    ASSERT(proc);
    
    tib_t * from_thread = proc->current_thread;
    ASSERT(from_thread->state = THREAD_RUNNING);
    
    proc->in_scheduler = true;
    
    spin_lock(&sched_lock);
    
    tib_t * to_thread = pull_next_runnable_thread();
    if (to_thread == NULL) {
        scheduler_leave();
        return;
    }

    /* Save some things that might or might not have changed since
       we last saved/created this threads state.
       All other general registers dont matter, 
       and esp, ebp, eip will be saved when we call thread_switch_sync */
    from_thread->cpu_state.cr3 = get_cr3();
	from_thread->cpu_state.cr2 = get_cr2();
	from_thread->cpu_state.cr0 = get_cr0();
	from_thread->cpu_state.eflags = get_eflags();

    thread_switch_sync(from_thread, to_thread);
}

void scheduler_init()
{
    spin_lock(&sched_lock);
    

    tib_t * idle_thread = create_thread(idle_loop, "Idle loop");
    ASSERT(idle_thread);
    idle_thread->state = THREAD_RUNNING;

    processor_t * proc = processor_get_info();
    proc->current_thread = idle_thread;

    


}

void thread_idle_loop()
{

}


void soft_lock_scheduler()
{

}
void soft_unlock_scheduler()
{

}
void hard_lock_scheduler()
{

}
void hard_unlock_scheduler()
{

}
void sleep_for(time_t ns_to_wait)
{

}

tib_t * get_current_task()
{
    return NULL;
}

void schedule()
{

}

void schedule_task_ready(int queue_num, tib_t *task)
{

}
void schedule_task_blocked(tib_t *task)
{

}
void unschedule_task(tib_t *task){

}
void init_scheduler()
{

}
tib_t *name_to_tcb(char *name)
{
    return NULL;
}

void print_ready_queue(int queue_num)
{

}
void print_scheduler_state()
{
    
}