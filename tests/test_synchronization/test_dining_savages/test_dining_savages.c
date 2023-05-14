#include "test_dining_savages.h"

Suite *
dining_savages_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("dining_savages");
	tc_core = tcase_create("core");
	tcase_set_timeout(tc_core, 10);

	tcase_add_test(tc_core, TEST_DINING_SAVAGES);

	suite_add_tcase(s, tc_core);

	return s;
}
