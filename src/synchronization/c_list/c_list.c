#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "c_list.h"

#define MAX 100

static c_list_cell_t sentinel = { MAX, NULL, PTHREAD_MUTEX_INITIALIZER };
static c_list_cell_t dummy = { -1, &sentinel, PTHREAD_MUTEX_INITIALIZER };
static c_list_cell_t *global = &dummy;

static void
toggle(c_list_cell_t *lst, int r)
{
	c_list_cell_t *prev = lst, *this = NULL, *removed = NULL, *new = NULL;

	pthread_mutex_lock(&prev->mutex);

	this = prev->next;

	pthread_mutex_lock(&this->mutex);

	removed = NULL;

	while (this->val < r) {
		pthread_mutex_unlock(&prev->mutex);

		prev = this;
		this = this->next;

		pthread_mutex_lock(&this->mutex);
	}

	if (this->val == r) {

		prev->next = this->next;
		removed = this;

	} else {

		new = malloc(sizeof(c_list_cell_t));

		new->val = r;
		new->next = this;

		pthread_mutex_init(&new->mutex, NULL);

		prev->next = new;
		new = NULL;
	}
	pthread_mutex_unlock(&prev->mutex);
	pthread_mutex_unlock(&this->mutex);

	if (removed) {
		free(removed);
	}
}

static void *
bench(void *arg)
{
	unsigned long inc = ((c_list_args_t *)arg)->inc;

	c_list_cell_t *lstp = ((c_list_args_t *)arg)->list;

	for (unsigned long i = 0; i < inc; i++) {
		int r = rand() % MAX;
		toggle(lstp, r);
	}
}

void
bench_c_list(unsigned long number_of_threads,
    unsigned long number_of_operations)
{
	struct timespec t_start, t_stop;
	long wall_sec = 0, wall_nsec = 0, wall_msec = 0;

	c_list_args_t *thra = malloc(
	    (unsigned long)number_of_threads * sizeof(c_list_args_t));
	pthread_t *thrt = malloc(number_of_threads * sizeof(pthread_t));

	for (unsigned long i = 0; i < number_of_threads; i++) {
		thra[i].inc = number_of_operations;
		thra[i].id = i;
		thra[i].list = global;
	}

	clock_gettime(CLOCK_MONOTONIC_COARSE, &t_start);

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
