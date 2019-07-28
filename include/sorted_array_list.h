#ifndef _SORTED_ARRAY_LIST_H
#define _SORTED_ARRAY_LIST_H
#include <stdint.h>


typedef struct sorted_array_list {
	void **arr;
	uint32_t arr_size;
	uint32_t curr_size;
	uint32_t expand_size;
	void (*expand)();
	void (*contract)();
}sorted_array_list_t;

void init_sorted_list(sorted_array_list_t *list, void **arr_ptr, 
		uint32_t expand_size, void (*expand)(), 
		void (*contract)());

int insert_list(void *item, sorted_array_list_t *list);

int remove_list(void *item, sorted_array_list_t *list);

void print_list();
#endif
