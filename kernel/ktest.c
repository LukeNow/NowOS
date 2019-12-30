#include <kdef.h>
#include <kprint.h>
#include <ktest.h>
#include <string.h>

void register_ktest(int (*test)(), ktest_t * ktest)
{
	if (!test || !ktest)
		return;
	
	int test_index = ktest->test_num;
	ktest->tests[test_index] = test;
	ktest->test_num ++;
}

void run_ktest(ktest_t * ktest)
{
	ASSERT(ktest && ktest->name);
	unsigned int test_num = ktest->test_num;
	
	int num_passed = 0;
	int num_failed = 0;

	kprint(INFO, "**Running Ktest for %s suite\n", ktest->name);
	for (int i = 0; i < test_num; i ++) {
		
		ASSERT(ktest->tests[i]);
		int result = ktest->tests[i]();
		if (result == 0) 
			num_passed ++;
		else
			num_failed ++;

		kprint(INFO, "-Running test num %d: %s\n", i, 
				(result == 0) ? "PASS" : "FAIL");
	}

	kprint(INFO, "--PASSED: %d\n", num_passed);
	kprint(INFO, "--FAILED: %d\n", num_failed);
}

void init_ktest(char * ktest_name, ktest_t * ktest)
{
	memset(ktest, 0, sizeof(ktest_t));
	memcpy(ktest->name, ktest_name, KTEST_NAME_LEN);
}
