#pragma once
#include <linux/futex.h>
#include <pthread.h>

typedef struct futex_args {
	unsigned long inc, id;
	volatile int *mutex;
} futex_args_t;

void demonstrate_futex(unsigned long number_of_operations);
