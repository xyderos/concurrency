#pragma once

#include <pthread.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#define HUNGER 100
#define nrPhil 500

struct Chopstick {

	pthread_mutex_t lock;
};

void
thinking()
{

	long time = (rand() & 2) / 4;

	sleep(time);
}

class monitor {

    public:
	void lFork(long id)
	{

		if (id != nrPhil)
			pthread_mutex_lock(&forks[id - 1].lock);

		else
			pthread_mutex_lock(&forks[id].lock);
	}

	void rFork(long id)
	{

		if (id != nrPhil)
			pthread_mutex_lock(&forks[id].lock);

		else
			pthread_mutex_lock(&forks[id - 1].lock);
	}

	void release(long id)
	{

		pthread_mutex_unlock(&forks[id].lock);

		pthread_mutex_unlock(&forks[id - 1].lock);
	}

    private:
	Chopstick forks[nrPhil + 1];

} table;