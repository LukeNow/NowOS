#include <kdef.h>
#include <ktest.h>
#include <kheap.h>
#include <kprint.h>
#include <string.h>

#define START_TEST \
	int ret_test = 0

#define END_TEST \
	return ret_test

static int test_0()
{
	START_TEST;

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
		TEST(ptr1[i] == 0x1);
	}
	
	for (int i = 0; i < ptr2_size; i++) {
		TEST(ptr2[i] == 0x2);
	}
	
	for (int i = 0; i < ptr3_size; i++) {
		TEST(ptr3[i] == 0x3);
	}
	
	for (int i = 0; i < ptr4_size; i++) {
		TEST(ptr4[i] == 0x4);
	}

	kfree(ptr1);
	kfree(ptr2);
	kfree(ptr3);
	kfree(ptr4);
	
	END_TEST;
}

void test_kheap()
{
	ktest_t * ktest = kmalloc(sizeof(ktest_t));
	init_ktest("Kheap ktest", ktest);
	
	register_ktest(test_0, ktest);
	
	run_ktest(ktest);
	kfree(ktest);
}
