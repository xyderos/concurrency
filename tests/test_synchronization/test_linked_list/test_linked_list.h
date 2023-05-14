#include <sys/time.h>

#include <check.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/synchronization/linked_list/linked_list.h"

Suite *linked_list_suite(void);

static void *
list_thread_function(void *args)
{
	list_t *l = (list_t *)args;
	list_lookup(l, 0);
	pthread_exit(EXIT_SUCCESS);
}

START_TEST(TEST_LINKED_LIST)
{
	int list_length = 100;
	int thread_count = 10;

	list_t *list = list_init();

	for (int i = 0; i < list_length; i++)
		list_insert(list, i);

	for (int i = 1; i <= thread_count; i++) {

		pthread_t *threads = malloc((size_t)i * sizeof(pthread_t));

		for (int j = 0; j < i; j++)
			pthread_create(&threads[j], NULL, &list_thread_function,
			    list);
		for (int k = 0; k < i; k++)
			pthread_join(threads[k], NULL);

		free(threads);
	}

	list_free(list);
}
END_TEST
