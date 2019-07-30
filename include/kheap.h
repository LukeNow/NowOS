#ifndef _KHEAP_H
#define _KHEAP_H
#include <stddef.h>
#include <stdint.h>

typedef struct node_header {
	uint32_t magic;
	void *next_ptr;
	void *prev_ptr;
	uint32_t size;
}node_header_t;

/* Early kheap methods 
 * 
 * The early kheap is the space that we give to things that
 * we dont plan on taking away i.e. its static storage. 
 *
 * We use it for assigning and manipulating kernel page directories
 * and for kheap managment*/
void init_early_kheap();
uint32_t early_kmalloc_sectors(size_t sector_num);
uint32_t early_kmalloc_pages(int num_pages);

/* kheap methods
 *
 * These are the methods for the kheap
 * i.e. our bread and butter dynamic memory management
 */
void init_kheap();
char *kmalloc(size_t size);
void kfree(void *ptr);

#endif
