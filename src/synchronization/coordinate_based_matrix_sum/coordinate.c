#include "coordinate.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define matrix_size 1000
#define number_of_workers 10

static const unsigned int strip_size = matrix_size / number_of_workers;

void barrier(void);
void *coordinate_worker(void *arg);

static pthread_cond_t conditional;

static pthread_mutex_t minimum_lock;

static pthread_mutex_t maximum_lock;

static pthread_mutex_t sum_lock;

static unsigned int sums[number_of_workers];

static unsigned int matrix[matrix_size][matrix_size];
static unsigned int minimum, minimum_row, minimum_column, maximum, maximum_row,
    maximum_column, final;

int coordinate_based_matrix_sum(void) {
  unsigned int i = 0, j = 0;

  unsigned long l = 0;

  pthread_attr_t attr;

  pthread_t workerid[number_of_workers];

  pthread_attr_init(&attr);

  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  pthread_cond_init(&conditional, NULL);

  for (i = 0; i < matrix_size; i++) {
    for (j = 0; j < matrix_size; j++) {
      matrix[i][j] = (unsigned int)(rand() % 99);
    }
  }

  for (l = 0; l < number_of_workers; l++) {
    pthread_create(&workerid[l], &attr, coordinate_worker, (void *)l);
  }

  // wait for all the threads to finish the actual work
  for (l = 0; l < number_of_workers; l++) {
    pthread_join(workerid[l], NULL);
  }

  pthread_exit(EXIT_SUCCESS);
}

void *coordinate_worker(void *arg) {
  unsigned long myid = (unsigned long)arg, first = 0, last = 0, i = 0, j = 0,
                total = 0;

  first = myid * strip_size;

  last = (myid == number_of_workers - 1) ? (matrix_size - 1)
                                         : (first + strip_size - 1);

  for (i = first; i <= last; i++) {
    for (j = 0; j < matrix_size; j++) {
      // need to double check because of race conditions
      // we might have been swapped from running and the actual value might have
      // been changed
      if (matrix[i][j] > maximum) {
        pthread_mutex_lock(&maximum_lock);
        if (matrix[i][j] > maximum) {
          maximum = matrix[i][j];
          maximum_row = (unsigned int)i;
          maximum_column = (unsigned int)j;
        }
        pthread_mutex_unlock(&maximum_lock);
      }
      if (matrix[i][j] <= minimum) {
        pthread_mutex_lock(&minimum_lock);
        if (matrix[i][j] <= minimum) {
          minimum = matrix[i][j];
          minimum_row = (unsigned int)i;
          minimum_column = (unsigned int)j;
        }
        pthread_mutex_unlock(&minimum_lock);
      }
      total += matrix[i][j];
    }
  }

  pthread_mutex_lock(&sum_lock);

  sums[myid] = (unsigned int)total;

  final += total;

  pthread_mutex_unlock(&sum_lock);
  return EXIT_SUCCESS;
}
