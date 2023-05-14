#pragma once

typedef struct s_list_cell {
	int val, mutex;
	struct s_list_cell *next;
} s_list_cell_t;

typedef struct s_list_args {
	unsigned long inc, id;
	s_list_cell_t *list;
} s_list_args_t;

void bench_s_list(unsigned long number_of_threads,
    unsigned long number_of_operations);
