#include "../include/kdef.h"
#include "../include/kheap.h"
#include "../include/string.h"
#include "../include/circ_buf.h"
#include "../include/kprint.h"

int push_circ_buf(void * entry_src, circ_buf_t * circ_buf)
{
	if (circ_buf->curr_size == circ_buf->size)
		return FAILURE;
	
	unsigned int push_index = (circ_buf->curr_index + circ_buf->curr_size) %
				     circ_buf->size;

	uint8_t * src = (uint8_t *)entry_src;
	uint8_t * dst = (uint8_t *)circ_buf->buf[push_index];
	
	for (size_t i = 0; i < circ_buf->type_size; i++){
		*dst = *src;
		dst++;
		src++;
	}

	circ_buf->curr_size++;
	
	ASSERT(circ_buf->curr_size <= circ_buf->size);

	return SUCCESS;
}

int pop_circ_buf(void * entry_dst, circ_buf_t * circ_buf)
{
	if (circ_buf->size == 0)
		return FAILURE;
	
	unsigned int pop_index = circ_buf->curr_index;
	
	uint8_t * src = (uint8_t *)circ_buf->buf[pop_index];
	uint8_t * dst = (uint8_t *)entry_dst;

	for (size_t i = 0; i < circ_buf->type_size; i++){
		*dst = *src;
		dst++;
		src++;
	}

	circ_buf->curr_index = (circ_buf->curr_index + 1) % circ_buf->size;
	circ_buf->curr_size--;
	
	return SUCCESS;
}

void init_circ_buf(unsigned int size, size_t type_size, circ_buf_t * buf)
{
	memset(buf, 0, sizeof(circ_buf_t));
	buf->size = size;
	buf->type_size = type_size;
	buf->buf = kmalloc(size * type_size);
}

void destroy_circ_buf(circ_buf_t * circ_buf)
{
	kfree(circ_buf->buf);
}
