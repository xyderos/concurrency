#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "find_heuristic.h"

#define NUM_THREADS 3
static pthread_attr_t attr;
static pthread_t threads[NUM_THREADS];
static pthread_mutex_t threads_lock = PTHREAD_MUTEX_INITIALIZER;
static sem_t death_lock;

static const unsigned int to_be_found = 2;

void count_attempts_heuristic(long attempts);
void start_searches_heuristic(void);
void count_cleanups_heuristic(void *arg);
void cleanup_lock_heuristic(void *arg);
void *search_heuristic(void *arg);

// increments on both cleanups (from count_cleanups) and exits
void
count_attempts_heuristic(long attempt)
{
	// use the maximum number of counts to print the actual
	static long counter = 0, maximum_number_of_counts = 0;
	static pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;

	// each time we try to find the
	pthread_mutex_lock(&count_lock);
	counter += attempt;
	if (attempt == -1)
		printf("Total attempt count : % ld\n",
		    maximum_number_of_counts);
	if (counter > maximum_number_of_counts)
		maximum_number_of_counts = counter;
	pthread_mutex_unlock(&count_lock);
}

// callback for the cleanup handler
void
count_cleanups_heuristic(void *arg)
{
	int *ip = (int *)arg;
	int i = *ip;
	count_attempts_heuristic(i);
	printf("%lu exited or cancelled on its %d try.\n", pthread_self(), ++i);
	/* Note that you can’t tell if the thread exited, or was cancelled*/
}

void *
search_heuristic(void *arg)
{
	long tries_accumulator = 0, err, target = (long)arg;
	pthread_t tid = pthread_self();

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_cleanup_push(count_cleanups_heuristic,
	    (void *)&tries_accumulator) while (1)
	{
		tries_accumulator++;

		if (target == to_be_found) {
			printf("%lu found the needle\n", pthread_self());

			// I could also simply do sem_wait() & let
			// cancellation work, this essentially acts as a lock
			// we decrement the semaphore because we essentially
			// know we are about ot get cancelled
			while (((err = (long)sem_trywait(&death_lock)) == -1) &&
			    (errno == EINTR))
				;

			if ((err == -1) && (errno == EBUSY)) {
				printf("Thread %lu is exiting\n",
				    pthread_self());
				pthread_exit(NULL);
			}
			count_attempts_heuristic(tries_accumulator);

			// since we found our solution, lets request to cancel
			// the remaining threads
			pthread_mutex_lock(&threads_lock);
			for (int j = 0; j < NUM_THREADS; j++) {
				if (!pthread_equal(threads[j], tid)) {
					if (pthread_cancel(threads[j]) ==
					    ESRCH) {
						printf(
						    "Could not find thread %lu, it has probably exited\n",
						    threads[j]);
					} else {
						printf(
						    "Successfully sent cancellation request to %lu\n",
						    threads[j]);
					}
				}
			}

			pthread_mutex_unlock(&threads_lock);
			break;
		}
		printf("%lu did not find the needle\n", pthread_self());
		pthread_testcancel();
	}
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
}

void
start_searches_heuristic(void)
{
	unsigned long number_of_threads_cancelled = 0,
		      number_of_thread_exited = 0;
	void *return_status;

	printf("Searching for %u in the grid\n", to_be_found);

	for (unsigned long accumulator = 0; accumulator < NUM_THREADS;
	     accumulator++) {
		pthread_create(&threads[accumulator], &attr, search_heuristic,
		    (void *)accumulator);
	}

	for (unsigned long accumulator = 0; accumulator < NUM_THREADS;
	     accumulator++) {
		pthread_join(threads[accumulator], (void *)&return_status);
		if (return_status == PTHREAD_CANCELED)
			number_of_threads_cancelled++;
		else
			number_of_thread_exited++;
	}

	sem_post(&death_lock);

	// use -1 to indicate that searching is over
	count_attempts_heuristic(-1);
	printf(
	    "A total of %lu threads were cancelled, whereas a total of %lu threads exited.\n",
	    number_of_threads_cancelled, number_of_thread_exited);
}
int
find_heuristic(void)
{
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	sem_init(&death_lock, 0, 1);
	start_searches_heuristic();
	pthread_exit(NULL);
}
