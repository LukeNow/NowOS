#include <stdint.h>
#include <stdint.h>
#include <stdbool.h>

//TODO make it so we dont explicitly declate tests here
extern void test_circ_buf();
extern void test_linked_list();
extern void test_kheap();

void run_ktest_suite()
{
	test_circ_buf();
	test_linked_list();
	test_kheap();
}
