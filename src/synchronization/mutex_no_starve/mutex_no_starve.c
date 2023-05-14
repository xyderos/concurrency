#include <sys/stat.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "mutex_no_starve.h"

static const char *const non_starving_mutex_string = "/mutex",
			 *const thread_one_string = "/t1",
			 *const thread_two_string = "/t2";

void
non_starving_mutex_init(non_starving_mutex_t *m)
{
	m->room1 = 0;
	m->room2 = 0;
	m->mutex = sem_open(non_starving_mutex_string, O_CREAT, S_IRWXU, 1);
	m->t1 = sem_open(thread_one_string, O_CREAT, S_IRWXU, 1);
	m->t2 = sem_open(thread_two_string, O_CREAT, S_IRWXU, 0);
}

void
non_starving_mutex_acquire(non_starving_mutex_t *mtx)
{
	sem_wait(mtx->mutex);
	mtx->room1++;
	sem_post(mtx->mutex);

	sem_wait(mtx->t1);
	mtx->room2++;
	sem_wait(mtx->mutex);
	mtx->room1--;

	if (!mtx->room1) {
		sem_post(mtx->mutex);
		sem_post(mtx->t2);
	} else {
		sem_post(mtx->mutex);
		sem_post(mtx->t1);
	}

	sem_wait(mtx->t2);
	mtx->room2--;
}

void
non_starving_mutex_release(non_starving_mutex_t *mtx)
{
	if (!mtx->room2) {
		sem_post(mtx->t1);
	} else {
		sem_post(mtx->t2);
	}
}

void
non_starving_mutex_destroy(non_starving_mutex_t *m)
{
	sem_close(m->mutex);
	sem_close(m->t1);
	sem_close(m->t2);
	sem_unlink(non_starving_mutex_string);
	sem_unlink(thread_one_string);
	sem_unlink(thread_two_string);
}
