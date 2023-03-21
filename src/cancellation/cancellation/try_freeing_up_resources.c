#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "try_freeing_up_resources.h"

#define NUM_THREADS 3
static pthread_attr_t attr;
static pthread_t threads[NUM_THREADS];
static pthread_mutex_t threads_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t rand_lock = PTHREAD_MUTEX_INITIALIZER;
static sem_t death_lock;

static unsigned long c = 0;

static const unsigned int to_be_found = 2;

void count_attempts_freeing_resources(long attempts);
void start_searches_freeing_resources(void);
void count_cleanups_freeing_resources(void *arg);
void cleanup_lock_freeing_resources(void *arg);
void *search_freeing_resources(void *arg);

// increments on both cleanups (from count_cleanups) and exits
void
count_attempts_freeing_resources(long attempt)
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
count_cleanups_freeing_resources(void *arg)
{
	int *ip = (int *)arg;
	int i = *ip;
	count_attempts_freeing_resources(i);
	printf("%lu exited or cancelled on its %d try.\n", pthread_self(), ++i);
}

void
cleanup_lock_freeing_resources(void *arg)
{
	printf("Freeing & releasing : %lu \n", pthread_self());
	free(arg);
	pthread_mutex_unlock(&rand_lock);
}

void *
search_freeing_resources(void *arg)
{
	char *trash_that_should_be_cleaned_up;
	long tries_accumulator = 0, err;
	pthread_t tid = pthread_self();
	(void)arg;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_cleanup_push(count_cleanups_freeing_resources,
	    (void *)&tries_accumulator) while (1)
	{
		tries_accumulator++;

		pthread_mutex_lock(&rand_lock);
		c++;
		trash_that_should_be_cleaned_up = (char *)malloc(10);

		// we are adding a cleanup handler for the memory to be called
		// in case we hit the cancellation point
		pthread_cleanup_push(cleanup_lock_freeing_resources,
		    (void *)trash_that_should_be_cleaned_up)

		    // calling them a lot of times so we can make sure that we
		    // have cancellation points

		    // above this line, the cleanup handler will clean the
		    // memory below, free will
		    pthread_testcancel();
		free(trash_that_should_be_cleaned_up);
		pthread_cleanup_pop(0);
		pthread_mutex_unlock(&rand_lock);

		if (c == to_be_found) {
			printf(
			    "%lu incremented the lock to the target from the critical section\n",
			    pthread_self());

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
			count_attempts_freeing_resources(tries_accumulator);

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
		printf(
		    "%lu Did not match the target within the critical section\n",
		    pthread_self());
		pthread_testcancel();
	}
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
}

void
start_searches_freeing_resources(void)
{
	unsigned long number_of_threads_cancelled = 0,
		      number_of_thread_exited = 0;
	void *return_status;

	printf("Searching for %u in the grid\n", to_be_found);

	for (unsigned long accumulator = 0; accumulator < NUM_THREADS;
	     accumulator++) {
		pthread_create(&threads[accumulator], &attr,
		    search_freeing_resources, NULL);
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
	count_attempts_freeing_resources(-1);
	printf(
	    "A total of %lu threads were cancelled, whereas a total of %lu threads exited.\n",
	    number_of_threads_cancelled, number_of_thread_exited);
}
int
try_freeing_up_resources(void)
{
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	sem_init(&death_lock, 0, 1);
	start_searches_freeing_resources();
	pthread_exit(NULL);
}
