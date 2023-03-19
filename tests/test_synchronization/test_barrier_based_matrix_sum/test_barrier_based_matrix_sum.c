#include "test_barrier_based_matrix_sum.h"

#include <check.h>

Suite *barrier_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("barrier");
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, TEST_BARRIER_SHOULD_BE_OKAY);

  suite_add_tcase(s, tc_core);

  return s;
}
