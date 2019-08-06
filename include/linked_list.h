#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#define LINKED_LIST_INIT(list) \
	static linked_list_t (list) = { NULL, NULL }

#define linked_list_for_each(pos, list) \
	for (linked_list_node_t *(pos) = (list)->start_ptr; pos != NULL; pos = pos->next_ptr)

typedef struct linked_list {
	void *start_ptr;
	void *end_ptr;
}linked_list_t;

typedef struct linked_list_node {
	void *next_ptr;
	void *prev_ptr;
	void *data;
}linked_list_node_t;

linked_list_t *linked_list_init();
void *linked_list_find(unsigned int index, linked_list_t *list);
int linked_list_search(void *data, linked_list_t *list);
int linked_list_add(void *data, linked_list_t *list);
void linked_list_remove(void *data, linked_list_t *list);

#endif
