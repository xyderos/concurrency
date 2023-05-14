#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "random_barbershop.h"

static sem_t *mutex, *customer_arrives, *barber_wakes, *customer_leaves,
    *barber_sleeps;
static int chairs = 4, customers = 0;

void
random_barbershop_init(void)
{
	mutex = malloc(sizeof(sem_t));
	sem_init(mutex, 0, 1);
	customer_arrives = malloc(sizeof(sem_t));
	sem_init(customer_arrives, 0, 0);
	barber_wakes = malloc(sizeof(sem_t));
	sem_init(barber_wakes, 0, 0);
	customer_leaves = malloc(sizeof(sem_t));
	sem_init(customer_leaves, 0, 0);
	barber_sleeps = malloc(sizeof(sem_t));
	sem_init(barber_sleeps, 0, 0);
}

void
random_barbershop_destroy(void)
{
	sem_destroy(mutex);
	free(mutex);
	sem_destroy(customer_arrives);
	free(customer_arrives);
	sem_destroy(barber_wakes);
	free(barber_wakes);
	sem_destroy(customer_leaves);
	free(customer_leaves);
	sem_destroy(barber_sleeps);
	free(barber_sleeps);
}

void *
random_barber(void *arg)
{
	(void)arg;

	while (1) {
		sem_wait(customer_arrives);
		sem_post(barber_wakes);
		sem_wait(customer_leaves);
		sem_post(barber_sleeps);
	}
	return NULL;
}

void *
random_customer(void *arg)
{
	int index = *(int *)arg;

	sem_wait(mutex);

	if (customers == chairs) {
		sem_post(mutex);
		printf("Customer %d balks.\n", index);
		pthread_exit(NULL);
	}

	customers++;

	printf("Customer %d arrives.\n", index);

	sem_post(mutex);

	sem_post(customer_arrives);
	sem_wait(barber_wakes);

	printf("Customer %d gets haircut.\n", index);

	sem_post(customer_leaves);
	sem_wait(barber_sleeps);
	sem_wait(mutex);

	customers--;

	sem_post(mutex);

	return NULL;
}
