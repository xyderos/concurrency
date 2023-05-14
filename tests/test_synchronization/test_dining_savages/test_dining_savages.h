#include <check.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/synchronization/dining_savages/dining_savages.h"

Suite *dining_savages_suite(void);

START_TEST(TEST_DINING_SAVAGES)
{
	int savages_num = 3;
	pthread_t cook_thread = 0;
	pthread_t savages_threads[savages_num];
	int argument_array[savages_num];

	savages_init();

	pthread_create(&cook_thread, NULL, cook, &savages_num);
	for (int i = 0; i < savages_num; i++) {
		argument_array[i] = i;
		pthread_create(&savages_threads[i], NULL, savage,
		    &argument_array[i]);
	}

	pthread_join(cook_thread, NULL);
	for (int i = 0; i < savages_num; i++) {
		pthread_join(savages_threads[i], NULL);
	}
	savages_destroy();
}
END_TEST
