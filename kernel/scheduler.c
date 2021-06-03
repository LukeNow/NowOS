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
#include <apic.h>
#include <timer.h>

SPINLOCK_INIT(sched_lock);
LINKED_LIST_INIT(ready_list);

bool scheduler_ready = false;

#define IDLE_TIME 100000


static void print_thread_state(tib_t * thread)
{
    kprint(WARN, "*** THREAD STATE ON PROC %d ***\n", processor_get_id());
    kprint(INFO, "EAX %x, EBX %x, ECX %x\n", thread->cpu_state.eax, thread->cpu_state.ebx, thread->cpu_state.ecx);
    kprint(INFO, "EDX %x, ESI %x, EDI %x\n", thread->cpu_state.edx, thread->cpu_state.esi, thread->cpu_state.edi);
    kprint(INFO, "EBP %x, ESP %x, EIP %x\n", thread->cpu_state.ebp, thread->cpu_state.esp, thread->cpu_state.eip);
    kprint(INFO, "EFLAGS %x, CR3 %x, CR0 %x\n", thread->cpu_state.eflags, thread->cpu_state.cr3, thread->cpu_state.cr0);
}



static void test_loop_1()
{
    
       

    int i = 0;
    for (;;) {
        
        i++;   
        processor_t * proc = processor_get_info();
        int j = 0;
        int k = 0;
        int ii = i;


        kprint(INFO, "TEST LOOP tid %d ON PROC %d\n", ((tib_t *)(proc->current_thread))->tid ,proc->id);
        
        //print_thread_state(proc->current_thread);
        //for (int time = 0; time < (IDLE_TIME / (proc->id + 1)) ; time++) ;


        //scheduler_yield();
    }

    VERIFY_UNREACHED();
}

static void idle_loop()
{
    for (;;) {


        processor_t * proc = processor_get_info();
        
        kprint(INFO, "IDLE LOOP tid %d ON PROC %d\n", ((tib_t *)(proc->current_thread))->tid, processor_get_id());
        
        //print_thread_state(proc->current_thread);
        //for (int time = 0; time < IDLE_TIME; time++) ;
        

        scheduler_yield();
    }

    VERIFY_UNREACHED();
}

static void save_register_state(tib_t * thread, int_state_t * state)
{
    thread->cpu_state.eax = state->eax;
    thread->cpu_state.ebx = state->ebx;
    thread->cpu_state.ecx = state->ecx;
    thread->cpu_state.edx = state->edx;
    thread->cpu_state.esi = state->esi;
    thread->cpu_state.edi = state->edi;
    thread->cpu_state.ebp = state->ebp;
    thread->cpu_state.esp = state->esp;
    /* The interrupt would normally return to this EIP, but we
       are instead saving it because this is where we will return
       when we next switch to this thread */
    thread->cpu_state.eip = state->eip; 
    thread->cpu_state.eflags = state->eflags;
    thread->cpu_state.cr3 = state->cr3;
    thread->cpu_state.cr2 = state->cr2;
    thread->cpu_state.cr0 = state->cr0;
}

static tib_t * pull_next_runnable_thread()
{
    tib_t * thread = linked_list_pop(&ready_list);
    if (thread == NULL)
        return thread;

    ASSERT(thread->state == THREAD_READY);
    ASSERT(!thread->is_active);

    thread->state = THREAD_RUNNING;
    thread->is_active = true;
    return thread;
}

void scheduler_push_ready_thread(tib_t * thread)
{
    thread->is_active = false;
    thread->state = THREAD_READY;
    linked_list_enqueue(thread, &ready_list);
}

void scheduler_async_leave()
{
    processor_t * proc = processor_get_info();
    proc->in_scheduler = false;
    spin_unlock(&sched_lock);
}

void scheduler_leave()
{
    processor_t * proc = processor_get_info();
    proc->in_scheduler = false;
    spin_unlock(&sched_lock);
    enable_int();
}

void scheduler_leave_and_queue(tib_t * from_thread)
{
    processor_t * proc = processor_get_info();
    proc->in_scheduler = false;
    
    ASSERT(from_thread->is_active);
    ASSERT(from_thread->state == THREAD_RUNNING);
    
    
    scheduler_push_ready_thread(from_thread);
    spin_unlock(&sched_lock);
    //enable_int();
}

void scheduler_async_enter(int_state_t * state)
{
    processor_t * proc = processor_get_info();
    
    /* If we are already in the scheduler when this handler was called, 
       we might as well leave because the thread was probably already
       yielding itself */
    if (proc && proc->in_scheduler)
        return;

    //tib_t * from_thread = proc->current_thread;
    //save_register_state(from_thread, state);
    
    spin_lock(&sched_lock);
    
    tib_t * to_thread = pull_next_runnable_thread();
    if (to_thread == NULL) {
        scheduler_async_leave();
        return;
    }
    
    //kprint(INFO, "ASYNC state %x\n", state);
    tib_t * from_thread = proc->current_thread;
    save_register_state(from_thread, state);
    
    proc->current_thread = to_thread;
    scheduler_push_ready_thread(from_thread); 

    ASSERT(from_thread != to_thread);
    //kprint(INFO, "ASYNC switching from tid %d -> tid %d on PROC %d\n", from_thread->tid, to_thread->tid, proc->id);
    /* We can safely leave before switching because we saved from_threads state
       and pushed it to the queue, as well as popped the next thread we are
       going to execute */
    //scheduler_async_leave();
    thread_switch_async(from_thread, to_thread, state);
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

    proc->current_thread = to_thread;
    
    ASSERT(to_thread != from_thread);
    thread_switch_sync(from_thread, to_thread);
}

void scheduler_create_idle_thread(uint16_t cpu_id, uint32_t stack_addr)
{

    tib_t * idle_thread = thread_create_thread(idle_loop, "Idle thread", stack_addr);
    ASSERT(idle_thread);
    idle_thread->state = THREAD_RUNNING;
    idle_thread->is_active = true;

    processor_t * proc = processor_get_info_id(cpu_id);
    proc->current_thread = idle_thread;
}

void scheduler_init()
{
    processor_set_info();
    processor_t * proc = processor_get_info();

    if (proc->id == 0) {
        init_threading();
        linked_list_stat_init(&ready_list);

        
        tib_t * test_thread = thread_create_thread(test_loop_1, 0, "Test loop 1");
	    scheduler_push_ready_thread(test_thread);

        scheduler_ready = true;
        mem_barrier();
    }

    /*
    if (proc->id == 1){

        kprint(INFO, "SCHEDULER 1 INIT\n");
    }
    else {


        kprint(INFO, "SCHEDULER 0 INIT\n");
    }*/
    
    while (atomic_load_16(&scheduler_ready, mem_order_consume) != true);

    timer_setup_lapic_timer(&proc->timer, scheduler_async_enter);

    spin_lock(&sched_lock);
    
    tib_t * idle_thread = thread_create_thread(idle_loop, 0, "Idle loop");
    ASSERT(idle_thread);
    idle_thread->state = THREAD_RUNNING;
    idle_thread->is_active = true;
    proc->current_thread = idle_thread;

    _thread_enter(idle_thread);
}