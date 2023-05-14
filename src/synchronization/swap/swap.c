#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "swap.h"
static int count = 0;
static volatile int global = 0;

static int
try(volatile int *mutex)
{
	return __sync_val_compare_and_swap(mutex, 0, 1);
}

static int
lock(volatile int *mutex)
{
	int spin = 0;
	while (try(mutex) != 0) {
		spin++;
		sched_yield();
	}
	return spin;
}

static void
unlock(volatile int *mutex)
{
	*mutex = 0;
}

static void *
increment(void *arg)
{
	unsigned long inc = ((swap_args_t *)arg)->inc;
	volatile int *mutex = ((swap_args_t *)arg)->mutex;
	int tot = 0;

	for (unsigned long i = 0; i < inc; i++) {
		tot += lock(mutex);
		count++;
		unlock(mutex);
	}
	printf("number of tries:  %d\n", tot);
}

void
demonstrate_swap(unsigned long number_of_calculations)
{
	pthread_t one_p, two_p;
	swap_args_t one_args, two_args;
	struct timespec t_start, t_stop;
	long wall_sec = 0, wall_nsec = 0, wall_msec = 0;

	one_args.inc = number_of_calculations;
	two_args.inc = number_of_calculations;

	one_args.id = 0;
	two_args.id = 1;

	one_args.mutex = &global;
	two_args.mutex = &global;

	clock_gettime(CLOCK_MONOTONIC_COARSE, &t_start);

	pthread_create(&one_p, NULL, increment, &one_args);
	pthread_create(&two_p, NULL, increment, &two_args);
	pthread_join(one_p, NULL);
	pthread_join(two_p, NULL);

	clock_gettime(CLOCK_MONOTONIC_COARSE, &t_stop);

	wall_sec = t_stop.tv_sec - t_start.tv_sec;
	wall_nsec = t_stop.tv_sec - t_start.tv_sec;
	wall_msec = (wall_sec * 1000) + (wall_nsec / 1000000);

	printf("done in %ld ms \n", wall_msec);
	printf("result is %d\n", count);
}
