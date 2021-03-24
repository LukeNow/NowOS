#ifndef _KERNDEF_H
#define _KERNDEF_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_THREAD_COUNT 128
#define MAX_CPU_COUNT 8
#define THREAD_STACK_SIZE 1024

/* General return definitions */
#define SUCCESS 0
#define FAILURE 1

/* General numerical/address definitions */
#define PAGE_SIZE 4096
#define KERN_MAX_VIRTUAL_ADDR 0xFFFFFFFF
#define KERN_VIRTUAL_ADDR 0xC0000000

/* MATH DEFINES */
#define MAX_INT ~0x0

/* Useful paging macros */
#define PHYS_ADDR_TO_PAGE(x) ((x) & ~0x3ff)
#define PHYS_ADDR_TO_PAGE_INDEX(x)  (((x) & ~0xfff) >> 12)

#define IS_ALIGNED(x, size) ((x) % (size) == 0)
#define ROUND_DOWN(x, size) ((x) - ((x) % (size)))
#define ROUND_UP(x, size) IS_ALIGNED((x), (size)) ? \
		(x) : ((x) + ((size) - ((x) % (size)))) 
#define ALIGN_DOWN(x, size) ROUND_DOWN((x) + (size) - 1, (size))
#define ALIGN_UP(x, size) ROUND_UP((x), (size))

/* Generally useful macros for making human readable code */
#define KB(x) (1024 * (x))
#define MB(x) (KB(1) * 1024 * (x))
#define GB(x) (MB(1) * 1024 * (x))

/* General typedefs 
 *
 * TODO Should they be here?
 * */
typedef long long time_t;
typedef uint32_t register_t;
typedef uint8_t bit_t;
/* 
 * Scheduler definitons
 */
#define NOT_SCHEDULED -1

/*
 * Task and process definitions
 */

#define PROC_NAME_LEN 64
#define TASK_NAME_LEN 64
#define MAX_TASKS_NUM 16
#define MAX_PROC_NUM 1024
#define MAX_PROC_ID (MAX_PROC_NUM - 1)
#define MAX_TASK_ID (MAX_TASKS_NUM - 1)

#define FAIL_ID -1

typedef int priority_t;
typedef unsigned int id_t;
typedef unsigned int proc_id_t;
typedef unsigned int task_id_t;
typedef uint8_t flags_t;

#define MAKE_ID(proc_id, task_id) \
	((proc_id) << 8) | (task_id)

#define GET_PROC_ID(id) \
	((id) >> 8)

#define GET_TASK_ID(id) \
	((id) & (MAX_TASK_ID))

/* 
 * Kernel ID definitions
 */

#define KMAIN_PID 0
#define SYSTEM_PID 1
#define TIMER_PID 2

#define ANY_ID (MAKE_ID(-1, 0))
#define KMAIN_ID (MAKE_ID(KMAIN_PID, 0))
#define SYSTEM_ID (MAKE_ID(SYSTEM_PID, 0))
#define TIMER_ID (MAKE_ID(TIMER_PID, 0))

/*
 * IPC definitions
 */

#define MESSAGE_BUF_LEN 4

#endif 
