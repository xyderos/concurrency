#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_key_t a_key;

typedef struct data {
  int an_integer;
  float a_float;
} data_t;

void initialize_specific_data(void);
void change_specific_data(int an_integer, float a_float);
int print_specific_data(unsigned long t_id);
void destructor(void *arg);
void *a_thread(void *arg);
void *another_thread(void *arg);

void initialize_specific_data(void) {
  data_t *tsd = pthread_getspecific(a_key);

  if (!tsd) {
    data_t *s = malloc(sizeof(data_t));

    memset(s, 0, sizeof(*s));

    pthread_setspecific(a_key, s);
  }
}

void change_specific_data(int an_integer, float a_float) {
  data_t *struct_data = pthread_getspecific(a_key);

  struct_data->an_integer = an_integer;
  struct_data->a_float = a_float;
}

int print_specific_data(unsigned long t_id) {
  printf("Thread id : %lu\n", t_id);
  printf("Address of global : %p \n", &a_key);
  printf("Address of local : %p \n", (data_t *)pthread_getspecific(a_key));
  printf("i is : %d , k is : %f\n",
         ((data_t *)pthread_getspecific(a_key))->an_integer,
         ((data_t *)pthread_getspecific(a_key))->a_float);
  return 0;
}

void destructor(void *arg) { free(arg); }

void *a_thread(void *arg) {
  unsigned long t_id = (unsigned long)arg;
  initialize_specific_data();
  change_specific_data(10, 3.141500f);
  print_specific_data(t_id);
  pthread_exit(EXIT_SUCCESS);
}

void *another_thread(void *arg) {
  initialize_specific_data();
  print_specific_data(2);
  change_specific_data(12, 2.141500f);
  print_specific_data(2);

  change_specific_data(122, 22.141500f);
  print_specific_data(2);
  pthread_exit(EXIT_SUCCESS);
}

int thread_specific_data(void) {
  pthread_t t1, t2;

  pthread_key_create(&a_key, NULL);

  pthread_create(&t1, NULL, (void *)a_thread, (void *)1);
  pthread_create(&t2, NULL, (void *)another_thread, (void *)1);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  pthread_key_delete(a_key);

  return 0;
}
