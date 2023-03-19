#pragma once

// cast to void* then re cast it back as a struct so it can be passed as an
// argumnt for the threads
typedef struct arguments {
  
int *array;

  unsigned int low;

  unsigned int high;

  unsigned int depth;
} args_t;

// There is the notion of depth, meaning how many "layers" of threads
// we can create as a limit before we switch to the sequential version
void quicksort(void*);
