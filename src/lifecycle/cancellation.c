#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "cancellation.h"

static pthread_t thr_a, thr_b, thr_c, thr_d, thr_e, core;
static pthread_attr_t attr;

void *first_routine(void *arg);
void *second_routine(void *arg);
void *third_routine(void *arg);
void *fourth_routine(void *arg);
void *fifth_routine(void *arg);
void cleanup(void *arg);

void *
first_routine(void *arg)
{
	(void)arg;
	sleep(1);
	pthread_create(&thr_d, &attr, fourth_routine, NULL);
	sleep(3);
	pthread_exit((void *)300);
}

void *
second_routine(void *arg)
{
	(void)arg;
	sleep(4);
	pthread_exit(NULL);
}

void *
third_routine(void *arg)
{
	void *status = NULL;
	(void)arg;

	sleep(2);
	pthread_join(core, &status);

	assert((int)status == 0);

	sleep(1);

	pthread_create(&thr_b, &attr, second_routine, NULL);
	sleep(4);

	pthread_exit((void *)400);
}

void *
fourth_routine(void *arg)
{
	void (*fptr)(void *) = cleanup;
	(void)arg;

	// add a callback, keep the comma to not break formatting
	pthread_cleanup_push(fptr, NULL);

	// do not do that!
	// Canceling asynchronously would follow the same route as passing a
	// signal into the thread to kill it, posing problems similar to those
	// in CON37-C. Do not call signal() in a multithreaded program, which is
	// strongly related to SIG02-C. Avoid using signals to implement normal
	// functionality. POS44-C and SIG02-C expand on the dangers of canceling
	// a thread suddenly, which can create a data race condition.
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	sleep(1);

	pthread_create(&thr_e, &attr, fifth_routine, NULL);
	sleep(5);
	pthread_cleanup_pop(0);
	return ((void *)200);
}

void *
fifth_routine(void *arg)
{
	void *status = NULL;

	sleep(3);

	pthread_join(thr_a, &status);

	assert((int)status == 300);
	sleep(2);
	pthread_join(thr_c, &status);
	assert((int)status == 400);
	sleep(2);
	pthread_join(thr_d, &status);
	assert((void *)status == (void *)PTHREAD_CANCELED);
	sleep(1);
	pthread_exit((void *)100);
}

void
cleanup(void *arg)
{
	(void)arg;
	// some cleanup routine
}

void
cancellation_simulation(void)
{
	core = pthread_self();

	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	sleep(1);

	pthread_create(&thr_a, &attr, first_routine, NULL);
	sleep(1);

	pthread_create(&thr_c, &attr, third_routine, NULL);
	sleep(2);

	pthread_cancel(thr_d);
	sleep(1);

	pthread_exit((void *)NULL);
}
