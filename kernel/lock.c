#include <lock.h>
#include <atomic.h>

void init_spinlock(spinlock_t * lock)
{
    *lock = LOCK_FREE;
}

void spin_lock(spinlock_t * lock)
{
    while (1) {
        if (atomic_exchange_32(lock, LOCK_BUSY, mem_order_acq_rel) == LOCK_FREE)
            return;
        while (*lock == LOCK_BUSY) {
            /* Do the cpu_pause here to relieve some bus usage
               while we wait on the lock */
            cpu_pause();
        }
    }
}

void spin_unlock(spinlock_t * lock)
{
    /* Reminder! When we leave a critical section we really
       want to make sure that all loads/stores have been completed
       and are visible to the next thread (possibly on another cpu)
       entering the critical section, so lets mem_barrier!*/
    mem_barrier();
    *lock = LOCK_FREE;
}

int spin_trylock(spinlock_t * lock)
{
    return atomic_exchange_32(lock, LOCK_BUSY, mem_order_acq_rel);
}