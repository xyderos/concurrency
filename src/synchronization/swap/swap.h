#pragma once

typedef struct swap_args {
	unsigned long inc, id;
	volatile int *mutex;
} swap_args_t;

void demonstrate_swap(unsigned long number_of_calculations);
