#include <kdef.h>
#include <atomic.h>
#include <kheap.h>
#include <kprint.h>
#include <shared_pool.h>
#include <string.h>

#define FREE_ENTRY_SIZE (sizeof(uint32_t) * 8)

static uint32_t set_next_index(shared_pool_t * pool)
{
    uint32_t free_list_size = (pool->max_obj_num / FREE_ENTRY_SIZE) + 1;
    uint32_t found = 0;
    for (uint32_t i = 0; i < free_list_size; i++) {
        uint32_t free_mask = pool->free_list[i];
        if (free_mask == ~0x0) {
            found += FREE_ENTRY_SIZE;
            continue; // This entry is full
        } 
        for (uint32_t j = 0; j < FREE_ENTRY_SIZE; j++) {
            uint32_t try_mask = free_mask | (1 << j);
            if (found >= pool->max_obj_num) 
                return MAX_INT;
            if (try_mask == free_mask) {
                found ++;
                continue;
            }
            if (atomic_cmpxchg_strong_32(&pool->free_list[i], &free_mask, 
                                         try_mask, mem_order_acq_rel)) {
                return j + (i * FREE_ENTRY_SIZE);
            }
            /* If we failed then we are either contending for an entry with
            another thread or the entry is already taken */
        }
    }
    /* Return max int if the whole pool is taken */
    return MAX_INT;
}

bool shared_pool_init(shared_pool_t * pool, uint32_t max_obj_num, size_t type_size)
{
    ASSERT(pool && type_size >= sizeof(uint8_t));

    void * buf = kmalloc(max_obj_num * type_size);
    if (buf == NULL) {
        WARN("Shared pool kmalloc failed\n");
        return false;
    }
    memset(buf, 0, max_obj_num * type_size);
    pool->buf = buf;

    uint32_t free_list_size = (max_obj_num / FREE_ENTRY_SIZE) + 1;
    buf = kmalloc(free_list_size * sizeof(uint32_t));
    if (buf == NULL) {
        WARN("Obj pool kmalloc failed\n");
        return false;
    }
    memset(buf, 0, free_list_size * sizeof(uint32_t));
    pool->free_list = buf;

    pool->max_obj_num = max_obj_num;
    pool->type_size = type_size;

    return true;
}

void shared_pool_destroy(shared_pool_t * pool)
{
    ASSERT(pool);
    kfree(pool->buf);
    kfree(pool->free_list);
}

void * shared_pool_get_next(shared_pool_t * pool)
{
    ASSERT(pool);
    
    uint32_t next_index = set_next_index(pool);
    if (next_index == MAX_INT) {
        return NULL;
    }

    ASSERT(next_index < pool->max_obj_num);

    size_t start_byte = next_index * pool->type_size;
    return (uint32_t)pool->buf + start_byte;
}

uint32_t shared_pool_get_index(shared_pool_t * pool, void * entry)
{
    ASSERT(pool && entry);
    
    size_t start_byte = entry - pool->buf;
    ASSERT(start_byte <= pool->max_obj_num * pool->type_size);
    return start_byte / pool->type_size;
}

bool shared_pool_free_index(shared_pool_t * pool, uint32_t index)
{
    ASSERT(pool && index < pool->max_obj_num);

    void * entry = (void *)((uint32_t)pool->buf + (index * pool->type_size));
    return shared_pool_free_entry(pool, entry);
}

bool shared_pool_free_entry(shared_pool_t * pool, void * entry)
{
    ASSERT(pool && entry);
    
    uint32_t index = shared_pool_get_index(pool, entry);
    uint32_t * free_list_entry = pool->free_list + (index / FREE_ENTRY_SIZE);
    uint32_t free_list = *free_list_entry;
    uint32_t try_list = free_list &~ (1 << (index % FREE_ENTRY_SIZE));

    /* There is a potential bug/problem where two threads are trying to free the same
       entry. If thread 1 gets prempted here, and thread two goes ahead and frees,
       and then another thread allocates an entry in this spot, thread 1 will free
       the entry that is not the original entry, because the compare still
       succeeds. Just a thought! 
       
       This is actually called the ABA problem. 
       TODO implement some sort of tag or count to increment so we can tell when
       a value in our list is different than it was previously. */
    return atomic_cmpxchg_strong_32(free_list_entry, &free_list, try_list, mem_order_acq_rel);
}