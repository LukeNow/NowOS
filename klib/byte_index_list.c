#include <stddef.h>
#include <stdint.h>
#include "../include/kprint.h"
#include "../include/byte_index_list.h"

void init_byte_index_list(void *array, size_t size, byte_index_list_t *list)
{
	list->array = array;
	list->size = size;
	list->free_ptr = array;
}

static void update_free_ptr(byte_index_list_t *list)
{
	uint8_t temp_entry = *(list->free_ptr);
	
	while (list->free_ptr < (list->array + list->size)) {
		if (temp_entry != 0xFF){
			return;
		}
		temp_entry = *++list->free_ptr;
	}

	PANIC("WE COULDNT FIND ANY MORE FREE SPACE");
}

void byte_index_set_tbl(unsigned int index, byte_index_list_t *list) 
{
	unsigned int tbl_index;
	unsigned int byte_index;
	uint8_t temp_entry;
	
	tbl_index = index / 8;
	byte_index = index % 8;
	
	temp_entry = *(list->array + tbl_index);
	
	*(list->array + tbl_index) = (temp_entry | (1 << byte_index));
}

void byte_index_release_tbl(unsigned int index, byte_index_list_t *list)
{
	unsigned int tbl_index;
	unsigned int byte_index;
	uint8_t temp_entry;

	tbl_index = index / 8;
	byte_index = index % 8;
	
	if (list->free_ptr > (list->array + tbl_index))
		list->free_ptr = (list->array + tbl_index);

	temp_entry = *(list->array + tbl_index);
	
	*(list->array + tbl_index) = (temp_entry & ~(1 << byte_index));
}

uint32_t byte_index_check_tbl(unsigned int index, byte_index_list_t *list)
{
	unsigned int tbl_index;
	unsigned int byte_index;
	unsigned int ret;
	uint8_t temp_entry;
	
	tbl_index = index / 8;
	byte_index = index % 8;
	
	temp_entry = *(list->array + tbl_index);
	ret = (temp_entry & (1 << byte_index));
	ret = ret >> byte_index;
	return ret;
}

unsigned int byte_index_get_free_slot(byte_index_list_t *list)
{
	uint8_t temp_entry;
	uint32_t tbl_index;
	uint32_t ret;
	
	uint32_t byte_index = -1; //set so we know if we didnt change
	unsigned int i = 0;

	tbl_index =(uint32_t)(list->free_ptr - list->array);

	temp_entry = list->array[tbl_index];
	if (temp_entry == 0xFF) {
		update_free_ptr(list);
	}
	
	tbl_index =(uint32_t)(list->free_ptr - list->array);
	temp_entry = list->array[tbl_index];
	if (temp_entry == 0xFF) {
		PANIC("MEM_TBL NOT FREE???");
	}
	
	for (i = 0; i < 8; i++) {
		if ((temp_entry & (1 << i)) == 0) {
			byte_index = i;
			break;
		}
	}
	
	if (byte_index == -1) {
		PANIC("BYTE_INDEX NOT SET");
	}
	
	ret = (tbl_index * 8) + byte_index;
	byte_index_set_tbl(ret, list);
	return ret;
}
