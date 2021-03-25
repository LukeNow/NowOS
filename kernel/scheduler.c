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

bool scheduler_ready = false;

static void test_loop_1()
{
    
    
    int i = 0;
    for (;;) {
        
        i++;   
        processor_t * proc = processor_get_info();
        int j = 0;
        int k = 0;
        int ii = i;


        kprint(INFO, "TEST LOOP ON PROC %d\n", proc->id);


        scheduler_yield();
    }

    VERIFY_UNREACHED();
}


static void idle_loop()
{
    for (;;) {


        processor_t * proc = processor_get_info();
        kprint(INFO, "IDLE LOOP tid %d ON PROC %d\n", ((tib_t *)(proc->current_thread))->tid, processor_get_id());
    
        scheduler_yield();
    }

    VERIFY_UNREACHED();
}

static tib_t * pull_next_runnable_thread()
{
    return linked_list_pop(&ready_list);
}

void scheduler_leave()
{
    processor_t * proc = processor_get_info();
    proc->in_scheduler = false;
    spin_unlock(&sched_lock);
    enable_int();

    //kprint(INFO, "Leaving scheduler %d with thread %d\n", (uint32_t)proc->id, ((tib_t *)(proc->current_thread))->tid);

}
void scheduler_leave_and_queue(tib_t * from_thread)
{
    processor_t * proc = processor_get_info();
    proc->in_scheduler = false;
    from_thread->is_active = false;
    from_thread->state = THREAD_READY;
    scheduler_push_ready_thread(from_thread);
    spin_unlock(&sched_lock);
    enable_int();

    //kprint(INFO, "L N Q proc %d with thread %d\n", (uint32_t)proc->id, from_thread->tid);
}


void scheduler_push_ready_thread(tib_t * thread)
{
    linked_list_enqueue(thread, &ready_list);
}

void scheduler_yield()
{
    disable_int(); 
    
    processor_t * proc = processor_get_info();
    ASSERT(proc);
    
    tib_t * from_thread = proc->current_thread;
    ASSERT(from_thread->state = THREAD_RUNNING);
    
    proc->in_scheduler = true;
    
    //kprint(INFO, "PROC %d yielding tid %d\n", proc->id, from_thread->tid);
    spin_lock(&sched_lock);
    //kprint(INFO, "PROC %d ENTERED\n", proc->id);
    tib_t * to_thread = pull_next_runnable_thread();
    if (to_thread == NULL) {
        scheduler_leave();
        return;
    }

    //kprint(INFO, "PROC %d pulled tid %d\n", proc->id, to_thread->tid);
    /* Save some things that might or might not have changed since
       we last saved/created this threads state.
       All other general registers dont matter, 
       and esp, ebp, eip will be saved when we call thread_switch_sync */
    from_thread->cpu_state.cr3 = get_cr3();
	from_thread->cpu_state.cr2 = get_cr2();
	from_thread->cpu_state.cr0 = get_cr0();
	from_thread->cpu_state.eflags = get_eflags();

    proc->current_thread = to_thread;
    /* We should put a memory barrier here to effectively flush
       data in flight, because this thread, or the thread after us
       might be on a different CPU and we need to make all the
       writes and reads we have done transparent */
    //kprint(INFO, "Proc %d switching from tid %d -> tid %d\n", proc->id, from_thread->tid, to_thread->tid);
    mem_barrier();
    thread_switch_sync(from_thread, to_thread);
}

void scheduler_init()
{
    processor_set_info();
    processor_t * proc = processor_get_info();

    if (proc->id == 0) {
        kprint(INFO, "SCHEDULER INIT\n");
        linked_list_stat_init(&ready_list);

        
        tib_t * test_thread = create_thread(test_loop_1, "Test loop 1");
	    scheduler_push_ready_thread(test_thread);
        test_thread = create_thread(test_loop_1, "Test loop 2");
	    scheduler_push_ready_thread(test_thread);

        scheduler_ready = true;
        mem_barrier();
    }
        
    while (atomic_load_16(&scheduler_ready, mem_order_consume) != true);

    spin_lock(&sched_lock);
    
    tib_t * idle_thread = create_thread(idle_loop, "Idle loop");
    ASSERT(idle_thread);
    idle_thread->state = THREAD_RUNNING;

    proc->current_thread = idle_thread;

    _thread_enter(idle_thread);
}