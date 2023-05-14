#include "test_queue.h"

Suite *
queue_suite(void)
{
	Suite *s = NULL;
	TCase *tc_core = NULL;

	s = suite_create("queue");
	tc_core = tcase_create("core");
	tcase_set_timeout(tc_core, 10);

	tcase_add_test(tc_core, TEST_QUEUE);

	suite_add_tcase(s, tc_core);

	return s;
}
