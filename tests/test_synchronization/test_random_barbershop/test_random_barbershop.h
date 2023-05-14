#include <check.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/synchronization/random_barbershop/random_barbershop.h"

Suite *random_barbershop_suite(void);

START_TEST(TEST_RANDOM_BARBERSHOP)
{
	int total_customers = 3;

	pthread_t barber_thread;
	pthread_t customer_threads[total_customers];
	int argument_arr[total_customers];

	random_barbershop_init();

	pthread_create(&barber_thread, NULL, random_barber, &total_customers);
	for (int i = 0; i < total_customers; i++) {
		argument_arr[i] = i;
		pthread_create(&customer_threads[i], NULL, random_customer,
		    &argument_arr[i]);
	}

	for (int i = 0; i < total_customers; i++)
		pthread_join(customer_threads[i], NULL);
	pthread_cancel(barber_thread);
	random_barbershop_destroy();
}
END_TEST
