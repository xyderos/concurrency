#include <check.h>

#include "test_row_based_matrix_sum.h"

Suite *
row_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("row");
	tc_core = tcase_create("core");

	tcase_add_test(tc_core, TEST_ROW_SHOULD_BE_OKAY);

	suite_add_tcase(s, tc_core);

	return s;
}
