#include <kdef.h>
#include <kheap.h>
#include <kprint.h>
#include <sorted_array_list.h>
#include <string.h>
#include <byte_index_list.h>
#include <lock.h>

#define HEAP_EXPAND_SIZE KB(4)
#define SECTOR_SIZE 512
#define HEAP_SPLIT_THRESHOLD 16
#define MAGIC_FREE 0x45455246 //FREE IN ASCII
#define MAGIC_USED 0x44455355 //USED IN ASCII
#define USED 1
#define FREE 0
#define HEADER_SIZE sizeof(node_header_t)

extern void heap_bottom();
extern void heap_top();
extern void early_heap_bottom();
extern void early_heap_top();
extern void page_heap_bottom();
extern void page_heap_top();

BYTE_INDEX_LIST_INIT(page_list);
SORTED_ARRAY_LIST_INIT(free_node_list);
SPINLOCK_INIT(lock);

uint32_t heap_ptr = 0;
uint32_t top_heap_ptr = 0;
uint32_t early_heap_ptr = 0;
uint32_t heap_list_end_ptr = 0;
uint32_t page_ptr = 0;


/*********************
 * Early kheap methods
 *********************/
void init_early_heap()
{

	early_heap_ptr = (uint32_t) early_heap_bottom;
	kprint(INFO, "[INIT EARLY KHEAP] Early heap ptr: %x\n", early_heap_ptr);
}

/* Sector is 512 bytes 
 * Linear address */
uint32_t early_kmalloc_sectors(size_t sector_num)
{	
	uint32_t temp = early_heap_ptr;
	uint32_t temp_bottom;
	
	temp_bottom = early_heap_ptr;
	temp += sector_num * SECTOR_SIZE;
	temp = ALIGN_UP(temp, sizeof(uint32_t));
	early_heap_ptr = temp;
	
	return temp_bottom;
}

/* return physical address */
uint32_t early_kmalloc_pages(int num_pages)
{
	uint32_t temp;
	uint32_t temp_bottom;
	
	early_heap_ptr  = ALIGN_UP(early_heap_ptr, PAGE_SIZE);
	
	temp = early_heap_ptr;
	temp = ALIGN_UP(temp, PAGE_SIZE);
	temp_bottom = early_heap_ptr;
	
	if (!IS_ALIGNED(early_heap_ptr, PAGE_SIZE)) {
		PANIC("PAGE IS NOT ALLIGNED");
	}
	
	for (int i = 0; i < num_pages; i++) {
		if (temp >= (uint32_t) early_heap_top){
			WARN("WEVE HIT THE TOP OF HEAP");
		}
		temp += PAGE_SIZE;
	}
	
	temp_bottom = temp_bottom - KERN_VIRTUAL_ADDR;
	early_heap_ptr += (num_pages * PAGE_SIZE);
	return temp_bottom;
}

/***************
 * kheap methods
 ***************/

/* This method will panic if it doesent find a MAGIC value */
static inline void check_header(node_header_t *header)
{
	if (header->magic != MAGIC_USED && header->magic != MAGIC_FREE){
		PANIC("BAD MAGIC VALUE FOUND");
	}
}

static void heap_set_header(uint32_t addr, uint8_t used, 
		     void *next_ptr, void *prev_ptr, uint32_t size)
{
	node_header_t *header = (node_header_t *)addr;
	header->magic = (used) ? MAGIC_USED : MAGIC_FREE;
	header->next_ptr = next_ptr;
	header->prev_ptr = prev_ptr;
	header->size = size;
}

static void heap_print_header(node_header_t *header)
{
	char *s;
	
	switch (header->magic) {
		case MAGIC_FREE:
			s = "FREE";
			break;
		case MAGIC_USED:
			s = "USED";
			break;
		default:
			s = "BAD";
	}
	
	kprint(INFO, "Node: %x MAGIC: %s next_ptr: %x prev_ptr: %x size: %d\n",
	       header, s, header->next_ptr, header->prev_ptr, header->size);
}

