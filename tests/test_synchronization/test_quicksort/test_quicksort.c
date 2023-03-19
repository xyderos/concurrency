#include <check.h>

#include "test_quicksort.h"

Suite *
quicksort_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("quicksort");
	tc_core = tcase_create("core");

	tcase_add_test(tc_core, TEST_QUICKSORT_SHOULD_BE_OKAY);

	suite_add_tcase(s, tc_core);

	return s;
}
