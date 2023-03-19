#include "bear_honey_bees_semaphores.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define number_of_honeybees 10
#define pot_capacity 10

static sem_t pot_is_full, bees_are_gathering;

static unsigned int current_size_of_the_pot = 0;

void *a_bear(void *arg);
void *some_honeybees(void *arg);

void *a_bear(void *arg) {
  (void)arg;

  for (;;) {
    // wait until the pot is full
    sem_wait(&pot_is_full);

    printf("Bear woke up and started eating from the pot.\n");

    current_size_of_the_pot = 0;

    printf(
        "The pot is now empty, the bear needs to wait for the bees to fill it "
        "up.\n");

    // signal to the bees to start gathering
    sem_post(&bees_are_gathering);
  }
}

void *some_honeybees(void *arg) {
  for (;;) {
    // wait until the bear eats all the honey
    sem_wait(&bees_are_gathering);

    current_size_of_the_pot++;

    printf("Honeybee %ld gathered some honey, the pot now is %.6f full\n",
           (long)arg,
           (double)((float)current_size_of_the_pot / (float)pot_capacity));

    // signal to the bear that the pot is full
    if (current_size_of_the_pot == pot_capacity) {
      printf("The bees just filled up the pot\n");
      sem_post(&pot_is_full);

    }

    else {
      printf("The bees are not done filling up the pot yet\n");
      // increase it indicating that you still need to work until it is full
      sem_post(&bees_are_gathering);
    }
  }
}

int bear_honey_bees_semaphores(void) {
  pthread_attr_t attr;
  long i = 0;

  pthread_t workerid[number_of_honeybees + 1];

  pthread_attr_init(&attr);

  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  sem_init(&pot_is_full, 0, 0);

  sem_init(&bees_are_gathering, 0, 1);

  pthread_create(&workerid[0], NULL, a_bear, (void *)i);

  for (i = 1; i <= number_of_honeybees; i++)
    pthread_create(&workerid[i], NULL, some_honeybees, (void *)i);

  for (i = 1; i <= number_of_honeybees; i++) pthread_join(workerid[i], NULL);

  pthread_exit(EXIT_SUCCESS);
}
