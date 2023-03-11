#include <check.h>
#include <pthread.h>
#include <stdlib.h>

#include "../../src/lifecycle/attributes.h"

Suite *lifecycle_suite(void);

START_TEST(TEST_CUSTOM_ATTRIBUTES_INIT_SHOULD_BE_OKAY)
{
	pthread_t t = 0;
	int res = 0;

	pthread_attr_t atr = init_attributes(PTHREAD_STACK_MIN + 0X5000);

	res = pthread_create(&t, &atr, worker, NULL);

	ck_assert_int_eq(res, 0);

	pthread_attr_destroy(&atr);
}
END_TEST

START_TEST(TEST_MIN_STACK_POSSIBLE_SHOULD_BE_OKAY)
{
	pthread_t t = 0;
	int res = 0, rc = 0;

	pthread_attr_t atr = min_stack_possible();

	res = pthread_create(&t, &atr, worker, NULL);

	ck_assert_int_eq(res, 0);

	rc = pthread_join(t, NULL);

	ck_assert_int_eq(rc, 0);

	pthread_attr_destroy(&atr);
}
END_TEST
