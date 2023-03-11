#include "test_lifecycle.h"

#include <check.h>

Suite *
lifecycle_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("lifecycle");
	tc_core = tcase_create("core");

	tcase_add_test(tc_core, TEST_CUSTOM_ATTRIBUTES_INIT_SHOULD_BE_OKAY);
	tcase_add_test(tc_core, TEST_MIN_STACK_POSSIBLE_SHOULD_BE_OKAY);

        suite_add_tcase(s, tc_core);

        return s;
}
