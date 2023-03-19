#include <stdlib.h>

#include "dining_philosophers.h"

static int tHunger;

void *
philosopher(void *arg)
{

	int hunger = tHunger;

	while (hunger > 0) {

		thinking();

		table.lFork((long)arg);

		table.rFork((long)arg);

		hunger--;

		printf("Philosopher %lu has a hunger of %d\n", (long)arg,
		    hunger);

		table.release((long)arg);
	}
	return nullptr;
}

int
main(void)
{

	tHunger = HUNGER;

	pthread_t philosophers[nrPhil + 1];

	for (long i = 1; i <= nrPhil; i++)
		pthread_create(&philosophers[i], nullptr, philosopher,
		    (void *)i);

	printf("threads created\n");

	for (int i = 1; i <= nrPhil; i++)
		pthread_join(philosophers[i], nullptr);

	return EXIT_SUCCESS;
}
