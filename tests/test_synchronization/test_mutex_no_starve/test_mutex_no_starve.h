#include <check.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/synchronization/mutex_no_starve/mutex_no_starve.h"

Suite *mutex_no_starve_suite(void);

static non_starving_mutex_t m;
static int counter = 0;

static void *
non_starving_mutex_worker(void *arg)
{
	(void)arg;
	non_starving_mutex_acquire(&m);
	counter++;
	non_starving_mutex_release(&m);

	return NULL;
}

START_TEST(TEST_MUTEX_NO_STARVE)
{
	int num_threads = 10;
	non_starving_mutex_init(&m);

	pthread_t p[num_threads];

	int i;
	for (i = 0; i < num_threads; i++)
		pthread_create(&p[i], NULL, non_starving_mutex_worker, NULL);

	for (i = 0; i < num_threads; i++)
		pthread_join(p[i], NULL);
}
END_TEST
