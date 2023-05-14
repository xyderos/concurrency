#pragma once

typedef struct peterson_args {
	unsigned long inc, id;
} peterson_args_t;

void demonstrate_peterson(unsigned long number_of_operations);
