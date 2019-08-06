#include <stdint.h>
#include "../include/kheap.h"
#include "../include/kprint.h"
#include "../include/string.h"
#include "../include/linked_list.h"

void test_kheap()
{
	
	
	kprint(INFO, "Testing kheap\n");
	
	init_kheap();

	char *ptr1, *ptr2, *ptr3, *ptr4;

	const int ptr1_size = 100;
	const int ptr2_size = 50;
	const int ptr3_size = 300;
	const int ptr4_size = 500;

	ptr1 = kmalloc(ptr1_size);
	memset(ptr1, 0x1, ptr1_size);
	
	ptr2 = kmalloc(ptr2_size);
	memset(ptr2, 0x2, ptr2_size);

	ptr3 = kmalloc(ptr3_size);
	memset(ptr3, 0x3, ptr3_size);

	ptr4 = kmalloc(ptr4_size);
	memset(ptr4, 0x4, ptr4_size);

	for (int i = 0; i < ptr1_size; i++) {
		if (ptr1[i] != 0x1) {
			PANIC("PART 1 wrong value");
		}
	}
	for (int i = 0; i < ptr2_size; i++) {
		if (ptr2[i] != 0x2) {
			PANIC("Part 2 wrong value");
		}
	}
	for (int i = 0; i < ptr3_size; i++) {
		if (ptr3[i] != 0x3) {
			PANIC("Part 3 wrong value");
		}
	}
	for (int i = 0; i < ptr4_size; i++) {
		if (ptr4[i] != 0x4) {
			PANIC("Part 4 wrong value");
		}
	}

	kfree(ptr1);
	kfree(ptr2);
	kfree(ptr3);
	kfree(ptr4);
}

static void test_linked_list()
{
	linked_list_t *list = linked_list_init();
	
	for (int i = 0; i < 10; i++) {
		int *val = kmalloc(sizeof(int));
		*val = i;
		linked_list_add(val, list);
	}
	
	//int *v = linked_list_find(9, list);
	
	//linked_list_remove(v, list);
	
	//int v2 = 10;

	//linked_list_add(&v2, list);

	linked_list_for_each(p, list){
		int *val = p->data;
		kfree(val);
		kfree(p);	
	}



	check_heap_integrity();

}

void test()
{	
	kprint(INFO, "***Starting testing***\n");
	kprint(INFO, "***Testing linked list***\n");
	test_linked_list();
	kprint(INFO, "***Testing done ***\n");
}

