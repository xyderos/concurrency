#include <pthread.h>
#include <stdlib.h>
#include <time.h>

void *
thread_sleep(void *arg)
{
	long secs = (long)arg;
	struct timespec remaining, request = { secs, 0 };
	nanosleep(&request, &remaining);

	pthread_exit(EXIT_SUCCESS);
}
