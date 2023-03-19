#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "tee.h"

// 0 is occupied from the parent thread, 4 is to not exceed the array
#define MAXWORKERS 4

static char *buffer;

static FILE *file;

static int run = 1;

static int signal_from_producer_to_one_consumer = 1;

static int signal_from_producer_to_another_consumer = 1;

static int signal_from_one_consumer_to_producer = 0;

static int signal_from_another_consumer_to_producer = 0;

void *input(void *arg);
void *one_output(void *arg);
void *another_output(void *arg);

void *
input(void *arg)
{
	// silence warnings
	(void)arg;

	while (run) {
		// tell the consumers to wait
		signal_from_one_consumer_to_producer =
		    signal_from_another_consumer_to_producer = 0;

		// time to exit
		if (feof(stdin)) {
			run = 0;
			break;
		}

		while (fgets(buffer, 10, stdin) == NULL && run) {
			// break when parsing the input is done
			if (feof(stdin)) {
				run = 0;
				break;
			}
		}

		// set conditions ready to be consumed
		signal_from_producer_to_one_consumer =
		    signal_from_producer_to_another_consumer = 0;

		// wait until the consumers processed the result
		while (!(signal_from_one_consumer_to_producer &&
		    signal_from_another_consumer_to_producer)) {
		}
	}
	return EXIT_SUCCESS;
}

void *
one_output(void *arg)
{
	// silence warnings
	(void)arg;

	while (run) {
		// wait until producer is done
		while (signal_from_producer_to_one_consumer) {
		}

		if (!feof(stdin)) {
			(void)fprintf(file, "%s", buffer);
			(void)fflush(file);
		}

		// signal the producer that we are done
		signal_from_producer_to_one_consumer =
		    signal_from_one_consumer_to_producer = 1;
	}
	return EXIT_SUCCESS;
}

void *
another_output(void *arg)
{
	// silence warnings
	(void)arg;

	while (run) {
		// wait until producer is done
		while (signal_from_producer_to_another_consumer) {
		}

		if (!feof(stdin))
			printf("%s", buffer);

		// signal the producer that we are done
		signal_from_producer_to_another_consumer =
		    signal_from_another_consumer_to_producer = 1;
	}

	return EXIT_SUCCESS;
}

int
tee(const char *const file_name)
{
	pthread_t workerid[MAXWORKERS];

	file = fopen(file_name, "w");

	if (!file)
		return EXIT_FAILURE;

	buffer = (char *)malloc(sizeof(char) * 256);

	pthread_create(&workerid[1], NULL, input, NULL);

	pthread_create(&workerid[2], NULL, one_output, NULL);

	pthread_create(&workerid[3], NULL, another_output, NULL);

	pthread_join(workerid[1], NULL);

	pthread_join(workerid[2], NULL);

	pthread_join(workerid[3], NULL);

	pthread_exit(EXIT_SUCCESS);
}
