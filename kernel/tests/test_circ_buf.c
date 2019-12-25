#include <stdint.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../include/kdef.h"
#include "../../include/kprint.h"
#include "../../include/kheap.h"
#include "../../include/circ_buf.h"
#include "../../include/ktest.h"

#define START_TEST \
	int ret_test = 0; \
	circ_buf_t circ_buf; \
	init_circ_buf(4, sizeof(int), &circ_buf)

#define END_TEST \
	destroy_circ_buf(&circ_buf); \
	return ret_test

static int test_0()
{
	START_TEST;
	TEST(circ_buf.size == 4);
	TEST(circ_buf.curr_size == 0);
	
	int src = 1;
	int dst = 0;
	
	push_circ_buf(&src, &circ_buf);
	TEST(circ_buf.curr_size == 1);
	
	pop_circ_buf(&dst, &circ_buf);
	TEST(dst == 1);
	TEST(circ_buf.curr_size == 0);
	
	END_TEST;
}

static int test_1()
{
	START_TEST;

	int src = 1;
	int dst = 0;
	int ret;
	
	src = 1;
	push_circ_buf(&src, &circ_buf);
	src = 2;
	push_circ_buf(&src, &circ_buf);
	src = 3;
	push_circ_buf(&src, &circ_buf);
	
	TEST(circ_buf.curr_size == 3);
	
	pop_circ_buf(&dst, &circ_buf);
	TEST(dst == 1);
	pop_circ_buf(&dst, &circ_buf);
	TEST(dst == 2);

	TEST(circ_buf.curr_size == 1);
	
	pop_circ_buf(&dst, &circ_buf);
	TEST(dst == 3);

	TEST(circ_buf.curr_size == 0);

	ret = pop_circ_buf(&dst, &circ_buf);
	TEST(ret == FAILURE);
	TEST(circ_buf.curr_size == 0);

	END_TEST;
}

static int test_2()
{
	START_TEST;
	
	int src = 1;
	int dst = 0;
	int ret;

	src = 1;
	push_circ_buf(&src, &circ_buf);
	src = 2;
	push_circ_buf(&src, &circ_buf);
	src = 3;
	push_circ_buf(&src, &circ_buf);
	src = 4;
	push_circ_buf(&src, &circ_buf);
	
	ret = push_circ_buf(&dst, &circ_buf);
	TEST(ret == FAILURE);
	TEST(circ_buf.curr_size == 4);

	END_TEST;
}

void test_circ_buf()
{
	ktest_t * ktest = kmalloc(sizeof(ktest_t));
	init_ktest("circ_buf ktest", ktest);
	
	register_ktest(test_0, ktest);
	register_ktest(test_1, ktest);
	register_ktest(test_2, ktest);
	
	run_ktest(ktest);
	kfree(ktest);
}
