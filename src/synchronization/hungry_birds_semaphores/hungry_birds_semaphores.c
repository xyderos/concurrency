#include "hungry_birds_semaphores.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NOF_KIDS 8

#define NOF_WORMS 5

// lock
static sem_t lock;

// queue
static sem_t worms_in_the_bowl;

static int bowl_of_worms[NOF_WORMS];

static int index_where_we_fill_up_the_bowl = 0;

static int index_where_we_eat_from_the_bowl = 0;

static int no_worms_in_the_bowl = 0;

static void put_a_worm_in_the_bowl(int value) {
  bowl_of_worms[index_where_we_fill_up_the_bowl] = value;

  // update the value of front that the producer is using
  index_where_we_fill_up_the_bowl =
      (index_where_we_fill_up_the_bowl + 1) % NOF_WORMS;
}

static int eat_a_worm_from_the_bowl() {
  int tmp = bowl_of_worms[index_where_we_eat_from_the_bowl];

  // update the value of the rear that the consumers are using
  index_where_we_eat_from_the_bowl =
      (index_where_we_eat_from_the_bowl + 1) % NOF_WORMS;

  return tmp;
}

void *father(void *arg) {
  for (;;) {
    // there are no worms remaining and father must fill the plate
    if (!no_worms_in_the_bowl) {
      printf(
          "There are no worms in the bowl, the father will fill it up soon\n");
      // lock critical section
      sem_wait(&lock);

      // fill in the plate
      for (int i = 0; i < NOF_WORMS; i++) put_a_worm_in_the_bowl(i);

      printf("Father just filled in the bowl\n");

      // plat is filled up again
      no_worms_in_the_bowl = 1;

      printf("Father is waiting for the birds to eat\n");
      // signal the queue that there are worms
      sem_post(&worms_in_the_bowl);

      // unlock critical section
      sem_post(&lock);
    }
  }
}

void *birds(void *arg) {
  long id = (long)arg;

  int temp, position;

  for (;;) {
    // lock critical section
    sem_wait(&lock);

    // empty bowl, signal and wait to be filled by father (needs to unlock the
    // mutex since the father will use that and we need to prevent deadlock)
    printf("There are no worms in the bowl, The father must be notified\n");
    while (!no_worms_in_the_bowl) {
      // unlock
      sem_post(&lock);

      // signal
      sem_wait(&worms_in_the_bowl);

      // reaquire again
      sem_wait(&lock);
    }

    // get the next position
    //(other threads might finish the plate and ask for a refill so some birds
    // might eat the same number of worms eg thread 0 ate number 2 and thread 3
    // ate number 2 as well)
    position = index_where_we_eat_from_the_bowl;

    // get the next position of available worn and eat it(the decrement is
    // happening on the get function by changing the position of the next
    // available worm)
    temp = eat_a_worm_from_the_bowl();

    printf("Baby bird %ld just ate worm %d from the bowl\n", id, temp);

    // there is no empty space for the birds to eat so emptySpace will signal
    // the father to refill the plate
    if (position == (NOF_WORMS - 1)) no_worms_in_the_bowl = 0;

    // unlock
    sem_post(&lock);

    // trying to avoid starvation to other threads since by eating and going to
    // sleep they will be blocvked and the next thread in the queue will execute
    sleep(5);
  }
}

/**
the solution given is kind of unfair since if there is no sleep or a small sleep
there will be a race condition whom takes the worms because of sem_wait
*/
int hungry_birds_semaphores(void) {
  pthread_t cons[NOF_KIDS];

  pthread_t prod;

  sem_init(&lock, 0, 1);

  sem_init(&worms_in_the_bowl, 0, 0);

  pthread_create(&prod, NULL, father, NULL);

  for (long i = 0; i < NOF_KIDS; i++)
    pthread_create(&cons[i], NULL, birds, (void *)i);

  pthread_exit(EXIT_SUCCESS);
}
