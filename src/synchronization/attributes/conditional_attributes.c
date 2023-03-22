#include <pthread.h>

#include "conditional_attributes.h"

void
conditional_attributes(void)
{
	clockid_t clock_id;
	pthread_condattr_t attrs;

	// sets whether or not this conditional variable shall be shareable
	// between other processes
	pthread_condattr_setpshared(&attrs, 1);

	// sets the clocked attribute for timedwait
	pthread_condattr_setclock(&attrs, clock_id);

	pthread_condattr_init(&attrs);
}
