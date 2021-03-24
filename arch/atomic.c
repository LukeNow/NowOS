#include <kdef.h>
#include <atomic.h>

inline void atomic_signal_fence(mem_order_t order)
{
	return __atomic_signal_fence(order);
}

inline void atomic_thread_fence(mem_order_t order)
{
	return __atomic_thread_fence(order);
}

inline uint32_t atomic_fetch_add_32(uint32_t * ptr, uint32_t val, mem_order_t order)
{
	return __atomic_fetch_add(ptr, val, order);
}

inline uint32_t atomic_fetch_add_16(uint16_t * ptr, uint16_t val, mem_order_t order)
{
	return __atomic_fetch_add(ptr, val, order);
}

inline uint32_t atomic_fetch_sub_32(uint32_t * ptr, uint32_t val, mem_order_t order)
{
	return __atomic_fetch_sub(ptr, val, order);
}

inline uint32_t atomic_fetch_sub_16(uint16_t * ptr, uint16_t val, mem_order_t order)
{
	return __atomic_fetch_sub(ptr, val, order);
}

inline uint32_t atomic_fetch_and_32(uint32_t * ptr, uint32_t val, mem_order_t order)
{
	return __atomic_fetch_and(ptr, val, order);
}

inline uint32_t atomic_fetch_and_16(uint16_t * ptr, uint16_t val, mem_order_t order)
{
	return __atomic_fetch_and(ptr, val, order);
}

inline uint32_t atomic_fetch_xor_32(uint32_t * ptr, uint32_t val, mem_order_t order)
{
	return __atomic_fetch_xor(ptr, val, order);
}

inline uint32_t atomic_fetch_xor_16(uint16_t * ptr, uint16_t val, mem_order_t order)
{
	return __atomic_fetch_xor(ptr, val, order);
}

inline uint32_t atomic_fetch_or_32(uint32_t * ptr, uint32_t val, mem_order_t order)
{
	return __atomic_fetch_or(ptr, val, order);
}

inline uint32_t atomic_fetch_or_16(uint16_t * ptr, uint16_t val, mem_order_t order)
{
	return __atomic_fetch_or(ptr, val, order);
}

inline uint32_t atomic_fetch_nand_32(uint32_t * ptr, uint32_t val, mem_order_t order)
{
	return __atomic_fetch_nand(ptr, val, order);
}

inline uint32_t atomic_fetch_nand_16(uint16_t * ptr, uint16_t val, mem_order_t order)
{
	return __atomic_fetch_nand(ptr, val, order);
}

inline uint32_t atomic_exchange_32(uint32_t * ptr, uint32_t val,
								   mem_order_t order)
{
	return __atomic_exchange_n(ptr, val, order);
}

inline uint32_t atomic_exchange_16(uint16_t * ptr, uint16_t val,
								   mem_order_t order)
{
	return __atomic_exchange_n(ptr, val, order);
}

inline bool atomic_cmpxchg_strong_32(uint32_t * ptr, uint32_t * expected,
									 uint32_t desired, mem_order_t order)
{
	if (order == mem_order_acq_rel || order == mem_order_release)
		return __atomic_compare_exchange_n(ptr, expected, desired, false, 
										   mem_order_release, mem_order_acquire);
	else
		return __atomic_compare_exchange_n(ptr, expected, desired, false, order, 
										   order);
}

inline bool atomic_cmpxchg_strong_16(uint16_t * ptr, uint16_t * expected,
									 uint16_t desired, mem_order_t order)
{
	if (order == mem_order_acq_rel || order == mem_order_release)
		return __atomic_compare_exchange_n(ptr, expected, desired, false, 
										   mem_order_release, mem_order_acquire);
	else
		return __atomic_compare_exchange_n(ptr, expected, desired, false, order, 
										   order);
}

inline bool atomic_ptrcmpxchg_strong_32(uint32_t ** ptr, uint32_t ** expected,
									 	uint32_t desired, mem_order_t order)
{
	if (order == mem_order_acq_rel || order == mem_order_release)
		return __atomic_compare_exchange_n(ptr, expected, desired, false, 
										   mem_order_release, mem_order_acquire);
	else
		return __atomic_compare_exchange_n(ptr, expected, desired, false, order, 
										   order);
}

inline bool atomic_ptrcmpxchg_strong_16(uint16_t ** ptr, uint16_t ** expected,
									 	uint16_t desired, mem_order_t order)
{
	if (order == mem_order_acq_rel || order == mem_order_release)
		return __atomic_compare_exchange_n(ptr, expected, desired, false, 
										   mem_order_release, mem_order_acquire);
	else
		return __atomic_compare_exchange_n(ptr, expected, desired, false, order, 
										   order);
}

inline uint32_t atomic_load_32(uint32_t * ptr, mem_order_t order)
{
	return __atomic_load_n(ptr, order);
}

inline uint32_t atomic_load_16(uint16_t * ptr, mem_order_t order)
{
	return __atomic_load_n(ptr, order);
}