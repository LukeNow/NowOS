#include <kdef.h>
#include <kheap.h>
#include <string.h>
#include <circ_buf.h>
#include <kprint.h>

int push_circ_buf(void * entry_src, circ_buf_t * circ_buf)
{
	if (!circ_buf || circ_buf->curr_size == circ_buf->size)
		return FAILURE;
	
	unsigned int push_index = (circ_buf->curr_index + circ_buf->curr_size) %
				     circ_buf->size;

	uint8_t * src = (uint8_t *)entry_src;
	uint8_t * dst = (uint8_t *)&circ_buf->buf[push_index];
	
	memcpy(dst, src, circ_buf->type_size);
	
	circ_buf->curr_size++;
	
	ASSERT(circ_buf->curr_size <= circ_buf->size);

	return SUCCESS;
}

int pop_circ_buf(void * entry_dst, circ_buf_t * circ_buf)
{
	if (circ_buf->curr_size == 0)
		return FAILURE;
	
	unsigned int pop_index = circ_buf->curr_index;
	
	uint8_t * src = (uint8_t *)&circ_buf->buf[pop_index];
	uint8_t * dst = (uint8_t *)entry_dst;
	
	memcpy(dst, src, circ_buf->type_size);
	
	circ_buf->curr_index = (circ_buf->curr_index + 1) % circ_buf->size;
	circ_buf->curr_size--;
	
	return SUCCESS;
}

void print_circ_buf(circ_buf_t * buf)
{
	kprint(INFO, "---Printing circ_buf---\n");
	kprint(INFO, "Size of buf: %d Size of type: %d\n", buf->size, buf->type_size);
	kprint(INFO, "Curr_size: %d\n", buf->curr_size);
	kprint(INFO, "Curr_index: %d\n", buf->curr_index);
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
