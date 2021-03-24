#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <lock.h>
#include <shared_pool.h>

#define LINKED_LIST_INIT(list) \
	static linked_list_t (list) = { NULL, NULL, NULL, 0, LOCK_FREE}

#define linked_list_for_each(pos, list) \
	for (linked_list_node_t *(pos) = (list)->start_ptr; pos != NULL; pos = pos->next_ptr)

typedef struct linked_list_node {
	void *next_ptr;
	void *prev_ptr;
	void *data;
}linked_list_node_t;

typedef struct linked_list {
	linked_list_node_t *start_ptr;
	linked_list_node_t *end_ptr;
	shared_pool_t * node_pool;
	unsigned int size;
	spinlock_t lock;
}linked_list_t;

/***
 * Cycle the linked list
 *
 * put the node at the front(index 0) to the back(index size - 1)
 * The second node(index 1) will now be at the front.
 *
 * Returns nothing
 */
void linked_list_cycle(linked_list_t *list);
/***
 * Enqueue list
 *
 * Puts data at the back of the list(index size) 
 * 
 * Returns index that the data was placed
 * Returns -1 if there was an error
 */
int linked_list_enqueue(void *data, linked_list_t *list);
/***
 * Dequeue list
 *
 * Takes data from the front of the list(index 0)
 * and return it. 
 *
 * Deletes data from the list and returns the deleted element
 */
void *linked_list_dequeue(linked_list_t *list);
/***
 * Push list
 *
 * Takes data and puts it at the front of the list(index 0)
 *
 * Return index that the data was placed
 * Returns -1 if there was an error
 */
int linked_list_push(void *data, linked_list_t *list);
/***
 * Pop list
 *
 * Takes data from the front of the list(index 0)
 * and returns it
 *
 * Deletes data from the list and returns deleted element
 */
void *linked_list_pop(linked_list_t *list);

/***
 * Get data at the index of the list
 *
 * Returns NULL if the node at the index doesent exist
 */
void *linked_list_get(unsigned int index, linked_list_t *list);
/***
 * Get index of the first node with the data
 *
 * Returns index if found
 * Returns -1 if not found
 */
int linked_list_search(void *data, linked_list_t *list);
/***
 * Add data to index
 *
 * If index is not within [0, size] it will return the index tha it
 * was placed at
 */
int linked_list_add(void *data, unsigned int index, linked_list_t *list);
/***
 * Remove data from index
 * 
 * Returns 0 if success
 * Returns -1 if error or the index is not in the list 
 */
int linked_list_remove(unsigned int index, linked_list_t *list);

/***
 * Destroy the linked list
 *
 * Frees the buffer associated with the linked list
 */
void linked_list_destroy(linked_list_t * list);

/***
 * Returns an instance of a list by dynamically allocating
 */
linked_list_t *linked_list_init();
#endif
