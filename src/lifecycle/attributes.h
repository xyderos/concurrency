#pragma once

#include <pthread.h>

pthread_attr_t init_attributes(unsigned long stack_size);

pthread_attr_t min_stack_possible(void);

void *worker(void *arg);
