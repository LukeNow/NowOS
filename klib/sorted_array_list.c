#include <kdef.h>
#include <sorted_array_list.h>
#include <kprint.h>
#include <lock.h>

void init_sorted_list(sorted_array_list_t *list, void **arr_ptr, 
		      uint32_t array_size)
{
	list->arr = arr_ptr;
	list->arr_size = array_size;
	list->curr_size = 1;
	init_spinlock(&list->lock);
}

static int insert_item(void *item, unsigned int index, 
		       sorted_array_list_t *list)
{
	unsigned int i;

	if (list->curr_size == list->arr_size){
		return -1;
	}
	
	
	for (i = list->curr_size - 1; i > index; i--) {
		list->arr[i] = list->arr[i - 1];
	}

	list->curr_size += 1;
	list->arr[index] = item;
	
	return 0;
}

static void remove_item(void *item, unsigned int index, 
			sorted_array_list_t *list)
{
	unsigned int i;

	for (i = index; i < list->curr_size - 1; i++) {
		list->arr[i] = list->arr[i + 1];
	}
	
	list->curr_size -= 1;
}

/* Search the list for an item. 
 *
 * non zero - returns index of item
 * -1 - not found, sorry
 */
int search_list(void *item, sorted_array_list_t *list)
{
	unsigned int l = 0;
	unsigned int r = list->curr_size - 1;
	unsigned int curr_index = l + (r - l) / 2;

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
 * -1 - List is probably full
 */
int insert_list(void *item, sorted_array_list_t *list)
{
	unsigned int l = 0;
	unsigned int r = list->curr_size - 1;
	unsigned int curr_index = l + (r - l) / 2;

	while (curr_index >= 0 && curr_index < list->arr_size) {
		if (item == list->arr[curr_index])
			return 1; //already exists

		if (l == r) {
			if (insert_item(item, curr_index, list) == -1) {
				kprint(ERROR, "Sorted_array_list: List is full\n");
				return -1;	
			};	
			return 0; //success
		}

		if (item < list->arr[curr_index]) {
			r = curr_index;
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
	unsigned int l = 0;
	unsigned int r = list->curr_size - 1;
	unsigned int curr_index = l + (r - l) / 2;

	while (curr_index >= 0 && curr_index < list->arr_size) {
		if (item == list->arr[curr_index]){
			remove_item(item, curr_index, list);
			return 0; //found
		}

		if (l == r) {
			return 1; //not found
		}

		if (item < list->arr[curr_index]) {
			r = curr_index;
		}
		else {
			l = curr_index + 1;
		}
			
		curr_index = l + (r - l) / 2;
	}

	return -1; //some error
}

void print_list(sorted_array_list_t *list)
{
	uint32_t arr_size = list->curr_size - 1;
	kprint(INFO, "***Printed sorted array list %x***\n", (uint32_t)list);
	for (unsigned int i = 0; i < arr_size; i++) {
		kprint(INFO, "[%d]: %x\n", i, list->arr[i]);
	}
}
