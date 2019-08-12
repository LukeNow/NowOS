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
	return list;
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

	while (node != NULL){
		if (node->data == data) {
			return 0; //found
		}
		node = node->next_ptr;
	}

	return 1;
}

int linked_list_add(void *data, linked_list_t *list)
{
	if (list == NULL) 
		return -1;
	
	if (linked_list_search(data, list) == 0)
		return 1; //already exists

	linked_list_node_t *node = kmalloc(sizeof(linked_list_node_t));
	memset(node, 0, sizeof(linked_list_node_t));
	
	/* Set data */
	node->data = data;
	
	/* If this is the first node */
	if (list->start_ptr == NULL) {
		list->start_ptr = node; //update list
		list->end_ptr = node;
	}
	/* Otherwise update nodes around us */
	else {
		linked_list_node_t *last_node = list->end_ptr;
		last_node->next_ptr = node; //update last_node
		node->prev_ptr = last_node; //update new node
		list->end_ptr = node; //update list
	}
	
	return 0;
}

void linked_list_remove(void *data, linked_list_t *list)
{
	if (list == NULL)
		return;
	
	linked_list_node_t *node = list->start_ptr;
	
	/* Find node */
	while (node != NULL) {
		if(node->data == data) {
			break;
		}

		node = node->next_ptr;
	}
	
	/* Node not found */
	if (node == NULL) 
		return; 
	
	linked_list_node_t *prev_node = node->prev_ptr;
	linked_list_node_t *next_node = node->next_ptr;
	
	/* This is the only node in the list */
	if (prev_node == NULL && next_node == NULL) {
		list->start_ptr = NULL;
		list->end_ptr = NULL;
		kfree(node);
		return;
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

	kfree(node);
}
