#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include "dining_savages.h"

static sem_t *empty_pot, *full_pot, *mutex;
static int stewed_missionaries = 0, pot_size = 2;

void
savages_init(void)
{
	empty_pot = malloc(sizeof(sem_t));
	sem_init(empty_pot, 0, 0);
	full_pot = malloc(sizeof(sem_t));
	sem_init(full_pot, 0, 0);
	mutex = malloc(sizeof(sem_t));
	sem_init(mutex, 0, 1);
}

void
savages_destroy(void)
{
	sem_destroy(empty_pot);
	free(empty_pot);
	sem_destroy(full_pot);
	free(full_pot);
	sem_destroy(mutex);
	free(mutex);
}

void *
cook(void *arg)
{
	int savages_num = *(int *)arg;

	int loop = 0;

	if (savages_num % pot_size == 0) {
		loop = savages_num / pot_size;
	} else {
		loop = savages_num / pot_size + 1;
	}

	for (int i = 0; i < loop; i++) {
		sem_wait(empty_pot);

		printf("Cook fills the pot with %d stewed missionaries\n",
		    pot_size);

		sem_post(full_pot);
	}
	return NULL;
}

void *
savage(void *arg)
{
	int index = *(int *)arg;

	sem_wait(mutex);

	if (stewed_missionaries == 0) {

		sem_post(empty_pot);
		sem_wait(full_pot);

		stewed_missionaries = pot_size;
	}
	stewed_missionaries--;

	printf("Savage %d feats on a missionary\n", index);

	sem_post(mutex);

	return NULL;
}
