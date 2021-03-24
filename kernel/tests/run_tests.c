#include <kdef.h>

//TODO make it so we dont explicitly declare tests here
extern void test_circ_buf();
extern void test_linked_list();
extern void test_kheap();
extern void test_shared_pool();

void run_ktest_suite()
{
	test_circ_buf();
	test_linked_list();
	test_kheap();
	test_shared_pool();
}