static void heap_print_list()
{
	node_header_t *curr_node = (node_header_t *)heap_bottom;

	kprint(INFO, "***Printing Heap Nodes***\n");
	while (curr_node != NULL) {
		/* Santity check */
		check_header(curr_node);
		heap_print_header(curr_node);
		curr_node = curr_node->next_ptr;
	}
}

static node_header_t *heap_find_free_node(uint32_t alloc_size)
{
	unsigned int list_len = free_node_list.curr_size - 1;
	node_header_t *best_node = NULL;
	
	for (int i = 0; i < list_len; i++) {
		node_header_t *node = (node_header_t *)free_node_list.arr[i];
		/* Santity check */
		check_header(node);
		
		if ((!best_node && node->size >= alloc_size) ||
		    (best_node && node->size >= alloc_size && 
		     node->size < best_node->size)) {
			best_node = node;
		}
	}
	
	/* Will be null if not found */
	return best_node;
}

static node_header_t *heap_merge_left(node_header_t *node)
{
	node_header_t *node_left = node->prev_ptr;
	/* the new node is the one whos header
	 * we will keep */
	node_header_t *new_node = node_left;
	node_header_t *new_prev_ptr = node_left->prev_ptr;
	node_header_t *new_next_ptr = node->next_ptr;
	uint32_t new_node_size = node->size + node_left->size +
				 HEADER_SIZE;
	
	/* Set the nodes next to us to point to us */
	if (new_next_ptr != NULL)
		new_next_ptr->prev_ptr = new_node;
	if (new_prev_ptr != NULL)
		new_prev_ptr->next_ptr = new_node;
	
	/* Updated node */
	heap_set_header((uint32_t)new_node, FREE, new_next_ptr, 
			new_prev_ptr, new_node_size);
	
	return new_node;
}

static node_header_t *heap_merge_right(node_header_t *node)
{
	node_header_t *node_right = node->next_ptr;
	/* the new node is the one whos header
	 * we will keep */
	node_header_t *new_node = node;
	node_header_t *new_prev_ptr = node->prev_ptr;
	node_header_t *new_next_ptr = node_right->next_ptr;
	uint32_t new_node_size = node->size + node_right->size +
				 HEADER_SIZE;
	
	/* Set the nodes next to us to point to us */
	if (new_next_ptr != NULL)
		new_next_ptr->prev_ptr = new_node;
	if (new_prev_ptr != NULL)
		new_prev_ptr->next_ptr = new_node;
	
	/* Updated node */
	heap_set_header((uint32_t)new_node, FREE, new_next_ptr, 
			new_prev_ptr, new_node_size);
	
	return new_node;
}

static void heap_merge_nodes(node_header_t *node)
{
	check_header(node);

	node_header_t *node_left = node->prev_ptr;
	node_header_t *node_right = node->next_ptr;
	node_header_t *merge_node = node;

	if (node_right != NULL && node_right->magic == MAGIC_FREE) {
		remove_list((void *)node, &free_node_list);
		remove_list((void *)node_right, &free_node_list);
		merge_node = heap_merge_right(node);
	}
	if (node_left != NULL && node_left->magic == MAGIC_FREE){
		remove_list((void *)node, &free_node_list);
		remove_list((void *)node_left, &free_node_list);
		merge_node = heap_merge_left(node);
	}
	
	/* If nothing is merged, then the node is added back
	 * to free list */
	insert_list((void *)merge_node, &free_node_list);
}

