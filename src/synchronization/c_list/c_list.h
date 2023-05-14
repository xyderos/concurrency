#pragma once
#include <pthread.h>

typedef struct c_list_cell {
	int val;
	struct c_list_cell *next;
	pthread_mutex_t mutex;
} c_list_cell_t;

typedef struct c_list_args {
	unsigned long inc, id;
	c_list_cell_t *list;
} c_list_args_t;

void bench_c_list(unsigned long number_of_threads,
    unsigned long number_of_operations);
