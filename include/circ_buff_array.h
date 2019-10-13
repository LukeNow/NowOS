#ifndef _CIRC_BUFF_ARRAY_H
#define _CIRC_BUFF_ARRAY_H

typedef struct circ_buff_array_t {
	unsigned int size;
	unsigned int curr_index;
	void **buff;
}circ_buff_array_t;


#endif
