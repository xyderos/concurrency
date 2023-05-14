#include <check.h>

#include "../../../src/synchronization/c_list/c_list.h"
#include "../../../src/synchronization/futex/futex.h"
#include "../../../src/synchronization/peterson/peterson.h"
#include "../../../src/synchronization/s_list/s_list.h"
#include "../../../src/synchronization/swap/swap.h"

Suite *locks_suite(void);

START_TEST(TEST_C_LIST)
{
	bench_c_list(3, 10);
}
END_TEST

START_TEST(TEST_FUTEX)
{
	demonstrate_futex(10);
}
END_TEST

START_TEST(TEST_PETERSON)
{
	demonstrate_peterson(10);
}
END_TEST

START_TEST(TEST_S_LIST)
{
	bench_s_list(3, 10);
}
END_TEST

START_TEST(TEST_SWAP)
{
	demonstrate_swap(10);
}
END_TEST
