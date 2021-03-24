
#include <ktest.h>
#include <shared_pool.h>
#include <string.h>

#define START_TEST(size, type) \
	int ret_test = 0; \
    shared_pool_t pool; \
	bool setup = shared_pool_init(&pool, size, type)

#define END_TEST \
	shared_pool_destroy(&pool); \
	return ret_test

/* TODO
   Write some sort of fuzzer to fuzz threads on shared data structures to make sure
   everything works */
static int test_0()
{
	uint32_t size = 10;
    size_t type = sizeof(uint32_t);
    START_TEST(size, type);
    
    TEST(setup == true);

    uint32_t * entry = NULL;
    for (int i = 0; i < size; i++) {
        TEST(shared_pool_get_next(&pool) != NULL);
    }
    // Next entry should be NUll, all entries are filled;
    TEST(shared_pool_get_next(&pool) == NULL);

    for (int i = 0; i < size; i++) {
        shared_pool_free_index(&pool, i);
    }

    // Everything has been freed, try again
    for (int i = 0; i < size; i++) {
        if (i == 5) {
            entry = shared_pool_get_next(&pool);
            TEST(entry != NULL);
        } 
        else
            TEST(shared_pool_get_next(&pool) != NULL);
    }

    TEST(shared_pool_get_next(&pool) == NULL);
    
    *entry = 0xDEADBEEF;
    shared_pool_free_index(&pool, 5);

    entry = shared_pool_get_next(&pool);
    TEST(entry != NULL);

    TEST(*entry == 0xDEADBEEF);
    TEST(entry == (uint32_t)&pool.buf[0] + (type * 5));
	
	END_TEST;
}

void test_shared_pool()
{
	ktest_t * ktest = kmalloc(sizeof(ktest_t));
	init_ktest("shared_pool test 0", ktest);
	
	register_ktest(test_0, ktest);
	
	run_ktest(ktest);
	kfree(ktest);
}
