#include "test_locks.h"

Suite *
locks_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("locks");
	tc_core = tcase_create("core");

	tcase_add_test(tc_core, TEST_C_LIST);
	tcase_add_test(tc_core, TEST_FUTEX);
	tcase_add_test(tc_core, TEST_PETERSON);
	tcase_add_test(tc_core, TEST_S_LIST);
	tcase_add_test(tc_core, TEST_SWAP);

	suite_add_tcase(s, tc_core);

	return s;
}
