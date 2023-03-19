#include "attributes.h"

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

// remember to pass at least the minimum + the resources, otherwise the setting
// stack function will fail
pthread_attr_t init_attributes(unsigned long stack_size) {
  int s = 0;
  void *sp = NULL;
  struct sched_param s_p;

  pthread_attr_t attr;

  s = pthread_attr_init(&attr);

  // set detach state
  s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  assert(!s);

  // set scope
  s = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  assert(!s);
  // set scheduling policy
  s = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  assert(!s);
  // set inherit scheduling policy
  s = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  assert(!s);
  // set priority
  s_p.sched_priority = 30;
  s = pthread_attr_setschedparam(&attr, &s_p);
  assert(!s);
  // align stack memory based on page size
  s = posix_memalign(&sp, (unsigned long)sysconf(_SC_PAGESIZE), stack_size);
  assert(!s);
  s = pthread_attr_setstack(&attr, sp, stack_size);
  assert(!s);

  return attr;
}

pthread_attr_t min_stack_possible(void) {
  int s = 0;
  void *sp = NULL;
  struct sched_param s_p;

  pthread_attr_t attr;
  s = pthread_attr_init(&attr);

  // set detach state
  s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // set scope
  s = pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);

  // set scheduling policy
  s = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

  // set inherit scheduling policy
  s = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

  // set priority
  s_p.sched_priority = 1;
  s = pthread_attr_setschedparam(&attr, &s_p);

  // align stack memory based on page size
  s = posix_memalign(&sp, (unsigned long)sysconf(_SC_PAGESIZE),
                     (unsigned long)PTHREAD_STACK_MIN);

  s = pthread_attr_setstack(&attr, sp, (unsigned long)PTHREAD_STACK_MIN);

  assert(!s);

  return attr;
}

// do not do any actual work, as long as it is initialized its fine

void *w(void *arg) {
  // disable compiler warnings
  (void)arg;
  pthread_exit(EXIT_SUCCESS);
}
