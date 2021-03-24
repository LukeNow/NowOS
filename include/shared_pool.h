#ifndef _SHARED_POOL_H
#define _SHARED_POOL_H
#include <kdef.h>

typedef struct shared_pool_t {
    void * buf;
    uint32_t * free_list;
    uint32_t max_obj_num;
    size_t type_size;
} shared_pool_t;

bool shared_pool_init(shared_pool_t * pool, uint32_t max_obj_num, size_t type_size);
void shared_pool_destroy(shared_pool_t * pool);
void * shared_pool_get_next(shared_pool_t * pool);
uint32_t shared_pool_get_index(shared_pool_t * pool, void * entry);
bool shared_pool_free_index(shared_pool_t * pool, uint32_t index);
bool shared_pool_free_entry(shared_pool_t * pool, void * entry);
#endif