#include <stddef.h>
#include <stdint.h>

#include "../include/string.h"
#include "../include/kheap.h"
#include "../include/linked_list.h"
#include "../include/kprint.h"

linked_list_t *linked_list_init()
{
	linked_list_t *list = kmalloc(sizeof(linked_list_t));
	list->start_ptr = NULL;
	list->end_ptr = NULL;
	list->size = 0;
	return list;
}

void linked_list_cycle(linked_list_t *list)
{
	linked_list_node_t *first_node = list->start_ptr;
	linked_list_node_t *last_node = list->end_ptr;
	
	/* If this is the only node in the list or there
	 * arent any nodes just return */
	if (first_node == last_node)
		return;
	
	linked_list_node_t *second_node = first_node->next_ptr;
	
	first_node->next_ptr = NULL;
	first_node->prev_ptr = last_node;
	last_node->next_ptr = first_node;
	list->end_ptr = first_node;

	second_node->prev_ptr = NULL;
	list->start_ptr = second_node;
}

int linked_list_enqueue(void *data, linked_list_t *list)
{
	return linked_list_add(data, list->size, list);	
}

void *linked_list_dequeue(linked_list_t *list)
{
	void *ret = linked_list_get(0, list);
	linked_list_remove(0, list);
	return ret;
}

int linked_list_push(void *data, linked_list_t *list)
{
	return linked_list_add(data, 0, list);
}

void *linked_list_pop(linked_list_t *list)
{
	void *ret = linked_list_get(0, list);
	linked_list_remove(0, list);
	return ret;
}

void *linked_list_get(unsigned int index, linked_list_t *list)
{
	if (list == NULL)
		return NULL;

	linked_list_node_t *node = list->start_ptr;
	void *data = node->data;

	for (int i = 0; i < index + 1; i++) {
		
		if (node == NULL) {
			return NULL;
		}
		
		data = node->data;
		node = node->next_ptr;
	}
	
	return data;
}

int linked_list_search(void *data, linked_list_t *list)
{
	if (list == NULL) 
		return -1;
	
	linked_list_node_t *node = list->start_ptr;
	int index = 0;
	while (node != NULL){
		if (node->data == data) {
			return index; //found
		}
		node = node->next_ptr;
		index++;
	}

	return -1;
}

int linked_list_add(void *data, unsigned int index, linked_list_t *list)
{
	if (list == NULL) 
		return -1;
	
	linked_list_node_t *node = kmalloc(sizeof(linked_list_node_t));
	memset(node, 0, sizeof(linked_list_node_t));
	
	/* Set data */
	node->data = data;
	
	/* If this is the first node */
	if (list->start_ptr == NULL) {
		list->start_ptr = node; //update list
		list->end_ptr = node;
		list->size++;
		return 0;
	}
	
	/* Otherwise update nodes around us */
	unsigned int temp_index = 0;
	linked_list_node_t *temp_node = list->start_ptr;
	
	/* Search for the place we want to place this node */
	while (temp_node != NULL && temp_index <= index) {
		temp_node = temp_node->next_ptr;
		temp_index++;
	}
	
	/* Our node will be the last node in the list */
	if (temp_node == NULL) {
		linked_list_node_t *last_node = list->end_ptr;
		list->end_ptr = node;
		last_node->next_ptr = node;
		node->prev_ptr = last_node;
		list->size++;
		return temp_index;
	}
	
	/* our node is the first node in the list */
	if (temp_node->prev_ptr == NULL)
		list->start_ptr = node;

	node->prev_ptr = temp_node->prev_ptr;
	node->next_ptr = temp_node;
	temp_node->prev_ptr = node;
	
	list->size++;
	return temp_index;
}

int linked_list_remove(unsigned int index, linked_list_t *list)
{
	if (list == NULL)
		return -1;
	
	linked_list_node_t *node = list->start_ptr;
	
	unsigned int temp_index = 0;
	while (node != NULL && temp_index <= index) {
		node = node->next_ptr;
		temp_index++;
	}

	if (temp_index != index)
		return -1;
	
	linked_list_node_t *prev_node = node->prev_ptr;
	linked_list_node_t *next_node = node->next_ptr;
	
	/* This is the only node in the list */
	if (prev_node == NULL && next_node == NULL) {
		list->start_ptr = NULL;
		list->end_ptr = NULL;
		kfree(node);
		list->size --;
		return 0; //success
	}

	/* update previous node */
	if (prev_node != NULL) 
		prev_node->next_ptr = next_node;
	else 
		list->start_ptr = next_node;
	
	/* Update next node */
	if (next_node != NULL)
		next_node->prev_ptr = prev_node; 
	/* Otherwise update list */
	else
		list->end_ptr = prev_node;	 
	
	list->size--;
	kfree(node);
	return 0;
}
