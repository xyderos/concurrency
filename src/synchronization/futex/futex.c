#include <sys/syscall.h>

#include <stdio.h>
#include <unistd.h>

#include "futex.h"

static int count = 0;
static volatile int global = 0;

static long
futex_wait(volatile int *futexp)
{
	return syscall(SYS_futex, futexp, FUTEX_WAIT, 1, NULL, NULL, 0);
}

static void
futex_wake(volatile int *futexp)
{
	syscall(SYS_futex, futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
}

static int
try(volatile int *lock)
{
	return __sync_val_compare_and_swap(lock, 0, 1);
}

static int
lock(volatile int *lock)
{
	int spin = 0;
	while (try(lock) != 0) {
		spin++;
		futex_wait(lock);
	}
	return spin;
}

static void
unlock(volatile int *lock)
{
	*lock = 0;
	futex_wake(lock);
}

static void *
increment(void *arg)
{
	unsigned long inc = ((futex_args_t *)arg)->inc, tot = 0;
	volatile int *mutex = ((futex_args_t *)arg)->mutex;

	for (unsigned long i = 0; i < inc; i++) {
		tot += (unsigned long)lock(mutex);
		count++;
		unlock(mutex);
	}
	printf("number of tries:  %lu\n", tot);
}

void
demonstrate_futex(unsigned long number_of_operations)
{
	pthread_t one_p, two_p;
	futex_args_t one_args, two_args;

	one_args.inc = number_of_operations;
	two_args.inc = number_of_operations;

	one_args.id = 0;
	two_args.id = 1;

	one_args.mutex = &global;
	two_args.mutex = &global;

	pthread_create(&one_p, NULL, increment, &one_args);
	pthread_create(&two_p, NULL, increment, &two_args);
	pthread_join(one_p, NULL);
	pthread_join(two_p, NULL);

	printf("result is %d\n", count);
}
