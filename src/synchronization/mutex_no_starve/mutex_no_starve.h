#pragma once

#include <semaphore.h>

typedef struct non_starving_mutex {
	int room1;
	int room2;
	sem_t *mutex;
	sem_t *t1;
	sem_t *t2;
} non_starving_mutex_t;

void non_starving_mutex_init(non_starving_mutex_t *);

void non_starving_mutex_acquire(non_starving_mutex_t *);

void non_starving_mutex_release(non_starving_mutex_t *);

void non_starving_mutex_destroy(non_starving_mutex_t *);
