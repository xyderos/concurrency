#include "hungry_birds_monitors.h"

static int babyB, babyHungry, hunger;

void *
parent(void *arg)
{
	while (babyHungry >= 0)
		dish.fillDish(arg);
}

void *
child(void *arg)
{
	while (hunger != 0)
		dish.eatWorm(arg);
}

int
hungry_birds_monitors()
{

	pthread_attr_t attr;

	pthread_t workerid[BABYBIRDS + 1];

	pthread_attr_init(&attr);

	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	hunger = HUNGER;

	babyB = BABYBIRDS;

	babyHungry = babyB;

	long i = 0;

	pthread_create(&workerid[0], NULL, parent, (void *)i);

	for (i = 1; i <= babyB; i++)
		pthread_create(&workerid[i], NULL, child, (void *)i);

	for (i = 1; i <= babyB; i++)
		pthread_join(workerid[i], NULL);
}
