#include <check.h>
#include <pthread.h>
#include <stdlib.h>

#include "../../../src/synchronization/coordinate_based_matrix_sum/coordinate.h"

Suite *coordinate_suite(void);

START_TEST(TEST_COORDINATE_SHOULD_BE_OKAY)
{
	coordinate_based_matrix_sum();
}
END_TEST
