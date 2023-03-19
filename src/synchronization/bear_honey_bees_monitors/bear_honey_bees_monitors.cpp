#include "bear_honey_bees_monitors.h"

static int number_of_bees, bear_runner, bee_runner;

void *
bear(void *arg)
{
	while (bear_runner >= 0)
		monitor.bear(arg);
}

void *
bee(void *arg)
{
	while (bee_runner != 0)
		monitor.bees(arg);
}

int
bear_honey_bees_monitors(void)
{

	pthread_attr_t attr;

	pthread_t workerid[BEES + 1];

	pthread_attr_init(&attr);

	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	number_of_bees = BEES;

	bear_runner = HUNGER;

	bee_runner = CAPACITY;

	long i = 0;
	pthread_create(&workerid[0], NULL, bear, (void *)i);

	printf("waiting\n");

	for (i = 1; i <= number_of_bees; i++)
		pthread_create(&workerid[i], NULL, bee, (void *)i);

	for (i = 1; i <= number_of_bees; i++)
		pthread_join(workerid[i], NULL);
}
