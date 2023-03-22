#include <pthread.h>
#include <semaphore.h>

#include "semaphore_attributes.h"

void
semaphore_attributes(void)
{
	sem_t sem;

	// pshared specifies if this will be a cross process semaphore (if non
	// zero)
	sem_init(&sem, 0, 1000);
}
