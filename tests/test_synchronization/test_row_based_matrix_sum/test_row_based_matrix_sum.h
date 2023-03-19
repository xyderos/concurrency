#include <check.h>
#include <pthread.h>
#include <stdlib.h>

#include "../../../src/synchronization/row_based_matrix_sum/row.h"

Suite *row_suite(void);

START_TEST(TEST_ROW_SHOULD_BE_OKAY)
{
    row_based_matrix_sum();
}
END_TEST
