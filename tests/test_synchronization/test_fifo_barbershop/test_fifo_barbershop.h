#include <check.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/synchronization/fifo_barbershop/fifo_barbershop.h"

Suite *fifo_barbershop_suite(void);

START_TEST(TEST_FIFO_BARBERSHOP)
{
	int total_customers = 3;
	pthread_t barber_thread;
	pthread_t customer_threads[total_customers];
	int argument_arr[total_customers];
	fifo_barbershop_init();

	pthread_create(&barber_thread, NULL, fifo_barber, NULL);
	for (int i = 0; i < total_customers; i++) {
		argument_arr[i] = i;
		pthread_create(&customer_threads[i], NULL, fifo_customer,
		    &argument_arr[i]);
	}

	for (int i = 0; i < total_customers; i++)
		pthread_join(customer_threads[i], NULL);

	pthread_cancel(barber_thread);

	fifo_barbershop_destroy();
}
END_TEST
