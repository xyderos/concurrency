#include "quicksort.h"

#include <pthread.h>
#include <stdlib.h>

static inline void swap(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

static unsigned int partition(int *arr, unsigned int low, unsigned int high,
                              unsigned int pivot) {
  int pivot_value = arr[pivot];
  unsigned int s = low;

  swap(&arr[pivot], &arr[high]);

  for (unsigned int i = low; i < high; i++)
    if (arr[i] <= pivot_value) swap(&arr[i], &arr[s++]);

  swap(&arr[s], &arr[high]);

  return s;
}

static void sequential_quicksort(args_t *arguments) {
  args_t first_half = {arguments->array, arguments->low, 0, 0};
  args_t second_half = {arguments->array, 0, arguments->high, 0};

  if (arguments->low < arguments->high) {
    unsigned int pivot_position =
        arguments->low + (arguments->high - arguments->low) / 2;

    pivot_position = partition(arguments->array, arguments->low,
                               arguments->high, pivot_position);

    first_half.high = pivot_position - 1;
    second_half.low = pivot_position + 1;

    sequential_quicksort(&first_half);

    sequential_quicksort(&second_half);
  }
}

void *quicksort_worker(void *arg) {
  quicksort(arg);

  return EXIT_SUCCESS;
}

void quicksort(void *arg) {
  args_t *arguments = arg;
  args_t first_half = {arguments->array, arguments->low, 0, 0},
         second_half = {arguments->array, 0, arguments->high, 0};
  pthread_t thread;

  if (arguments->low < arguments->high) {
    unsigned int pivot_position =
        arguments->low + (arguments->high - arguments->low) / 2;

    pivot_position = partition(arguments->array, arguments->low,
                               arguments->high, pivot_position);

    first_half.high = pivot_position - 1;

    second_half.low = pivot_position + 1;

    if (arguments->depth > 0) {
      unsigned int new_depth = arguments->depth - 1;
      first_half.depth = new_depth;
      second_half.depth = new_depth;
      pthread_create(&thread, NULL, quicksort_worker, &first_half);

      quicksort(&second_half);

      pthread_join(thread, NULL);

    } else {
      sequential_quicksort(&first_half);

      sequential_quicksort(&second_half);
    }
  }
}
