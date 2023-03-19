#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CARS 3
#define TRIPS 2
#define MAXCAREACHTIME 100

static sem_t north, south, counter;

static int totalCars = CARS;

static int totalPass = MAXCAREACHTIME;

static int counterNorth, counterSouth;

static int totalCarsNorth = (CARS / 2);

static int totalCarsSouth = ((CARS + 1) / 2);

void *
Car(void *arg)
{
	int trips = TRIPS;

	int value = (long)arg;

	int tmp1;

	int tmp2;

	while (trips > 0) {
		if (value % 2 == 0) {
			sem_getvalue(&north, &tmp1);

			if (totalCarsNorth == 0)
				sem_post(&south);

			else {
				sem_wait(&north);

				counterNorth++;

				trips--;

				value++;

				if (totalCarsNorth < totalPass)
					totalPass = totalCarsNorth;

				totalCarsNorth--;

				if (trips == 0)
					totalCarsSouth--;

				printf(
				    "\nCar %ld coming from north has %d trips left , counter from "
				    "north "
				    "is %d\n",
				    (long)arg, trips, counterNorth);

				if (counterNorth >= totalPass) {
					counterNorth = 0;

					totalCarsSouth++;

					totalPass = MAXCAREACHTIME;

					sem_post(&south);
				} else {
					totalCarsSouth++;

					sem_post(&north);
				}
			}
		} else {
			sem_getvalue(&north, &tmp2);

			if (totalCarsSouth == 0)
				sem_post(&north);

			else {
				sem_wait(&south);

				counterSouth++;

				trips--;

				value++;

				if (totalCarsSouth < totalPass)
					totalPass = totalCarsSouth;

				totalCarsSouth--;

				if (trips == 0)
					totalCarsNorth--;

				printf(
				    "\nCar %ld coming from south has %d trips left , counter from "
				    "south "
				    "is %d\n",
				    (long)arg, trips, counterSouth);

				if (counterSouth >= totalPass) {
					printf(
					    "\nNumber of cars that were allowed to go is %d\n",
					    totalPass);

					counterSouth = 0;

					totalCarsNorth++;

					totalPass = MAXCAREACHTIME;

					sem_post(&north);
				} else {
					totalCarsNorth++;
					sem_post(&south);
				}
			}
		}
	}
	sem_wait(&counter);

	totalCars--;

	sem_getvalue(&north, &tmp1);

	sem_getvalue(&south, &tmp2);

	printf("\nCar %lu finished and exited, There are %d cars left, {N=%d, "
	       "S=%d},{CarN "
	       "= "
	       "%d, CarS = %d}\n",
	    (unsigned long)arg, totalCars, tmp1, tmp2, totalCarsNorth,
	    totalCarsSouth);

	if (totalCarsNorth == 0)
		sem_post(&south);

	if (totalCarsSouth == 0)
		sem_post(&north);

	sem_post(&counter);
}

int
main(void)
{
	pthread_attr_t attr;

	pthread_t workerid[CARS];

	pthread_attr_init(&attr);

	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	sem_init(&north, 0, 1);

	sem_init(&south, 0, 0);

	sem_init(&counter, 0, 1);

	printf(
	    "Total cars coming from the north %d, total cars coming from the south "
	    "%d\n",
	    totalCarsSouth, totalCarsNorth);

	int cars = CARS;

	long i = 0;

	for (i = 1; i <= cars; i++)
		pthread_create(&workerid[i], NULL, Car, (void *)i);

	for (i = 1; i <= cars; i++)
		pthread_join(workerid[i], NULL);
}
