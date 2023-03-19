#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diff.h"
#define MAXSIZE 25
#define MAXWORKERS 3

static char *b1, *b2, *l1, *l2;

static FILE *file1, *file2;

static int done_reading_a_line_from_first_source = 0;

static int done_reading_a_line_from_second_source = 0;

static int run = 0;

void *a_source(void *arg);
void *another_source(void *arg);
void *output(void *arg);

void *
a_source(void *arg)
{
	// silence warnings
	(void)arg;

	while (!run) {
		if (!done_reading_a_line_from_first_source) {
			l1 = fgets(b1, MAXSIZE, file1);
			done_reading_a_line_from_first_source = 1;
		}
	}
	return NULL;
}
void *
another_source(void *arg)
{
	// silence warnings
	(void)arg;
	while (!run) {
		if (!done_reading_a_line_from_second_source) {
			l2 = fgets(b2, MAXSIZE, file2);
			done_reading_a_line_from_second_source = 1;
		}
	}
	return NULL;
}
void *
output(void *arg)
{
	// silence warnings
	(void)arg;

	while (!run) {
		// wait until you have read two lines sucessfully
		while (!(done_reading_a_line_from_first_source &&
		    done_reading_a_line_from_second_source)) {
		}

		if (!l1 && !l2)
			run = 1;

		else if (l1 == NULL) {
			printf("> %s", l2);
			done_reading_a_line_from_second_source = 0;
		} else if (l2 == NULL) {
			printf("> %s", l1);
			done_reading_a_line_from_first_source = 0;
		} else {
			if (strcmp(l1, l2) == 0) {
				done_reading_a_line_from_first_source = 0;
				done_reading_a_line_from_second_source = 0;
			} else {
				printf("< %s---\n> %s", l1, l2);
				done_reading_a_line_from_first_source = 0;
				done_reading_a_line_from_second_source = 0;
			}
		}
	}
	return EXIT_SUCCESS;
}

int
diff(const char *const a_file, const char *const another_file)
{
	pthread_t workerid[MAXWORKERS];

	file1 = fopen(a_file, "r");

	if (!file1)
		return EXIT_FAILURE;

	file2 = fopen(another_file, "r");

	if (!file2)
		return EXIT_FAILURE;

	b1 = malloc(sizeof(char) * MAXSIZE);

	b2 = malloc(sizeof(char) * MAXSIZE);

	pthread_create(&workerid[1], NULL, a_source, (void *)1);

	pthread_create(&workerid[2], NULL, another_source, (void *)2);

	pthread_create(&workerid[3], NULL, output, (void *)3);

	pthread_exit(EXIT_SUCCESS);
}
