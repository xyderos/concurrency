#include "test_cigarette_smokers.h"

Suite *
cigarette_smokers_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("cigarette_smokers");
	tc_core = tcase_create("core");
	tcase_set_timeout(tc_core, 10);

	tcase_add_test(tc_core, TEST_CIGARETTE_SMOKERS);

	suite_add_tcase(s, tc_core);

	return s;
}
