#include <stdint.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../include/kdef.h"
#include "../../include/kprint.h"
#include "../../include/kheap.h"
#include "../../include/linked_list.h"
#include "../../include/ktest.h"

#define START_TEST \
	int ret_test = 0; \
	linked_list_t * list = linked_list_init()

#define END_TEST \
	linked_list_destroy(list); \
	return ret_test

static int test_0()
{
	START_TEST;
	
	for (int i = 0; i < 10; i++) {
		int *val = kmalloc(sizeof(int));
		*val = i;
		linked_list_enqueue(val, list);
	}
	
	int i = 0;
	linked_list_for_each(p, list){
		int *val = p->data;
		TEST(*val == i);
		i++;
	}


	linked_list_for_each(p, list){
		int *val = p->data;
		kfree(val);
	}

	END_TEST;
}

void test_linked_list()
{
	ktest_t * ktest = kmalloc(sizeof(ktest_t));
	init_ktest("circ_buf ktest", ktest);
	
	register_ktest(test_0, ktest);
	
	run_ktest(ktest);
	kfree(ktest);
}
