#ifndef _KERNDEF_H
#define _KERNDEF_H

#include <stdint.h>

/* General numerical/address definitions */
#define PAGE_SIZE 4096
#define KERN_MAX_VIRTUAL_ADDR 0xFFFFFFFF
#define KERN_VIRTUAL_ADDR 0xC0000000

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

/*
 * Task and process definitions
 */
#define MAX_TASKS_NUM 256
#define PROC_NAME_LEN 64
#define TASK_NAME_LEN 64
#define MAX_PROC_ID 0xFFFFFF
#define MAX_TASK_ID 0xFF

typedef int priority_t;
typedef unsigned int id_t;
typedef unsigned int proc_id_t;
typedef unsigned int task_id_t;

#endif 
