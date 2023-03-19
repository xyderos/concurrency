#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "unisex_bathroom.h"

#define SHARED 0       /* The semaphores should not be shared. */
#define MAX_MEN 10     /* The max number of males possible. */
#define MAX_WOMEN 10   /* The max number of women possible. */
#define MAX_VISITS 20  /* Max number of times the bathroom is used. */
#define MAX_TIMEIN 3   /* Max seconds in the bathroom. */
#define MAX_INTERVAL 5 /* Max time between visits to the bathroom. */

static sem_t lock, waiting_male, waiting_female;

static int males_inside = 0;
static int females_inside = 0;
static int males_queue = 0;
static int females_queue = 0;

static unsigned long number_of_men, number_of_women, number_of_visits;

static int men_leaving = 0, women_leaving = 0;

void *male(void *arg);
void *female(void *arg);

void *
male(void *arg)
{
	unsigned long visit, id = (unsigned long)arg;

	for (visit = 0; visit < number_of_visits; visit++) {
		sleep((unsigned int)rand() % MAX_INTERVAL);

		sem_wait(&lock);
		if (females_inside > 0 || men_leaving) {
			males_queue++;
			sem_post(&lock);
			sem_wait(&waiting_male);
		}
		males_inside++;
		if (males_queue > 0) {
			males_queue--;
			sem_post(&waiting_male);
		} else {
			sem_post(&lock);
		}

		printf("man %lu enters the bathroom - visit :  #%lu\n", id,
		    visit + 1);

		sleep((unsigned int)rand() % MAX_TIMEIN);

		sem_wait(&lock);
		males_inside--;
		men_leaving = 1;

		if (males_inside == 0)
			men_leaving = 0;

		if (males_inside == 0 && females_queue > 0) {
			females_queue--;
			sem_post(&waiting_female);
		} else {
			sem_post(&lock);
		}
	}
	pthread_exit(NULL);
}

void *
female(void *arg)
{
	unsigned long id = (unsigned long)arg, visit;

	for (visit = 0; visit < number_of_visits; visit++) {
		sleep(rand() % MAX_INTERVAL);

		sem_wait(&lock);
		if (males_inside > 0 || women_leaving) {
			females_queue++;
			sem_post(&lock);
			sem_wait(&waiting_female);
		}
		females_inside++;
		if (females_queue > 0) {
			females_queue--;
			sem_post(&waiting_female);
		} else {
			sem_post(&lock);
		}

		printf("woman %lu enters the bathroom - visit :  #%lu\n", id,
		    visit + 1);

		sleep((unsigned int)rand() % MAX_TIMEIN);

		sem_wait(&lock);
		females_inside--;

		women_leaving = 1;
		if (females_inside == 0)
			women_leaving = 0;

		if (females_inside == 0 && males_queue > 0) {
			males_queue--;
			sem_post(&waiting_male);
		} else {
			sem_post(&lock);
		}
	}
	pthread_exit(NULL);
}

int
unisex_bathroom(void)
{
	unsigned long i;

	number_of_women = MAX_WOMEN;
	number_of_men = MAX_MEN;
	number_of_visits = MAX_VISITS;

	pthread_t men[number_of_men], women[number_of_women];

	sem_init(&lock, 0, 1);
	sem_init(&waiting_male, 0, 0);
	sem_init(&waiting_female, 0, 0);

	for (i = 0; i < number_of_women; i++) {
		pthread_create(&women, NULL, female, (void *)i);
	}

	for (i = 0; i < number_of_men; i++) {
		pthread_create(&men[i], NULL, male, (void *)i);
	}

	for (i = 0; i < number_of_women; i++) {
		pthread_join(women[i], NULL);
	}

	for (i = 0; i < number_of_men; i++) {
		pthread_join(men[i], NULL);
	}

	sem_close(&lock);
	sem_close(&waiting_female);
	sem_close(&waiting_male);

	return EXIT_SUCCESS;
}
