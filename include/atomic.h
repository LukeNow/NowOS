#ifndef _ATOMIC_H
#define _ATOMIC_H

#define atomic_xadd(P, V) __sync_fetch_and_add((P), (V))
#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))
#define atomic_inc(P) __sync_add_and_fetch((P), 1)
#define atomic_dec(P) __sync_add_and_fetch((P), -1) 
#define atomic_add(P, V) __sync_add_and_fetch((P), (V))
#define atomic_set_bit(P, V) __sync_or_and_fetch((P), 1<<(V))
#define atomic_clear_bit(P, V) __sync_and_and_fetch((P), ~(1<<(V)))

/* Compile read-write barrier */
#define mem_barrier() asm volatile("": : :"memory")

/* Pause instruction to prevent excess processor bus usage */ 
#define cpu_pause() asm volatile("pause\n": : :"memory")

typedef enum mem_order_t {
    mem_order_relaxed = __ATOMIC_RELAXED,
    mem_order_consume = __ATOMIC_CONSUME,
    mem_order_acquire = __ATOMIC_ACQUIRE,
    mem_order_release = __ATOMIC_RELEASE,
    mem_order_acq_rel = __ATOMIC_ACQ_REL,
    mem_order_seq_cst = __ATOMIC_SEQ_CST
} mem_order_t;

void atomic_signal_fence(mem_order_t order);
void atomic_thread_fence(mem_order_t order);
uint32_t atomic_fetch_add_32(uint32_t * ptr, uint32_t val, mem_order_t order);
uint32_t atomic_fetch_add_16(uint16_t * ptr, uint16_t val, mem_order_t order);
uint32_t atomic_fetch_sub_32(uint32_t * ptr, uint32_t val, mem_order_t order);
uint32_t atomic_fetch_sub_16(uint16_t * ptr, uint16_t val, mem_order_t order);
uint32_t atomic_fetch_and_32(uint32_t * ptr, uint32_t val, mem_order_t order);
uint32_t atomic_fetch_and_16(uint16_t * ptr, uint16_t val, mem_order_t order);
uint32_t atomic_fetch_xor_32(uint32_t * ptr, uint32_t val, mem_order_t order);
uint32_t atomic_fetch_xor_16(uint16_t * ptr, uint16_t val, mem_order_t order);
uint32_t atomic_fetch_or_32(uint32_t * ptr, uint32_t val, mem_order_t order);
uint32_t atomic_fetch_or_16(uint16_t * ptr, uint16_t val, mem_order_t order);
uint32_t atomic_fetch_nand_32(uint32_t * ptr, uint32_t val, mem_order_t order);
uint32_t atomic_fetch_nand_16(uint16_t * ptr, uint16_t val, mem_order_t order);
uint32_t atomic_exchange_32(uint32_t * ptr, uint32_t val, mem_order_t order);
uint32_t atomic_exchange_16(uint16_t * ptr, uint16_t val, mem_order_t order);
bool atomic_cmpxchg_strong_32(uint32_t * ptr, uint32_t * expected, 
                              uint32_t desired, mem_order_t order);
bool atomic_cmpxchg_strong_16(uint16_t * ptr, uint16_t * expected, 
                              uint16_t desired, mem_order_t order);
bool atomic_ptrcmpxchg_strong_32(uint32_t ** ptr, uint32_t ** expected,
								 uint32_t desired, mem_order_t order);
bool atomic_ptrcmpxchg_strong_16(uint16_t ** ptr, uint16_t ** expected,
								 uint16_t desired, mem_order_t order);
uint32_t atomic_load_32(uint32_t * ptr, mem_order_t order);
uint32_t atomic_load_16(uint16_t * ptr, mem_order_t order);

#endif