static void heap_split_node(node_header_t *free_node, uint32_t new_alloc_size)
{
	node_header_t *new_free_node;
	node_header_t *new_used_node;
	
	/* Check if this is a valid node */
	check_header(free_node);
	
	/* Lets remove this node from the list to make
	 * new ones. */
	remove_list((void *)free_node, &free_node_list);

	node_header_t *old_next_ptr = free_node->next_ptr;
	node_header_t *old_prev_ptr = free_node->prev_ptr;

	uint32_t total_free_size = free_node->size + HEADER_SIZE;
	uint32_t total_new_alloc_size = new_alloc_size + HEADER_SIZE;
	uint32_t total_size_left = total_free_size - total_new_alloc_size;
	uint32_t new_free_alloc_size = total_size_left - HEADER_SIZE;
	
	/* Our new used header will be in the exact same place as the old
	 * free node we are spliting */
	new_used_node = free_node;
	/* This new free node gets the rest of the space */
	new_free_node = (node_header_t *)((uint32_t)free_node + total_new_alloc_size);
	
	if (new_free_alloc_size < 0) {
		PANIC("NEW ALLOC IS LESS THAN 0");
	}
	
	/* IF we dont have enough space for the threshold, 
	 * We only make one used node */
	if (total_size_left <= HEAP_SPLIT_THRESHOLD) {
		new_alloc_size += total_size_left;
				
		/* Set the nodes next to this one to point to our new node */
		if (old_next_ptr != NULL)
			old_next_ptr->prev_ptr = new_used_node;
		if (old_prev_ptr != NULL)
			old_prev_ptr->next_ptr = new_used_node;
		
		/* If this is the last node in the list, set the last ptr */
		if (new_free_node->next_ptr == NULL)
			heap_list_end_ptr = (uint32_t)new_used_node;
		
		heap_set_header((uint32_t)new_used_node, USED, old_next_ptr,
				old_prev_ptr, new_alloc_size);
		
		if (new_used_node->next_ptr == NULL)
			heap_list_end_ptr = (uint32_t) new_used_node;

		return;
	}
	
	/*ELSE we make a used node and a free node */

	/* Set the nodes next to this one to point to our new node */
	if (old_next_ptr != NULL)
		old_next_ptr->prev_ptr = new_free_node;
	if (old_prev_ptr != NULL)
		old_prev_ptr->next_ptr = new_used_node;
	
	
	heap_set_header((uint32_t)new_free_node, FREE, old_next_ptr, 
			new_used_node, new_free_alloc_size);
	
	
	heap_set_header((uint32_t)new_used_node, USED, new_free_node, 
			old_prev_ptr, new_alloc_size);
	
	/* Add our new free node to the free list */
	insert_list((void *)new_free_node, &free_node_list);

	/* If this is the last node in the list, set the last ptr */
	if (new_free_node->next_ptr == NULL)
		heap_list_end_ptr = (uint32_t) new_free_node;
}

static void expand_heap(size_t size)
{
	top_heap_ptr += size;
	
	if (top_heap_ptr >= (uint32_t)heap_top){
		PANIC("Weve expanded past our heap top...");
	}

	node_header_t *last_node = (node_header_t *)heap_list_end_ptr;
	
	/* sanity check our node */
	check_header(last_node);
	
	uint32_t new_free_node = ((uint32_t)last_node) + HEADER_SIZE +
				 last_node->size;
	uint32_t new_alloc_size = top_heap_ptr - new_free_node;
	
	heap_set_header(new_free_node, FREE, NULL, last_node, new_alloc_size);
	last_node->next_ptr = (node_header_t *)new_free_node;
	
	/* Insert our new expanded node to the list of free nodes */
	insert_list((void *)new_free_node, &free_node_list);
	
	/* Merge the nodes around our new node to make one big free node */
	heap_merge_nodes((node_header_t *)new_free_node);
	
	/* Set the last pointer to this because it is gaurenteed to be last */
	heap_list_end_ptr = (uint32_t)new_free_node;
}

void check_kheap_integrity()
{
	
	uint32_t heap_size = (uint32_t)top_heap_ptr - (uint32_t)heap_bottom;
	node_header_t *node = (node_header_t *)heap_bottom;
	check_header(node);
	
	uint32_t heap_count = 0;
	while (node != NULL) {
		heap_count += (node->size + HEADER_SIZE);
		node = node->next_ptr;
	}

	if (heap_count != heap_size) {
		kprint(ERROR,  "Kheap integrity compromised: heap_count: %d, heap_size: %d\n", 
				heap_count, heap_size);
		
	}
}

