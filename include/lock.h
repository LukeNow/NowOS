#ifndef _LOCK_H
#define _LOCK_H

#include <kdef.h>
#define LOCK_BUSY 1
#define LOCK_FREE 0

typedef uint32_t spinlock_t;

#define SPINLOCK_INIT(lock) \
    static spinlock_t (lock) = LOCK_FREE

void init_spinlock(spinlock_t * lock);

void spin_lock(spinlock_t * lock);

void spin_unlock(spinlock_t * lock);

int spin_trylock(spinlock_t * lock);

#endif