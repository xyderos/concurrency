#include "test_random_barbershop.h"

Suite *
random_barbershop_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("random_barbershop");
	tc_core = tcase_create("core");
	tcase_set_timeout(tc_core, 10);

	tcase_add_test(tc_core, TEST_RANDOM_BARBERSHOP);

	suite_add_tcase(s, tc_core);

	return s;
}
