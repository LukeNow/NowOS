#ifndef _KTEST_H
#define _KTEST_H

#define KTEST_NUM 64
#define KTEST_NAME_LEN 64

/* We must define a variable ret_test for a test in a local scope
 * so that it will set it equal to 1 if a test fails 
 *
 * This then returns the value of the entire test to the ktest*/
#define TEST(x) if (!(x)) ret_test = 1

typedef struct ktest {
	int (*tests[KTEST_NUM])();
	unsigned int test_num;
	char name[KTEST_NAME_LEN];
} ktest_t;

void run_ktest_suite();
void register_ktest(int (*test)(), ktest_t * ktest);
void run_ktest(ktest_t * ktest);
void init_ktest(char * ktest_name, ktest_t * ktest);
#endif
