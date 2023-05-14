#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "s_list.h"

#define MAX 100

static s_list_cell_t sentinel = { MAX, 0, NULL };
static s_list_cell_t dummy = { -1, 0, &sentinel };
static s_list_cell_t *global = &dummy;

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
	}
	return spin;
}

static void
unlock(volatile int *mutex)
{
	*mutex = 0;
}

static void
toggle(s_list_cell_t *lst, int r)
{
	s_list_cell_t *prev = lst, *this = NULL, *removed = NULL, *new = NULL;

	lock(&prev->mutex);

	this = prev->next;

	lock(&this->mutex);

	removed = NULL;

	while (this->val < r) {
		unlock(&prev->mutex);
		prev = this;
		this = this->next;
		lock(&this->mutex);
	}

	if (this->val == r) {
		prev->next = this->next;
		removed = this;
	} else {

		new = malloc(sizeof(s_list_cell_t));

		new->val = r;
		new->next = this;
		new->mutex = 0;
		prev->next = new;
		new = NULL;
	}
	unlock(&prev->mutex);
	unlock(&this->mutex);
	if (removed) {
		free(removed);
	}
}

static void *
bench(void *arg)
{
	unsigned long inc = ((s_list_args_t *)arg)->inc;
	s_list_cell_t *lstp = ((s_list_args_t *)arg)->list;

	for (unsigned long i = 0; i < inc; i++) {
		int r = rand() % MAX;
		toggle(lstp, r);
	}
}

void
bench_s_list(unsigned long number_of_threads,
    unsigned long number_of_operations)
{
	struct timespec t_start, t_stop;
	s_list_args_t *thra = malloc(number_of_threads * sizeof(s_list_args_t));
	pthread_t *thrt = NULL;

	long wall_sec = 0, wall_nsec = 0, wall_msec = 0;

	for (unsigned long i = 0; i < number_of_threads; i++) {
		thra[i].inc = number_of_operations;
		thra[i].id = i;
		thra[i].list = global;
	}

	clock_gettime(CLOCK_MONOTONIC_COARSE, &t_start);

	thrt = malloc(number_of_threads * sizeof(pthread_t));
	for (unsigned long i = 0; i < number_of_threads; i++) {
		pthread_create(&thrt[i], NULL, bench, &thra[i]);
	}

	for (unsigned long i = 0; i < number_of_threads; i++) {
		pthread_join(thrt[i], NULL);
	}

	clock_gettime(CLOCK_MONOTONIC_COARSE, &t_stop);

	wall_sec = t_stop.tv_sec - t_start.tv_sec;
	wall_nsec = t_stop.tv_sec - t_start.tv_sec;
	wall_msec = (wall_sec * 1000) + (wall_nsec / 1000000);

	printf("done in %ld ms \n", wall_msec);
}
