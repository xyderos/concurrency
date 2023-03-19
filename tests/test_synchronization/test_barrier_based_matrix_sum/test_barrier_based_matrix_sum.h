#include <check.h>
#include <pthread.h>
#include <stdlib.h>

#include "../../../src/synchronization/barrier_based_matrix_sum/barrier.h"

Suite *barrier_suite(void);

START_TEST(TEST_BARRIER_SHOULD_BE_OKAY)
{
	barrier_based_matrix_sum();
}
END_TEST
