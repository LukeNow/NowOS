#ifndef _CIRC_BUF_ARRAY_H
#define _CIRC_BUF_ARRAY_H

#include <stddef.h>
#include <lock.h>

typedef struct circ_buf_t {
	unsigned int curr_size;
	unsigned int curr_index;
	unsigned int size;
	size_t type_size;
	void **buf;
	spinlock_t lock;
}circ_buf_t;

int push_circ_buf(void * entry_src, circ_buf_t * circ_buf);
int pop_circ_buf(void * entry_dst, circ_buf_t * circ_buf);
void init_circ_buf(unsigned int size, size_t type_size, circ_buf_t * circ_buf);
void print_circ_buf(circ_buf_t * buf);
void destroy_circ_buf(circ_buf_t * circ_buf);
#endif
