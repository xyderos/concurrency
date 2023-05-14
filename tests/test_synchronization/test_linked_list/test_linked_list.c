#include "test_linked_list.h"

Suite *
linked_list_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("linked_list");
	tc_core = tcase_create("core");
	tcase_set_timeout(tc_core, 10);

	tcase_add_test(tc_core, TEST_LINKED_LIST);

	suite_add_tcase(s, tc_core);

	return s;
}
