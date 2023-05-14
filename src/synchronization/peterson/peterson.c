#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "peterson.h"

static volatile int request[2] = { 0, 0 };
static volatile unsigned long turn = 0, count = 0;

static void
lock(unsigned long id)
{
	unsigned long other = 1 - id;

	request[id] = 1;
	turn = other;

	while (request[other] == 1 && turn == other) {
	}
}

static void
unlock(unsigned long id)
{
	request[id] = 0;
}

static void *
increment(void *arg)
{
	unsigned long inc = ((peterson_args_t *)arg)->inc;
	unsigned long id = ((peterson_args_t *)arg)->id;

	for (unsigned long i = 0; i < inc; i++) {
		lock(id);
		count++;
		unlock(id);
	}
}

void
demonstrate_peterson(unsigned long number_of_operations)
{
	pthread_t one_p, two_p;
	peterson_args_t one_args, two_args;

	one_args.inc = number_of_operations;
	two_args.inc = number_of_operations;

	one_args.id = 0;
	two_args.id = 1;

	pthread_create(&one_p, NULL, increment, &one_args);
	pthread_create(&two_p, NULL, increment, &two_args);
	pthread_join(one_p, NULL);
	pthread_join(two_p, NULL);

	printf("result is %lu\n", count);
}
