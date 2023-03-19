#include "barrier.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define matrix_size 1000
#define number_of_workers 10

static const unsigned int strip_size = matrix_size / number_of_workers;

void barrier(void);
void *worker(void *arg);
void initialize(unsigned int *_sums, unsigned int **_matrix,
                unsigned int **_min_values, unsigned int **_max_values);

static pthread_mutex_t lock;

static pthread_cond_t conditional;

static unsigned int number_of_arrived_threads = 0;

static unsigned int sums[number_of_workers];

static unsigned int matrix[matrix_size][matrix_size];

static unsigned int minimum_values[number_of_workers][3];

static unsigned int maximum_values[number_of_workers][3];

// wrapper function to the barrier
// waits until the number of arrived threads
// is equal to the number of workers allocated
// signals all threads to continue if the limit is reached
void barrier(void) {
  pthread_mutex_lock(&lock);

  number_of_arrived_threads++;

  if (number_of_arrived_threads == number_of_workers) {
    number_of_arrived_threads = 0;
    pthread_cond_broadcast(&conditional);
  } else {
    pthread_cond_wait(&conditional, &lock);
  }

  pthread_mutex_unlock(&lock);
}

int barrier_sum(void) {
  unsigned int i = 0, j = 0;

  unsigned long l = 0;

  pthread_attr_t attr;

  pthread_t workerid[number_of_workers];

  pthread_attr_init(&attr);

  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  pthread_mutex_init(&lock, NULL);

  pthread_cond_init(&conditional, NULL);

  for (i = 0; i < matrix_size; i++) {
    for (j = 0; j < matrix_size; j++) {
      matrix[i][j] = (unsigned int)(rand() % 99);
    }
  }

  for (l = 0; l < number_of_workers; l++) {
    pthread_create(&workerid[l], &attr, worker, (void *)l);
  }

  pthread_exit(EXIT_SUCCESS);
}

void *worker(void *arg) {
  long myid = (long)arg, first = 0, last = 0, i = 0;

  unsigned total = 0, j = 0, min = 0, minimum_row = 0, minimum_column = 0,
           max = 0, maxRow = 0, maximum_column = 0;

  first = myid * strip_size;

  last = (myid == number_of_workers - 1) ? (matrix_size - 1)
                                         : (first + strip_size - 1);

  total = 0;

  max = matrix[first][0];

  min = matrix[first][0];

  for (i = first; i <= last; i++) {
    for (j = 0; j < matrix_size; j++) {
      total += matrix[i][j];

      if (matrix[i][j] >= max) {
        max = matrix[i][j];
        maxRow = (unsigned int)i;
        maximum_column = (unsigned int)j;
      }
      if (matrix[i][j] <= min) {
        min = matrix[i][j];
        minimum_row = (unsigned int)i;
        minimum_column = (unsigned int)j;
      }
    }
  }

  minimum_values[myid][0] = min;

  minimum_values[myid][1] = minimum_row;

  minimum_values[myid][2] = minimum_column;

  maximum_values[myid][0] = max;

  maximum_values[myid][1] = maxRow;

  maximum_values[myid][2] = maximum_column;

  sums[myid] = total;

  // wait for all the threads to be done
  barrier();

  // leave the first thread handle the rest
  if (myid == 0) {
    total = 0;
    max = 0;
    min = 100;
    for (i = 0; i < number_of_workers; i++) {
      total += sums[i];

      if (maximum_values[i][0] >= max) {
        max = maximum_values[i][0];
        maxRow = maximum_values[i][1];
        maximum_column = maximum_values[i][2];
      }
      if (minimum_values[i][0] <= min) {
        min = minimum_values[i][0];
        minimum_row = minimum_values[i][1];
        minimum_column = minimum_values[i][2];
      }
    }
  }
  return EXIT_SUCCESS;
}
