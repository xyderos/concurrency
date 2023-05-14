#include "test_mutex_no_starve.h"

Suite *
mutex_no_starve_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("mutex_no_starve");
	tc_core = tcase_create("core");
	tcase_set_timeout(tc_core, 10);

	tcase_add_test(tc_core, TEST_MUTEX_NO_STARVE);

	suite_add_tcase(s, tc_core);

	return s;
}