void *kmalloc(size_t size)
{
	spin_lock(&lock);
#ifdef DEBUG
	/* Check the heap integrity on every malloc */
	check_kheap_integrity();
#endif
	node_header_t *node;
	/* Align the size to natural word */
	size = ALIGN_UP(size, sizeof(uint32_t));

	/* Find free node*/
	node = heap_find_free_node(size);
	
	/* we need to expand the heap to a greater size */
	if (node == NULL) {
		uint32_t expand_count = 0;
		while (expand_count <= size) expand_count += HEAP_EXPAND_SIZE;
		expand_heap(expand_count);
		node = heap_find_free_node(size);
		if (node == NULL) {
			PANIC("Not enough memory?");
		}
	}
	
			
	/* Split this free node into a used and a free one */
	heap_split_node(node, size);
	
	spin_unlock(&lock);
	/* Return a ptr to the non-header section */
	return (char *)((uint32_t)node + HEADER_SIZE);
}

void kfree(void *ptr)
{	
	if (ptr == NULL)
		return;
	
	spin_lock(&lock);
	/* Grab the header section of this pointer */
	node_header_t *node = (node_header_t *)
			      ((uint32_t)ptr - HEADER_SIZE);
	
	/* Santity check that this isnt a bad free...*/
	check_header(node);	
	
	/* Make this node free */
	heap_set_header((uint32_t)node, FREE, node->next_ptr, 
			node->prev_ptr, node->size);
	
	//TODO shrink heap
	
	/* Merge potential free nodes next to this one 
	 * to make one large free node */
	heap_merge_nodes(node);

	spin_unlock(&lock);
}

void init_kheap()
{
	heap_ptr = (uint32_t)heap_bottom;
	
	uint32_t total_heap_size = (uint32_t)heap_top - (uint32_t)heap_bottom;
	memset((void *)heap_ptr, 0, total_heap_size);
	/* Align just in case */
	heap_ptr = ALIGN_UP(heap_ptr, sizeof(uint32_t));
	top_heap_ptr = heap_ptr + HEAP_EXPAND_SIZE;
	
	uint32_t list_size = ((uint32_t)early_heap_top - early_heap_ptr) /
			     sizeof(node_header_t *);
	
	init_sorted_list(&free_node_list, (void **)early_heap_ptr, list_size);
	
	uint32_t heap_begin = heap_ptr + HEADER_SIZE;
	uint32_t heap_end = top_heap_ptr;
	uint32_t beginning_heap_size = heap_end - heap_begin;
	
	/* Set the first header at the beginning of the heap */
	heap_set_header(heap_ptr, 0, NULL, NULL, beginning_heap_size);
	
	/* Insert our first big node into the list */
	insert_list((void *)heap_ptr, &free_node_list);
	
	heap_list_end_ptr = heap_ptr;
	
	kprint(INFO, "[INIT KHEAP] Heap ptr: %x Heap size: %d\n", 
	       heap_ptr, beginning_heap_size);
}

void *kmalloc_page()
{
	spin_lock(&page_list.lock);

	unsigned int index = byte_index_get_free_slot(&page_list);
	if (index == -1){
		spin_unlock(&page_list.lock);
		kprint(WARN, "Kmalloc_page: couldnt find free page\n");
		return NULL;
	}
	
	spin_unlock(&page_list.lock);

	return (void *)(page_ptr + (index * PAGE_SIZE));
}

void kfree_page(void *ptr)
{
	if (!IS_ALIGNED((uint32_t)ptr, PAGE_SIZE)){
		PANIC("kfree_page ptr not aligned");
	}


	uint32_t index = ((uint32_t)ptr - page_ptr) / PAGE_SIZE;
	
	spin_lock(&page_list.lock);
	byte_index_release_tbl(index, &page_list);
	spin_unlock(&page_list.lock);
}
	

void init_page_heap()
{
	char *page_list_ptr = (char *)page_heap_bottom;
	uint32_t page_num = ((uint32_t)page_heap_top - 
			     (uint32_t)page_heap_bottom) / PAGE_SIZE;
	page_num -= 1; //Use one page for the byte_index_list
	
	page_ptr = (uint32_t)page_heap_bottom + PAGE_SIZE;
	/* Align up to page boundary just in case */
	page_ptr = ALIGN_UP(page_ptr, PAGE_SIZE); 
	
	init_byte_index_list(page_list_ptr, page_num, &page_list);

	kprint(INFO, "[INIT PAGE HEAP] Page ptr: %x Page number: %d\n", 
	       page_ptr, page_num);

}