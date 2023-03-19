#pragma once

#include <pthread.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#define BEES 10
#define CAPACITY 20
#define HUNGER 10

class monitor {
    public:
	explicit monitor()
	{

		pthread_mutex_init(&mutex, nullptr);

		pthread_cond_init(&fill, nullptr);

		pthread_cond_init(&empty, nullptr);
	}

	void bear(void *arg)
	{
		(void)arg;

		pthread_mutex_lock(&mutex);

		while (honeypot != CAPACITY) {
			pthread_cond_wait(&empty, &mutex);
			printf("The bear woke up\n");
		}
		honeypot = 0;

		printf("The bear ate everything\n");

		pthread_cond_signal(&fill);

		pthread_mutex_unlock(&mutex);
	}

	void bees(void *arg)
	{
		pthread_mutex_lock(&mutex);
		while (honeypot == CAPACITY) {
			pthread_cond_wait(&fill, &mutex);
		}
		honeypot++;
		printf("Bee %ld fills up honeypot = %d\n", (long)arg, honeypot);
		if (honeypot == CAPACITY) {
			pthread_cond_signal(&empty);
		}
		pthread_mutex_unlock(&mutex);
		sleep(0.5);
	}

    private:
	int honeypot = 0;

	pthread_mutex_t mutex;

	pthread_cond_t fill;

	pthread_cond_t empty;
} monitor;

int bear_honey_bees_monitors(void);
