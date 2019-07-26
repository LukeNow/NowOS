#include <stdint.h>
#include "../include/sorted_array_list.h"
#include "../include/kprint.h"


void init_sorted_list(sorted_array_list_t *list, void **init_arr_ptr, 
		      uint32_t init_expand_size, void (*init_expand)(), 
		      void (*init_contract)())
{
	list->arr = init_arr_ptr;
	list->arr_size = init_expand_size;
	list->curr_size = 1;
	list->expand_size = init_expand_size;
	list->expand = init_expand;
	list->contract = init_contract;
	kprint(INFO, "ARR_SIZE %d, EXPAN_SIZE %d\n", 
			list->arr_size, list->expand_size);
}



static void insert_item(void *item, unsigned int index, 
			sorted_array_list_t *list)
{
	int i;

	if (list->curr_size == list->arr_size){
		kprint(INFO, "EXPANDED\n");
		list->expand();
		list->arr_size += list->expand_size;
	}
	
	list->curr_size += 1;
	
	for (i = list->curr_size - 1; i > index; i--) {
		list->arr[i] = list->arr[i - 1];
	}

	list->arr[index] = item;
	
	kprint(INFO, "INSERTED AT %d, curr_size %d, arr_size %d\n", 
			index, list->curr_size, list->arr_size);
}

static void remove_item(void *item, unsigned int index, 
			sorted_array_list_t *list)
{
	int i;

	if (list->curr_size == (list->arr_size - list->expand_size)) {
		kprint(INFO, "CONTRACTED\n");
		list->contract();
		list->arr_size -= list->expand_size;
	}

	list->curr_size -= 1;

	for (i = index; i < list->arr_size - 1; i++) {
		list->arr[i] = list->arr[i + 1];
	}


	kprint(INFO, "REMOVED AT %d, curr size %d, arr_size %d\n", 
			index, list->curr_size, list->arr_size);
}

/* Search the list for an item. 
 *
 * non zero - returns index of item
 * -1 - not found, sorry
 */
int search_list(void *item, sorted_array_list_t *list)
{
	int l = 0;
	int r = list->curr_size - 1;
	int curr_index = l + (r - l) / 2;

	while (curr_index >= 0 && curr_index < list->arr_size) {
		if (item == list->arr[curr_index]){
			return curr_index; //found
		}

		if (l == r) {
			return -1; //not found
		}

		if (item < list->arr[curr_index]) {
			r = curr_index - 1;
		}
		else {
			l = curr_index + 1;
		}
			
		curr_index = l + (r - l) / 2;
	}

	return -1; //some error

}

/* Insert into the list
 *
 * 0 - success
 * 1 - Item already exists
 * -1 - inexplicable error occured. 
 */
int insert_list(void *item, sorted_array_list_t *list)
{
	int l = 0;
	int r = list->curr_size - 1;
	int curr_index = l + (r - l) / 2;

	while (curr_index >= 0 && curr_index < list->arr_size) {
		if (item == list->arr[curr_index])
			return 1; //already exists

		if (l == r) {
			insert_item(item, l, list);	
			return 0; //success
		}

		if (item < list->arr[curr_index]) {
			r = curr_index - 1;
		}
		else {
			l = curr_index + 1;
		}
			
		curr_index = l + (r - l) / 2;
	}

	return -1;
}

/* Remove item from the list
 *
 * 0 - success
 * 1 - item is not in this list
 * -1 - oh god, we dont know what happened
 */
int remove_list(void *item, sorted_array_list_t *list)
{
	int l = 0;
	int r = list->curr_size - 1;
	int curr_index = l + (r - l) / 2;

	while (curr_index >= 0 && curr_index < list->arr_size) {
		if (item == list->arr[curr_index]){
			remove_item(item, curr_index, list);
			return 0; //found
		}

		if (l == r) {
			return 1; //not found
		}

		if (item < list->arr[curr_index]) {
			r = curr_index - 1;
		}
		else {
			l = curr_index + 1;
		}
			
		curr_index = l + (r - l) / 2;
	}

	return -1; //some error

}
