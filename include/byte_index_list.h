#ifndef _BYTE_INDEX_H
#define _BYTE_INDEX_H

#include <stddef.h>
#include <stdint.h>

#define BYTE_INDEX_LIST_INIT(list) \
	static byte_index_list_t (list) = { 0, 0, 0 }

typedef struct byte_index_list {
	char *array;
	size_t size; //in bytes
	char *free_ptr;
}byte_index_list_t;

void init_byte_index_list(void *array, size_t size, byte_index_list_t *list);
void byte_index_release_tbl(unsigned int index, byte_index_list_t *list);
void byte_index_set_tbl(unsigned int index, byte_index_list_t *list);
uint32_t byte_index_check_tbl(unsigned int index, byte_index_list_t *list);
unsigned int byte_index_get_free_slot(byte_index_list_t *list);

#endif
