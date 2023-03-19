#include <check.h>
#include <pthread.h>
#include <stdlib.h>

#include "../../../src/synchronization/concurrent_quicksort/quicksort.h"

Suite *quicksort_suite(void);

static int
is_sorted(int *a, unsigned int l)
{
	unsigned int i = l - 1;
	if (i <= 0)
		return 1;
	if ((i & 1) > 0) {
		if (a[i] < a[i - 1])
			return 0;
		i--;
	}
	for (int ai = a[i]; i > 0; i -= 2)
		if (ai < (ai = a[i - 1]) || ai < (ai = a[i - 2]))
			return 0;
	return a[0] <= a[1];
}

START_TEST(TEST_QUICKSORT_SHOULD_BE_OKAY)
{
	unsigned int size = 8, depth = 5;

	int *array = malloc(size * sizeof(long));

	args_t arguments = { array, 0, size - 1, depth };

	for (unsigned int i = 0; i < size; i++)
		arguments.array[i] = rand() % 999;

	quicksort(&arguments);

	ck_assert(is_sorted(arguments.array, size));

	free(arguments.array);
}
END_TEST
