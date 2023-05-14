#include <sys/param.h>

#include <check.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/synchronization/hilzers_barbershop/hilzers_barbershop.h"

Suite *hilzers_barbershop_suite(void);

START_TEST(TEST_HILZERS_BARBERSHOP)
{
	int total_customers = 9, chairs = 3, sofa_size = 4;
	pthread_t barber_threads[chairs];
	pthread_t usher_threads[sofa_size];
	pthread_t customer_threads[total_customers];

	int arr_len = MAX(MAX(chairs, total_customers), sofa_size);
	int stupid_arr[arr_len];
	for (int i = 0; i < arr_len; i++)
		stupid_arr[i] = i;
	hilzers_barbershop_init();

	for (int i = 0; i < chairs; i++)
		pthread_create(&barber_threads[i], NULL, hilzers_barber,
		    &stupid_arr[i]);
	for (int i = 0; i < sofa_size; i++)
		pthread_create(&usher_threads[i], NULL, usher, &stupid_arr[i]);
	for (int i = 0; i < total_customers; i++)
		pthread_create(&customer_threads[i], NULL, hilzers_customer,
		    &stupid_arr[i]);

	for (int i = 0; i < total_customers; i++)
		pthread_join(customer_threads[i], NULL);
	for (int i = 0; i < chairs; i++)
		pthread_cancel(barber_threads[i]);
	for (int i = 0; i < sofa_size; i++)
		pthread_cancel(usher_threads[i]);
	hilzers_barbershop_destroy();
}
END_TEST
