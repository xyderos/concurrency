#pragma once

#include <pthread.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#define BABYBIRDS 10
#define WORMS 10
#define HUNGER 10

class monitor {

    public:
	explicit monitor()
	{

		pthread_mutex_init(&mutex, NULL);

		pthread_cond_init(&fill, NULL);

		pthread_cond_init(&empty, NULL);
	}
	void fillDish(void *arg)
	{

		pthread_mutex_lock(&mutex);

		while (worms != 0)
			pthread_cond_wait(&empty, &mutex);

		worms = WORMS;

		printf("Parent filled the dish, it is now full\n");

		pthread_cond_signal(&fill);

		pthread_mutex_unlock(&mutex);
	}

	void eatWorm(void *arg)
	{

		pthread_mutex_lock(&mutex);

		while (worms == 0)
			pthread_cond_wait(&fill, &mutex);

		worms--;

		printf("Bird %ld is eating worm #%d\n", (long)arg, worms);

		pthread_cond_signal(&empty);

		pthread_mutex_unlock(&mutex);

		sleep(static_cast<unsigned int>(0.5));
	}

    private:
	int worms = WORMS;

	pthread_mutex_t mutex;

	pthread_cond_t fill;

	pthread_cond_t empty;

} dish;
